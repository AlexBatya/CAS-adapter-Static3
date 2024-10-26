#include "SerialPortReader.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"  // Подключаем библиотеку cJSON

HANDLE openPort(const char *portName) {
    HANDLE hSerial = CreateFile(
        portName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Ошибка открытия COM-порта.\n");
        return NULL;
    }
    printf("COM-порт открыт успешно.\n");
    return hSerial;
}

void configurePort(HANDLE hSerial, int baudRate, int byteSize, int stopBits, int parity) {
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Не удалось получить параметры COM-порта.\n");
        return;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = byteSize;
    dcbSerialParams.StopBits = (stopBits == 1) ? ONESTOPBIT : TWOSTOPBITS;
    dcbSerialParams.Parity = parity;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Не удалось установить параметры COM-порта.\n");
        return;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Не удалось установить тайм-ауты COM-порта.\n");
        return;
    }

    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

// Создаем JSON-объект с переданным значением DinWeight
void createJsonResponse(int dinWeight) {
    cJSON *response_json = cJSON_CreateObject();
    if (response_json == NULL) {
        printf("Ошибка создания JSON-объекта.\n");
        return;
    }

    cJSON_AddNumberToObject(response_json, "TargetID", 1);
    cJSON_AddNumberToObject(response_json, "DinWeight", dinWeight);
    cJSON_AddNumberToObject(response_json, "DinStabil", 1);
    cJSON_AddNumberToObject(response_json, "OverloadDin", 0);
    cJSON_AddNumberToObject(response_json, "ErrorDin", 0);
    cJSON_AddNumberToObject(response_json, "FixStat", 0);

    char *json_string = cJSON_PrintUnformatted(response_json);
    if (json_string != NULL) {
        printf("JSON Ответ: %s\n", json_string);
        cJSON_free(json_string);
    }

    cJSON_Delete(response_json);
}

void readData(HANDLE hSerial) {
    unsigned char buffer[1024];
    DWORD bytesRead;
    int bufferIndex = 0;
    int dinWeight = 0;
    int isNegative = 0;
    int valueReceived = 0; // Флаг, указывающий на получение валидного значения

    printf("Чтение данных с COM-порта...\n");

    while (1) {
        if (ReadFile(hSerial, &buffer[bufferIndex], 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                if (buffer[bufferIndex] == '-') {
                    isNegative = 1;
                } else if (buffer[bufferIndex] >= '0' && buffer[bufferIndex] <= '9') {
                    dinWeight = dinWeight * 10 + (buffer[bufferIndex] - '0');
                    valueReceived = 1; // Устанавливаем флаг при получении числового значения
                } else if (buffer[bufferIndex] == '\n' || buffer[bufferIndex] == '\r') {
                    if (valueReceived) { // Обрабатываем только, если было получено валидное значение
                        dinWeight = isNegative ? -dinWeight : dinWeight;
                        createJsonResponse(dinWeight);
                    }

                    dinWeight = 0;
                    isNegative = 0;
                    valueReceived = 0;
                }

                bufferIndex++;

                if (bufferIndex >= sizeof(buffer) - 1) {
                    bufferIndex = 0;
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        } else {
            DWORD error = GetLastError();
            if (error == 0) {
                Sleep(100);
                continue;
            }
            printf("Ошибка чтения с COM-порта: %lu\n", error);
            break;
        }
    }
}

