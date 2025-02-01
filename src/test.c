#include "yhook.h"
#include <stdio.h>

yHook_t hook;

void orig(int x) { printf("called orig(%d)\n", x); }

void hooked(int x) {
  printf("called hooked(%d)\n", x);
  x = 123;
  yHookDisable(hook);
  orig(x);
  yHookEnable(hook);
}

int main() {
  hook = yHookInstall(orig, hooked);
  yHookEnable(hook);
  orig(69);
}
