#ifndef _STORAGE_H
#define _STORAGE_H

bool storageInit(); // inicializa o sistema de arquivos, retorna true se for...
void storageLog(const char *msg); // manda uma mensagem de texto para o data logger
bool mountSDCard();
int readLog();
int readSFC();
int readSLE();
int readSNL();
int readSBZ();
int readSTL();
int readSQS();
int readSPX();
int readSNE();
int readSTE();
void readSNE1();

#endif