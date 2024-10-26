// main.c
#include "SerialPortReader.h"
#include "ConfigLoader.h"
#include <stdio.h>

int main() {
    // Переменные для параметров порта
    char portName[20];
    int baudRate, byteSize, stopBits, parity;

    // Загружаем конфигурацию из файла config.json
    if (loadConfig(portName, &baudRate, &byteSize, &stopBits, &parity) != 0) {
        printf("Ошибка загрузки конфигурации.\n");
        return 1;
    }

    // Подключаемся к COM-порту с параметрами из config.json
    HANDLE hSerial = openPort(portName);
    if (hSerial != INVALID_HANDLE_VALUE) {
        configurePort(hSerial, baudRate, byteSize, stopBits, parity);
        readData(hSerial);
        CloseHandle(hSerial);
    } else {
        printf("Не удалось открыть COM-порт.\n");
    }

    return 0;
}

