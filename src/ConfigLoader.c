// ConfigLoader.c
#include "ConfigLoader.h"
#include "cJSON.h"
#include <windows.h>  // Для NOPARITY, ODDPARITY, EVENPARITY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "config.json"

int loadConfig(char* portName, int* baudRate, int* byteSize, int* stopBits, int* parity) {
    FILE* file = fopen(CONFIG_FILE, "r");
    if (!file) {
        printf("Файл конфигурации не найден. Создание базового config.json...\n");

        cJSON* config = cJSON_CreateObject();
        cJSON_AddStringToObject(config, "port", "COM3");
        cJSON_AddNumberToObject(config, "baudRate", 9600);
        cJSON_AddNumberToObject(config, "byteSize", 8);
        cJSON_AddNumberToObject(config, "stopBits", 1);
        cJSON_AddStringToObject(config, "parity", "none");

        file = fopen(CONFIG_FILE, "w");
        if (!file) {
            printf("Ошибка создания файла config.json\n");
            cJSON_Delete(config);
            return -1;
        }

        char* jsonString = cJSON_Print(config);
        if (jsonString) {
            fprintf(file, "%s", jsonString);
            free(jsonString);
        } else {
            printf("Ошибка преобразования JSON в строку.\n");
        }

        fclose(file);
        cJSON_Delete(config);

        strcpy(portName, "COM3");
        *baudRate = 9600;
        *byteSize = 8;
        *stopBits = 1;
        *parity = NOPARITY;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* jsonContent = (char*)malloc(fileSize + 1);
    if (!jsonContent) {
        printf("Ошибка выделения памяти для чтения JSON\n");
        fclose(file);
        return -1;
    }

    fread(jsonContent, 1, fileSize, file);
    jsonContent[fileSize] = '\0';
    fclose(file);

    cJSON* config = cJSON_Parse(jsonContent);
    if (!config) {
        printf("Ошибка чтения файла config.json\n");
        free(jsonContent);
        return -1;
    }

    strcpy(portName, cJSON_GetObjectItem(config, "port")->valuestring);
    *baudRate = cJSON_GetObjectItem(config, "baudRate")->valueint;
    *byteSize = cJSON_GetObjectItem(config, "byteSize")->valueint;
    *stopBits = cJSON_GetObjectItem(config, "stopBits")->valueint;
    const char* parityStr = cJSON_GetObjectItem(config, "parity")->valuestring;

    if (strcmp(parityStr, "none") == 0) *parity = NOPARITY;
    else if (strcmp(parityStr, "odd") == 0) *parity = ODDPARITY;
    else if (strcmp(parityStr, "even") == 0) *parity = EVENPARITY;
    else *parity = NOPARITY;

    cJSON_Delete(config);
    free(jsonContent);
    return 0;
}

