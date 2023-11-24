#ifndef _STORAGE_H
#define _STORAGE_H

// Inicializa o módulo do cartão SD e monta o sistema de arquivos.
bool storageInit();

// Escreve uma mensagem no arquivo CSV do data logger.
void storageLog(const char *msg);

#endif