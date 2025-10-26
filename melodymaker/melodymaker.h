#pragma once

#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME_WAV "melody.wav"
#define SAMPLE_RATE 44100

typedef struct {
    int func; // �g�`�^�C�v�i0: �T�C���g, 1: ���`�g, 2: �O�p�g�j
    double frequencies[3]; // ���g���iHz�j
    double duration;       // �����i�b�j
} Note;

// �a���𐶐�
short* generateChord(Note* note, int* outSampleCount) {
    int sampleCount = (int)(SAMPLE_RATE * note->duration);
    short* samples = (short*)malloc(sizeof(short) * sampleCount);
    if (!samples) return NULL;

    for (int i = 0; i < sampleCount; ++i) {
        double t = (double)i / SAMPLE_RATE;
        double value = 0.0;
        for (int j = 0; j < 3; ++j) {
            if (note->frequencies[j] > 0.0) {
                if (note->func == 1) { // ���`�g
                    for (int k = 0; k < 300; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1)) / (2 * k + 1));
                    }
                }
                else if (note->func == 2) {// �O�p�g
                    for (int k = 0; k < 100; k++) {
                        value += (pow(-1, k) * sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1))) / ((2 * k + 1) * (2 * k + 1));
                    }
                }
                else if (note->func == 3) {// �m�R�M���g
                    for (int k = 1; k < 500; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * k)) / k;
                    }
                }
                else if (note->func == 4) {// �z���C�g�m�C�Y
                    value += ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                }
                else { // �T�C���g
                    value += sin(2 * M_PI * note->frequencies[j] * t);
                }
            }
        }
        value /= 3.0; // ���ω�
        samples[i] = (short)(value * 32767);
    }

    *outSampleCount = sampleCount;
    return samples;
}

// �x������
void applyDelay(short* samples, int sampleCount, double delaySeconds, double decay) {
    int delaySamples = (int)(SAMPLE_RATE * delaySeconds);
    // delaySeconds: �x�����ԁi��F0.2�b�j
    // decay : �x�����̌������i��F0.5�j
    for (int i = delaySamples; i < sampleCount; ++i) {
        int delayed = (int)(samples[i - delaySamples] * decay);
        int mixed = samples[i] + delayed;

        // �N���b�s���O����
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        samples[i] = (short)mixed;
    }
}

// ��������
void applyDecay(short* samples, int sampleCount, double decayRate) {
    // decayRate: �������i��F3.0�j
    for (int i = 0; i < sampleCount; ++i) {
        double factor = exp(-decayRate * i / SAMPLE_RATE); // �w������
        int attenuated = (int)(samples[i] * factor);

        // �N���b�s���O����
        if (attenuated > 32767) attenuated = 32767;
        if (attenuated < -32768) attenuated = -32768;

        samples[i] = (short)attenuated;
    }
}

// �G�R�[���ʂ�K�p
void applyEcho(short* samples, int sampleCount, double delaySeconds, double decay) {
    // delaySeconds: �G�R�[�̒x�����ԁi��F0.3�b�j
    // decay: �G�R�[���̌������i��F0.4�j
    int delaySamples = (int)(SAMPLE_RATE * delaySeconds);
    for (int i = delaySamples; i < sampleCount; ++i) {
        int echo = (int)(samples[i - delaySamples] * decay);
        int mixed = samples[i] + echo;

        // �N���b�s���O����
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        samples[i] = (short)mixed;
    }
}

// WAV�w�b�_�[���������ފ֐�
void writeWavHeader(FILE* file, int dataSize) {
    int sampleRate = SAMPLE_RATE;
    short bitsPerSample = 16;
    short channels = 1;
    int byteRate = sampleRate * channels * bitsPerSample / 8;
    short blockAlign = channels * bitsPerSample / 8;

    // RIFF�w�b�_�[
    fwrite("RIFF", 1, 4, file);
    int chunkSize = 36 + dataSize;
    fwrite(&chunkSize, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    // fmt�`�����N
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

    // data�`�����N
    fwrite("data", 1, 4, file);
    fwrite(&dataSize, 4, 1, file);
}