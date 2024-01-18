#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"
#include "storage.h"
#include "CH9121.h"
#include "SP.h"

int valConfig = 0;
int whichConfig = 0;
bool configurador = false;
void standby(){
    configurador = false;
    while (1)
    { 
        
            CH9121_init();
            RX_TX();
         
    }
     

}