#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <pico/stdlib.h>

// Includes da biblioteca FatFS.
#include "ff.h"
#include "sd_card.h"

#include "storage.h"

// Header do arquivo CSV, que será escrito na primeira linha do mesmo, definindo as colunas da tabela.
#define LOG_FILE_HEADER "Date;Temperature\n"

// Caminho para onde devemos salvar o arquivo de log.
#define LOG_FILE_PATH "/temp.csv"

// Flag utilizada para determinar se o header já foi escrito no cartão SD.
// Quando o módulo é inicializado, verificamos se o arquivo de log já existe,
// caso o arquivo não exista, esta flag será definida como true e o header
// será escrito na primeira linha do arquivo quando storageLog for chamada.
bool headerWritten = false;

// Referência para o sistema de arquivos do cartão SD.
FATFS fs;

// Referência para o arquivo de log.
FIL logFile;

// Função auxiliar que inicializa o driver do cartão SD e monta o sistema de arquivos FAT.
bool mountSDCard()
{
	// Função da biblioteca FatFS que inicializa o driver do cartão SD.
    sd_init_driver();

	// Tenta montar o sistema de arquivos no drive "0:" (definido em hw_config.c).
    FRESULT res = f_mount(&fs, "0:", 1);
    
	// Caso o retorno seja FR_OK, significa que deu tudo certo.
    if (res == FR_OK) {
        puts("Sistema de arquivos montado com sucesso");
        return true;
    } else {
		// Caso o retorno não seja FR_OK, res conterá um código de erro.
        printf("Falha ao montar sistema de arquivos: %d\n", res);
        return false;
    }
}

// Função que será chamada pelo firmware para inicializar o módulo.
bool storageInit()
{
	// Tenta montar o sistema de arquivos e desiste se não der certo.
    if (!mountSDCard()) return false;
	
	// Utiliza a função f_stat para buscar informações sobre o arquivo de log.
	// Estamos interessados apenas no retorno desta função, que será FR_NO_FILE
	// quando o arquivo ainda não existir.
    FILINFO info;
    if (f_stat(LOG_FILE_PATH, &info) == FR_NO_FILE) {
		// Arquivo ainda não existe, seta a flag headerWritten como false para que
		// a função storageLog escreva o header no arquivo quando chamada.
        puts("Arquivo de log ainda não existe.");
        headerWritten = false;
    } else {
		// Arquivo já existe, seta a flag headerWritten como true para
		// que a função storageLog não escreva o header.
        puts("Arquivo de log já existe.");
        headerWritten = true;
    }
}

void storageLog(const char *msg)
{
    FRESULT res;
	
	// Abre o arquivo de log para escrita, esta chamada criará o arquivo caso ele ainda
	// não existe. A opção FA_OPEN_APPEND indica que tudo o que escrevermos no arquivo
	// será adicionado no final dele.
    res = f_open(&logFile, LOG_FILE_PATH, FA_OPEN_APPEND | FA_WRITE);
	
	// O retorno será diferente de FR_OK caso a abertura do arquivo tenha falhado.
    if (res != FR_OK) {
        printf("Falha ao abrir o arquivo de log para escrita: %d\n", res);
        return;
    }

	// Se ainda não escrevemos o header do arquivo CSV, escreve.
    if (!headerWritten) {
        f_puts(LOG_FILE_HEADER, &logFile);
        headerWritten = true;
    }

	// Escreve a mensagem de log no arquivo.
    f_puts(msg, &logFile);
	
	// Fecha o arquivo. Estamos abrindo e fechando ele a cada escrita para garantir
	// que toda linha de log é de fato escrita no cartão SD, para evitar perda de
	// dados em casos de perda de energia ou remoção do cartão SD com o firmware rodando.
    f_close(&logFile);
}