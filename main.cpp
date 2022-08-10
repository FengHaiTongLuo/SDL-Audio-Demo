#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>


#define FREQ 44100
#define SAMPLES 2048
static const double SoundFreq = 261.63;
static const double TimeLag = 1.0 / FREQ;
static int g_callbackIndex = 0;

static void AudioCallback(void* userdata, Uint8* stream, int len) {
    int16_t* source = (int16_t*)stream;
    int count = len / 2;
    double r = 0.0;
    int startIndex = (g_callbackIndex * count) % (int)(FREQ/SoundFreq*10);
    for (int i = 0; i < count; ++i) {
        r = M_PI * 2.0 * SoundFreq * TimeLag * (startIndex + i);
        source[i] = INT16_MAX * sin(r);
    }
    g_callbackIndex++;
}

int main(int argc, const char * argv[]) {
    int sdlRetCode = SDL_Init(SDL_INIT_AUDIO);
    if (sdlRetCode) {
        printf("SDL init error with code: %d\n", sdlRetCode);
        return -1;
    }
    printf("SDL init successful.\n");
    
    // 结构体 SDL_AudioSpec 用于配置音频模块的信息，包括回调函数
    SDL_AudioSpec spec {};
    spec.freq = FREQ; // 采样率，这里叫freq，即频率，因为采样率也是一种频率，请不要与声音频率混淆
    spec.format = AUDIO_S16SYS; // 音频格式，即位深度，这里选择 16 位整形
    spec.channels = 1; // 声道个数
    spec.silence = 0; // 静音值，选 0
    spec.samples = SAMPLES; // 这里 samples 的意思是每次采样时的缓冲区大小，以 format 选定的基本数据大小为单位
    spec.callback = AudioCallback;
    spec.userdata = NULL;
    
    sdlRetCode = SDL_OpenAudio(&spec, NULL);
    if (sdlRetCode) {
        printf("SDL Open Audio error with code: %d\n", sdlRetCode);
        return -1;
    }
    printf("SDL open audio successful.\n");
    
    SDL_PauseAudio(0); // 启动音频
    int runtime = 10'000; // 这里设置 10 秒结束循环
    while (runtime) {
        SDL_Delay(1); // sleep 1 毫秒
        runtime--;
    }
    
    SDL_CloseAudio();
    SDL_Quit();
    
    return 0;
}
