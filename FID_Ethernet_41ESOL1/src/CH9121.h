#ifndef _CH9121_H_
#define _CH9121_H_

#include <stdlib.h> // malloc() free()
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"

#define RESET_PIN 28// Pino nRESET (pino 28) na Raspberry Pi Pico
#define LED_PIN 25

/// \tag::uart_advanced[]
#define UART_ID0 uart0
#define UART_ID1 uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN0 0
#define UART_RX_PIN0 1
#define UART_TX_PIN1 4
#define UART_RX_PIN1 5
#define UART_RX_PIN2 9 // 2° pino de leitura da UART1

#define CFG_PIN 14
#define RST_PIN 17
#define GPIO_OUT 1
#define GPIO_IN 0

#define UCHAR unsigned char
#define UBYTE uint16_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

#define TCP_SERVER 0
#define TCP_CLIENT 1
#define UDP_SERVER 2
#define UDP_CLIENT 3

#define READ_CHIP_VERSION_NUMBER 0x01 // Query chip version number
#define RESET_CHIP 0x02               // Query chip version number

/*********************************************************************************************************************************************************************
READ MODULE NETWORK PARAMETERS --> LÊ OS PARÂMETROS DE REDE DO MÓDULO
*********************************************************************************************************************************************************************/
#define READ_CHIP_IP_ADDRESS 0x61 // Read the chip IP address
#define READ_CHIP_MASK 0x62       // Read chip mask
#define READ_CHIP_GATEWAY 0x63    // Read chip gateway
#define GET_CHIP_MAC_ADDRESS 0x81 // Get chip MAC address

/*********************************************************************************************************************************************************************
SET THE NETWORK CONFIGURATIONS OF PORT 1 --> DEFINIR AS CONFIGURAÇÕES DE REDE DA PORTA 1
*********************************************************************************************************************************************************************/
#define Mode1 0x10               // Port 1: Setup Mode   0x00:TCP Server 0x01:TCP Client 0x02:UDP Server 0x03:UDP Client
#define LOCAL_IP 0x11            // Local IP
#define SUBNET_MASK 0x12         // Subnet Mask
#define GATEWAY 0x13             // Gateway
#define LOCAL_PORT1 0X14         // Port 1:Local Port
#define TARGET_IP1 0x15          // Port 1:Target IP
#define TARGET_PORT1 0x16        // Port 1:Target Port
#define PORT_RANDOM_ENABLE1 0x17 // Port 1:Port Random Enable
#define UART1_BAUD1 0x21         // Port 1:Baud rate of serial port 1

/*********************************************************************************************************************************************************************
READ PORT 1 NETWORK SETTINGS --> LÊ AS CONFIGURAÇÕES DE REDE DA PORTA 1
*********************************************************************************************************************************************************************/
#define READ_WORKING_MODE_PORT1 0x60                // Port 1: Read the working mode of chip port 1 --> 0x00:TCP Server 0x01:TCP Client 0x02:UDP Server 0x03:UDP Client
#define READ_CHIP_SOURCE_PORT_PORT1 0x64            // Read chip port 1 source port
#define READ_DESTINATION_IP_ADDRESS_CHIP_PORT1 0x65 // Read the destination IP address of chip port 1
#define READ_CHIP_DESTINATION_PORT_PORT1 0x66       // Read the destination port number of chip port 1
#define READ_BAUD_RATE_PORT1 0x71                   // Read port 1 serial port baud rate
#define READ_CHECK_BIT_DATA_BIT_STOP_BIT_PORT1 0x72 // Read port 1 serial port check bit data bit stopbit
#define READ_TIMEOUT_TIME_PORT1 0x73                // Read port 1 serial port timeout time

/*********************************************************************************************************************************************************************
SET THE NETWORK CONFIGURATIONS OF PORT 2 --> DEFINIR AS CONFIGURAÇÕES DE REDE DA PORTA 2
*********************************************************************************************************************************************************************/
#define ON_OFF_UART2 0X39        // Port 2: Enable/disable port 2
#define Mode2 0x40               // Port 2: Setup Mode   0x00:TCP Server 0x01:TCP Client 0x02:UDP Server 0x03:UDP Client
#define LOCAL_PORT2 0X41         // Port 2:Local Port
#define TARGET_IP2 0x42          // Port 2:Target IP
#define TARGET_PORT2 0x43        // Port 2:Target Port
#define UART1_BAUD2 0x44         // Port 2:Baud rate of serial port 2
#define PORT_RANDOM_ENABLE2 0x17 // Port 2:Port Random Enable
#define READ_IP2 0x92            // Read the destination IP address of chip port 2

/*********************************************************************************************************************************************************************
READ PORT 2 NETWORK SETTINGS --> LÊ AS CONFIGURAÇÕES DE REDE DA PORTA 2
*********************************************************************************************************************************************************************/
#define READ_WORKING_MODE_PORT2 0x90                // Port 2: Read the working mode of chip port 2 --> 0x00:TCP Server 0x01:TCP Client 0x02:UDP Server 0x03:UDP Client
#define READ_CHIP_SOURCE_PORT_PORT2 0x91            // Read chip port 2 source port
#define READ_DESTINATION_IP_ADDRESS_CHIP_PORT2 0X92 // Read the destination IP address of chip port 2
#define READ_CHIP_DESTINATION_PORT_PORT2 0X93       // Read the destination port number of chip port 2
#define READ_BAUD_RATE_PORT2 0X94                   // Read port 2 serial port baud rate
#define READ_CHECK_BIT_DATA_BIT_STOP_BIT_PORT2 0X95 // Read port 2 serial port check bit data bit stopbit
#define READ_TIMEOUT_TIME_PORT2 0X96                // Read port 2 serial port timeout time

/*********************************************************************************************************************************************************************
DECLARES THE PROTOTYPES OF THE FUNCTIONS OF THE .C FILE --> DECLARA OS PROTÓTIPOS DAS FUNÇÕES DO ARQUIVO .C
*********************************************************************************************************************************************************************/
void CH9121_TX_4_bytes(UCHAR data, int command);
void CH9121_TX_5_bytes(UWORD data, int command);
void CH9121_TX_7_bytes(UCHAR data[], int command);
void CH9121_TX_BAUD(UDOUBLE data, int command);
void CH9121_Eed();
void CH9121_init(void);
void RX_TX();
void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

#endif