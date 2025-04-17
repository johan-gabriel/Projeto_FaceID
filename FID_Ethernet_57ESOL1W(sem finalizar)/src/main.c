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
#include <math.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/uart.h>
#include "storage.h"
#include "CH9121.h"
#include "SP.h"

int valConfigMain = 0;
#define DATA0 5
#define DATA1 9

int counter = 1;
char wiegand[50] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
int facilitCI[8];
int idCodeI[16];
int facilitR = 0;
int idCodeR = 0;
// responsável por transformar dec em hex!!!
int fOne = 0;
int fTwo = 0;
int idOne = 0;
int idTwo = 0;
int idThr = 0;
int idFou = 0;
// fim
char facilite[3];
char ID[5];
// 0 1 2 3  4  5  6  7   8   9   10   11   12   13    14    15
int pot[16] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
bool trava = false;

char devolveIDHex(int valor);
char devolveFacilHex(int valor);
int core1Main()
{
    // stdio_init_all();
    gpio_init(DATA0);
    gpio_set_dir(DATA0, GPIO_IN);

    gpio_init(DATA1);
    gpio_set_dir(DATA1, GPIO_IN);

    bool chave = true;

    counter = 1;
    //
    // if(acionaW()){
    //     //chave = true;
    // }else{
    //     //chave = false;
    // }

    while (chave == true)
    {

        if (gpio_get(DATA1))
        {
            // if (counter == 1)
            // {
            //     // counter++;
                // wiegand[counter] = 'P';
            // }
            // if (counter == 26)
            // {
                // wiegand[counter] = 'E';
        //}
            counter++;
            wiegand[counter] = '1';
            sleep_us(1699);
        }
        if (gpio_get(DATA0))
        {
            // if (counter == 1)
            // {
                // counter++;
                // wiegand[counter] = 'P';
            // }
            // if (counter == 26)
            // {
                // wiegand[counter] = 'E';
            // }
            counter++;
            wiegand[counter] = '0';
            sleep_us(1699);
        }
        if (counter == 26)
        {
            facilitR = 0;
            idCodeR = 0;
            counter = 1;

            facilitCI[0] = (int)wiegand[3] - 48;
            facilitCI[1] = (int)wiegand[4] - 48;
            facilitCI[2] = (int)wiegand[5] - 48;
            facilitCI[3] = (int)wiegand[6] - 48;
            facilitCI[4] = (int)wiegand[7] - 48;
            facilitCI[5] = (int)wiegand[8] - 48;
            facilitCI[6] = (int)wiegand[9] - 48;
            facilitCI[7] = (int)wiegand[10] - 48;

            idCodeI[0] = (int)wiegand[11] - 48;
            idCodeI[1] = (int)wiegand[12] - 48;
            idCodeI[2] = (int)wiegand[13] - 48;
            idCodeI[3] = (int)wiegand[14] - 48;
            idCodeI[4] = (int)wiegand[15] - 48;
            idCodeI[5] = (int)wiegand[16] - 48;
            idCodeI[6] = (int)wiegand[17] - 48;
            idCodeI[7] = (int)wiegand[18] - 48;
            idCodeI[8] = (int)wiegand[19] - 48;
            idCodeI[9] = (int)wiegand[20] - 48;
            idCodeI[10] = (int)wiegand[21] - 48;
            idCodeI[11] = (int)wiegand[22] - 48;
            idCodeI[12] = (int)wiegand[23] - 48;
            idCodeI[13] = (int)wiegand[24] - 48;
            idCodeI[14] = (int)wiegand[25] - 48;
            idCodeI[15] = (int)wiegand[26] - 48;

            fOne = (facilitCI[4] * pot[3]) + (facilitCI[5] * pot[2]) + (facilitCI[6] * pot[1]) + (facilitCI[7] * pot[0]);
            fTwo = (facilitCI[0] * pot[3]) + (facilitCI[1] * pot[2]) + (facilitCI[2] * pot[1]) + (facilitCI[3] * pot[0]);

            idOne = (idCodeI[12] * pot[3]) + (idCodeI[13] * pot[2]) + (idCodeI[14] * pot[1]) + (idCodeI[15] * pot[0]);
            idTwo = (idCodeI[8] * pot[3]) + (idCodeI[9] * pot[2]) + (idCodeI[10] * pot[1]) + (idCodeI[11] * pot[0]);
            idThr = (idCodeI[4] * pot[3]) + (idCodeI[5] * pot[2]) + (idCodeI[6] * pot[1]) + (idCodeI[7] * pot[0]);
            idFou = (idCodeI[0] * pot[3]) + (idCodeI[1] * pot[2]) + (idCodeI[2] * pot[1]) + (idCodeI[3] * pot[0]);

            facilite[1] = devolveFacilHex(fOne);
            facilite[0] = devolveFacilHex(fTwo);

            ID[3] = devolveFacilHex(idOne);
            ID[2] = devolveFacilHex(idTwo);
            ID[1] = devolveFacilHex(idThr);
            ID[0] = devolveFacilHex(idFou);

            printf(" Fcilt: %s ", facilite);
            printf("Facilit code: %d %d\n", fTwo, fOne);
            printf("ID: %s ", ID);
            printf("ID Code: %d %d %d %d\n", idFou, idThr, idTwo, idOne);
            printf("\n\n STRING DE ENTRADA: %s",wiegand);

            uart_puts(uart1, "EVA03WIE26");
            uart_puts(uart1, facilite);
            uart_puts(uart1, ID);

            facilite[2] = '\0';
            facilite[1] = '\0';
            facilite[0] = '\0';
            ID[4] = '\0';
            ID[3] = '\0';
            ID[2] = '\0';
            ID[1] = '\0';
            ID[0] = '\0';
            fOne = 0;
            fTwo = 0;
            idOne = 0;
            idTwo = 0;
            idThr = 0;
            idFou = 0;
            // printf("Facilit:  %s   \n ID: %s ",facilitC,idCode);
            // printf("Facilite: %d%d%d%d%d%d%d%d \n", facilitCI[0], facilitCI[1], facilitCI[2], facilitCI[3], facilitCI[4], facilitCI[5], facilitCI[6], facilitCI[7]);
            //  printf("ID: %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n ", idCodeI[0], idCodeI[1], idCodeI[2],idCodeI[3], idCodeI[4], idCodeI[5], idCodeI[6],idCodeI[7],idCodeI[8],idCodeI[9],idCodeI[10],idCodeI[11],idCodeI[12],idCodeI[13],idCodeI[14],idCodeI[15]);
            wiegand[50] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
            sleep_us(890);
            
        }
        // tentativa de reinicializar;
        
    }
    return 0;
}
char devolveFacilHex(int valor)
{

    if (valor >= 10)
    {
        if (valor == 10)
        {
            return 'A';
        }
        if (valor == 11)
        {
            return 'B';
        }
        if (valor == 12)
        {
            return 'C';
        }
        if (valor == 13)
        {
            return 'D';
        }
        if (valor == 14)
        {
            return 'E';
        }
        if (valor == 15)
        {
            return 'F';
        }
    }
    else if (valor < 10)
    {
        if (valor == 9)
        {
            return '9';
        }
        if (valor == 8)
        {
            return '8';
        }
        if (valor == 7)
        {
            return '7';
        }
        if (valor == 6)
        {
            return '6';
        }
        if (valor == 5)
        {
            return '5';
        }
        if (valor == 4)
        {
            return '4';
        }
        if (valor == 3)
        {
            return '3';
        }
        if (valor == 2)
        {
            return '2';
        }
        if (valor == 1)
        {
            return '1';
        }
        if (valor == 0)
        {
            return '0';
        }
    }
}

int main()
{
    stdio_init_all();
    //  sleep_ms(5000);
    if (!storageInit())
    {
        puts("Storage Faio!");
    }

    multicore_launch_core1(core1Main);
    standby();

    return 0;
}

// int devolveID(int valor){
//     return valor;
// }
