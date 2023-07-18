#ifndef UARTAUX_H
#define UARTAUX_H

#include <stdio.h>
#include <stdlib.h>


#define UART1 uart0
#define UART2 uart1

#define UART_TX_PIN0 0
#define UART_RX_PIN0 1
#define UART_TX_PIN1 4
#define UART_RX_PIN1 5

#define CFG_PIN 14
#define RES_PIN 17
#define GPIO_OUT 1
#define GPIO_IN 5

#define VSCHIP 0x01 // query chip version
#define RESET_CHIP 0x02 // RESET the chip
#define TCPPORT2 0x04 // query port 2 TCP conection status
#define SAVEEP 0x0d // Save parameter to EEPROM
#define EXEC 0x0e // execute the config command and Reset the CH9121
#define LEAVES 0x5e // Leave serial port config mode
#define IP_SET 0x11 // set chip IP
#define SUBM_SET 0x12 // set chip mask
#define GATEW_SET 0x13 // set chip gateway
#define DHCP_ON_OFF 0x33 // Turn on/off the DHCP function use the 0x01 to open or the 0x00 to close the MDF mode :)
#define OPEN_PORT2 0x39 // Turn on/off the port 2; Use the 0x01 or the 0x00 close
#define SET_P2_NWMODE 0x40 // Set chip port 2 network mode; Use the 00 -> TCP S; Use the 01 -> TCP C; Use the  02 -> UDP S; Use the 03 -> UDP C
#define SOURCE_P2 0x41 // set the port number
#define IP_SERVER 0x42 // Set the ip destination of chip port 2
#define PORT_DEST 0x43 // set chip port 2 destination port
#define BOUD 0x44 // set port2 serial port boud rate
#define READ_IP_CHIP 0x61 //Read the chip IP address
#define READ_SUBM_CHIP 0x62 //Read chip mask
#define READ_GATEW 0x63 // Read chip Gateway


#endif