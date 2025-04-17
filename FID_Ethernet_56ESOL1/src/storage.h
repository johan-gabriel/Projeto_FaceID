#ifndef _STORAGE_H
#define _STORAGE_H

bool storageInit(); // inicializa o sistema de arquivos, retorna true se for...
void storageLog(const char *msg); // manda uma mensagem de texto para o data logger
bool mountSDCard();
int readSNL();
void readSNE1();

#endif