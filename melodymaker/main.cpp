#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "melodymaker.h"

#define FUNC 3 // �g�`�^�C�v�i0: �T�C���g, 1: ���`�g, 2: �O�p�g, 3: �m�R�M���g, 4: �z���C�g�m�C�Y�j

int main() {
    Note melody[] = {
        {0, {261.63, 0, 0}, 1}, // �h�iC4�j
        {0, {293.66, 0, 0}, 1}, // ���iD4�j
        {0, {329.63, 0, 0}, 1}, // �~�iE4�j
        {0, {261.63, 329.63, 392.00}, 2.0} // C���W���[�R�[�h
    };

    Note melody2[] = {
        // �t���[�YA�iC�R�[�h�j
        {FUNC, {261.63, 329.63, 392.00}, 0.25}, // C4-E4-G4
        {FUNC, {261.63, 0, 0}, 0.25},           // C4
        {FUNC, {329.63, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4

        // �t���[�YB�iG�R�[�h�j
        {FUNC, {196.00, 246.94, 392.00}, 0.25}, // G3-B3-G4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0}, 0.25},           // A4
        {FUNC, {493.88, 0, 0}, 0.25},           // B4

        // �t���[�YC�iAm�R�[�h�j
        {FUNC, {220.00, 261.63, 329.63}, 0.25}, // A3-C4-E4
        {FUNC, {329.63, 0, 0}, 0.25},           // E4
        {FUNC, {392.00, 0, 0}, 0.25},           // G4
        {FUNC, {440.00, 0, 0}, 0.25},           // A4

        // �t���[�YD�iF�R�[�h�j
        {FUNC, {174.61, 220.00, 261.63}, 0.25}, // F3-A3-C4
        {FUNC, {261.63, 0, 0}, 0.25},           // C4
        {FUNC, {293.66, 0, 0}, 0.25},           // D4
        {FUNC, {329.63, 0, 0}, 0.25*5}            // E4
    };

    Note metalRiff[] = {
        {FUNC, {82.41, 110.00, 164.81}, 0.3},  // E2 + A2 + E3�iE�p���[�R�[�h�j
        {FUNC, {92.50, 123.47, 185.00}, 0.3},  // F#2 + B2 + F#3
        {FUNC, {98.00, 130.81, 196.00}, 0.3},  // G2 + C3 + G3
        {FUNC, {82.41, 110.00, 164.81}, 0.3},  // E2�p���[�R�[�h�i�J��Ԃ��j
        {FUNC, {98.00, 130.81, 196.00}, 0.3},  // G2�p���[�R�[�h
        {FUNC, {110.00, 146.83, 220.00}, 0.3}, // A2�p���[�R�[�h
        {FUNC, {123.47, 164.81, 246.94}, 0.3}, // B2�p���[�R�[�h
        {FUNC, {130.81, 174.61, 261.63}, 0.3},  // C3�p���[�R�[�h
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
        {FUNC, {659.25, 0, 0}, 0.3}   // E5�i���߁j
    };

    short* allSamples = NULL;
    int totalSamples = 0;

    int size = sizeof(melody) / sizeof(Note);
    for (int i = 0; i < size; ++i) {
        int sampleCount = 0;
        short* samples = generateChord(&melody[i], &sampleCount);
        applyDecay(samples, sampleCount, 0.8);     // �Z���s������
        applyDelay(samples, sampleCount, 0.1, 0.4); // �^�C�g�ȃf�B���C
        applyEcho(samples, sampleCount, 0.2, 0.3);  // ��ԓI�ȍL����

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
        applyDecay(samples, sampleCount, 1.2);     // ��������
        applyDelay(samples, sampleCount, 0.05, 0.3); // �y�߂̃f�B���C
        applyEcho(samples, sampleCount, 0.1, 0.2);   // �T���߂ȃG�R�[

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
        applyDecay(samples, sampleCount, 0.8);     // �Z���s������
        applyDelay(samples, sampleCount, 0.1, 0.4); // �^�C�g�ȃf�B���C
        applyEcho(samples, sampleCount, 0.2, 0.3);  // ��ԓI�ȍL����

        if (!samples) continue;

        allSamples = (short*)realloc(allSamples, sizeof(short) * (totalSamples + sampleCount));
        memcpy(allSamples + totalSamples, samples, sizeof(short) * sampleCount);
        totalSamples += sampleCount;
        free(samples);
    }

    FILE* outFile = fopen(FILENAME_WAV, "wb");
    if (!outFile) {
        perror("�t�@�C�����J���܂���ł���");
        free(allSamples);
        return 1;
    }

    int dataSize = totalSamples * sizeof(short);
    writeWavHeader(outFile, dataSize);
    fwrite(allSamples, sizeof(short), totalSamples, outFile);
    fclose(outFile);
    free(allSamples);

    printf("%s�𐶐����܂����I\n", FILENAME_WAV);
    system(FILENAME_WAV);
    return 0;
}
