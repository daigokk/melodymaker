#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "melodymaker.h"

#define FUNC 2 // 波形タイプ（0: サイン波, 1: 方形波, 2: 三角波, 3: ノコギリ波, 4: ホワイトノイズ）

int main() {
    Note melody[] = {
        {0, {261.63, 0, 0, 0, 0, 0}, 1}, // ド（C4）
        {0, {293.66, 0, 0, 0, 0, 0}, 1}, // レ（D4）
        {0, {329.63, 0, 0, 0, 0, 0}, 1}, // ミ（E4）
        {0, {261.63, 329.63, 392.00, 0, 0, 0}, 2.0} // Cメジャーコード
    };

    Note melody2[] = {
        // フレーズA（Cコード）
        {FUNC, {261.63, 329.63, 392.00, 0, 0, 0}, 0.25}, // C4-E4-G4
        {FUNC, {261.63, 0, 0, 0, 0, 0}, 0.25},           // C4
        {FUNC, {329.63, 0, 0, 0, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0, 0, 0, 0}, 0.25},           // G4

        // フレーズB（Gコード）
        {FUNC, {196.00, 246.94, 392.00, 0, 0, 0}, 0.25}, // G3-B3-G4
        {FUNC, {392.00, 0, 0, 0, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0, 0, 0, 0}, 0.25},           // A4
        {FUNC, {493.88, 0, 0, 0, 0, 0}, 0.25},           // B4

        // フレーズC（Amコード）
        {FUNC, {220.00, 261.63, 329.63, 0, 0, 0}, 0.25}, // A3-C4-E4
        {FUNC, {329.63, 0, 0, 0, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0, 0, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0, 0, 0, 0}, 0.25},           // A4

        // フレーズD（Fコード）
        {FUNC, {174.61, 220.00, 261.63, 0, 0, 0}, 0.25}, // F3-A3-C4
        {FUNC, {261.63, 0, 0, 0, 0, 0}, 0.25},           // C4
        {FUNC, {293.66, 0, 0, 0, 0, 0}, 0.25},           // D4
        {FUNC, {329.63, 0, 0, 0, 0, 0}, 0.25*5}            // E4
    };

    Note metalRiff[] = {
        {FUNC, {82.41, 110.00, 164.81, 0, 0, 0}, 0.3},  // E2 + A2 + E3（Eパワーコード）
        {FUNC, {92.50, 123.47, 185.00, 0, 0, 0}, 0.3},  // F#2 + B2 + F#3
        {FUNC, {98.00, 130.81, 196.00, 0, 0, 0}, 0.3},  // G2 + C3 + G3
        {FUNC, {82.41, 110.00, 164.81, 0, 0, 0}, 0.3},  // E2パワーコード（繰り返し）
        {FUNC, {98.00, 130.81, 196.00, 0, 0, 0}, 0.3},  // G2パワーコード
        {FUNC, {110.00, 146.83, 220.00, 0, 0, 0}, 0.3}, // A2パワーコード
        {FUNC, {123.47, 164.81, 246.94, 0, 0, 0}, 0.3}, // B2パワーコード
        {FUNC, {130.81, 174.61, 261.63, 0, 0, 0}, 0.3},  // C3パワーコード
        { FUNC, {659.25, 0, 0, 0, 0, 0}, 0.15 }, // E5
        {FUNC, {698.46, 0, 0, 0, 0, 0}, 0.15}, // F5
        {FUNC, {783.99, 0, 0, 0, 0, 0}, 0.15}, // G5
        {FUNC, {880.00, 0, 0, 0, 0, 0}, 0.15}, // A5
        {FUNC, {987.77, 0, 0, 0, 0, 0}, 0.15}, // B5
        {FUNC, {1046.50, 0, 0, 0, 0, 0}, 0.15}, // C6
        {FUNC, {987.77, 0, 0, 0, 0, 0}, 0.15}, // B5
        {FUNC, {880.00, 0, 0, 0, 0, 0}, 0.15}, // A5
        {FUNC, {783.99, 0, 0, 0, 0, 0}, 0.15}, // G5
        {FUNC, {698.46, 0, 0, 0, 0, 0}, 0.15}, // F5
        {FUNC, {659.25, 0, 0, 0, 0, 0}, 0.3}   // E5（締め）
    };

    short* allSamples = NULL;
    int totalSamples = 0;

    Note* song[] = { melody, melody2, metalRiff };
    int songSizes[] = {
        sizeof(melody) / sizeof(Note),
        sizeof(melody2) / sizeof(Note),
        sizeof(metalRiff) / sizeof(Note)
    };

    for (int s = 0; s < 3; ++s) {
        for (int i = 0; i < songSizes[s]; ++i) {
            int sampleCount = 0;
            short* samples = generateChord(&song[s][i], &sampleCount);
            applyDecay(samples, sampleCount, 1.0);
            applyDelay(samples, sampleCount, 0.08, 0.35);
            applyEcho(samples, sampleCount, 0.15, 0.25);

            if (!samples) continue;
            allSamples = (short*)realloc(allSamples, sizeof(short) * (totalSamples + sampleCount));
            memcpy(allSamples + totalSamples, samples, sizeof(short) * sampleCount);
            totalSamples += sampleCount;
            free(samples);
        }
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
