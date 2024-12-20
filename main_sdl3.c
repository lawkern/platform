/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "SDL3/SDL.h"
#include "platform.h"

#include <stdlib.h>

PLATFORM_LOG(platform_log)
{
   va_list arguments;
   va_start(arguments, fmt);
   {
      SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, arguments);
   }
   va_end(arguments);
}

PLATFORM_ALLOCATE(platform_allocate)
{
   (void)ctx;

   return SDL_calloc(1, size);
}

PLATFORM_DEALLOCATE(platform_deallocate)
{
   (void)ctx;
   (void)size;

   SDL_free(memory);
}

typedef struct {
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *texture;
   SDL_Gamepad *controllers[4];
   const SDL_DisplayMode *display_mode;

   bool running;
   Uint64 frequency;
   Uint64 frame_start;
   Uint64 frame_count;

   int refresh_rate;
   float target_frame_seconds;
   float actual_frame_seconds;
} sdl3_context;

static void sdl3_log_error(void)
{
   platform_log("SDL3 ERROR: %s", SDL_GetError());
}

static void sdl3_initialize(sdl3_context *sdl3, int width, int height)
{
   if(!SDL_Init(SDL_INIT_VIDEO))
   {
      sdl3_log_error();
      return;
   }

   if(!SDL_CreateWindowAndRenderer(PROGRAM_NAME, width, height, 0, &sdl3->window, &sdl3->renderer))
   {
      sdl3_log_error();
      return;
   }

   int display_count;
   SDL_DisplayID *displays = SDL_GetDisplays(&display_count);
   if(!displays)
   {
      sdl3_log_error();
      return;
   }
   platform_log("Display Count: %d\n", display_count);

   // TODO: Handle multiple monitors properly, instead of just using the first
   // of the returned display.
   sdl3->display_mode = SDL_GetDesktopDisplayMode(displays[0]);
   if(!sdl3->display_mode)
   {
      sdl3_log_error();
      return;
   }
   SDL_free(displays);

   // NOTE: Initialize frame information.
   sdl3->frequency = SDL_GetPerformanceFrequency();

   sdl3->refresh_rate = 60;
   if(sdl3->display_mode->refresh_rate > 0)
   {
      sdl3->refresh_rate = sdl3->display_mode->refresh_rate;
   }

   sdl3->target_frame_seconds = 1.0f / sdl3->refresh_rate;

   platform_log("Monitor refresh rate: %d\n", sdl3->refresh_rate);
   platform_log("Target frame time: %0.03fms\n", sdl3->target_frame_seconds * 1000.0f);

   sdl3->running = true;
}

static void sdl3_process_input(sdl3_context *sdl3)
{
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
         case SDL_EVENT_QUIT: {
            sdl3->running = false;
         } break;

         case SDL_EVENT_KEY_UP:
         case SDL_EVENT_KEY_DOWN: {
            switch(event.key.key)
            {
               case SDLK_ESCAPE: {
                  sdl3->running = false;
               } break;

               case SDLK_F: {
                  if(event.key.down)
                  {
                     bool is_fullscreen = (SDL_GetWindowFlags(sdl3->window) & SDL_WINDOW_FULLSCREEN);
                     SDL_SetWindowFullscreen(sdl3->window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                  }
               } break;
            }
         } break;
      }
   }
}

static void sdl3_render(sdl3_context *sdl3)
{
   if(!SDL_SetRenderDrawColor(sdl3->renderer, 0x00, 0x00, 0x00, 0xFF))
   {
      sdl3_log_error();
   }

   if(!SDL_RenderClear(sdl3->renderer))
   {
      sdl3_log_error();
   }

   if(!SDL_RenderPresent(sdl3->renderer))
   {
      sdl3_log_error();
   }
}

#define SDL3_ELAPSED_SECONDS(start, end, freq) ((float)((end) - (start)) / (float)(freq))

static void sdl3_frame_end(sdl3_context *sdl3)
{
   // NOTE: Compute how much time has elapsed this frame.
   float target = sdl3->target_frame_seconds;
   Uint64 freq = sdl3->frequency;
   Uint64 start = sdl3->frame_start;
   Uint64 end = SDL_GetPerformanceCounter();

   float dt = SDL3_ELAPSED_SECONDS(start, end, freq);
   float work_ms = dt * 1000.0f;
   int sleep_ms = 0;

   // NOTE: Sleep through excess frame time to enforce frame rate.
   if(dt < target)
   {
      sleep_ms = (int)((target - dt) * 1000.0f) - 1;
      if(sleep_ms > 0)
      {
         SDL_Delay(sleep_ms);
      }
   }

   // NOTE: Spinlock for the remaining frame time, if any remains.
   while(dt < target)
   {
      end = SDL_GetPerformanceCounter();
      dt = SDL3_ELAPSED_SECONDS(start, end, freq);
   }

   // NOTE: Update values for next frame.
   sdl3->frame_start = SDL_GetPerformanceCounter();
   sdl3->actual_frame_seconds = dt;
   sdl3->frame_count++;

   if((sdl3->frame_count % sdl3->refresh_rate) == 0)
   {
      float frame_ms = sdl3->actual_frame_seconds * 1000.0f;
      platform_log("Frame time: % .3fms (Worked: % .3fms, Requested sleep: % 3dms)\n", frame_ms, work_ms, sleep_ms);
   }
}

int main(void)
{
   sdl3_context sdl3 = {};
   sdl3_initialize(&sdl3, WINDOW_WIDTH, WINDOW_HEIGHT);

   while(sdl3.running)
   {
      sdl3_process_input(&sdl3);
      sdl3_render(&sdl3);
      sdl3_frame_end(&sdl3);
   }

   SDL_Quit();

   return(0);
}
