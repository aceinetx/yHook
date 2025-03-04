/*
 * yHook
 * Copyright (c) aceinet
 * License: GPL-2.0
 */

#include <yhook.h>
#ifndef YHOOK_ARM
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
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
#define yHookWriteMemory(addr, buffer, size)                                   \
  memcpy((void *)addr, (void *)buffer, (size_t)size)
#else
#include <Windows.h>
#define yHookWriteMemory(addr, buffer, size)                                   \
  WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, (LPCVOID)buffer,       \
                     (SIZE_T)size, NULL)
#define yHookProtect(address, size, prot) ;
#endif

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

#ifdef YHOOK_ARM
  memcpy(hook.hookCode, "\xe5\x1f\x00\x00", 4);
  memcpy(hook.hookCode + 4, "\xe1\x2f\xff\x10", 4);
  memcpy(hook.hookCode + 8, &hook.to, sizeof(yaddr_t));
#endif

  return hook;
}

int yHookEnable(yHook_t hook) {
#ifndef _WIN32
  // unprotect the region
  if (yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC) ==
      -1) {
    return -1;
  }
#endif

  yHookWriteMemory(hook.from, hook.hookCode, JMP_SIZE);

#ifndef _WIN32
  yHookProtect(hook.from, JMP_SIZE, PROT_READ | PROT_EXEC);
#endif

  return 0;
}

int yHookDisable(yHook_t hook) {
#ifndef _WIN32
  // unprotect the region
  if (yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC) ==
      -1) {
    return -1;
  }
#endif

  yHookWriteMemory(hook.from, hook.originalCode, JMP_SIZE);

#ifndef _WIN32
  yHookProtect(hook.from, JMP_SIZE, PROT_READ | PROT_EXEC);
#endif

  return 0;
}

#endif

#ifdef YHOOK_ARM
#include <stdio.h>
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

__attribute__((naked)) void opcarm() {
  asm volatile("movw r0, #0x5678\n\t" // Load lower 16 bits of address
               "movt r0, #0x1234\n\t" // Load upper 16 bits of address
               "bx   r0\n\t"          // Jump to address in r0
  );
}

void __yHookPatch(uint32_t new_addr) {
  uint8_t *code = (uint8_t *)opcarm;

  yHookProtect(opcarm, 8, PROT_WRITE | PROT_EXEC | PROT_READ);

  // Patch the MOVT instruction (lower 16 bits)
  code[4] = new_addr >> 16 & 0xff;
  code[5] = new_addr >> 16 >> 8 & 0xf;
  code[6] = (new_addr >> 16 >> 12 & 0xf) + 0x40;

  // Patch the MOVW instruction (upper 16 bits)
  code[0] = new_addr & 0xff;
  code[1] = new_addr >> 8 & 0xf;
  code[2] = new_addr >> 12 & 0xf;

  // Ensure cache coherence
  __builtin___clear_cache(code, code + 8);
}

yHook_t yHookInstall(void *from, void *to) {
  yHook_t hook;

  hook.from = from;
  hook.to = to;

  memcpy(hook.originalCode, hook.from, JMP_SIZE);

  return hook;
}

int yHookEnable(yHook_t hook) {
  yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_EXEC | PROT_READ);

  __yHookPatch((uint32_t)hook.to);
  memcpy(hook.from, opcarm, JMP_SIZE);
  __builtin___clear_cache(hook.from, hook.from + JMP_SIZE);
}

int yHookDisable(yHook_t hook) {
  yHookProtect(hook.from, JMP_SIZE, PROT_WRITE | PROT_EXEC | PROT_READ);

  memcpy(hook.from, hook.originalCode, JMP_SIZE);
  __builtin___clear_cache(hook.from, hook.from + JMP_SIZE);
}
#endif
