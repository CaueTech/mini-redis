# Mini-Redis

Uma implementação simplificada de um servidor estilo Redis, escrita em C, com suporte a armazenamento em memória e processamento concorrente baseado em `select()` (sem uso de threads).

`Versão: Mark I`

## 🚀 Visão Geral

O **Mini-Redis Mark I** é a primeira versão de um projeto voltado para o entendimento de sistemas distribuídos e servidores de alto desempenho.

Nesta versão, o foco está em:

- Manipulação de sockets TCP
- Multiplexação de conexões com `select()`
- Armazenamento em memória (key-value store)
- Parsing de comandos simples

## ⚙️ Funcionalidades

- ✅ Comando `SET key value`
- ✅ Comando `GET key`
- ✅ Armazenamento em memória (não persistente)
- ✅ Suporte a múltiplos clientes simultâneos (via `select()`)

Ainda falta:
- ❌ Persistência (AOF ainda não implementado)
- ❌ TTL
- ❌ Threads (concorrência simulada)

## 🛠️ Como Rodar

Certifique-se de ter:

- GCC instalado
- Ambiente Linux ou WSL

Compile com:

`gcc -o mini-redis src/*.c`

Execute com:

`./mini-redis`

Por padrão ele irá executar em
`localhost:8080`

Use _telnet_ ou _netcat_ para se conectar. Os comandos suportados são `SET [chave] [valor]` e `GET [chave]`, 
os _logs_ são exbidos na aba do terminal que iniciou o programa de execução.

## 🧠 Arquitetura

O sistema é composto por:

- **Servidor TCP**: escuta conexões na porta definida
- **Event Loop**: gerenciado com `select()`
- **Parser de comandos**: interpreta input do cliente
- **Dicionário em memória**: armazena pares chave-valor
