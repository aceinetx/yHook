#include <stdio.h>
#ifndef _WIN32
#include "yhook.h"
#include <string.h>

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
  hook = yHookInstall((void*)(&Test::dostuff), (yaddr_t)dostuffH);
  yHookEnable(hook);

  Test x = Test();
  x.dostuff();
}
#else
int main() {
	printf("second example is not available because of stupid msvc compiler that doesn't like casting functions :(");
}
#endif