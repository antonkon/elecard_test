/**
 * Функиции для отладки
 */
#include "debug.h"


/**
 * Вывод части картинки на консоль
 */
void prov(uint8_t *frame, int width) {
    int i,j;
    // Проверка правельности пеобразования
    for(i=0;i<5;i++) {
        for(j=0;j<33;j++) {
            printf("%d\t", frame[j+i*width*3]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Вывод картинки в YUV формате в файл
 */
void prov_file(uint8_t *frame, int width, int height) {
    /**
     * Запись в файл
     */
    int i,j;
    FILE *f2 = fopen("3.yuv", "wb");
    if (f2 == NULL) {
        printf("Ошибка создания файла !\n");
        return;
    }
    for(i=0; i<height; i++) {
        for(j=0; j<width*3; j+=3){
            fwrite(&frame[j+i*width*3], 1, 1, f2);
        }
    }
    for(i=0; i<height; i+=2) {
        for(j=1; j<width*3; j+=3*2){
            fwrite(&frame[j+i*width*3], 1, 1, f2);
        }
    }
    for(i=0; i<height; i+=2) {
        for(j=2; j<width*3; j+=3*2){
            fwrite(&frame[j+i*width*3], 1, 1, f2);
        }
    }
    fclose(f2);
    f2 = NULL;
}