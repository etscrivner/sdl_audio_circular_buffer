#include <SDL.h>

#define internal static

///////////////////////////////////////////////////////////////////////////////

struct platform_program_state
{
  bool IsRunning;
  SDL_Event LastEvent;
};

struct platform_audio_buffer
{
  Uint8* Buffer;
  int BufferSize;
  int SamplesPerSecond;
  int BytesPerSample;
  int ReadCursor;
  int WriteCursor;
  SDL_AudioDeviceID DeviceID;
};

///////////////////////////////////////////////////////////////////////////////

internal void
PlatformFillAudioBuffer(void*, Uint8* SystemBuffer, int Length)
{
  memset(SystemBuffer, 0, Length);
}

///////////////////////////////////////////////////////////////////////////////

internal void
PlatformInitializeAudio(platform_audio_buffer* AudioBuffer)
{
  SDL_AudioSpec AudioSettings = {};
  AudioSettings.freq = AudioBuffer->SamplesPerSecond;
  AudioSettings.format = AUDIO_S16LSB;
  AudioSettings.channels = 2;
  AudioSettings.samples = 4096;
  AudioSettings.callback = &PlatformFillAudioBuffer;
  AudioSettings.userdata = AudioBuffer;

  SDL_AudioSpec Obtained = {};
  AudioBuffer->DeviceID = SDL_OpenAudioDevice(
    NULL, 0, &AudioSettings, &Obtained, 0
  );

  if (AudioSettings.format != Obtained.format)
  {
    SDL_Log("Unable to obtain expected audio settings: %s", SDL_GetError());
    exit(1);
  }

  // Start playing the audio buffer
  SDL_PauseAudioDevice(AudioBuffer->DeviceID, 0);
}

///////////////////////////////////////////////////////////////////////////////

internal void
PlatformHandleEvent(platform_program_state* ProgramState)
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

  platform_audio_buffer AudioBuffer = {};
  AudioBuffer.SamplesPerSecond = 44800;
  // Two data points per sample. One for the left speaker, one for the right.
  AudioBuffer.BytesPerSample = 2 * sizeof(Sint16);
  AudioBuffer.ReadCursor = 0;
  // NOTE: Offset by 1 sample in order to cause the circular buffer to
  // initially be filled.
  AudioBuffer.WriteCursor = AudioBuffer.BytesPerSample;
  AudioBuffer.BufferSize =
    AudioBuffer.SamplesPerSecond * AudioBuffer.BytesPerSample;
  AudioBuffer.Buffer = new Uint8[AudioBuffer.BufferSize];

  PlatformInitializeAudio(&AudioBuffer);

  platform_program_state ProgramState = {};
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

  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_CloseAudioDevice(AudioBuffer.DeviceID);
  SDL_Quit();

  delete[] AudioBuffer.Buffer;
  return 0;
}
