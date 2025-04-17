/*
    Desenvolvimento da Placa de Controle de Catraca (PCC) para sistema RFID;

    Inicio do desenvolvimento - 14/05/2024
    Fim da da primeira versão - Em processo.....

    Equipe de desenvolvimento:
    Firmware - JOHAN SILVA / BRUNO RIBEIRO
    Hardware - BRUNO RIBEIRO V5.0

    Sistema desenvolvido para a empresa PiscoIDC, para o equipamento FACEID - CONTROLID e Leitor LN-101 NICE
    para controle de catracas de controle de acesso.


    Versão 1RFID - Visa ter todas as funções para Facial, mas com possibilidade de possuir um ou 2 leitores RFID;
        - Leitor Utilizado LN-101 (NICE)
     


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "storage.h"
#include "CH9121.h"
#include "SP.h"

int valConfigMain = 0;

int main()
{
    stdio_init_all();
    //  sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
    }
    
    standby();

   
   
    return 0;
}

  