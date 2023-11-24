#include <stdio.h>
#include <pico/stdlib.h>

#define LED_PIN 25
#define LEDLED_PIN 15
#define AMARELO_PIN 14
#define ENT2_PIN 20
#define BUTTON_PIN 16
#define ENTRADA_PIN 21

void loop(void);
int liberado(int var, int soma);


int aux;
int gaveta = 0;
int contador = 0;

bool stateLibera = false;
bool lastStateButton = false;

int main()
{
    stdio_init_all();
    //Exemplo de saida
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(AMARELO_PIN);
    gpio_set_dir(AMARELO_PIN, GPIO_OUT);
    // Exemplo de saida
    gpio_init(LEDLED_PIN);
    gpio_set_dir(LEDLED_PIN, GPIO_OUT);

    //Exemplo de entrada
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    //Exemplo de entrada
    gpio_init(ENT2_PIN);
    gpio_set_dir(ENT2_PIN, GPIO_IN);
    gpio_pull_up(ENT2_PIN);

    //Exemplo de entrada
    gpio_init(ENTRADA_PIN);
    gpio_set_dir(ENTRADA_PIN, GPIO_IN);
    gpio_pull_up(ENTRADA_PIN);


    while(1) loop();

    return 0;
}

void loop(void){
    
    bool stateButton = !gpio_get(BUTTON_PIN);
    int altState =0;
     printf (" R: %d\n ", altState);
    if (stateButton == true)
    {
        contador = 1;
       
    }else{
        contador = 0;
    }

    liberado(contador, altState);

    //printf(" F: %d\n",liberado(contador));

    if (contador == 1 || aux > 0)
    {
        
      // S1 = 0 && S2 = 0 
       gpio_put(LED_PIN, 1);
       if (gpio_get(ENTRADA_PIN) && gpio_get(ENT2_PIN)){
            aux = 0;
            printf("%d", aux);}
       
        // S1 = 1 and S2 = 0 
        if (!gpio_get(ENTRADA_PIN) && gpio_get(ENT2_PIN)){
            aux = 2;
            printf("%d", aux);}
        //S1 = 0 and S2 = 1
        if(gpio_get(ENTRADA_PIN) && !gpio_get(ENT2_PIN)){
            aux = 1;
            printf("%d", aux);
        }
        
        // S1 = 1 and S2 = 1
        if (!gpio_get(ENTRADA_PIN) && !gpio_get(ENT2_PIN)){
            aux = 3; 
            printf("%d", aux);}

        

        if (aux == 3)
        {
            gaveta = 3;
           
           
        }

         if (gaveta == 3)
            {
                if (gpio_get(ENTRADA_PIN) && gpio_get(ENT2_PIN))
                {
                    aux = 4;
                    printf("%d", aux);
                   // sleep_ms(10);
                }
                if (aux == 4)
                {
                    contador = 0;
                    gaveta = 0;
                     gpio_put(LED_PIN, 0);
                    if (!gpio_get(ENTRADA_PIN))
                    {
                        gpio_put(AMARELO_PIN, 1);
                        
                    }
                    else{
                        gpio_put(AMARELO_PIN, 0);
                    }
                    if (!gpio_get(ENT2_PIN))
                    {
                        gpio_put(LEDLED_PIN, 1);
                        
                    }
                    else{
                        gpio_put(LEDLED_PIN, 0);
                    }
                                
                    
                }
                
            }
         printf(" G: %d ", gaveta);
    } 
    if(contador == 0) {
        gpio_put(LED_PIN, 0);
        if (!gpio_get(ENTRADA_PIN))
        {
            gpio_put(AMARELO_PIN, 1);
            
        }
        else{
             gpio_put(AMARELO_PIN, 0);
        }
         if (!gpio_get(ENT2_PIN))
        {
            gpio_put(LEDLED_PIN, 1);
            
        }
        else{
             gpio_put(LEDLED_PIN, 0);
        }
    }

    sleep_ms(100);

}

int liberado(int var, int soma){

    int R = 0;
    int ant = 0;
    int branco;

    if (var !=  ant) {
        branco = ant;
        ant = var;
    }else{
        branco = 1;
    }
    if (branco == 0)
    {
        while (R != 5)
        {
            R++;
        }
        R=0;
        
    }
    

    printf (" ANT: %d ", ant);
    printf (" Branco: %d\n ", branco);
    printf (" R: %d\n ", R);
   
    return R ;
}


