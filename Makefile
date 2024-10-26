# Makefile

CC = gcc
CFLAGS = -Wall

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

TARGET = $(BUILD_DIR)/com_reader

# Убедимся, что все объектные файлы включены
OBJ_FILES = $(BUILD_DIR)/main.o $(BUILD_DIR)/SerialPortReader.o $(BUILD_DIR)/ConfigLoader.o $(BUILD_DIR)/cJSON.o

# Основное правило
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(OBJ_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ_FILES) -lsetupapi

# Правила для компиляции каждого файла .c в .o
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/SerialPortReader.o: $(SRC_DIR)/SerialPortReader.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/SerialPortReader.c -o $(BUILD_DIR)/SerialPortReader.o

$(BUILD_DIR)/ConfigLoader.o: $(SRC_DIR)/ConfigLoader.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/ConfigLoader.c -o $(BUILD_DIR)/ConfigLoader.o

$(BUILD_DIR)/cJSON.o: $(SRC_DIR)/cJSON.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/cJSON.c -o $(BUILD_DIR)/cJSON.o

# Создаем директорию build, если она не существует
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Очистка
clean:
	rm -rf $(BUILD_DIR)

