#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <windows.h>

// Функции для работы с COM-портом
HANDLE openPort(const char *portName);
void configurePort(HANDLE hSerial, int baudRate, int byteSize, int stopBits, int parity);
void readData(HANDLE hSerial);
void processBuffer(const unsigned char* buffer, int bufferSize);

// Функция для создания и вывода JSON-ответа с фиксированной структурой
void createJsonResponse(int dinWeight);

#endif // SERIALPORTREADER_H

