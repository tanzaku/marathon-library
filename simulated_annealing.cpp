
inline double calcTemp(double startTime, double endTime, double curTime, double startTemp, double endTemp)
{
  const double c = 1 - (curTime - startTime) / (endTime - startTime);
  return c * (startTemp - endTemp) + endTemp;
}

void sa(double t)
{
  const int TIME_LIMIT = 9500;
  const int START_TIME = 0;
  const double T0 = 1;
  const double T1 = 0.5;

  while (true) {
    const double t = timer.elapsed();
    if (t >= TIME_LIMIT) {
      break;
    }

    // diffは近傍とのコストの差分で、近傍のコスト - 今のコスト
    // コストは小さい方が良い。
    // 改善していればdiffは負になる
    const int diff = 0;

    const double temp = calcTemp(START_TIME, TIME_LIMIT, t, T0, T1);

    if (diff <= -temp * rng.nextLog()) {
      // acceptされたので遷移する
      return;
    }
  }
}
