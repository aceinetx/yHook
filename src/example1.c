/*
 * yHook
 * Copyright (c) aceinet
 * License: GPL-2.0
 */
#include "yhook.h"
#include <stdio.h>

yHook_t hook;
yHook_t hook2;

void orig(int x) { printf("called orig(%d)\n", x); }

void hooked(int x) {
  printf("called hooked(%d)\n", x);
  yHookTrampoline(hook, orig, x);
}

void hooked2(int x) {
  printf("called hooked2(%d)\n", x);
  yHookTrampoline(hook2, orig, x);
}

int main() {
  hook = yHookInstall(orig, hooked);
  yHookEnable(hook);
  hook2 = yHookInstall(orig, hooked2);
  yHookEnable(hook2);
  orig(69);
}
