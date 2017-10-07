#include <stdio.h>
#include <SDL.h>

#define internal static

struct sdl2_program_state
{
  bool IsRunning;
  SDL_Event LastEvent;
};

///////////////////////////////////////////////////////////////////////////////

internal void
PlatformHandleEvent(sdl2_program_state* ProgramState)
{
  if (ProgramState->LastEvent.type == SDL_QUIT)
  {
    ProgramState->IsRunning = false;
  }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    SDL_Log("Unable to initialized SDL: %s", SDL_GetError());
    return 1;
  }

  // Always SDL_Quit when the application exits.
  atexit(SDL_Quit);

  SDL_Window* Window = SDL_CreateWindow(
    "Circular Audio Buffer Example",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    1280,
    720,
    SDL_WINDOW_OPENGL
  );

  if (!Window)
  {
    SDL_Log("Unable to initialize window: %s", SDL_GetError());
    return 1;
  }

  SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);

  sdl2_program_state ProgramState = {};
  ProgramState.IsRunning = true;

  while (ProgramState.IsRunning)
  {
    while (SDL_PollEvent(&ProgramState.LastEvent))
    {
      PlatformHandleEvent(&ProgramState);
    }

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);
    SDL_RenderPresent(Renderer);
  }

  return 0;
}
