#pragma once
/* (c) copyright 2024 Lawrence D. Kern ////////////////////////////////////// */

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

#include <stdbool.h>
#include <stddef.h> // ptrdiff_t

// NOTE: This file specifies the API for any functionality provided by the host
// platform. The function signatures only appear here - each implementation
// should use the macros below when a signature is required.

// NOTE: Perform any initialization required by the platform, e.g. creating a
// window, allocating textures, etc.
#define PLATFORM_INITIALIZE(name) void name(char *title, int width, int height)
PLATFORM_INITIALIZE(platform_initialize);

// NOTE: Perform any actions that should occur at the beginning of each frame,
// e.g handling input and other incoming OS events. Return false to indicate the
// program should quit.
#define PLATFORM_FRAME_BEGIN(name) bool name(void)
PLATFORM_FRAME_BEGIN(platform_frame_begin);

// NOTE: Perform any actions that should occur at the end of each frame,
// e.g. frame time computations.
#define PLATFORM_FRAME_END(name) void name(void)
PLATFORM_FRAME_END(platform_frame_end);

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
