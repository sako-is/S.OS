#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>

#define MB 1024

#define attr(x) __attribute__((x))
#define asm     __asm__
#define asmvol  __asm__ __volatile__

#define typeof __typeof

typedef float  float32;
typedef double float64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uintptr_t uintptr;

#if defined(__x86_64__) || defined(_M_X64)
	#define X86_64
	#define __PHYSICAL_MEMORY_EXTENSION__
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
	#define X86_32
#elif defined(__ARM_ARCH_2__)
	#define ARMv2
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
	#define ARMv3
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
	#define ARMv4T
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
	#define ARMv5
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
	#define ARMv6T2
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
	#define ARMv6
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	#define ARMv7
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	#define ARMv7A
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	#define ARMv7R
#elif defined(__ARM_ARCH_7M__)
	#define ARMv7M
#elif defined(__ARM_ARCH_7S__)
	#define ARMv7S
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define ARMv64
#elif defined(mips) || defined(__mips__) || defined(__mips)
	#define MIPS
#elif defined(__sh__)
	#define SUPERH
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
	#define POWERPC
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
	#define POWERPC64
#elif defined(__sparc__) || defined(__sparc)
	#define SPARC
#elif defined(__m68k__)
	#define M68K
#else
	#define UNKNOWN_ARCH
#endif
