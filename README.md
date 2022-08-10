# 前言

用 `SDL2` 做音频编程，这里写一个 demo ，该 demo 的目标是播放一个固定的频率的声音，这里我们不借助任何音频文件，而是直接通过写 `buffer` 的方式播放声音。

# 基础的声音知识

声音的传播是通过**介质的震动**，这个介质大多数时候是空气，其本质就是**声波**，通过**波**的频率变化，使人听到了各种各样的声音。

声音有两个重要的属性，一个是频率，即单位时间内震动的次数，其单位为赫兹（Hz）。另一个是振幅，即震动的幅度。从人的感受上，频率影响了声音的音调，振幅影响了声音的大小。

人的耳朵接收的波的频率是有限的，其范围约在 `20Hz` 到 `20000Hz` 之间，最敏感的范围在 `1000Hz` 到 `3000Hz` 之间。

如以下频率对应的音高表：

| 频率（Hz） | 音高 |
| --- | --- |
| 261.63 | 哆 |
| 293.66 | 唻 |
| 329.63 | 咪 |

回到编程的视角，对于计算机来说，只要通过编程命令硬件播放一段频率为`261.63Hz`，并且振幅（音量）合适的声音，那么理所当然的，人的耳朵就能听到音高为“哆”的声音。

# 采样率

虽然描述波相关的时间是个连续的概念，但是计算机的数据确实通过一个个比特位组成的，如何用离散的数据描述波这个连续的概念，这就需要用到“采样率”这个术语。

> 采样频率，也称为采样速度或者采样率，定义了单位时间内从连续信号中提取并组成离散信号的采样个数，它用赫兹（Hz）来表示。

例如，主流音频文件的采样频率为 44100Hz，它的意思是，音频数据会采集一秒内共计 44100 个采样点，对于每个采样点均提供音频的样本数据。

# 位深度

对于采样点的每一个数据是一个二进制数，其二进制位数被称之为位深度。不难得出结论，位深度越大，其精度也越高。但同时也会导致音频数据变大。一般而言 16 位深度已经足够。


# 正弦波与正弦音

正弦波定义如下：

> 正弦波是频率成分最为单一的一种信号，因这种信号的波形是数学上的正弦曲线而得名。任何复杂信号——例如光谱信号，都可以看成由许许多多频率不同、大小不等的正弦波复合而成。 --- 百度百科

正弦音定义如下：

> 正弦音是最纯的音响，它只由一个力度水平均匀的单一频率构成，即只有一个基频，也就是它自己本身，而没有其他泛音。之所以称作“正弦”音，是因为在图表显示中，正弦波波形振动曲线是随三角函数正弦曲线的规律来变化的。其它波形的音，如三角波、方波等，均可以分解为若干正弦音，即可视为一个基频和若干泛音的组合。 -- 网络

# 用正弦函数描述声音

用一个正弦函数描述一个频率固定的声波，可以很简单的用 `sin(t)` 来描述。这里 `t` 代表的是时间。那么很容易可以得出，当 `t=2π` 时，刚好完成了一个正弦周期。

我们规定 `t` 的单位为秒，如果我们希望一秒内完成一个正弦周期，也就是说振动频率刚好是 `1Hz` ，那么正弦函数的参数应该改为 `sin(2πt)` 。

以上基础知识，即 `sin(2πt)` 非常重要，通过以上知识，我们可以进一步得出，如果想要设计一个表达式，使得正弦函数的频率为 `n`，那么显然可以通过 `sin(2πtn)` 获得，这就构成了编写一个固定频率的正弦函数的数学基础。

# 用 `C/C++` 描述给定采样率下的正弦波

以采样率 `44100Hz` ，声音频率为 `261.63Hz` 为例，那么我们以此获得一个一秒的正弦采样数据，可以通过如下代码示例：

```C++
const int SamplingRate = 44100;
const double Lag = 1.0 / SamplingRate; // 每一个采样点的时间间隔
const double Freq = 261.63; // 声音的固定频率
double buf[SamplingRate] = { 0 };
for (int i = 0; i < SamplingRate; ++i) {
    buf[i] = sin(M_PI * 2.0 * i * Lag * Freq);
}
```

