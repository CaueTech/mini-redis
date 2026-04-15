#include <string.h>  // memset
#include <stdio.h>   // sscanf

#include "../headers/structs.h" // command_block

command_block parsing_command(char* buffer) {
    command_block cmd;
    memset(&cmd, 0, sizeof(cmd));
    sscanf(buffer, "%5s %50s %500s", cmd.command, cmd.key, cmd.value);
    return cmd;
}