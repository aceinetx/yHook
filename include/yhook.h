#pragma once
#include <stddef.h>

#define YHOOK_X64

#if defined(YHOOK_X64)
#define JMP_SIZE 4 + sizeof(void *)
#elif defined(YHOOK_X86)
#define JMP_SIZE 3 + sizeof(void *)
#else
#error "Neither of those are defined: YHOOK_X86 | YHOOK_64"
#endif

typedef void *yaddr_t;

typedef struct {
  char originalCode[JMP_SIZE];
  char hookCode[JMP_SIZE];
  yaddr_t from;
  yaddr_t to;
} yHook_t;

int yHookProtect(void *address, size_t size, int prot);
yHook_t yHookInstall(yaddr_t from, yaddr_t to);
int yHookEnable(yHook_t hook);
int yHookDisable(yHook_t hook);
