#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME_WAV "melody.wav"
#define SAMPLE_RATE 44100
#define FUNC 1 // �g�`�^�C�v�i0: �T�C���g, 1: ���`�g, 2: �O�p�g�j

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
                    for (int k = 0; k < 100; k++) {
                        value += (sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1)) / (2 * k + 1));
                    }
                }
                else if (note->func == 2) {// �O�p�g
                    for (int k = 0; k < 100; k++) {
                        value += (pow(-1, k) * sin(2 * M_PI * note->frequencies[j] * t * (2 * k + 1))) / ((2 * k + 1) * (2 * k + 1));
                    }
                }
                else if (note->func == 3) {// �m�R�M���g
                    for (int k = 1; k < 100; k++) {
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
