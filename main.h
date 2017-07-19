
#ifndef TS1_1_MAIN_H
#define TS1_1_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/**
 * Функция преобразования из rgb в yuv цветовое пространство
 */
typedef struct {
    uint8_t y;
    uint8_t u;
    uint8_t v;
} yuv;

/**
 * Структура заголовка bmp файла
 */
typedef struct {
    uint16_t bfType;
    int64_t  byte16;
    int32_t  biWidth;
    int32_t  biHeight;
    int64_t  byte32;
} BMPheader;

#endif //TS1_1_MAIN_H
