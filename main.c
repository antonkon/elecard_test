#include "main.h"
#include "debug.h"

/**
 * Функция перечёта rgb в yuv
 *
 * @param r
 * @param g
 * @param b
 * @param pix
 */
void png_in_yuv_pixel(uint8_t r, uint8_t g, uint8_t b, yuv *pix) {
    pix->y = 0.299 * r + 0.587 * g + 0.114 * b;
    pix->u = -0.169 * r - 0.331 * g + 0.500 * b;
    pix->v = 0.500 * r - 0.419 * g - 0.081 * b;
    return;
}

/**
 * Функция преобразования RGB в YUV формат
 *
 * @param img_rgb
 * @param img_yuv
 * @param width
 * @param height
 */
void rgb_in_yuv(uint8_t *img_rgb, uint8_t *img_yuv, int32_t width, int32_t height) {
    int64_t i,j;
    yuv pix;

    /**
     * Преобразование картинки из RGB в YUV
     */
    for(i=0; i<height; i++) {
        for(j=0; j<width*3; j+=3){
            png_in_yuv_pixel(img_rgb[j+i*width*3], img_rgb[j+i*width*3+1], img_rgb[j+i*width*3+2], &pix);
            img_yuv[j+i*width*3] = pix.y;
            img_yuv[j+i*width*3+1] = pix.u;
            img_yuv[j+i*width*3+2] = pix.v;
        }
    }

    FILE *f2 = fopen("3.yuv", "wb");
    if (f2 == NULL) {
        printf("Ошибка создания файла !\n");
        return;
    }
    for(i=0; i<height; i++) {
        for(j=0; j<width*3; j+=3){
            fwrite(&img_yuv[j+i*width*3], 1, 1, f2);
        }
    }
    for(i=0; i<height; i+=2) {
        for(j=1; j<width*3; j+=3*2){
            fwrite(&img_yuv[j+i*width*3], 1, 1, f2);
        }
    }
    for(i=0; i<height; i+=2) {
        for(j=2; j<width*3; j+=3*2){
            fwrite(&img_yuv[j+i*width*3], 1, 1, f2);
        }
    }
    fclose(f2);
    f2 = NULL;

    return;
}

/**
 * Функция загрузки картинки формата bmp в память
 *
 * @param img_rgb
 * @param pic_name
 * @param width
 * @param height
 * @return 0
 */
int8_t load_bmp(uint8_t *img_rgb, char *pic_name, int32_t width, int32_t height) {
    /* Открывает файл */
    FILE *f;
    f = fopen(pic_name, "rb");
    if (!f) {
        printf("Ошибка при открытия файла !\n");
        return 1;
    }

    /* Считываем заголовок файла */
    BMPheader bmPheader;
    fread(&bmPheader.bfType,1, 2, f);
    fread(&bmPheader.byte16,1, 8, f);
    fread(&bmPheader.byte16,1, 8, f);
    fread(&bmPheader.biWidth,1, 4, f);
    fread(&bmPheader.biHeight,1, 4, f);
    fread(&bmPheader.byte32,1, 8, f);
    fread(&bmPheader.byte32,1, 8, f);
    fread(&bmPheader.byte32,1, 8, f);
    fread(&bmPheader.byte32,1, 8, f);

    /* Проверяем сигнатуру файла */
    if( bmPheader.bfType!=0x4d42 && bmPheader.bfType!=0x4349 && bmPheader.bfType!=0x5450 ) {
        printf("Файл не bmp формата !\n");
        fclose(f);
        return 1;
    }

    /* Проверяем соответствует ли размер картинки размеру видео */
    if ((bmPheader.biWidth != width) || (bmPheader.biHeight != height)) {
        printf("Размер картинки не соответствует размеру видео !\n");
        fclose(f);
        return 1;
    }

    /* Считывает (пропускаем) таблицу цветов */
    uint8_t *color_header = (uint8_t *)malloc(66);
    if (!color_header) {
        printf("Ошибка при выделении памяти !\n");
        fclose(f);
        f = NULL;
        return 1;
    }
    fread(color_header, 1, 66,f);

    /* После всех действий проверим зиписалось ли что нибудь */
    *img_rgb = '\0';

    /* Считываем картинку, преобразовываем в rgb и переворачиваем */
    int64_t i,j;
    uint8_t *tmp_rbg = (uint8_t *)malloc(sizeof(uint8_t)*3);
    if (!tmp_rbg) {
        printf("Ошибка при выделении памяти !\n");
        return 1;
    }

    for (i=bmPheader.biHeight-1; i >= 0; i--)
        for (j=0; j < bmPheader.biWidth*3; j+=3) {
            fread(tmp_rbg, 1, 3, f);
            img_rgb[j+bmPheader.biWidth*i*3] = tmp_rbg[0];
            img_rgb[(j + 1)+bmPheader.biWidth*i*3] = tmp_rbg[2];
            img_rgb[(j + 2)+bmPheader.biWidth*i*3] = tmp_rbg[1];
        }

    free(tmp_rbg);
    tmp_rbg = NULL;
    fclose(f);
    f = NULL;
    free(color_header);
    color_header = NULL;

    /* Проверяем на запись */
    if (*img_rgb == '\0') {
        printf("Ошибка записи !\n");
        return 1;
    }

    return 0;
}

