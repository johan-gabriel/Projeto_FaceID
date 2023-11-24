#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"

#include "storage.h"


#define LOG_FILE_PATH "/store.txt"

bool headerWritten = false; // se a variável acabou de ser criada ou já havia uma anterior
FATFS fs; // sistema de arquivos...
FIL logFile;
int acionador = 0;


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
    }else{
        puts("Arquivo de Log já existe.");
    }
   

}
// manda uma mensagem de texto para o data logger
void storageLog(const char *msg) {
    FRESULT res;
    FILINFO info2;
    if(f_stat(LOG_FILE_PATH, &info2) == FR_NO_FILE){
        puts("Arquivo de log ainda não existe.");
    }else{
        puts("Arquivo de Log já existe.");
        f_unlink(LOG_FILE_PATH);
    }
    

    res = f_open(&logFile, LOG_FILE_PATH,FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK)
    {
        printf("Falha ao abrir o arquivo de log para escrita: %d\n", res);
        return;
    }
    
    
    f_puts(msg, &logFile);
    f_close(&logFile);
}
int readLog() {
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[5];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);

    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '1')
    {   
        acionador =  1;
        return acionador;
    }
     if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '0')
    {   
        acionador =  2;
        return acionador;
    }
     if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '1')
    {   
        acionador =  3;
        return acionador;
    }
     if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '0')
    {   
        acionador = 4;
        return acionador;
    }
     if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '1')
    {   
        acionador =  5;
        return acionador;
    }
     if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '0')
    {   
        acionador =  6;
        return acionador;
    }
     if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '1')
    {   
        acionador =  7;
        return acionador;
    }
     if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '0')
    {   
        acionador = 0;
        return acionador;
    }
    else{
        acionador = 0;
    }
    
    f_close(&leitor);
}