#include <locale.h>
#include <stdio.h>
#include <wchar.h>

#include "strutils.h"

char* read_file_to_string(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = malloc(length + 1);
    if (source == NULL) {
        fclose(file);
        return NULL;
    }

    fread(source, 1, length, file);
    source[length] = '\0';

    fclose(file);
    return source;
}

int main(void) {
    setlocale(LC_ALL, "");

    char* source = read_file_to_string("../utf8_encoded_text_file.txt");

    if (source == NULL) {
        return 1;
    }

    return 0;
}

