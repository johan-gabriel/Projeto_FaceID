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
#include <pico/multicore.h> // inclue a biblioteca multicore 

#define LED_PIN PICO_DEFAULT_LED_PIN

void loop(void);
void loop1(void);

void core1Main(){

    
    while (1) loop1();
   
}
int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);
    
    multicore_launch_core1(core1Main);

    while(1) loop();

    return 0;
}

void loop(void){
    
    gpio_put(LED_PIN, !gpio_get(LED_PIN));
    sleep_ms(250);
    
    //--------------------------- FIM DO LOOP --------------------------------------

}

void loop1(void){
    printf("\nOlá sengudo nucleo!!!!");
    sleep_ms(2000);
}