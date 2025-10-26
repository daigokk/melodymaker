#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME_WAV "melody.wav"
#define SAMPLE_RATE 44100
#define FUNC 1 // 波形タイプ（0: サイン波, 1: 方形波, 2: 三角波）

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
                    for (int k = 0; k < 100; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1)) / (2 * k + 1));
                    }
                }
                else if (note->func == 2) {// 三角波
                    for (int k = 0; k < 100; k++) {
                        value += (pow(-1, k) * sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1))) / ((2 * k + 1) * (2 * k + 1));
                    }
                }
                else if (note->func == 3) {// ノコギリ波
                    for (int k = 1; k < 100; k++) {
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

int main() {
    Note melody[] = {
        {0, {261.63, 0, 0}, 1}, // ド（C4）
        {0, {293.66, 0, 0}, 1}, // レ（D4）
        {0, {329.63, 0, 0}, 1}, // ミ（E4）
        {0, {261.63, 329.63, 392.00}, 2.0} // Cメジャーコード
    };

    Note melody2[] = {
        // フレーズA（Cコード）
        {FUNC, {261.63, 329.63, 392.00}, 0.25}, // C4-E4-G4
        {FUNC, {261.63, 0, 0}, 0.25},           // C4
        {FUNC, {329.63, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4

        // フレーズB（Gコード）
        {FUNC, {196.00, 246.94, 392.00}, 0.25}, // G3-B3-G4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0}, 0.25},           // A4
        {FUNC, {493.88, 0, 0}, 0.25},           // B4

        // フレーズC（Amコード）
        {FUNC, {220.00, 261.63, 329.63}, 0.25}, // A3-C4-E4
        {FUNC, {329.63, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0}, 0.25},           // A4

        // フレーズD（Fコード）
        {FUNC, {174.61, 220.00, 261.63}, 0.25}, // F3-A3-C4
        {FUNC, {261.63, 0, 0}, 0.25},           // C4
        {FUNC, {293.66, 0, 0}, 0.25},           // D4
        {FUNC, {329.63, 0, 0}, 0.25*5}            // E4
    };

    Note metalRiff[] = {
        {FUNC, {82.41, 110.00, 164.81}, 0.3},  // E2 + A2 + E3（Eパワーコード）
        {FUNC, {92.50, 123.47, 185.00}, 0.3},  // F#2 + B2 + F#3
        {FUNC, {98.00, 130.81, 196.00}, 0.3},  // G2 + C3 + G3
        {FUNC, {82.41, 110.00, 164.81}, 0.3},  // E2パワーコード（繰り返し）
        {FUNC, {98.00, 130.81, 196.00}, 0.3},  // G2パワーコード
        {FUNC, {110.00, 146.83, 220.00}, 0.3}, // A2パワーコード
        {FUNC, {123.47, 164.81, 246.94}, 0.3}, // B2パワーコード
        {FUNC, {130.81, 174.61, 261.63}, 0.3},  // C3パワーコード
        { FUNC, {659.25, 0, 0}, 0.15 }, // E5
        {FUNC, {698.46, 0, 0}, 0.15}, // F5
        {FUNC, {783.99, 0, 0}, 0.15}, // G5
        {FUNC, {880.00, 0, 0}, 0.15}, // A5
        {FUNC, {987.77, 0, 0}, 0.15}, // B5
        {FUNC, {1046.50, 0, 0}, 0.15}, // C6
        {FUNC, {987.77, 0, 0}, 0.15}, // B5
        {FUNC, {880.00, 0, 0}, 0.15}, // A5
        {FUNC, {783.99, 0, 0}, 0.15}, // G5
        {FUNC, {698.46, 0, 0}, 0.15}, // F5
        {FUNC, {659.25, 0, 0}, 0.3}   // E5（締め）
    };

    short* allSamples = NULL;
    int totalSamples = 0;

    int size = sizeof(melody) / sizeof(Note);
    for (int i = 0; i < size; ++i) {
        int sampleCount = 0;
        short* samples = generateChord(&melody[i], &sampleCount);
        applyDecay(samples, sampleCount, 0.8);     // 短く鋭い減衰
        applyDelay(samples, sampleCount, 0.1, 0.4); // タイトなディレイ
        applyEcho(samples, sampleCount, 0.2, 0.3);  // 空間的な広がり

        if (!samples) continue;

        allSamples = (short*)realloc(allSamples, sizeof(short) * (totalSamples + sampleCount));
        memcpy(allSamples + totalSamples, samples, sizeof(short) * sampleCount);
        totalSamples += sampleCount;
        free(samples);
    }

    size = sizeof(melody2) / sizeof(Note);
    for (int i = 0; i < size; ++i) {
        int sampleCount = 0;
        short* samples = generateChord(&melody2[i], &sampleCount);
        applyDecay(samples, sampleCount, 1.2);     // 速く減衰
        applyDelay(samples, sampleCount, 0.05, 0.3); // 軽めのディレイ
        applyEcho(samples, sampleCount, 0.1, 0.2);   // 控えめなエコー

        if (!samples) continue;

        allSamples = (short*)realloc(allSamples, sizeof(short) * (totalSamples + sampleCount));
        memcpy(allSamples + totalSamples, samples, sizeof(short) * sampleCount);
        totalSamples += sampleCount;
        free(samples);
    }

    size = sizeof(metalRiff) / sizeof(Note);
    for (int i = 0; i < size; ++i) {
        int sampleCount = 0;
        short* samples = generateChord(&metalRiff[i], &sampleCount);
        applyDecay(samples, sampleCount, 0.8);     // 短く鋭い減衰
        applyDelay(samples, sampleCount, 0.1, 0.4); // タイトなディレイ
        applyEcho(samples, sampleCount, 0.2, 0.3);  // 空間的な広がり

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
