/*
    Desenvolvimento da Placa de Controle de Catraca (PCC) para sistema Facial;

    Inicio do desenvolvimento - 20/07/2022
    Fim da da primeira versão - 17/08/2022

    Equipe de desenvolvimento:
    Firmware - JOHAN SILVA / BRUNO NUNES
    Hardware - BRUNO RIBEIRO

    Sistema desenvolvido para a empresa PiscoIDC, para o equipamento FACEID - CONTROLID
    para controle de catracas de controle de acesso.

*/


#include <stdio.h>
#include <pico/stdlib.h>

#define S_LIBERADO 25
#define S_ESQ 10
#define S_DIR 11
#define S_BLOCK 12
#define S_BUZZ 6
#define SOL1 15
#define SOL2 14
#define SENSOR2 20
#define FACE_ID 17
#define SENSOR1 21

void loop(void);
int liberado(int var);
int bloqueado(int blok);


int contador = 0;
int libBlok = 0;
int blokLib = 0;
int giroGirado = 0;
int sslOn = 0;
int autoriza = 0;
int tempo = 0;
int auxTemp = 100000;
int inTemp = 0;


bool sinalMisto = false;
bool stateLibera = false;
bool lastStateButton = false;


int main()
{
    stdio_init_all();
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


    while(1) loop();

    return 0;
}

void loop(void){
    
    bool stateButton = !gpio_get(FACE_ID);
    int liberar = liberado(contador);
    int fimG = liberado(1);
    
    // ----------------------- Inicia a verificação ------------------------------
    // Estagio liberado
    if (stateButton == true)
    {
        autoriza = 1;
        inTemp++;
    }
    if (inTemp >= 1500)
    {
        auxTemp = inTemp;
    }
    
    //Tempo de liberação em ms
    if(tempo > auxTemp){
        autoriza = 0;
        tempo = 0;
        inTemp = 0;
        
    }
    printf(" %d \n",auxTemp);
    printf(" %d \n",inTemp);
    if (autoriza == 1)
    { 
        tempo++;
        printf("Tempo %d\n", tempo);
        contador = 1;
        liberar = liberado(contador);
        liberado(contador);
        
        // Bloqueia giro > 1
        if (liberar == 3 && libBlok == 0) {
            libBlok = 1;
            giroGirado = 1;
        }  
        if (libBlok == 1)
        {
            if(liberar == 1 || liberar == 2){
                libBlok = 2;
                giroGirado = 1;
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
            bloqueado(0);
            giroGirado=0;
            gpio_put(S_ESQ, 0);
            gpio_put(S_DIR, 0);
            autoriza = 0;
            tempo =0;
            
        }
        //gpio_put(S_LIBERADO, 1);
        
    }
    // bloqueado no stand by
    if (autoriza == 0 && giroGirado == 0){
        gpio_put(S_LIBERADO, 0);
        gpio_put(S_ESQ, 0);
        gpio_put(S_DIR, 0);
        contador = 0;
        if (contador == 0){
            bloqueado(contador);
            libBlok = 0;
            blokLib = 0;
        }   
    }
   
     if (autoriza == 1 && sinalMisto == false){
            gpio_put(S_LIBERADO, 1);
            // meio do giro no liberado, mas travou
        if (autoriza == 0 && giroGirado == 1 && fimG >= 1){
            gpio_put(S_LIBERADO, 0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 1);
            if (giroGirado == 1 && (fimG == 1|| fimG == 2)){
                blokLib = 1;
                      
            }
        }
        if (blokLib == 1 && fimG ==0)
        {   gpio_put(S_LIBERADO,0);
            bloqueado(0);
            giroGirado =0;
            gpio_put(S_ESQ, 0);
            gpio_put(S_DIR, 0);
            autoriza = 0;
            
                
        }
        if (contador == 0 && giroGirado == 1){
            gpio_put(S_LIBERADO,0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 1);
        }
        if (contador == 1 && libBlok == 0)
        {
            gpio_put(S_LIBERADO,0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            gpio_put(S_ESQ, 1);
            gpio_put(S_DIR, 1);
            if (libBlok == 3 && contador == 1)
            { 
                gpio_put(S_LIBERADO,0);
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 0);
                autoriza = 0;
            }
            
        }
    }

    
    //--------------------------- FIM DO LOOP --------------------------------------

}

int liberado(int var){
    //gpio_put(S_LIBERADO, 1);
    int aux = 0;
    int gaveta = 0;
    if (var == 1 && aux == 0)
    {   
        // S1 = 0 && S2 = 0 
        if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2)){
            aux = 0;
            //printf("%d", aux);
            return aux;}

        // S1 = 1 and S2 = 0 
        if (gpio_get(SENSOR1) && !gpio_get(SENSOR2)){
            aux = 2;
            //printf("S1 %d", aux);
            return aux;}
        //S1 = 0 and S2 = 1
        if(!gpio_get(SENSOR1) && gpio_get(SENSOR2)){
            aux = 1;
            //printf("S2 %d", aux);
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
int bloqueado (int blok){
    //gpio_put(S_LIBERADO, 0);
      if(blok == 0) {
        if(sslOn == 0){
        if (gpio_get(SENSOR2))
        {
            gpio_put(SOL2, 1);

            // pictograma Vermelho
            gpio_put(S_BLOCK,true);
            gpio_put(S_BUZZ, 1);
            printf("ACIONOU O S1\n");
            sinalMisto = true;
            blokLib = 0;
        }
        else{
             gpio_put(SOL2, 0);
             gpio_put(S_BLOCK,0);
             gpio_put(S_BUZZ, 0);
             sinalMisto = false;
        }
        }
        if(gpio_get(SENSOR1)){
            sslOn = 1;
        }
        else{
            sslOn = 0;
        }
      }
       
}

