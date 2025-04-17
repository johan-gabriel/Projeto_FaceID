#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"

#include "storage.h"
#include "CH9121.h"

#define LOG_FILE_PATH "/store.txt"

bool headerWritten = false; // se a variável acabou de ser criada ou já havia uma anterior
FATFS fs;                   // sistema de arquivos...
FIL logFile;
int acionador = 0;
int SNE_dez = 0;
int SNE_uni = 0;
int SNE_Value = 0;
char SNE_return[4] = "00\0";

bool mountSDCard()
{

    sd_init_driver();
    FRESULT res = f_mount(&fs, "0:", 1);

    if (res == FR_OK)
    {
        puts("Sistema de arquivos montado com sucesso!");
        return true;
    }
    else
    {
        printf("Erro inicializando cartão: %d\n", res);
        return false;
    }
}

// inicializa o sistema de arquivos, retorna true se for...
bool storageInit()
{

    if (!mountSDCard())
    {
        return false;
    }

    FILINFO info;
    if (f_stat(LOG_FILE_PATH, &info) == FR_NO_FILE)
    {
        puts("Arquivo de log ainda não existe.");
    }
    else
    {
        puts("Arquivo de Log já existe.");
    }
}
// manda uma mensagem de texto para o data logger
void storageLog(const char *msg)
{
    FRESULT res;
    FILINFO info2;
    if (f_stat(LOG_FILE_PATH, &info2) == FR_NO_FILE)
    {
    }
    else
    {
        f_unlink(LOG_FILE_PATH);
    }

    res = f_open(&logFile, LOG_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK)
    {
        printf("Falha ao abrir o arquivo de log para escrita: %d\n", res);
        return;
    }

    f_puts(msg, &logFile);
    f_close(&logFile);
}
int readLog()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[17];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);

    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 1;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 2;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 3;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 4;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 5;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 6;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 7;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 8;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 9;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 10;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 11;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 12;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 13;
        return acionador;
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 14;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 15;
        return acionador;
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 0;
        return acionador;
    }
    else
    {
        acionador = 0;
    }

    f_close(&leitor);
}
int readSNE()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[15];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);
    SNE_dez = (int)leitura[10];
    SNE_uni = (int)leitura[11];

    SNE_return[0] = leitura[10];
    SNE_return[1] = leitura[11];

    printf("\nDezena %d Unidade %d", (SNE_dez - 48), (SNE_uni - 48));

    SNE_Value = ((SNE_dez - 48) * 10) + (SNE_uni - 48);

    printf("Valor SNE %d", SNE_Value);
    return SNE_Value;
    f_close(&leitor);
}

void readSNE1()
{
    uart_puts(UART_ID1, SNE_return);
}

int readSFC()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[17];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[8])
    {
    case '0':
        comand = 0;
        return comand;
        break;
    case '1':
        comand = 1;
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSNL()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[17];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[13])
    {
    case '1':
        comand = 49;
        return comand;
        break;
    case '2':
        comand = 50;
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}