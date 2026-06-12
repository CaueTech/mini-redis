# 1. Variáveis de Configuração
CC = gcc
CFLAGS = -Wall -Wextra -g -I./headers

# 2. Definição dos Arquivos de Origem (.c) e Objetos (.o)
# Listamos todos os arquivos .c do projeto
SOURCES = main/main.c server/network.c server/states.c server/data/llist_hash.c server/data/mheap.c server/data/parsing.c

# O Makefile vai converter a lista de .c para .o automaticamente na pasta build
OBJECTS = $(SOURCES:%.c=build/%.o)

# Nome do binário final
TARGET = mini_redis

# 3. Regra Principal (Default)
all: $(TARGET)

# Regra para linkar os arquivos objeto e gerar o executável final
$(TARGET): $(OBJECTS)
	@echo "Linking objects to create executable: $(TARGET)..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
	@echo "Compilation successful!"

# Regra genérica para compilar os arquivos .c em arquivos .o
# O pipe '|' garante que a pasta build seja criada antes da compilação
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 4. Regras Utilitárias
# Limpa os arquivos gerados pela compilação para forçar um recomeço do zero
clean:
	@echo "Cleaning up build artifacts..."
	rm -rf build $(TARGET)
	@echo "Clean complete!"

# Define que essas regras não representam arquivos físicos
.PHONY: all clean