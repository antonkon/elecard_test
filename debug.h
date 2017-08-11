/**
 * Функиции для отладки
 */

#ifndef TS1_1_DEBUG_H
#define TS1_1_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * Вывод части картинки на консоль
 *
 * @param frame
 * @param width
 */
void prov(uint8_t *frame, int width);

/**
 * Вывод картинки в YUV формате в файл
 *
 * @param frame
 * @param width
 * @param height
 */
void prov_file(uint8_t *frame, int width, int height);

/**
 *
 * @param frame
 * @param width
 * @param height
 */
void prov_file2(uint8_t *frame, int64_t width, int64_t height);

/**
 *
 * @param frame
 * @param width
 * @param height
 */
void prov2(uint8_t *frame, int width, int height);

#endif //TS1_1_DEBUG_H
