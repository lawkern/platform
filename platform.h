#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: Default configurable values.

#ifndef PROGRAM_NAME
#   define PROGRAM_NAME "Platform Demo Program"
#endif
#ifndef WINDOW_WIDTH
#   define WINDOW_WIDTH 600
#endif
#ifndef WINDOW_HEIGHT
#   define WINDOW_HEIGHT 400
#endif

#include <stddef.h> // ptrdiff_t

// NOTE: This file specifies the API for any functionality provided by the host
// platform. The function signatures only appear here - each implementation
// should use the macros below when a signature is required.

// NOTE: Output a printf-style formatted string to some reasonable location for
// a given platform.
#define PLATFORM_LOG(name) void name(char *fmt, ...)
PLATFORM_LOG(platform_log);

// NOTE: Allocate the requested number of bytes. Cleared to zero. This is
// intended for bulk-allocations at startup, it makes no promises of being fast.
#define PLATFORM_ALLOCATE(name) void *name(void *ctx, ptrdiff_t size)
PLATFORM_ALLOCATE(platform_allocate);

// NOTE: Deallocate memory previous allocated by the platform API. This is
// mainly intended for undoing bulk-allocations at startup, it makes no
// promises of being fast.
#define PLATFORM_DEALLOCATE(name) void name(void *ctx, void *memory, ptrdiff_t size)
PLATFORM_DEALLOCATE(platform_deallocate);
