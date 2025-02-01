#include "yhook.h"
#include <stdio.h>

yHook_t hook;

class Test {
public:
  Test() { printf("Test() constructed\n"); }
  void dostuff() { printf("dostuff(%p) called\n", this); }
};

void dostuffH(Test *thiz) {
  // we'll have to cast the function because c++
  yHookTrampoline(hook, ((void (*)(Test *))hook.from), (Test *)thiz);

  printf("dostuffH(%p) called\n", thiz);
}

int main() {
  hook = yHookInstall((yaddr_t)(&Test::dostuff), (yaddr_t)dostuffH);
  yHookEnable(hook);

  Test x = Test();
  x.dostuff();
}
