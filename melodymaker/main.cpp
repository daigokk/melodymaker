#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME_WAV "melody.wav"
#define SAMPLE_RATE 44100

typedef struct {
    double frequencies[3]; // 周波数（Hz）
    double duration;       // 長さ（秒）
} Note;

// 和音を生成
short* generateChord(double frequencies[3], double duration, int* outSampleCount) {
    int sampleCount = (int)(SAMPLE_RATE * duration);
    short* samples = (short*)malloc(sizeof(short) * sampleCount);
    if (!samples) return NULL;

    for (int i = 0; i < sampleCount; ++i) {
        double t = (double)i / SAMPLE_RATE;
        double value = 0.0;
        for (int j = 0; j < 3; ++j) {
            if (frequencies[j] > 0.0) {
                value += sin(2 * M_PI * frequencies[j] * t);
            }
        }
        value /= 3.0; // 平均化
        samples[i] = (short)(value * 32767);
    }

    *outSampleCount = sampleCount;
    return samples;
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

int main() {
    Note melody[] = {
        {{261.63, 0, 0}, 0.5}, // ド（C4）
        {{293.66, 0, 0}, 0.5}, // レ（D4）
        {{329.63, 0, 0}, 0.5}, // ミ（E4）
        {{261.63, 329.63, 392.00}, 2.0} // Cメジャーコード
    };
    int melodySize = sizeof(melody) / sizeof(Note);

    short* allSamples = NULL;
    int totalSamples = 0;

    for (int i = 0; i < melodySize; ++i) {
        int sampleCount = 0;
        short* samples = generateChord(melody[i].frequencies, melody[i].duration, &sampleCount);
        if (!samples) continue;

        allSamples = (short*)realloc(allSamples, sizeof(short) * (totalSamples + sampleCount));
        memcpy(allSamples + totalSamples, samples, sizeof(short) * sampleCount);
        totalSamples += sampleCount;
        free(samples);
    }

    FILE* outFile = fopen(FILENAME_WAV, "wb");
    if (!outFile) {
        perror("ファイルを開けませんでした");
        free(allSamples);
        return 1;
    }

    int dataSize = totalSamples * sizeof(short);
    writeWavHeader(outFile, dataSize);
    fwrite(allSamples, sizeof(short), totalSamples, outFile);
    fclose(outFile);
    free(allSamples);

    printf("%sを生成しました！\n", FILENAME_WAV);
    system(FILENAME_WAV);
    return 0;
}
