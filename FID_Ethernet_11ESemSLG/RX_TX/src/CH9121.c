#include "CH9121.h"

UCHAR CH9121_LOCAL_IP[4] = {192, 168, 1, 200};    // LOCAL IP
UCHAR CH9121_GATEWAY[4] = {192, 168, 1, 1};      // GATEWAY
UCHAR CH9121_SUBNET_MASK[4] = {255, 255, 255, 0}; // SUBNET MASK
UCHAR CH9121_TARGET_IP[4] = {192, 168, 1, 123};   // TARGET_IP
UWORD CH9121_PORT1 = 700;                        // LOCAL PORT1
UWORD CH9121_TARGET_PORT = 3000;                  // TARGET PORT
UDOUBLE CH9121_BAUD_RATE = 115200;                // BAUD RATE

UCHAR tx[8] = {0x57, 0xAB};

/******************************************************************************
function:	Send four bytes
parameter:
    data: parameter
    command: command code
Info:  Set mode, enable port, clear serial port, switch DHCP, switch port 2
******************************************************************************/
void CH9121_TX_4_bytes(UCHAR data, int command)
{
    for (int i = 2; i < 4; i++)
    {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = data;
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 4; o++)
        uart_putc(UART_ID0, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 4; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	Send five bytes
parameter:
    data: parameter
    command: command code
Info:  Set the local port and target port
******************************************************************************/
void CH9121_TX_5_bytes(UWORD data, int command)
{
    UCHAR Port[2];
    Port[0] = data & 0xff;
    Port[1] = data >> 8;
    for (int i = 2; i < 5; i++)
    {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = Port[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 5; o++)
        uart_putc(UART_ID0, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 5; i++)
        tx[i] = 0;
}
/******************************************************************************
function:	Send seven bytes
parameter:
    data: parameter
    command: command code
Info:  Set the IP address, subnet mask, gateway,
******************************************************************************/
void CH9121_TX_7_bytes(UCHAR data[], int command)
{
    for (int i = 2; i < 7; i++)
    {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = data[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 7; o++)
        uart_putc(UART_ID0, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 7; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	CH9121_TX_BAUD
parameter:
    data: parameter
    command: command code
Info:  Set baud rate
******************************************************************************/
void CH9121_TX_BAUD(UDOUBLE data, int command)
{
    UCHAR Port[4];
    Port[0] = (data & 0xff);
    Port[1] = (data >> 8) & 0xff;
    Port[2] = (data >> 16) & 0xff;
    Port[3] = data >> 24;

    for (int i = 2; i < 7; i++)
    {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = Port[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 7; o++)
        uart_putc(UART_ID0, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 7; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	CH9121_Eed
parameter:
Info:  Updating configuration Parameters
******************************************************************************/
void CH9121_Eed()
{
    tx[2] = 0x0d;
    uart_puts(UART_ID0, tx);
    DEV_Delay_ms(200);
    tx[2] = 0x0e;
    uart_puts(UART_ID0, tx);
    DEV_Delay_ms(200);
    tx[2] = 0x5e;
    uart_puts(UART_ID0, tx);
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
    sleep_ms(xms);
}

void DEV_Delay_us(UDOUBLE xus)
{
    sleep_us(xus);
}

/******************************************************************************
function:	CH9121_init
parameter:
Info:  Initialize CH9121
******************************************************************************/
void CH9121_init(void)
{

    uart_init(UART_ID0, BAUD_RATE);
    uart_init(UART_ID1, BAUD_RATE);
    gpio_set_function(UART_TX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_TX_PIN1, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN1, GPIO_FUNC_UART);

    gpio_init(CFG_PIN);
    gpio_init(RES_PIN);
    gpio_set_dir(CFG_PIN, GPIO_OUT);
    gpio_set_dir(RES_PIN, GPIO_OUT);
    gpio_put(CFG_PIN, 1);
    gpio_put(RES_PIN, 1);

    UCHAR CH9121_LOCAL_IP[4] = {192, 168, 1, 200};    // LOCAL IP
    UCHAR CH9121_GATEWAY[4] = {192, 168, 1, 1};      // GATEWAY
    UCHAR CH9121_SUBNET_MASK[4] = {255, 255, 255, 0}; // SUBNET MASK
    UCHAR CH9121_TARGET_IP[4] = {192, 168, 1, 123};   // TARGET_IP
    UWORD CH9121_PORT1 = 701;                        // LOCAL PORT1
    UWORD CH9121_TARGET_PORT = 700;                  // TARGET PORT
    UDOUBLE CH9121_BAUD_RATE = 115200;                // BAUD RATE
}

/******************************************************************************
function:	RX_TX
parameter:
Info:  Serial port 1 and serial port 2 receive and dispatch
******************************************************************************/