/**
 * Вставка картинки в видео
 *
 * @param input_name_steam
 * @param output_name_stream
 * @param img_yuv
 * @param width
 * @param height
 */
int8_t img_insert_video(uint8_t *img_yuv, uint8_t *img_rgb, char *input_name_steam, char *output_name_stream, int32_t width, int32_t height) {
    /**
     * Открытие входного и создание выходного потока
     */
    FILE *input_stream;
    FILE *output_stream;
    input_stream = fopen(input_name_steam,"rb");
    if (!input_stream) {
        printf("Ошибка при открытия файла !\n");
        return 1;
    }

    output_stream = fopen(output_name_stream,"wb");
    if (!output_stream) {
        printf("Ошибка при создании файла !\n");
        fclose(input_stream);
        input_stream = NULL;
        return 1;
    }

    /**
     * Переменные для кадра
     *
     * frame_tmp - считанный кадр
     * frame - преобразованный кадр
     */
    uint8_t *frame_tmp = (uint8_t *)malloc((size_t)(width*height*3));
    if (!frame_tmp) {
        printf("Ошибка при выделении памяти !\n");
        fclose(input_stream);
        input_stream = NULL;
        fclose(output_stream);
        output_stream = NULL;

        return 1;
    }
    uint8_t *frame = (uint8_t *)malloc((size_t)(width*height*3));
    if (!frame) {
        printf("Ошибка при выделении памяти !\n");
        free(frame_tmp);
        frame_tmp = NULL;
        fclose(input_stream);
        input_stream = NULL;
        fclose(output_stream);
        output_stream = NULL;

        return 1;
    }

    /**
     * Считывание кадра
     */
    while (1) {
        int32_t i, j, k, l;

        /**
         * Считывание Y
         */
        fread(frame_tmp, (size_t) (width * height), 1, input_stream);
        /**
         * Проверка на конец файла (проверка делается не в while, что бы не добавлялся лишний кадр)
         */
        if (feof(input_stream)) break;

        for (i = 0; i < height; i++)
            for (j = 0, k = 0; j < width * 3; j += 3, k++) {
                frame[j + i * width * 3] = frame_tmp[k + i * width];
            }

        /**
         * Считывание U
         */
        fread(frame_tmp, (size_t) ((width / 2) * (height / 2)), 1, input_stream);
        for (i = 0, l = 0; i < height && (l + 1) < height; i += 2, l++)
            for (j = 1, k = 0; j < width * 3; j += 3 * 2, k++) {
                frame[j + i * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + 3 + i * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + (i + 1) * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + 3 + (i + 1) * width * 3] = frame_tmp[k + l * width / 2];
            }

        /**
         * Считывание V
         */
        fread(frame_tmp, (size_t) ((width / 2) * (height / 2)), 1, input_stream);
        for (i = 0, l = 0; i < height && (l + 1) < height; i += 2, l++)
            for (j = 2, k = 0; j < width * 3; j += 3 * 2, k++) {
                frame[j + i * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + 3 + i * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + (i + 1) * width * 3] = frame_tmp[k + l * width / 2];
                frame[j + 3 + (i + 1) * width * 3] = frame_tmp[k + l * width / 2];
            }
        /**
         * Кадр загружен
         */

        /**
         * Наложение изображении
         */
        for (i = 0; i < height; i++)
            for (j=0; j < width*3; j+=3) {
                if (30  < img_rgb[j+i*width*3]            && img_rgb[j+i*width*3]           < 40)
                if (210 < img_rgb[(j + 1) + i * width*3]  && img_rgb[(j + 1) + i * width*3] < 220)
                if (240 < img_rgb[(j + 2) + i * width*3]  && img_rgb[(j + 2) + i * width*3] < 250) {
                    /**
                     * Записываем исходный цвет кадра
                     */
                    continue;
                }

                /**
                 * Записываем цвет пикселя из RGB картинки
                 */
                frame[j+i*width*3] = img_yuv[j+i*width*3];
                frame[(j+1)+i*width*3] = img_yuv[(j+1)+i*width*3];
                frame[(j+2)+i*width*3] = img_yuv[(j+2)+i*width*3];
            }

        /**
         * Сохранение кадра
         */
        for (i = 0; i < height; i++) {
            for (j = 0; j < width * 3; j += 3) {
                fwrite(&frame[j + i * width * 3], 1, 1, output_stream);
            }
        }
        for (i = 0; i < height; i += 2) {
            for (j = 1; j < width * 3; j += 3 * 2) {
                fwrite(&frame[j + i * width * 3], 1, 1, output_stream);
            }
        }
        for (i = 0; i < height; i += 2) {
            for (j = 2; j < width * 3; j += 3 * 2) {
                fwrite(&frame[j + i * width * 3], 1, 1, output_stream);
            }
        }
    }

    free(frame_tmp);
    frame_tmp = NULL;
    free(frame);
    frame = NULL;
    fclose(input_stream);
    input_stream = NULL;
    fclose(output_stream);
    output_stream = NULL;
    return 0;
}

