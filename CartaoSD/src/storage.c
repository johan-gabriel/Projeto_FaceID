#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"

#include "storage.h"

#define LOG_FILE_HEADER "Date; Temperature\n"
#define LOG_FILE_PATH "/store.txt"

bool headerWritten = false; // se a variável acabou de ser criada ou já havia uma anterior
FATFS fs; // sistema de arquivos...
FIL logFile;

bool mountSDCard(){
    

    sd_init_driver();
     FRESULT res = f_mount(&fs, "0:",1);

    if (res == FR_OK)
    {
        puts("Sistema de arquivos montado com sucesso!");
        return true;
    }else{
        printf("Erro inicializando cartão: %d\n", res);
        return false;
    }

}

// inicializa o sistema de arquivos, retorna true se for...
bool storageInit(){
    
   if (!mountSDCard())
   {
        return false;
   }

    FILINFO info;
    if(f_stat(LOG_FILE_PATH, &info) == FR_NO_FILE){
        puts("Arquivo de log ainda não existe.");
        headerWritten = false;
    }else{
        puts("Arquivo de Log já existe.");
        headerWritten = true;
    }
   

}
// manda uma mensagem de texto para o data logger
void storageLog(const char *msg) {
    FRESULT res;

    res = f_open(&logFile, LOG_FILE_PATH,FA_OPEN_EXISTING | FA_WRITE);

    if (res != FR_OK)
    {
        printf("Falha ao abrir o arquivo de log para escrita: %d\n", res);
        return;
    }
    
    if (!headerWritten)
    {
        f_puts(LOG_FILE_HEADER, &logFile);
        headerWritten = true;

    }
    
    f_puts(msg, &logFile);
    

    f_close(&logFile);
}
void readLog() {
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[20];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);
    
    f_close(&leitor);
}