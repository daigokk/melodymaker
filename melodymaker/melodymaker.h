#pragma once

#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME_WAV "melody.wav"
#define SAMPLE_RATE 44100

typedef struct {
    int func; // 波形タイプ（0: サイン波, 1: 方形波, 2: 三角波）
    double frequencies[3]; // 周波数（Hz）
    double duration;       // 長さ（秒）
} Note;

// 和音を生成
short* generateChord(Note* note, int* outSampleCount) {
    int sampleCount = (int)(SAMPLE_RATE * note->duration);
    short* samples = (short*)malloc(sizeof(short) * sampleCount);
    if (!samples) return NULL;

    for (int i = 0; i < sampleCount; ++i) {
        double t = (double)i / SAMPLE_RATE;
        double value = 0.0;
        for (int j = 0; j < 3; ++j) {
            if (note->frequencies[j] > 0.0) {
                if (note->func == 1) { // 方形波
                    for (int k = 0; k < 300; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1)) / (2 * k + 1));
                    }
                }
                else if (note->func == 2) {// 三角波
                    for (int k = 0; k < 100; k++) {
                        value += (pow(-1, k) * sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1))) / ((2 * k + 1) * (2 * k + 1));
                    }
                }
                else if (note->func == 3) {// ノコギリ波
                    for (int k = 1; k < 500; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * k)) / k;
                    }
                }
                else if (note->func == 4) {// ホワイトノイズ
                    value += ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                }
                else { // サイン波
                    value += sin(2 * M_PI * note->frequencies[j] * t);
                }
            }
        }
        value /= 3.0; // 平均化
        samples[i] = (short)(value * 32767);
    }

    *outSampleCount = sampleCount;
    return samples;
}

// 遅延効果
void applyDelay(short* samples, int sampleCount, double delaySeconds, double decay) {
    int delaySamples = (int)(SAMPLE_RATE * delaySeconds);
    // delaySeconds: 遅延時間（例：0.2秒）
    // decay : 遅延音の減衰率（例：0.5）
    for (int i = delaySamples; i < sampleCount; ++i) {
        int delayed = (int)(samples[i - delaySamples] * decay);
        int mixed = samples[i] + delayed;

        // クリッピング処理
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        samples[i] = (short)mixed;
    }
}

// 減衰効果
void applyDecay(short* samples, int sampleCount, double decayRate) {
    // decayRate: 減衰率（例：3.0）
    for (int i = 0; i < sampleCount; ++i) {
        double factor = exp(-decayRate * i / SAMPLE_RATE); // 指数減衰
        int attenuated = (int)(samples[i] * factor);

        // クリッピング処理
        if (attenuated > 32767) attenuated = 32767;
        if (attenuated < -32768) attenuated = -32768;

        samples[i] = (short)attenuated;
    }
}

// エコー効果を適用
void applyEcho(short* samples, int sampleCount, double delaySeconds, double decay) {
    // delaySeconds: エコーの遅延時間（例：0.3秒）
    // decay: エコー音の減衰率（例：0.4）
    int delaySamples = (int)(SAMPLE_RATE * delaySeconds);
    for (int i = delaySamples; i < sampleCount; ++i) {
        int echo = (int)(samples[i - delaySamples] * decay);
        int mixed = samples[i] + echo;

        // クリッピング処理
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        samples[i] = (short)mixed;
    }
}

// WAVヘッダーを書き込む関数
void writeWavHeader(FILE* file, int dataSize) {
    int sampleRate = SAMPLE_RATE;
    short bitsPerSample = 16;
    short channels = 1;
    int byteRate = sampleRate * channels * bitsPerSample / 8;
    short blockAlign = channels * bitsPerSample / 8;

    // RIFFヘッダー
    fwrite("RIFF", 1, 4, file);
    int chunkSize = 36 + dataSize;
    fwrite(&chunkSize, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    // fmtチャンク
    fwrite("fmt ", 1, 4, file);
    int subchunk1Size = 16;
    short audioFormat = 1;
    fwrite(&subchunk1Size, 4, 1, file);
    fwrite(&audioFormat, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sampleRate, 4, 1, file);
    fwrite(&byteRate, 4, 1, file);
    fwrite(&blockAlign, 2, 1, file);
    fwrite(&bitsPerSample, 2, 1, file);

    // dataチャンク
    fwrite("data", 1, 4, file);
    fwrite(&dataSize, 4, 1, file);
}