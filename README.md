# Mini-Redis

Uma implementação robusta e simplificada de um servidor estilo Redis, escrita em C. Este projeto suporta armazenamento em memória com resolução de colisões, expiração de chaves (TTL) e processamento concorrente baseado em multiplexação com `select()`.

**Versão: Mark II**

## 🚀 Visão Geral

O **Mini-Redis** é um projeto focado no entendimento aprofundado de sistemas distribuídos, gerenciamento de memória em C e construção de servidores de alto desempenho do zero.

Nesta versão atualizada, o motor do banco de dados ganhou persistência de dados e um gerenciador de tempo de vida (TTL) baseado em uma estrutura de Min-Heap.

> Implementação educacional inspirada no Redis com Hash Table customizada, gerenciamento de TTL por Min-Heap, persistência AOF e servidor concorrente baseado em `select()`.

---

## 📊 Tecnologias e Conceitos

* Linguagem C
* Sockets TCP/IP
* Multiplexação de I/O com `select()`
* Hash Tables
* Linked Lists
* Min-Heaps
* Persistência baseada em logs (AOF)
* Gerenciamento manual de memória
* Arquitetura orientada a eventos

---

## ⚙️ Funcionalidades

* ✅ **Tabela Hash Customizada:** Armazenamento em memória com encadeamento (Linked Lists) para resolução de colisões.
* ✅ **Multiplexação de I/O:** Suporte a múltiplos clientes simultâneos via `select()`, sem a sobrecarga de threads.
* ✅ **TTL (Time-to-Live):** Expiração passiva e ativa de chaves gerenciada através de uma Min-Heap.
* ✅ **Persistência de Dados (AOF):** Gravação de logs transacionais para recuperação do banco de dados após reinicializações.
* ✅ **Comandos Suportados:** `SET`, `GET` e `DEL`.

---

## 📂 Arquitetura de Diretórios

O código foi refatorado e modularizado para separar responsabilidades de rede, estado e estruturas de dados.

```text
.
├── build/                  # Diretório auto-gerado para arquivos objeto (.o)
├── headers/                # Arquivos de cabeçalho e definições
│   ├── server.h
│   └── structs.h
├── main/
│   └── main.c              # Ponto de entrada (Loop principal do servidor)
├── server/
│   ├── data/               # Lógica de estruturas de dados e parser
│   │   ├── llist_hash.c    # Implementação da Tabela Hash e Listas Encadeadas
│   │   ├── mheap.c         # Gerenciamento da Min-Heap (TTL)
│   │   └── parsing.c       # Validador e interpretador de comandos
│   ├── network.c           # Gerenciamento de Sockets TCP e conexões
│   └── states.c            # Máquina de estados e execução de comandos
├── .gitignore
├── log.txt                 # Arquivo de persistência (Append-Only File)
├── Makefile                # Script de automação de build
└── README.md
```

---

## 🛠️ Como Compilar e Rodar

O projeto utiliza um **Makefile** para automatizar a compilação, mantendo os artefatos de build organizados e separados do código-fonte.

### Pré-requisitos

* GCC instalado
* Linux ou WSL (Windows Subsystem for Linux)

### I. Compilação

Para compilar o projeto:

```bash
make
```

Para remover todos os artefatos gerados e realizar uma compilação limpa:

```bash
make clean
make
```

### II. Execução

Após a compilação, inicie o servidor:

```bash
./mini_redis
```

O servidor ficará escutando em:

```text
localhost:8080
```

Para conectar um cliente de teste utilizando Netcat:

```bash
nc localhost 8080
```

Ou utilizando Telnet:

```bash
telnet localhost 8080
```

---

## 💬 Comandos e Utilização

O banco de dados armazena pares chave-valor.

> **Observação:** Os comandos e nomes das chaves são *case sensitive*. Portanto, `SET`, `set` e `Set` são tratados como comandos diferentes.

### SET

```text
SET <chave> <valor> [EX segundos]
```

Cria ou atualiza uma chave com um valor.

O modificador opcional `EX` permite definir um tempo de vida (TTL) em segundos para a chave.

**Exemplos:**

```text
SET usuario_123 Joao EX 60
```

Armazena o valor `Joao` por 60 segundos.

```text
SET configuracao dark_mode
```

Armazena o valor permanentemente.

---

### GET

```text
GET <chave>
```

Busca e retorna o valor associado à chave.

Se a chave não existir ou tiver expirado, retorna uma resposta de erro ou valor nulo.

**Exemplo:**

```text
GET usuario_123
```

---

### DEL

```text
DEL <chave>
```

Remove imediatamente uma chave da memória.

Caso a chave possua TTL associado, ela também é removida da Min-Heap responsável pelo gerenciamento das expirações.

**Exemplo:**

```text
DEL configuracao
```

---

## 💾 Persistência de Dados (AOF)

Para evitar perda de dados em desligamentos inesperados, o sistema implementa um mecanismo inspirado no **Append-Only File (AOF)** utilizado pelo Redis.

Sempre que um comando de escrita (`SET` ou `DEL`) é executado com sucesso:

1. A operação é aplicada na memória.
2. O comando é imediatamente anexado ao arquivo `log.txt`.

Durante a inicialização do servidor, o arquivo é lido sequencialmente e todos os comandos são reexecutados internamente, reconstruindo o estado da Tabela Hash e da Min-Heap antes que novas conexões sejam aceitas.

Esse mecanismo garante recuperação consistente do estado do banco após reinicializações.

---

## 📈 Conceitos Praticados

Este projeto explora diversos conceitos fundamentais de Engenharia de Software e Sistemas:

### Estruturas de Dados

* Hash Tables
* Linked Lists
* Min-Heaps

### Programação de Sistemas

* Sockets TCP
* Multiplexação de I/O com `select()`
* Gerenciamento manual de memória

### Bancos de Dados

* Armazenamento chave-valor
* Persistência baseada em logs (AOF)
* Expiração de chaves (TTL)

### Arquitetura de Software

* Modularização de componentes
* Separação de responsabilidades
* Máquina de estados para processamento de comandos

---

## 🎯 Objetivos do Projeto

Este projeto foi desenvolvido com fins educacionais para aprofundar conhecimentos em:

* Sistemas distribuídos
* Desenvolvimento de servidores de alta performance
* Estruturas de dados de baixo nível
* Persistência de dados
* Programação de redes em C

Inspirado na arquitetura do Redis, o Mini-Redis busca reproduzir seus conceitos fundamentais de forma simplificada, permitindo estudar internamente o funcionamento de um banco de dados em memória moderno.

---

## 📄 Licença

Este projeto é distribuído para fins educacionais e de aprendizado.