以上示例用的采样数据 `buf` 的数组元素是 `double` 类型，其数值范围显然在 `[-1, 1]` 之间，这样该浮点数可以表示震动的频率变化，而不依赖于具体的位深度。

有了以上理论基础，就可以通过 `SDL` 编写音频相关的开发代码了。

# 什么是SDL

> SDL（Simple DirectMedia Layer） 是一套开放源代码的跨平台多媒体开发库，使用 C 语言写成。SDL 提供了数种控制图像、声音、输出入的函数，让开发者只要用相同或是相似的代码就可以开发出跨多个平台（Linux、Windows、Mac OS X等）的应用软件。现 SDL 多用于开发游戏、模拟器、媒体播放器等多媒体应用领域。 --- 百度百科

由于进行音频编程的时候还是希望尽可能跨平台，所以可以考虑用第三方库来编写音频代码，而 `SDL` 就是其中一个不错的选择。

# SDL函数

用 `SDL` 做基础的音频编程，一般需要用到的 `SDL` 函数如下。（详细的 `API` 描述就不写了，可以自行查阅文档。)

| 函数名 | 用途 | 是否必要 |
|---|---| --- |
| SDL_Init | 初始化SDL | 是 |
| SDL_OpenAudio | 打开音频 | 是 |
| SDL_PauseAudio | 暂停音频 | 是 |
| SDL_Delay | 相当于sleep，按毫秒计 | 否 |
| SDL_MixAudio | 音频混合 | 否 |
| SDL_CloseAudio | 关闭音频模块 | 是 |
| SDL_Quit | 退出SDL | 是 |

基本编程思路如下：

1. 通过 `SDL_Init` 初始化 `SDL` 。
2. 通过 `SDL_OpenAudio` 配置和打开音频模块。
3. 实现音频模块的异步回调函数，用于填充固定的音频缓冲区。如需要混音，可通过 `SDL_MixAudio` 完成。
4. 通过 `SDL_PauseAudio` 启动音频。
5. 循环等待。直到满足自己设定的退出条件。
6. 在退出前，通过 `SDL_CloseAudio` 关闭音频模块。
7. 在退出前，通过 `SDL_Quit` 退出 `SDL` 。

# Demo

首先，定义几个宏，全局变量和全局常量以备后用。
```C++
#define FREQ 44100 // 采样率
#define SAMPLES 2048 // 缓冲区大小
static const double SoundFreq = 261.63; // 希望播放的声音频率
static const double TimeLag = 1.0 / FREQ; // 每个采样点的时间间隔
static int g_callbackIndex = 0; // 回调次数统计
```

初始化 `SDL`，由于我们只用到了音频部分，所以初始化参数只添加 `SDL_INIT_AUDIO` 。

```C++
int sdlRetCode = SDL_Init(SDL_INIT_AUDIO);
if (sdlRetCode) {
    printf("SDL init error with code: %d\n", sdlRetCode);
    return -1;
}
printf("SDL init successful.\n");
```

配置和打开音频。这里回调函数 `AudioCallback` 最后来实现。

```C++
// 结构体 SDL_AudioSpec 用于配置音频模块的信息，包括回调函数
SDL_AudioSpec spec {};
spec.freq = FREQ; // 采样率，这里叫freq，即频率，因为采样率也是一种频率，请不要与声音频率混淆
spec.format = AUDIO_S16SYS; // 音频格式，即位深度，这里选择 16 位整形
spec.channels = 1; // 声道个数
spec.silence = 0; // 静音值，选 0 。
spec.samples = SAMPLES; // 这里 samples 的意思是每次采样时的缓冲区大小，以 format 选定的基本数据大小为单位
spec.callback = AudioCallback;
spec.userdata = NULL;

sdlRetCode = SDL_OpenAudio(&spec, NULL);
if (sdlRetCode) {
    printf("SDL Open Audio error with code: %d\n", sdlRetCode);
    return -1;
}
printf("SDL open audio successful.\n");
```

