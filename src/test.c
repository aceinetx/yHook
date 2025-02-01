#include "yhook.h"
#include <stdio.h>

yHook_t hook;
yHook_t hook2;

void orig(int x) { printf("called orig(%d)\n", x); }

void hooked(int x) {
  printf("called hooked(%d)\n", x);
  yHookDisable(hook);
  orig(x);
  yHookEnable(hook);
}

void hooked2(int x) {
  printf("called hooked2(%d)\n", x);
  yHookDisable(hook2);
  orig(x);
  yHookEnable(hook2);
}

int main() {
  hook = yHookInstall(orig, hooked);
  yHookEnable(hook);
  hook2 = yHookInstall(orig, hooked2);
  yHookEnable(hook2);
  orig(69);
}