int main(int argc, char *argv[]) {

    /**
     * Обработка аргументов запуска
     *
     * Проверка кол-во аргументов
     */
    if (argc < 7 || argc > 7) {
        printf("Неверное число аргументов !\n");
        return 0;
    }

    /**
     * Вывод справки
     */
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("Использование: %s [--size=640x360] [--pic=name.bmp] "
                       "-i input_stream.yuv -o output_stream.yuv\n\n"
                       "  --size \tУказать размер видео. Ширина, высота.\n"
                       "  --pic \tУказать картинку которая будет накладываться. Картинка в формате .bmp.\n"
                       "  -i \t\tНазвание входного потока(видео).\n"
                       "  -o \t\tНазвание выходного потока(видео).\n"
                       "  -h --help \tДанная справка.\n", argv[0]);
        return 0;
    }

    /**
     * Парсинг аргументов
     */
    int32_t width = 0, height = 0;
    char *pic_name = NULL,
         *input_name_steam = NULL,
         *output_name_stream = NULL;
    uint8_t i;

    for (i=1; i<argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (strstr(argv[i],"size")-argv[i] == 2) {
                strtok(argv[i],"=x");
                width = atoi(strtok(NULL,"=x"));
                height = atoi(strtok(NULL,"=x"));
            } else
            if (strstr(argv[i],"pic")-argv[i] == 2) {
                strtok(argv[i], "=x");
                pic_name = strtok(NULL, "=x");

            } else {
                printf("Неверный аргумент !\n");
                return 0;
            }
        } else
        if (argv[i][0] == '-') {
            if (argc == i) {
                printf("Не указано значение ключа !\n");
                return 0;
            }
            if (argv[i][1] == 'i') {
                if (strstr(argv[i+1],".yuv\0") != NULL) {
                    input_name_steam = argv[i+1];
                    i++;
                } else {
                    printf("Неверный аргумент !\nВходной поток должен иметь формат .yuv !");
                    return 0;
                }
            } else
            if (argv[i][1] == 'o') {
                if (strstr(argv[i+1],".yuv\0") != NULL) {
                    output_name_stream = argv[i+1];
                    i++;
                } else {
                    printf("Неверный аргумент !\nВыходной поток должен иметь формат .yuv !");
                    return 0;
                }
            } else {
                printf("Неверный аргумент !\n");
                return 0;
            }
        } else {
            printf("Неверный аргумент !\n");
            return 0;
        }
    }

    /**
     * Загрузка картинки
     */
    /* Выделяем память под картинку в rgb */
    uint8_t *img_rgb = (uint8_t *)malloc((size_t)(width * height * 3));
    if (!img_rgb) {
        printf("Ошибка при выделении памяти !\n");
        return 1;
    }

    int8_t ret_lb = load_bmp(img_rgb, pic_name, width, height);
    if (ret_lb) {
        free(img_rgb);
        img_rgb = NULL;
        return 0;
    }
    else printf("Картинка загружена.\n");

    /**
     * Преобразование картинки в yuv
     */
    /* Выделяем память под картинку в yuv */
    uint8_t *img_yuv = (uint8_t *)malloc((size_t)(width * height * 3));
    if (!img_yuv) {
        printf("Ошибка при выделении памяти !\n");
        free(img_rgb);
        img_rgb = NULL;
        return 0;
    }

    rgb_in_yuv(img_rgb, img_yuv, width, height);

    /**
     * Обработка видео потока (вставка картинки)
     */
    int8_t ret_iiv = img_insert_video(img_yuv, img_rgb, input_name_steam, output_name_stream, width, height);
    if (ret_iiv) {
        free(img_rgb);
        img_rgb = NULL;
        free(img_yuv);
        img_yuv = NULL;
        return 0;
    }
    else printf("ОК.\n");

    free(img_rgb);
    img_rgb = NULL;
    free(img_yuv);
    img_yuv = NULL;

    return  0;
}
