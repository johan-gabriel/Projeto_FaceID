/*
     Desenvolvimento da Placa de Controle de Catraca (PCC) para sistema Facial;

    Inicio do desenvolvimento - 20/07/2022
    Fim da da primeira versão - 17/08/2022

    Equipe de desenvolvimento:
    Firmware - JOHAN SILVA / BRUNO NUNES
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
     Versão: 1.2E (armazenar o estádo das vars de configuração).
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "CH9121_Test.h"
#include "storage.h"

#define S_LIBERADO 25//Led interno da rasp
#define S_ESQ 10 // Pictograma verde
#define S_DIR 11 // Pictograma verde
#define S_BLOCK 7 // Pictograma vermelho

#define S_BUZZ 6 // BUZER
#define SOL1 15 // SOLENOIDE    
#define SOL2 27 //SOLENOIDE
#define SENSOR2 20 //SENSOR
#define FACE_ID 26 //Sinal de liberardo externo;
#define SENSOR1 21 //SENSOR

void loop(void);
int liberado(int var);
int bloqueado(int blok);
void RX_TX(int mensagem);
void Regula_Tensao();


int contador = 0;
int libBlok = 0;
int blokLib = 0;
int giroGirado = 0;
int autoriza = 0;
int tempo = 0;
int auxTemp = 5000; // tempo reserva; Para conseguir aumentar o tempo de liberação
int inTemp = 0;

int valConfig = 0;

int saida = 0; // sinaliza a mensagem que é enviada
//int indice = 0;
char comand;// comando do catraca
char versFirmware[10] = "1.1E";
int hash,q,valor =0; // vars responsáveis por ler e interpretar a mensagem;
bool hora_ante;
int y = 0; 
int sentido = 0;

int SLGtime = 10000;
bool SLGlibera;

int armario;

bool sinalMisto = false;
bool stateLibera = false;
bool lastStateButton = false;

// Variáveis dos comandos possíveis / comparativos
int main()
{
    stdio_init_all();
    Pico_ETH_CH9121_test();
    //Exemplo de saida
    gpio_init(S_LIBERADO);
    gpio_set_dir(S_LIBERADO, GPIO_OUT);

    gpio_init(S_DIR);
    gpio_set_dir(S_DIR, GPIO_OUT);

    gpio_init(S_ESQ);
    gpio_set_dir(S_ESQ, GPIO_OUT);

    gpio_init(S_BLOCK);
    gpio_set_dir(S_BLOCK, GPIO_OUT);
    gpio_pull_up(S_BLOCK);

    gpio_init(S_BUZZ);
    gpio_set_dir(S_BUZZ, GPIO_OUT);
    gpio_pull_up(S_BUZZ);
    
    gpio_init(SOL2);
    gpio_set_dir(SOL2, GPIO_OUT);
    // Exemplo de saida
    gpio_init(SOL1);
    gpio_set_dir(SOL1, GPIO_OUT);

    //Exemplo de entrada
    gpio_init(FACE_ID);
    gpio_set_dir(FACE_ID, GPIO_IN);
    gpio_pull_up(FACE_ID);

    //Exemplo de entrada
    gpio_init(SENSOR2);
    gpio_set_dir(SENSOR2, GPIO_IN);
    gpio_pull_down(SENSOR2);

    //Exemplo de entrada
    gpio_init(SENSOR1);
    gpio_set_dir(SENSOR1, GPIO_IN);
    gpio_pull_down(SENSOR1);

    sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
        while (true) tight_loop_contents();
    }


    while(1) loop();

    return 0;
}

void loop(void){
    
    bool stateButton = !gpio_get(FACE_ID);
    int liberar = liberado(contador);
    int fimG = liberado(1);
    valConfig = readLog();
    printf("Valor da leitura: %d\n", valConfig);


    // ----------------------- Inicia a verificação ------------------------------
    // Estagio liberado
    if (stateButton == true)
    {
        autoriza = 1;
        inTemp++;
    }
    if (inTemp >= 1510)
    {
        auxTemp = inTemp;
        
       
    }
    if (inTemp < 1400)
    {
        inTemp = 0;
    }
    
    //Tempo de liberação em ms
    if(tempo >= auxTemp)
    {
        autoriza = 0;
        tempo = 0;
        inTemp = 0;
    }
   // printf("|a%d|",auxTemp);
   // printf(" %d \n",inTemp);
   // printf("|t%d|",tempo);
   //Sinal de autorização
    if (autoriza == 1)
    { //incremento do tempo para bloqueio.
        tempo++;
        //printf("Tempo %d\n", tempo);
        contador = 1; // Entra na função liberado e executa os retornos das posições dos sensores.
        liberar = liberado(contador);// var liberado recebe posição dos sensores no momento do giro.
        liberado(contador); // aciona a função liberado
        
        // Após posição meio giro, se finalizou
        if (liberar == 3 && libBlok == 0) {
            libBlok = 1;
            giroGirado = 1;
        }
        // Informa qual o sentido que girou saida ou entrada 
        if (libBlok == 1)
        {   // Sentido Horário
            if(hora_ante){
                if(liberar == 1)
                {
                    libBlok = 2;
                    giroGirado = 1;
                    sentido = 3;
                }
                if (liberar == 2)
                {
                    libBlok = 2;
                    giroGirado =1;
                    sentido = 4;
                }
            }
            // Sentido anti-horário
            else{
                if(liberar == 1)
                {
                    libBlok = 2;
                    giroGirado = 1;
                    sentido = 4; // manda para a função RX_TX no '¹'
                }
                if (liberar == 2)
                {
                    libBlok = 2;
                    giroGirado =1;
                    sentido = 3;
                    
                }
            }
        }  
        if (libBlok == 2){
            if (liberar == 0)
            {
                libBlok = 3;
                giroGirado = 1;  
            } 
        }
        if (libBlok == 3){
            //autoriza = 0;
            bloqueado(0); // Volta a bloquear o giro!
            giroGirado=0; // Reseta os valores para uma nova liberação
            gpio_put(S_ESQ, 0);// Apaga o LED
            gpio_put(S_DIR, 0);
            autoriza = 0;// Reseta os valores para uma nova liberação
            tempo =0;// Reseta os valores para uma nova liberação
            RX_TX(sentido);// '¹'
        }
    }
    // bloqueado no stand by
    if (autoriza == 0 && giroGirado == 0){
        gpio_put(S_LIBERADO, 0);
        gpio_put(S_ESQ, 0);
        //gpio_put(S_DIR, 1);
        contador = 0;
        if (contador == 0){
            bloqueado(contador);
            libBlok = 0;
            blokLib = 0;
            tempo =0;
            
        }   
    }

   // Mantem o sistema liberado se não terminar o giro.
    if (autoriza == 1 && sinalMisto == false){
            gpio_put(S_LIBERADO, 1);
            // meio do giro no liberado, mas travou
        if (autoriza == 0 && giroGirado == 1 && fimG >= 1){
            gpio_put(S_LIBERADO, 0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 0);
            if (giroGirado == 1 && (fimG == 1|| fimG == 2)){
                blokLib = 1;  // sinal de liberar mesmo depois do fim do tempo     
            }
        }
        // Bloqueia o sistema novamente.s
        if (blokLib == 1 && fimG ==0)
        {   gpio_put(S_LIBERADO,0);
            bloqueado(0);
            giroGirado =0;
            gpio_put(S_ESQ, 0);
            gpio_put(S_DIR, 0);
            tempo =0;
            autoriza = 0; 
        }
        // Mantem Liberado
        if (contador == 0 && giroGirado == 1){
            gpio_put(S_LIBERADO,0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 0);
        }
        // Mantem liberado
        if (contador == 1 && libBlok == 0)
        {
            gpio_put(S_LIBERADO,0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 0);
            if (libBlok == 3 && contador == 1)
            { 
                gpio_put(S_LIBERADO,0);
                gpio_put(S_ESQ, 0);
               // gpio_put(S_DIR, 1);
               tempo =0;
                autoriza = 0;
            }
            
        }
    }  
    //--------------------------- FIM DO LOOP --------------------------------------
// ----------------------------- LEITURA QUE VEM DO CATRACA ------------------------
    while (uart_is_readable(UART_ID1))
    {

        comand = uart_getc(UART_ID1);
//---------- Verifica Letra por Letra ----------
        if(comand == '#')
        {hash = 1; } // habilita o dado para entrada em *
        if(comand == 'S')
        {   q = 5;
            valor = valor + q; // acumula a soma dos valores_letra para verificar a função.
            q=0; 
        }
        if(comand == 'G')
        {   q = 18;
            valor = valor + q;
            q=0; 
        }
        if(comand == 'P')
        {   q = 18;
            valor = valor + q;
            q=0; 
        }
        if(comand == 'B')
        {   q = 12;
            valor = valor + q;
            q=0; 
        }
        if(comand == 'U')
        {   q = 13;
            valor = valor + q; 
            q=0; 
        }
        if(comand == 'Z')
        {   q = 14;
            valor = valor + q;
            q=0; 
        }
        if(comand == 'R')
        {   q = 10;
            valor = valor + q;
            q=0; 
        }
        if(comand == 'T')
        {   q = 9;
            valor = valor + q;
            q=0; 
        } 
        if(comand == 'F')
        {   q = 7;
            valor = valor + q;
            q=0; 
        } 
        if(comand == 'W')
        {   q = 6;
            valor = valor + q;
            q=0; 
        } 
        if (comand == 'V')
        {
            hash =0;
        }
        if(comand == 'H')
        {
            q = 18;
            valor = valor + q;
            q = 0;
        }
        if(comand == 'h')
        {
            q = 1;
            valor = valor + q;
            q = 0;
        }
        if(comand == 'a')
        {
            q = 2;
            valor = valor + q;
            q = 0;
        }
        if(comand == 'v')
        {
            q = 3;
            valor = valor + q;
            q = 0;
        }
        if (comand == '-')
        {
            q=200;
            valor = valor + q;
            q=0;
        }
        

//---------- Verifica Letra por Letra ----------
// -------- Executa a Função do comando --------
        if (uart_is_writable(UART_ID0))
        {
           // printf("Valor: %d", valor);
            if(hash == 1){// *
                if (valor)
                {
                    RX_TX(valor); // imprime e executa a função do comando.
                }
            }
            if(valor > 200){
                valor = 0;
            // Limpa o acumulador valor para proximos comandos.
            }
        }   
    }
//------------------------------ FIM DA LEITURA ------------------------------------
}
//Controla a liberação (Na praica o sinal de giro)
int liberado(int var){
    //gpio_put(S_LIBERADO, 1);
    int aux = 0;
    int gaveta = 0;
    if (var == 1 && aux == 0)
    {   
        // S1 = 0 && S2 = 0 
        if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2)){
            aux = 0;
           // printf("%d", aux);
            return aux;}

        // S1 = 1 and S2 = 0 
        if (gpio_get(SENSOR1) && !gpio_get(SENSOR2)){
            aux = 2;
           // printf("S1 %d", aux);
            return aux;}
        //S1 = 0 and S2 = 1
        if(!gpio_get(SENSOR1) && gpio_get(SENSOR2)){
            aux = 1;
           // printf("S2 %d", aux);
            return aux;
        }

            // S1 = 1 and S2 = 1
        if (gpio_get(SENSOR1) && gpio_get(SENSOR2)){
            aux = 3; 
            //printf("S1 + S2 %d", aux);
            return aux;
        }
        //printf(" G: %d ",gaveta);
    } 
    
}
// controla bloqueio
int bloqueado (int blok){

      if(blok == 0) {
        tempo =0;
      // Acionamento S1
        if (gpio_get(SENSOR1))
        {   
            gpio_put(SOL2, 1);
            storageLog("0;0;1\n");
            // pictograma Vermelho
            gpio_put(S_BLOCK,1);
            armario = 1;
            gpio_put(S_BUZZ, 1);
            Regula_Tensao();
            blokLib = 0;
            RX_TX(1);
        }
        else{
            saida = 0;
             gpio_put(SOL2, 0);
             gpio_put(S_BLOCK,0);
             gpio_put(S_BUZZ, 0);
            //printf("ACIONOU O S1\n");
            RX_TX(0);
        }
        // Acionamento S2
         if (gpio_get(SENSOR2))
        {
            
            gpio_put(SOL1, 1);
            // pictograma Vermelho
            gpio_put(S_BLOCK,1);
            armario = 2;
            gpio_put(S_BUZZ, 1);
            Regula_Tensao();
            blokLib = 0;
            RX_TX(1);
            
        }
        else{
            
             gpio_put(SOL1, 0);
             gpio_put(S_BLOCK,0);
             gpio_put(S_BUZZ, 0);
             //printf("ACIONOU O S1\n");
             RX_TX(0);
        }
    }
}
// Controle de mensagem
void RX_TX(int mensagem)
{
        switch (mensagem)
        {
            //Confirmação do giro Bloqueio;
            case 1:
                uart_puts(UART_ID0, "RPAb");
                sleep_ms(300);
                mensagem = 0;
                valor = 0;
                break;
            case 2:
                mensagem = 0;
                break;
            //Confirmação do giro Entrada
            case 3:
                uart_puts(UART_ID0, "RPAe");
                sleep_ms(1);
                mensagem = 0;
                valor = 0;
                break;
            //Confirmação do giro Saida
            case 4:
                uart_puts(UART_ID0, "RPAs");
                sleep_ms(1);
                mensagem = 0;
                valor = 0;
                break;
            //Função Buzzer
            case 39:
                gpio_put(S_BUZZ, 1);
                Regula_Tensao();
                uart_puts(UART_ID0,"BUZok");
                sleep_ms(50);
                gpio_put(S_BUZZ, 0);
                valor = 0;
                break;
            // função RST
            case 21:
                uart_puts(UART_ID0,"RSTok");
                valor = 0;
                hash=0;
                break;
            case 28:
                uart_puts(UART_ID0,"RPAok");
                valor = 0;
                hash=0;
                break;
            //Versão firmware;
            case 31:
                uart_puts(UART_ID0,"GFW:");
                printf(versFirmware);
                valor = 0;
                hash =0;
                break;
            //Retorno do sentido do giro
            case 41:
                uart_puts(UART_ID0,"SSH");
                if (hora_ante)
                {
                    printf("h");
                }else{
                    printf("a");
                }
                valor = 0;
                hash =0;
                break;
            //Set de sentido de giro Horário
            case 29:
                uart_puts(UART_ID0,"SSHhok");
                hora_ante = true;
                valor = 0;
                hash = 0;
                break;
            //Set de sentido de giro Ante-horário 
            case 30:
                uart_puts(UART_ID0,"SSHaok");
                hora_ante = false;
                valor = 0;
                hash = 0;
                break;
            case 57:
                uart_puts(UART_ID0,"SLGok");
                valor = 0;
                hash =0;
                break;
            default:
                mensagem = 0;
                break;
        }       
}


// Regula a tensão de saida
void Regula_Tensao(){
     printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
            printf(NULL);
}