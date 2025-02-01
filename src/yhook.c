#include "yhook.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int yHookProtect(void *address, size_t size, int prot) {
  long page_size;
  void *aligned_address;
  void *end;
  size_t new_size;

  page_size = sysconf(_SC_PAGESIZE);
  aligned_address = (void *)((long)address & ~(page_size - 1));

  end = address + size;
  new_size = end - aligned_address;

  int error = mprotect(aligned_address, new_size, prot);
  return error;
}

yHook_t yHookInstall(yaddr_t from, yaddr_t to) {
  yHook_t hook;

  hook.from = from;
  hook.to = to;

  // save old code
  memcpy(hook.originalCode, from, JMP_SIZE);

  // generate jmp opcode
#ifdef YHOOK_X64
  memcpy(hook.hookCode, "\x48\xB8", 2);
  memcpy(hook.hookCode + 2, &hook.to, sizeof(yaddr_t));
  memcpy(hook.hookCode + 2 + sizeof(yaddr_t), "\xff\xe0", 2);
#endif

#ifdef YHOOK_X86
  memcpy(hook.hookCode, "\xB8", 1);
  memcpy(hook.hookCode + 1, &hook.to, sizeof(yaddr_t));
  memcpy(hook.hookCode + 1 + sizeof(yaddr_t), "\xff\xe0", 2);
#endif

  return hook;
}

int yHookEnable(yHook_t hook) {
  // unprotect the region
  if (yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC) ==
      -1) {
    return -1;
  }

  memcpy(hook.from, hook.hookCode, JMP_SIZE);

  yHookProtect(hook.from, JMP_SIZE, PROT_READ | PROT_EXEC);
}

int yHookDisable(yHook_t hook) {
  // unprotect the region
  if (yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC) ==
      -1) {
    return -1;
  }

  memcpy(hook.from, hook.originalCode, JMP_SIZE);

  yHookProtect(hook.from, JMP_SIZE, PROT_READ | PROT_EXEC);
}
