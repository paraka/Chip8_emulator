#ifndef _BEEP_H_
#define _BEEP_H_

#include <SDL/SDL_audio.h>
#include <cmath>

namespace
{
    void audio_callback(void *beep, uint8_t *stream, int lenght);
}

class Beep
{
public:
    Beep(): pos(0.0), inc(2 * M_PI * 1000 / FREQUENCY)
    {
        SDL_AudioSpec desiredSpec;

        desiredSpec.freq = FREQUENCY;
        desiredSpec.format = AUDIO_U8;
        desiredSpec.channels = CHANNELS;
        desiredSpec.samples = SAMPLES;
        desiredSpec.callback = audio_callback;
        desiredSpec.userdata = this;

        SDL_AudioSpec obtainedSpec;
        SDL_OpenAudio(&desiredSpec, &obtainedSpec);
    }

    ~Beep()
    {
        SDL_CloseAudio();
    }

    void StartBeep()
    {
        SDL_PauseAudio(0);
    }

    void StopBeep()
    {
        SDL_PauseAudio(1);
    }

public:
    double pos;
    double inc;

private:
    static const int AMPLITUDE = 2;
    static const int FREQUENCY = 44100;
    static const int CHANNELS = 1;
    static const int SAMPLES = 2048;
};

namespace
{
    void audio_callback(void *beep, uint8_t *stream, int lenght)
    {
        Beep *beeper = (Beep*) beep;
        if (!beeper) return;
        for (auto i=0; i<lenght;++i)
        {
            stream[i] = std::sin(beeper->pos) + 127;
            beeper->pos += beeper->inc;
        }
    }
}

#endif //_BEEP_H_
