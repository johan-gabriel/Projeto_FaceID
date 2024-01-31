/*
    Desenvolvimento da Placa de Controle de Catraca (PCC) para sistema Facial;

    Inicio do desenvolvimento - 20/07/2022
    Fim da da primeira versão - 17/08/2022

    Equipe de desenvolvimento:
    Firmware - JOHAN SILVA / BRUNO RIBEIRO
    Hardware - BRUNO RIBEIRO

    Sistema desenvolvido para a empresa PiscoIDC, para o equipamento FACEID - CONTROLID
    para controle de catracas de controle de acesso.


    Versão: 1.0E (Com atualização para leitura e escrita no catraca.).Funcionando!
        comandos implementados: #BUZ OK
                                #RST (Sem a função executada só dando retorno). OK
                                #SSHa/h OK
                                #GSH OK
                                #RPAe/s/b OK
                                #GFW OK
    Versão: 1.1E (Teste de Busca e Set de IP e IS).
        comandos implementados:#ASN1/2
                               #SLG
                               #SPP / cancelado (Fora de uso)
                               #Dupla passagem
                               #STG
    Versão: 1.13E (Teste recepção e emissão de Comandos RIBAS).
        comandos implementados:

        OBS: Tempo do rele na pagina do FaceID = 500 ms
             Sem solenoide 2, pic bloc (versão placa V1.0)
    Versão: 2.5EQr 
        - SD storage, funcionando somente na Protoboard 
        - Qr code fazendo leitura implantada pelo Ribeiro
        - Regulagem do tempo de SLGa'tempo' e RPAa'tempo'
     


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
    // sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
    }
    
    standby();

   
   
    return 0;
}

  