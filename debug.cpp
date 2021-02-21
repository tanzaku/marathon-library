

/**
 * assertだとスタックトレースが表示されないので、その代わりに
 * gdb,vs,vscodeなどのデバッガを使う方がいい
 */
void SHOW_STACK_TRACE()
{
  volatile int *t = 0;
  *t = 1;
}
