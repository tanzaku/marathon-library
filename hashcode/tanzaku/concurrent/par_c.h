#ifndef PAR_C_H
#define PAR_C_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace tanzaku
{
namespace concurrent
{
namespace par_c
{

bool g_finished;
std::vector<std::thread> g_pool;
std::queue<std::function<void()>> g_task_queue;
std::condition_variable g_thread_cond;
std::mutex g_job_mutex;

void init_thread_pool(int num_threads)
{
  for (int thread_index = 0; thread_index < num_threads; thread_index++) {
    g_pool.emplace_back([thread_index]() {
      while (1) {
        std::function<void()> job;

        {
          std::unique_lock<std::mutex> lock(g_job_mutex);
          g_thread_cond.wait(
              lock, []() { return !g_task_queue.empty() || g_finished; });

          if (g_finished) {
            return;
          }

          job = g_task_queue.front();
          g_task_queue.pop();
        }

        job();
      }
    });
  }
}

int get_num_threads()
{
  return int(g_pool.size());
}

void finalize_thread_pool()
{
  g_finished = true;
  g_thread_cond.notify_all();

  for (auto &th : g_pool) {
    th.join();
  }
}

template <typename F>
auto enqueue(F func) -> std::future<typename std::result_of<F()>::type>
{
  using return_type = typename std::result_of<F()>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::forward<F>(func));
  auto res = task->get_future();

  {
    std::unique_lock<std::mutex> lock(g_job_mutex);
    g_task_queue.emplace([task]() { (*task)(); });
  }

  g_thread_cond.notify_one();
  return res;
}

template <typename F, typename T>
auto map(F func, const std::vector<T> &x) -> std::vector<typename std::result_of<F(T)>::type>
{
  using return_type = typename std::result_of<F(T)>::type;

  std::vector<return_type> mapped_values(x.size());
  std::vector<std::future<void>> futures;

  for (int thread = 0; thread < int(g_pool.size()); thread++) {
    int from = int(x.size()) * thread / int(g_pool.size());
    int to = int(x.size()) * (thread + 1) / int(g_pool.size());
    if (from == to) continue;

    auto future = enqueue([&mapped_values, func, &x, from, to] {
      for (int i = from; i < to; i++) {
        mapped_values[i] = func(x[i]);
      }
    });

    futures.emplace_back(std::move(future));
  }

  for (auto &future : futures) {
    future.get();
  }

  return mapped_values;
}

template <typename F, typename R, typename T>
auto map_reduce(F mapper, R reducer, const std::vector<T> &x) -> typename std::result_of<F(T)>::type
{
  auto mapped_values = map(mapper, x);

  for (int i = 1; i < int(mapped_values.size()); i++) {
    mapped_values[0] = reducer(mapped_values[0], mapped_values[i]);
  }

  return mapped_values[0];
}

template <typename F, typename T>
void run(F runner, const std::vector<T> &x)
{
  std::vector<std::future<void>> futures;

  for (int i = 0; i < int(x.size()); i++) {
    auto f = [runner, &x, i] {
      runner(x[i]);
    };
    auto future = enqueue(f);
    futures.emplace_back(std::move(future));
  }

  for (auto &future : futures) {
    future.get();
  }
}

struct Range {
  int from;
  int to;
};

std::vector<Range> split(int n)
{
  std::vector<Range> ranges;
  for (int i = 0; i < int(g_pool.size()); i++) {
    Range r;
    r.from = n * i / int(g_pool.size());
    r.to = n * (i + 1) / int(g_pool.size());
    if (r.from != r.to) {
      ranges.push_back(r);
    }
  }
  return ranges;
};

struct ThreadPoolRAII {
  ThreadPoolRAII()
  {
    init_thread_pool(std::thread::hardware_concurrency());
  }

  ~ThreadPoolRAII()
  {
    finalize_thread_pool();
  }
};

} // namespace par_c
} // namespace concurrent
} // namespace tanzaku

#endif // PAR_C_H