开始音频，以及循环逻辑。

```C++
SDL_PauseAudio(0); // 启动音频
int runtime = 10'000; // 这里设置 10 秒结束循环
while (runtime) {
    SDL_Delay(1); // sleep 1 毫秒
    runtime--;
}
```

关闭音频和 `SDL` 。

```C++
SDL_CloseAudio();
SDL_Quit();
```

最后，实现最重要的异步回调函数。

```C++
static void AudioCallback(void* userdata, Uint8* stream, int len) {
    int16_t* source = (int16_t*)stream; // 将缓冲区以 16 位数据看待
    int count = len / 2; // 由于是 16 位数据，所以缓冲区的数据个数应该是原来的二分之一
    double r = 0.0; // sin 函数的参数
    int startIndex = (g_callbackIndex * count) % (int)(FREQ/SoundFreq*10);
    for (int i = 0; i < count; ++i) {
        r = M_PI * 2.0 * SoundFreq * TimeLag * (startIndex + i);
        source[i] = INT16_MAX * sin(r);
    }
    g_callbackIndex++;
}
```

这是要重点解释的部分，首先解释下 `sin` 函数的参数，即 `r` 的计算。
```C++
r = M_PI * 2.0 * SoundFreq * TimeLag * (startIndex + i);
```
由于 `SoundFreq` 是我们期待的声音频率，那么每一个一秒，`r` 应等于 `r = M_PI * 2.0 * SoundFreq`。但是这个循环计算的并不是每一个一秒，而是每一个采样点，所以应该乘以采样点所占的比例，所以需要再乘以 `TimeLag * (startIndex + i)` 。这样拆分开理解就清晰了。

接下来还有一个需要解释的重点，就是 `startIndex`，`startIndex` 的值不应该直接是 `g_callbackIndex * count` 吗？

理论上确实如此，但是我们必须考虑到一个事实，如果 `startIndex` 不断增长下去，那么最终计算 `r` 的时候会导致数据计算溢出。

而本 demo 的实现是始终播放固定频率的声音，所以我们完全可以通过取模的方式，让 `startIndex` 在某个时候回到 0 。但是怎么取模呢？首先我们考虑采样点的个数是 44100 ，即 `FREQ` ， 而声音的频率为 261.63 ，即 `SoundFreq` ,由此可以得出，在每一个频率周期，将得到 `FREQ/SoundFreq` 个采样点。也就是说 `FREQ/SoundFreq` 个采样点后，频率周期完成一轮回到原点，那么 **理论上** 可以这么写：

```C++
int startIndex = (g_callbackIndex * count) % (int)(FREQ/SoundFreq);
```

然而实际上如果这么写，播放出来的声音会出现明显的撕裂感，这是为什么？因为 `FREQ/SoundFreq` 并不能整除，那么这个取模计算的结果会出现比较大的误差，解决的办法是将误差缩小到人耳朵无法分辨的范围，这里通过乘以 10 来解决。

最终才得出
```C++
r = M_PI * 2.0 * SoundFreq * TimeLag * (startIndex + i);
```

最后，送上完整的代码：

```C++
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
```

# 参考文章

[NES APU](https://blog.csdn.net/ZML086/article/details/125645483)

[音视频开发系列（15）使用SDL播放音频](https://blog.csdn.net/yinshipin007/article/details/124134325)

[建议收藏 | 音频属性相关：声道、采样率、采样位数、样本格式、比特率](https://blog.csdn.net/zhying719/article/details/121154726)

[百度百科 正弦波](https://baike.baidu.com/item/%E6%AD%A3%E5%BC%A6%E6%B3%A2)


附，关于参数配置 silence，具体含义暂不明确，只知道填 0 即可，这里有个 [相关讨论帖] 。(https://stackoverflow.com/questions/4312660/what-precisely-is-the-silence-value-in-the-sdl-audio-api)
