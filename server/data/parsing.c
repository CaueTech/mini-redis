#include "../headers/structs.h"
#include "../headers/server.h"

#define _GNU_SOURCE

/*
    Essa função é parece ser complicada mas em essência é um tratamento de strings para validar os comandos que vem direto da rede ou da leitura do log.txt ao iniciar o servidor
*/

command_block parsing_command(char* buffer, char reading_type) {
    command_block cmd;
    memset(&cmd, 0, sizeof(cmd));

    // Limpa quebras de linha da rede ou do arquivo
    buffer[strcspn(buffer, "\r\n")] = 0;

    // 1. Tratamento do buffer direto da rede

    if(reading_type == 'C') {
        char * token = strtok(buffer, " ");
        if (!token) return cmd;
        strncpy(cmd.command, token, sizeof(cmd.command) - 1);
        
        token = strtok(NULL, " ");
        if (!token) return cmd;
        strncpy(cmd.key, token, sizeof(cmd.key) - 1);

        // Isso é uma manobra para viabilizar valores compostos (ex: uma chave cujo valor é um nome composto, como 'Cauê Oliveira')
        char * remainder = token + strlen(token) + 1;
        while (* remainder == ' ') remainder++;
        
        if (*remainder == '\0') {
            // Se for um SET tentando passar sem valor, matamos o comando aqui mesmo.
            if (strcmp(cmd.command, "SET") == 0) {
                memset(cmd.command, 0, sizeof(cmd.command));
            }
            return cmd;
        }

        // O método strcasestr viabiliza comparações desconsiderando caracteres maiúsculos ou minúsculos, ele vai retornar um ponteiro para onde começar a string que buscamos, que é " EX "
        char * flag_ptr = strcasestr(remainder, " EX ");

        if (flag_ptr != NULL) {
            // Fazemos uma aritmética para chegar até o valor da TTL a partir da chave EX
            char * ttl_ptr = flag_ptr + 4;

            // atoll() faz a conversão do valor do CHAR para númeral
            cmd.ttl_val = atoll(ttl_ptr);
            * flag_ptr = '\0'; 

            if (cmd.ttl_val > 0) {
                cmd.has_ttl = 1;
                strncpy(cmd.value, remainder, sizeof(cmd.value) - 1);
                cmd.expires_at = time(NULL) + cmd.ttl_val;
            }
            else {
                // Apagamos o comando para o servidor ignorar a requisição e abortamos no caso do TTL ser igual ou menor a zero.
                memset(cmd.command, 0, sizeof(cmd.command));
                return cmd; 
            }
        }
        else {
            cmd.has_ttl = 0;
            cmd.expires_at = -1;
            strncpy(cmd.value, remainder, sizeof(cmd.value) - 1);
        }
        return cmd;
    }
    
    /* 2. Tratamento do buffer vindo do log.txt
       Formato no log com TTL:    SET [key] [value com espacos] [timestamp]
       Formato no log sem TTL:    SET [key] [value com espacos]
    */

    else {

        char * token = strtok(buffer, " ");
        if (!token) return cmd;
        strncpy(cmd.command, token, sizeof(cmd.command) - 1);
        
        token = strtok(NULL, " ");
        if (!token) return cmd;
        strncpy(cmd.key, token, sizeof(cmd.key) - 1);

        char * remainder = token + strlen(token) + 1;
        while (* remainder == ' ') remainder++;
        
        if (* remainder == '\0') {
            // Se for um SET tentando passar sem valor, matamos o comando
            if (strcmp(cmd.command, "SET") == 0) {
                memset(cmd.command, 0, sizeof(cmd.command));
            }
            return cmd;
        }

        // Procura a flag explícita que você adicionou no log
        char * flag_ptr = strstr(remainder, " EX_AT ");

        if (flag_ptr != NULL) {

            cmd.has_ttl = 1;
            * flag_ptr = '\0'; // Isola o VALUE cortando a string aqui
            strncpy(cmd.value, remainder, sizeof(cmd.value) - 1);
            
            // Avança o ponteiro exatamente 7 posições (tamanho de " EX_AT ") para ler o timestamp
            char * ts_ptr = flag_ptr + 7;
            cmd.expires_at = atoll(ts_ptr);

        } 
        else {

            cmd.has_ttl = 0;
            cmd.expires_at = -1;
            
            strncpy(cmd.value, remainder, sizeof(cmd.value) - 1);

        }

        return cmd;
    }
}

int add_aof(command_block cmd){
    FILE * f = fopen("log.txt", "a");

    if(f == NULL) {
        return 0;
        perror("SystemError: failed loading log.txt pointer\n");
    }

    if(cmd.has_ttl){
        fprintf(f, "%s %s %s EX_AT %lld\n", cmd.command, cmd.key, cmd.value, cmd.expires_at);
    }
    else{
        fprintf(f, "%s %s %s\n", cmd.command, cmd.key, cmd.value);
    }
    
    fclose(f);

    return 1;
}

int load_aof(linked_node ** pointer_collection, min_heap * mheap){
    FILE * f = fopen("log.txt", "r");

    if(f == NULL) {
        return 0;
        perror("SystemError: failed loading log.txt pointer\n");
    }

    char buffer [BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    int status = 1;


    while(fgets(buffer, sizeof(buffer), f)){
        command_block cmd = parsing_command(buffer, 'L');
        
        if(!strcmp(cmd.command, "SET")){
            
            if(cmd.has_ttl){
                long long cur_timestamp = time(NULL);
                
                if (cur_timestamp >= cmd.expires_at) continue;
                // Se a chave já tiver expirado ele não salva na memória
            }
            if(!set(cmd, pointer_collection, mheap)){
                status = 0;
            }

        }
        else if(!strcmp(cmd.command, "DEL")){
            if(!del(cmd, mheap, pointer_collection)){
                status = 0;
            }
        }
    }

    fclose(f);

    return status;
}