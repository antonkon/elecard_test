#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // Обработка аргументов запуска
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        printf("Использование: %s [--size=100x100] [--pic=name.bmp] \n"
                       "-i input_stream.yuv -o output_stream.yuv\n\n"
                       "  --size \tУказать размер изображения. Высота, ширина.\n"
                       "  --pic \tУказать картинку которая будет накладываться. Картинка в формате .bmp.\n"
                       "  -i \t\tНазвание входного потока(видео).\n"
                       "  -o \t\tНазвание выходного потока(видео).\n"
                       "  -h --help \tДанная справка.\n", argv[0]);

    return 0;
}