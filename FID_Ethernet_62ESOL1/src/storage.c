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
int STE_dez = 0;
int STE_uni = 0;
int STE_Value = 0;
char STE_return[4] = "00\0";

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
    char leitura[30];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);

    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 0;
        return acionador; // 0000
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 1;
        return acionador; // 0001
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 2;
        return acionador; // 0010
    }
    if (leitura[0] == '0' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 3;
        return acionador; // 0011
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 4;
        return acionador; // 0100
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 5;
        return acionador; // 0101
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 6;
        return acionador; // 0110
    }
    if (leitura[0] == '0' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 7;
        return acionador; // 0111
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 8;
        return acionador; // 1000
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 9;
        return acionador; // 1001
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 10;
        return acionador; // 1010
    }
    if (leitura[0] == '1' && leitura[2] == '0' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 11;
        return acionador; // 1011
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '0')
    {
        acionador = 12;
        return acionador; // 1100
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '0' && leitura[6] == '1')
    {
        acionador = 13;
        return acionador; // 1101
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '0')
    {
        acionador = 14;
        return acionador; // 1110
    }
    if (leitura[0] == '1' && leitura[2] == '1' && leitura[4] == '1' && leitura[6] == '1')
    {
        acionador = 15;
        return acionador; // 1111
    }
    else
    {
        acionador = 0;
    }

    f_close(&leitor);
}

int readSFC()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[8])
    {
    case 'c':
        comand = 'c';
        return comand;
        break;
    case 'p':
        comand = 'p';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSLE()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[10])
    {
    case '0':
        comand = '0';
        return comand;
        break;
    case '1':
        comand = '1';
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
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[12])
    {
    case '1':
        comand = '1';
        return comand;
        break;
    case '2':
        comand = '2';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSBZ()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[14])
    {
    case '0':
        comand = '0';
        return comand;
        break;
    case '1':
        comand = '1';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSTL()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[16])
    {
    case '0':
        comand = '0';
        return comand;
        break;
    case '1':
        comand = '1';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSQS()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[18])
    {
    case '1':
        comand = '1';
        return comand;
        break;
    case '2':
        comand = '2';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSPX()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;
    int comand = 0;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    switch (leitura[20])
    {
    case 'a':
        comand = 'a';
        return comand;
        break;
    case 'w':
        comand = 'w';
        return comand;
        break;

    default:
        break;
    }
    f_close(&leitor);
}

int readSNE()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);
    SNE_dez = (int)leitura[22];
    SNE_uni = (int)leitura[23];

    SNE_return[0] = leitura[22];
    SNE_return[1] = leitura[23];

    printf("\nDezena %d Unidade %d", (SNE_dez - 48), (SNE_uni - 48));

    SNE_Value = (((SNE_dez - 48) * 10) + (SNE_uni - 48));

    printf("Valor SNE %d", SNE_Value);
    return SNE_Value;
    f_close(&leitor);
}

int readSTE()
{
    FRESULT res;
    FRESULT readResult;
    FIL leitor;
    char leitura[30];
    UINT bytesRead;

    res = f_open(&leitor, LOG_FILE_PATH, FA_READ);

    readResult = f_read(&leitor, leitura, sizeof(leitura), &bytesRead);

    printf("Lido: %s", leitura);
    STE_dez = (int)leitura[25];
    STE_uni = (int)leitura[26];

    STE_return[0] = leitura[25];
    STE_return[1] = leitura[26];

    printf("\nDezena %d Unidade %d", (STE_dez - 48), (STE_uni - 48));

    STE_Value = (((STE_dez - 48) * 10) + (STE_uni - 48));

    printf("Valor SNE %d", STE_Value);
    return STE_Value;
    f_close(&leitor);
}

void readSNE1()
{
    uart_puts(UART_ID1, SNE_return);
}

void readSTE1()
{
    uart_puts(UART_ID1, STE_return);
}