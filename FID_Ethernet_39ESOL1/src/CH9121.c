#include "CH9121.h"
#include "storage.h"

UCHAR CH9121_LOCAL_IP[4] = {};    // LOCAL IP
UCHAR CH9121_GATEWAY[4] = {};     // GATEWAY
UCHAR CH9121_SUBNET_MASK[4] = {}; // SUBNET MASK
UCHAR CH9121_TARGET_IP[4] = {};   // TARGET_IP
UWORD CH9121_PORT1 = 2000;        // LOCAL PORT1
UWORD CH9121_TARGET_PORT = 700;   // TARGET PORT
UDOUBLE CH9121_BAUD_RATE = 9600;  // BAUD RATE

UCHAR CH9121_VERSION_NUMBER = 0x01; // CHIP VERSION NUMBER

UCHAR tx[8] = {0x57, 0xAB};
UCHAR open_configuration[4] = {0x55, 0xAA, 0x5A};
int salva_e_reinicia = 0;
int le_e_salva = 0;
#define S_LIBERADO 25 // Led interno da rasp
#define S_ESQ 10      // Pictograma verde
#define S_DIR 11      // Pictograma verde
#define S_BLOCK 27    // Pictograma vermelho
#define S_STANDBY 26  // luz azul

#define S_BUZZ 6     // BUZER
#define SOL1 15      // SOLENOIDE
#define SOL2 22      // SOLENOIDE
#define SENSOR2 20   // SENSOR
#define FACE_ID_1 19 // Sinal de liberar do externo;
#define FACE_ID_2 18 // Sinal de liberar do externo;
#define SENSOR1 21   // SENSOR

void loop(void);
int liberado(int var);
int bloqueado(int blok);
void RX(int mensagem);
void Regula_Tensao();

bool sinaliza();
// Vars from Johan
int sisConfig = 1;

int contador = 0;
int libBlok = 0;
int blokLib = 0;
int giroGirado = 0;
int autoriza = 0;
int tempo = 0;
int auxTemp = 3000; // tempo reserva; Para conseguir aumentar o tempo de liberação
int inTemp = 0;

int saida = 0; // sinaliza a mensagem que é enviada
// int indice = 0;
char comand; // comando do catraca
char versFirmware[10] = "FD-3.9";
int hash, q, valor = 0; // vars responsáveis por ler e interpretar a mensagem;
bool hora_ante = true;
int y = 0;
int sentido = 0;
int sslOn = 0;

bool sinalMisto = false;
bool stateLibera = false;
bool lastStateButton = false;
bool abre = false;
bool SPP = false;
// Novo
char antSSL = 'f';
char antSSH = 'a';
bool stateConfig = true;
bool alternador = false;
int valorConfig2 = 0;
int valorSNE = 0;
char stringSNE[2] = "00";
bool mensagemInit = true;
bool trava_RPAn = false;
bool naoBlock = false;
bool sslOff = false;

int inicializa = 0;

char stringConfig[] = "0;0;0;0;00;\n";

char antSPG = 'p';
int antSNE = 48;
int antSNEu = 49;

// Novo QR
UCHAR caractere[18] = {};
int posicao = 1;
bool seleciona_leitor = false;
int segura_segunda_leitura_qrcode = 0;
char RPA = '\0';
int timer1, timer2; // Não utilizado
bool termino_leitura_qr = false;
int reseta_posicao = 0;

// Novo GNE
int SNE = 1;
int dezena_SNE = 48;
int unidade_SNE = 49;
int mensagem_inicializacao = 2;
int segura_primeiro_envio = 0;
// end
/******************************************************************************
function:	Open configuration mode
parameter:
    data: don't have parameters
    command: command code
Info:
******************************************************************************/
void CH9121_Open_Config(UCHAR teste)
{
    uart_putc(UART_ID0, open_configuration[0]);
    DEV_Delay_ms(10);
    uart_putc(UART_ID0, open_configuration[1]);
    DEV_Delay_ms(10);
    uart_putc(UART_ID0, open_configuration[2]);
    DEV_Delay_ms(10);
}

void CH9121_Read_4_bytes()
{
    for (int o = 0; o < 3; o++)
        uart_getc(UART_ID0);
    DEV_Delay_ms(10);
    for (int i = 2; i < 4; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	Send three bytes
parameter:
    data: don't have parameters
    command: command code
Info:
******************************************************************************/
void CH9121_TX_3_bytes(int command)
{
    for (int i = 2; i < 3; i++)
    {
        if (i == 2)
            tx[i] = command;
    }
    le_e_salva++;
    DEV_Delay_ms(10);
    for (int o = 0; o < 3; o++)
        uart_putc(UART_ID0, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 4; i++)
        tx[i] = 0;
}

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
    salva_e_reinicia++;
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
/**
 * delay x us
 **/
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
    // Defina o estado inicial do pino
    bool initial_state = true; // Substitua true por false se desejar um estado inicial baixo (LOW)

    // stdio_init_all();
    gpio_init(RESET_PIN);
    gpio_set_dir(RESET_PIN, GPIO_OUT);
    gpio_put(RESET_PIN, initial_state);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 1);
    DEV_Delay_ms(100);
    gpio_put(LED_PIN, 0);
    DEV_Delay_ms(100);

    uart_init(UART_ID0, BAUD_RATE);
    uart_init(UART_ID1, BAUD_RATE);
    gpio_set_function(UART_TX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_TX_PIN1, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN1, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN2, GPIO_FUNC_UART);

    gpio_init(CFG_PIN);
    gpio_init(RST_PIN);
    gpio_set_dir(CFG_PIN, GPIO_OUT);
    gpio_set_dir(RST_PIN, GPIO_OUT);
    gpio_put(CFG_PIN, 1);
    gpio_put(RST_PIN, 1);
    gpio_init(S_LIBERADO);
    gpio_set_dir(S_LIBERADO, GPIO_OUT);

    gpio_init(S_DIR);
    gpio_set_dir(S_DIR, GPIO_OUT);

    gpio_init(S_STANDBY);
    gpio_set_dir(S_STANDBY, GPIO_OUT);

    gpio_init(S_ESQ);
    gpio_set_dir(S_ESQ, GPIO_OUT);

    gpio_init(S_BLOCK);
    gpio_set_dir(S_BLOCK, GPIO_OUT);
    gpio_pull_up(S_BLOCK);

    gpio_init(S_BUZZ);
    gpio_set_dir(S_BUZZ, GPIO_OUT);
    gpio_pull_up(S_BUZZ);

    mensagem_inicializacao = 1;

    gpio_init(SOL2);
    gpio_set_dir(SOL2, GPIO_OUT);
    // Exemplo de saida
    gpio_init(SOL1);
    gpio_set_dir(SOL1, GPIO_OUT);

    // Exemplo de entrada
    gpio_init(FACE_ID_1);
    gpio_set_dir(FACE_ID_1, GPIO_IN);
    gpio_pull_up(FACE_ID_1);

    // Exemplo de entrada
    gpio_init(FACE_ID_2);
    gpio_set_dir(FACE_ID_2, GPIO_IN);
    gpio_pull_up(FACE_ID_2);

    // Exemplo de entrada
    gpio_init(SENSOR2);
    gpio_set_dir(SENSOR2, GPIO_IN);
    gpio_pull_down(SENSOR2);

    // Exemplo de entrada
    gpio_init(SENSOR1);
    gpio_set_dir(SENSOR1, GPIO_IN);
    gpio_pull_down(SENSOR1);
}

/******************************************************************************
function:	RX_TX
parameter:
Info:  Serial port 1 and serial port 2 receive and dispatch
******************************************************************************/
// Vars from Ribas
int i = 0;
UCHAR comando[39];
signed int indice;
int resultado;
int valida_parametros = 0;

char SFC = 's';
char SLE[3] = {'o', 'f', 'f'};
char SPG = 'p';
int SPI = 2;
int STE = 5;
int dezena_STE;
int unidade_STE;
int aux_STE = 0;
char SSH = 'a';
char STG = 'i';
char SSL[3] = {'o', 'f', 'f'};
char STL[3] = {'2', '3', '2'};
int exibe_GTL = 0;
char SPX = 'w';
int GMA_valido = 0;
int GIP_valido = 0;
int GIS_valido = 0;
int GGW_valido = 0;
int GMR_valido = 0;
int dados_4_bytes = 0;
int dados_6_bytes = 0;
int get_dados = 0;
char *mensagem;
char ASN1, ASN2, ASN3;
int destrava = 0;
int d_mais = 0;
int d_menos = 0;

int primeira_faixa_ip;
int segunda_faixa_ip;
int terceira_faixa_ip;
int quarta_faixa_ip;
int primeira_faixa_is;
int segunda_faixa_is;
int terceira_faixa_is;
int quarta_faixa_is;
int primeira_faixa_gateway;
int segunda_faixa_gateway;
int terceira_faixa_gateway;
int quarta_faixa_gateway;
int primeira_faixa_mascara_de_rede;
int segunda_faixa_mascara_de_rede;
int terceira_faixa_mascara_de_rede;
int quarta_faixa_mascara_de_rede;

int Trpa = 10000;
int help_gsl = 0;
int help_gsh = 0;

void zera_comando()
{
    get_dados = 0;
    valida_parametros = 0;
    le_e_salva = 0;
    GMA_valido = 0;
    GIP_valido = 0;
    GIS_valido = 0;
    GGW_valido = 0;
    GMR_valido = 0;
    dados_4_bytes = 0;
    dados_6_bytes = 0;

    comando[0] = '#';
    comando[1] = 'I';
    comando[2] = 'D';
    comando[3] = 'Y';
    comando[4] = '\0';
    comando[5] = '\0';
    comando[6] = '\0';
    comando[7] = '\0';
    comando[8] = '\0';
    comando[9] = '\0';
    comando[10] = '\0';
    comando[11] = '\0';
    comando[12] = '\0';
    comando[13] = '\0';
    comando[14] = '\0';
    comando[15] = '\0';
    comando[16] = '\0';
    comando[17] = '\0';
    comando[18] = '\0';
    comando[19] = '\0';
    comando[20] = '\0';
    comando[21] = '\0';
    comando[22] = '\0';
    comando[23] = '\0';
    comando[24] = '\0';
    comando[25] = '\0';
    comando[26] = '\0';
    comando[27] = '\0';
    comando[28] = '\0';
    comando[29] = '\0';
    comando[30] = '\0';
    comando[31] = '\0';
    comando[32] = '\0';
    comando[33] = '\0';
    comando[34] = '\0';
    comando[35] = '\0';
    comando[36] = '\0';
    comando[37] = '\0';
    comando[38] = '\0';
    comando[39] = '\0';
}

void apaga_qr()
{
    caractere[0] = '\0';
    caractere[1] = '\0';
    caractere[2] = '\0';
    caractere[3] = '\0';
    caractere[4] = '\0';
    caractere[5] = '\0';
    caractere[6] = '\0';
    caractere[7] = '\0';
    caractere[8] = '\0';
    caractere[9] = '\0';
    caractere[10] = '\0';
    caractere[11] = '\0';
    caractere[12] = '\0';
    caractere[13] = '\0';
    caractere[14] = '\0';
    caractere[15] = '\0';
    caractere[16] = '\0';
}

void envia_qr()
{

    for (int i = 1; i < 17; i++)
    {
        if (caractere[1] == 'W')
        {
            if (i == 1)
            {
                for (size_t a = 0; a < 1000; a++)
                {
                    gpio_put(S_BUZZ, 1);
                    DEV_Delay_us(250);
                    gpio_put(S_BUZZ, 0);
                    if (a == 999)
                    {
                        uart_puts(UART_ID1, "EVA01");
                    }
                }
            }

            uart_putc(UART_ID1, caractere[i]);

            if (i == 16)
            {
                segura_segunda_leitura_qrcode = 1;
                termino_leitura_qr = false;
                altera_leitor_qrcode();
            }
        }
    }
}

void altera_leitor_qrcode()
{
    posicao = 1;
    seleciona_leitor = !seleciona_leitor;
    // DEV_Delay_ms(17);

    if (seleciona_leitor == false)
    {
        gpio_set_function(UART_RX_PIN1, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN2, GPIO_OUT);
        printf("Esquerda    <<--  \n");
        // apaga_qr();
    }

    if (seleciona_leitor == true)
    {
        gpio_set_function(UART_RX_PIN1, GPIO_OUT);
        gpio_set_function(UART_RX_PIN2, GPIO_FUNC_UART);
        printf("Direita     -->>  \n");
        // apaga_qr();
    }
}

// end
void RX_TX()
{

    valorConfig2 = readLog();

    valorSNE = readSNE();
    itoa(valorSNE, stringSNE, 10); // converte int em string
    dezena_SNE = stringSNE[0];
    unidade_SNE = stringConfig[1];
    printf("dez:%d, uni: %d", dezena_SNE, unidade_SNE);
    antSNE = dezena_SNE;
    antSNEu = unidade_SNE;
    switch (valorConfig2)
    {
    case 0:
        SSL[1] = 'f';
        SSL[2] = 'f';
        antSSL = 'f';
        printf("\n%c , %c\n", SSL[1], antSSL);
        break;
    case 1:
        SSL[1] = 'n';
        SSL[2] = NULL;
        antSSL = 'n';
        printf("\n%c , %c\n", SSL[1], antSSL);
        break;
    case 2:
        SSH = 'h';
        antSSH = 'h';
        printf("\n%c , %c\n", SSH, antSSH);
        break;
    case 3:
        SSL[1] = 'n';
        SSL[2] = NULL;
        antSSL = 'n';
        SSH = 'h';
        antSSH = 'h';
        printf("\n%c ,%c\n", SSL[1], SSH);
        break;
    case 9:
        SPG = 'n';
        antSPG = 'n';
        printf("\n%c , %c\n", SPG, antSPG);
        break;
    case 10:
        SSL[1] = 'n';
        SSL[2] = NULL;
        antSSL = 'n';
        SPG = 'n';
        antSPG = 'n';
        printf("\n%c ,%c\n", SSL[1], SPG);
        break;
    case 11:
        SPG = 'n';
        antSPG = 'n';
        SSH = 'h';
        antSSH = 'h';
        printf("\n%c ,%c\n", SPG, SSH);
        break;
    case 15:
        SSH = 'h';
        antSSH = 'h';
        SPG = 'n';
        antSPG = 'n';
        SSL[1] = 'n';
        SSL[2] = NULL;
        antSSL = 'n';
        printf("\n%c ,%c\n", SSL[1], SSH);
        break;
    default:
        break;
    }

    stateConfig = true;
    while (stateConfig)
    {
        loop();
        gpio_put(LED_PIN, 1);
        DEV_Delay_ms(1);
        gpio_put(LED_PIN, 0);
        DEV_Delay_ms(1);

        // printf("Termino %02d\n", segura_segunda_leitura_qrcode);
        // printf("Posição %02d\n", posicao);

        if (STL[0] == '2')
        {
            if (posicao < 2)
            {
                altera_leitor_qrcode();
            }

            if (posicao > 1)
            {
                reseta_posicao++;
            }

            if (reseta_posicao > 15)
            {
                posicao = 1;
                reseta_posicao = 0;
            }

            if ((caractere[16] != '\0') && (termino_leitura_qr == true) && (segura_segunda_leitura_qrcode == 0))
            {
                envia_qr();
            }

            if (posicao == 17)
            {
                termino_leitura_qr = true;
                posicao = 1;
            }

            if (posicao > 17)
            {
                posicao = 1;
            }
        }

        if (STL[0] == 'u')
        {
            // gpio_set_function(UART_RX_PIN1, GPIO_IN);
            // gpio_set_function(UART_RX_PIN2, GPIO_IN);
            gpio_set_function(gpio_pull_up, UART_RX_PIN2);
            gpio_set_function(gpio_pull_down, UART_RX_PIN1);

            if (UART_RX_PIN1 == 0)
            {
                d_mais++;
            }

            if (UART_RX_PIN2 == 0)
            {
                d_menos++;
            }

            printf("d+ %02d\n", d_mais);
            printf("d- %02d\n", d_menos);
        }

        if (mensagem_inicializacao == 1)
        {
            gpio_put(S_BUZZ, 1);
            sleep_ms(200);
            gpio_put(S_BUZZ, 0);
            sleep_ms(200);
            gpio_put(S_BUZZ, 1);
            sleep_ms(200);
            gpio_put(S_BUZZ, 0);
            sleep_ms(200);
            gpio_put(S_BUZZ, 1);
            sleep_ms(200);
            gpio_put(S_BUZZ, 0);
            sleep_ms(200);
            gpio_put(S_BUZZ, 1);
            sleep_ms(200);
            gpio_put(S_BUZZ, 0);
            sleep_ms(200);
            gpio_put(S_BUZZ, 1);
            sleep_ms(200);
            gpio_put(S_BUZZ, 0);
            sleep_ms(200); // 2000
            uart_puts(UART_ID1, "Placa ");
            uart_puts(UART_ID1, stringSNE);
            // uart_putc(UART_ID1, dezena_SNE);
            // uart_putc(UART_ID1, unidade_SNE);
            uart_puts(UART_ID1, " Inicializada");
            mensagem_inicializacao = 0;
            altera_leitor_qrcode();
        }
        destrava++;

        while (uart_is_readable(UART_ID0))
        {
            UBYTE ch0 = uart_getc(UART_ID0);

            if ((ch0 != '\0') && (ch0 != 0xAA)) //&& (ch0 != 'V') && (ch0 != 'O') && (ch0 != 'L'))
            {
                if (ch0 == '#')
                {
                    indice == 0;
                    comando[indice] = ch0;
                }

                else
                {
                    indice = indice + 1;
                    comando[indice] = ch0;
                }

                if ((comando[0] == '#') && (comando[1] == 'V') && (comando[2] == 'O') && (comando[3] == 'L'))
                {
                    comando[0] = '#';
                    comando[1] = 'I';
                    comando[2] = 'D';
                    comando[3] = 'Y';
                }

                if ((indice >= 3) && ((comando[1] == 'G') || (comando[1] == 'I') || ((comando[1] == 'R') && (comando[2] == 'S') && (comando[3] == 'T')) || ((comando[1] == 'A') && (comando[2] == 'B') && (comando[3] == 'Q'))))
                {
                    indice = 0;
                }

                if ((indice >= 4) && (((comando[1] == 'S') && (comando[2] == 'F') && (comando[3] == 'C')) || ((comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'G')) || ((comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'I')) || ((comando[1] == 'S') && (comando[2] == 'S') && (comando[3] == 'H')) || ((comando[1] == 'S') && (comando[2] == 'T') && (comando[3] == 'G')) || ((comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'X'))))
                {
                    indice = 0;
                }

                if ((indice >= 5) && (((comando[1] == 'S') && (comando[2] == 'L') && (comando[3] == 'E') && (comando[4] == 'o') && (comando[5] == 'n')) || ((comando[1] == 'S') && (comando[2] == 'T') && (comando[3] == 'E')) || ((comando[1] == 'S') && (comando[2] == 'S') && (comando[3] == 'L') && (comando[4] == 'o') && (comando[5] == 'n'))))
                {
                    indice = 0;
                }

                if ((indice >= 6) && ((comando[1] == 'S') && (comando[2] == 'L') && (comando[3] == 'E') && (comando[4] == 'o') && (comando[5] == 'f') && (comando[6] == 'f')) || ((comando[1] == 'S') && (comando[2] == 'S') && (comando[3] == 'L') && (comando[4] == 'o') && (comando[5] == 'f') && (comando[6] == 'f')))
                {
                    indice = 0;
                }

                if ((indice >= 7) && ((comando[1] == 'B') || ((comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'P'))))
                {
                    indice = 0;
                }

                if (((indice >= 18) && (comando[1] == 'S')) && (((comando[2] == 'I') && ((comando[3] == 'P') || (comando[3] == 'S'))) || ((comando[2] == 'G') && (comando[3] == 'W')) || ((comando[2] == 'M') && (comando[3] == 'R'))))
                {
                    indice = 0;
                }

                if ((indice >= 39) && (((comando[1] == 'S') && (comando[2] == 'L') && (comando[3] == 'G')) || ((comando[1] == 'A') && (comando[2] == 'S') && (comando[3] == 'N'))))
                {
                    indice = 0;
                }
            }

            if (i == 0)
            {
                // Essa sequencia de printf é necessaria para entender possiveis defeitos de eventuais novos comandos
                printf("%c", comando[0]);
                printf("%c", comando[1]);
                printf("%c", comando[2]);
                printf("%c", comando[3]);
                printf("%c\n", comando[4]);

                if (help_gsh == 2)
                {
                    if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'S') && (comando[3] == 'H')))
                    {
                        uart_puts(UART_ID1, "SSH");
                        uart_putc(UART_ID1, SSH);
                        if (SSH == 'a')
                        {
                            // se o sentido é ante-horário
                            hora_ante == false;
                        }
                        if (SSH == 'h')
                        {
                            hora_ante == true;
                        }

                        zera_comando();
                        help_gsh = 0;
                        help_gsl = 0;
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[3] = 'Y';
                        i = 0;
                    }
                }

                if (help_gsl == 2)
                {
                    if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'S') && (comando[3] == 'L')))
                    {
                        uart_puts(UART_ID1, "SSL");
                        uart_putc(UART_ID1, SSL[0]);
                        uart_putc(UART_ID1, SSL[1]);
                        uart_putc(UART_ID1, SSL[2]);
                        zera_comando();
                        help_gsh = 0;
                        help_gsl = 0;
                        comando[0] = '#';
                        comando[1] = 'V';
                        comando[2] = 'O';
                        comando[3] = 'L';
                        i = 0;
                    }
                }

                resultado = (comando[1] + comando[2] + comando[3]);

                if (destrava > 4) // Função para impedir que a placa trave ao receber um comando errado
                {
                    segura_segunda_leitura_qrcode = 0;
                    apaga_qr();
                    indice = -39;
                    RPA = '\0';
                    comando[0] = '\0';
                    comando[1] = '\0';
                    comando[2] = '\0';
                    comando[3] = '\0';
                    comando[4] = '\0';
                    comando[5] = '\0';
                    comando[6] = '\0';
                    comando[7] = '\0';
                    comando[8] = '\0';
                    comando[9] = '\0';
                    comando[10] = '\0';
                    comando[11] = '\0';
                    comando[12] = '\0';
                    comando[13] = '\0';
                    comando[14] = '\0';
                    comando[15] = '\0';
                    comando[16] = '\0';
                    comando[17] = '\0';
                    comando[18] = '\0';
                    comando[19] = '\0';
                    comando[20] = '\0';
                    comando[21] = '\0';
                    comando[22] = '\0';
                    comando[23] = '\0';
                    comando[24] = '\0';
                    comando[25] = '\0';
                    comando[26] = '\0';
                    comando[27] = '\0';
                    comando[28] = '\0';
                    comando[29] = '\0';
                    comando[30] = '\0';
                    comando[31] = '\0';
                    comando[32] = '\0';
                    comando[33] = '\0';
                    comando[34] = '\0';
                    comando[35] = '\0';
                    comando[36] = '\0';
                    comando[37] = '\0';
                    comando[38] = '\0';
                    comando[39] = '\0';
                    DEV_Delay_ms(20);
                    indice = 0;
                    comando[0] = '#';
                    comando[1] = 'V';
                    comando[2] = 'O';
                    comando[3] = 'L';
                    destrava = 0;
                }

                if (((comando[0] == '#') && (comando[1] == '#')) && ((comando[2] == 'O') || (comando[2] == 'V')) && ((comando[3] == 'L') || (comando[3] == 'O')))
                // if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'I') && (comando[3] == 'P')))
                {
                    comando[0] = '#';
                    comando[1] = 'I';
                    comando[2] = 'D';
                    comando[3] = 'Y';
                }

                if (resultado == 0xD0) //   Comando: #GFC Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'F') && (comando[3] == 'C'))
                    {
                        uart_puts(UART_ID1, "SFC");
                        uart_putc(UART_ID1, SFC);
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'I') && (comando[2] == 'D') && (comando[3] == 'Y')) // Invalida o #VOL
                    {
                        zera_comando();
                        // i = 0;
                        destrava = 0;
                    }
                }

                if (resultado == 0xD4) //   Comando: #ABQ Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'A') && (comando[2] == 'B') && (comando[3] == 'Q'))
                    {
                        uart_puts(UART_ID1, "ABQok");
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xD5) //   Comando: #GMA Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'M') && (comando[3] == 'A'))
                    {
                        DEV_Delay_ms(30);                        // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                        gpio_put(CFG_PIN, 0);                    // Entra no modo de configuração
                        CH9121_TX_3_bytes(GET_CHIP_MAC_ADDRESS); // Obtem o endereço MAC do chip
                        GMA_valido = 1;
                        le_e_salva++;
                        i++;
                    }

                if (resultado == 0xD8) //   Comando: #GLE Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'L') && (comando[3] == 'E'))
                    {
                        uart_puts(UART_ID1, "SLE");
                        uart_putc(UART_ID1, SLE[0]);
                        uart_putc(UART_ID1, SLE[1]);
                        uart_putc(UART_ID1, SLE[2]);
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xD9) //   Comando: #HELP Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'H') && (comando[2] == 'E') && (comando[3] == 'L') && (comando[4] == 'P'))
                    {
                        uart_puts(UART_ID1, "COMANDOS DE ACIONAMENTOS\n");
                        // uart_puts(UART_ID1, "                                       \n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#ABQ : Aciona Braco que cai\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#ASN : Aciona Solenoide --> Ex: #ASN104\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#ASU : Aciona Solenoide de Urna\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#BUZ : Acionamento do buzzer em x ms\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#EVA : Evento de acesso\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#RLG : Resposta do Comando #SLG\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#RPA : Resposta de acesso \n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#RST : Realiza reset na placa\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "                                       \n");
                        uart_puts(UART_ID1, "COMANDOS GET ( OBTENCAO DE DADOS )   \n");
                        // uart_puts(UART_ID1, "                                         \n");
                        uart_puts(UART_ID1, "#GFC : Get funcao catraca \n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GFW : Get versao do Firmware\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GGW : Get Geteway \n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GIP : Get endereco de IP\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GIS : Get endereco do servidor\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GLE : Get Libera Espontaneo\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GMA : Get endereco MAC \n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GMR : Get Mascara de rede\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GNE : Get numero do equipamento\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GPG : Get Polaridade de giro\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GPI : Get numero de pictogramas\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GPX : Get tipo leitura de cartao\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GSH : Get Sentido de giro\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GSL : Get saida liberada\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GTE : Get tempo de espera\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GTG : Get tipo de giro\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#GTL : Get tipo de leitura Qrcode\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "                                       \n");
                        uart_puts(UART_ID1, "COMANDOS SET ( DEFINIR PARAMETROS )  \n");
                        // uart_puts(UART_ID1, "                                         \n");
                        uart_puts(UART_ID1, "#SFC : Set funcao catraca ( s ou n )\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SGW : Set geteway (www.xxx.yyy.zzz)\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SHD : Set Data e hora (Indisponivel)\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SIP : Endereco de IP (www.xxx.yyy.zzz)\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SIS : End. servidor (www.xxx.yyy.zzz)\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SLE : Set libera Espontaneo\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SLG : Set libera giro\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SMF : Set mensagem Full ( display )\n");
                        DEV_Delay_ms(40);
                        uart_puts(UART_ID1, "#SMR : Mascara de rede(www.xxx.yyy.zzz)\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SNE : Set num equipamento ( 01 a 99 )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SPG : Set polaridade giro ( p ou g )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SPI : Set num. pictogramas (3 padrao)\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SPP : Set pisca pisctograma ( av )   \n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SPX : Set leitura proximity ( w ou a )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SSH : Set sentido de giro ( a ou h )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#SSL : Set saida liberada ( on ou off )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#STE : Set tempo de espera ( 01 a 99 )\n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#STG : Set tipo de giro ( i ou m )   \n");
                        DEV_Delay_ms(30);
                        uart_puts(UART_ID1, "#STL : Set leitura Qrcode (232 ou usb)\n");
                        DEV_Delay_ms(30);
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xDA) //   Comando: #GNE Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'N') && (comando[3] == 'E'))
                    {
                        uart_puts(UART_ID1, "SNE");

                        if ((SNE >= 0) && (SNE <= 99))
                        {
                            dezena_SNE = (SNE / 10) + 48;

                            unidade_SNE = (SNE % 10) + 48;
                        }

                        uart_putc(UART_ID1, dezena_SNE);
                        uart_putc(UART_ID1, unidade_SNE);
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xDC) //   Comando: #EVA / #SFC Status: Em Implementação :)
                {

                    if ((comando[0] == '#') && (comando[1] == 'E') && (comando[2] == 'V') && (comando[3] == 'A')) // && (comando[4] == 'n'))
                    {
                        if (comando[4] == 'n')
                        {
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            i = 0;
                        }

                        if (comando[4] == 's')
                        {
                            posicao = 1;
                            uart_puts(UART_ID1, "EVAok");
                            segura_segunda_leitura_qrcode = 1;
                            DEV_Delay_ms(30);
                            // indice = -34;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            i = 0;
                        }
                    }
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'F') && (comando[3] == 'C') && ((comando[4] == 's') || (comando[4] == 'n')))
                    {
                        uart_puts(UART_ID1, "SFCok");
                        SFC = comando[4];
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xDE) //   Comando: #GPG Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'P') && (comando[3] == 'G'))
                    {
                        uart_puts(UART_ID1, "SPG");
                        uart_putc_raw(UART_ID1, SPG);
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xDF) //   Comando: #SDH Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'D') && (comando[3] == 'H'))
                    {
                        uart_puts(UART_ID1, "Indisponivel neste Hardware :(");
                        indice = -14;
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[3] = 'Y';
                        i = 0;
                    }

                if (resultado == 0xE0) //   Comando: #GIP / #GPI / #GTE Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'I') && (comando[3] == 'P'))
                    {
                        DEV_Delay_ms(30);                        // Tempo ocioso necessario para a comparação dos dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                        gpio_put(CFG_PIN, 0);                    // Entra no modo de configuração
                        CH9121_TX_3_bytes(READ_CHIP_IP_ADDRESS); // Lê o endereço IP do chip
                        GIP_valido = 1;
                        le_e_salva++;
                        i++;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'P') && (comando[3] == 'I'))
                    {
                        uart_puts(UART_ID1, "SPI");

                        if ((SPI == 2) || (SPI == 3))
                        {
                            SPI = SPI + 48;
                        }

                        uart_putc(UART_ID1, SPI);
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'T') && (comando[3] == 'E'))
                    {
                        uart_puts(UART_ID1, "STE");

                        if ((STE >= 0) && (STE <= 99))
                        {
                            dezena_STE = (STE / 10) + 48;
                            unidade_STE = (STE % 10) + 48;
                        }

                        if (dezena_STE != 48)
                        {
                            uart_putc(UART_ID1, dezena_STE);
                        }

                        uart_putc(UART_ID1, unidade_STE);
                        zera_comando();
                        i = 0;
                        exibe_GTL = 0;
                    }
                }

                if (resultado == 0xE2) //   Comando: #ASN / #GSH / #GTG Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'A') && (comando[2] == 'S') && (comando[3] == 'N'))
                    {
                        ASN1 = comando[4];
                        ASN2 = comando[5];
                        ASN3 = comando[6];
                        uart_puts(UART_ID1, "ASNok");
                        indice = -35;
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[3] = 'Y';
                        i = 0;
                    }
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'O') && (comando[3] == 'L'))
                    {
                        help_gsh = 1;
                        help_gsl = 1;
                    }

                    if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'S') && (comando[3] == 'H')))
                    {
                        uart_puts(UART_ID1, "SSH");
                        uart_putc(UART_ID1, SSH);
                        if (SSH == 'a')
                        {
                            // se o sentido é ante-horário
                            hora_ante == false;
                        }
                        if (SSH == 'h')
                        {
                            hora_ante == true;
                        }

                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'T') && (comando[3] == 'G'))
                    {
                        uart_puts(UART_ID1, "STG");
                        uart_putc(UART_ID1, STG);
                        zera_comando();
                        i = 0;
                        exibe_GTL = 0;
                    }
                }

                if (resultado == 0xE3) //   Comando: #GIS / #RPA Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'I') && (comando[3] == 'S'))
                    {
                        DEV_Delay_ms(30);                                          // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                        gpio_put(CFG_PIN, 0);                                      // Entra no modo de configuração
                        CH9121_TX_3_bytes(READ_DESTINATION_IP_ADDRESS_CHIP_PORT2); // Lê o endereço IP de destino da porta 2 do chip
                        GIS_valido = 1;
                        le_e_salva++;
                        i++;
                    }
                    // if ((comando[0] == '#') && (comando[1] == 'R') && (comando[2] == 'P') && (comando[3] == 'A') && (comando[4] == '\0') && (comando[5] == '\0') && (comando[6] == '\0'))
                    // {
                    //     uart_puts(UART_ID1, "RPAn");
                    //     DEV_Delay_ms(30);
                    //     indice = -32;
                    //     comando[0] = '#';
                    //     comando[1] = 'I';
                    //     comando[2] = 'D';
                    //     comando[3] = 'Y';
                    //     comando[4] = '\0';
                    //     comando[5] = '\0';
                    //     comando[6] = '\0';
                    //     i = 0;
                    // }
                    if ((comando[0] == '#') && (comando[1] == 'R') && (comando[2] == 'P') && (comando[3] == 'A') && (comando[4] != '\0') && (comando[5] != '\0') && (comando[6] != '\0'))
                    {

                        comando[5] = (comando[5] - 48) * 10;
                        comando[6] = comando[6] - 48;
                        Trpa = (comando[5] + comando[6]);

                        if (comando[4] == 'a')
                        {
                            indice = -32;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            comando[4] = '\0';
                            comando[5] = '\0';
                            comando[6] = '\0';
                            i = 0;
                            RPA = 'a';
                            abre = true;
                        }

                        if (comando[4] == 'b')
                        {
                            uart_puts(UART_ID1, "RPAb");
                            apaga_qr();
                            segura_segunda_leitura_qrcode = 0;
                            // DEV_Delay_ms(30);
                            indice = -32;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            comando[4] = '\0';
                            comando[5] = '\0';
                            comando[6] = '\0';
                            i = 0;
                        }

                        if (comando[4] == 'e')
                        {
                            indice = -32;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            comando[4] = '\0';
                            comando[5] = '\0';
                            comando[6] = '\0';
                            i = 0;
                            RPA = 'e';
                            abre = true;
                        }

                        if (comando[4] == 's')
                        {
                            indice = -32;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            comando[4] = '\0';
                            comando[5] = '\0';
                            comando[6] = '\0';
                            i = 0;
                            RPA = 's';
                            abre = true;
                        }
                    }
                }

                if (resultado == 0xE4) //   Comando: #GFW / #SLE Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'F') && (comando[3] == 'W'))
                    {
                        uart_puts(UART_ID1, versFirmware); // Nome do Firmware
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'L') && (comando[3] == 'E') && (((comando[4] == 'o') && (comando[5] == 'n') && (comando[6] == '\0')) || ((comando[4] == 'o') && (comando[5] == 'f') && (comando[6] == 'f'))))
                    {
                        uart_puts(UART_ID1, "SLEok");
                        SLE[0] = comando[4];
                        SLE[1] = comando[5];
                        SLE[2] = comando[6];
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xE5) //   Comando: #GGW / #RLG Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'G') && (comando[3] == 'W'))
                    {
                        DEV_Delay_ms(30);                     // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                        gpio_put(CFG_PIN, 0);                 // Entra no modo de configuração
                        CH9121_TX_3_bytes(READ_CHIP_GATEWAY); // Lê o gateway do chip
                        GGW_valido = 1;
                        le_e_salva++;
                        i++;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'R') && (comando[2] == 'L') && (comando[3] == 'G'))
                    {
                        uart_puts(UART_ID1, "RLGok");
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xE6) //   Comando: #GMR / #GSL / #SLG Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'M') && (comando[3] == 'R'))
                    {
                        DEV_Delay_ms(30);                  // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                        gpio_put(CFG_PIN, 0);              // Entra no modo de configuração
                        CH9121_TX_3_bytes(READ_CHIP_MASK); // Lê a máscara de rede do chip
                        GMR_valido = 1;
                        le_e_salva++;
                        i++;
                    }
                    ///*GSL Novo
                    if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'S') && (comando[3] == 'L')))
                    {
                        help_gsh = 2;
                        help_gsl = 2;
                    }
                    //*/
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'L') && (comando[3] == 'G') && (comando[4] != '\0') && (comando[5] != '\0') && (comando[6] != '\0'))
                    {

                        comando[5] = (comando[5] - 48) * 10;
                        comando[6] = comando[6] - 48;
                        Trpa = (comando[5] + comando[6]);

                        if (comando[4] == 'a')
                        {
                            uart_puts(UART_ID1, "RLGok");
                            DEV_Delay_ms(30);
                            indice = -32;
                            comando[0] = '#';
                            comando[1] = 'I';
                            comando[2] = 'D';
                            comando[3] = 'Y';
                            comando[4] = '\0';
                            comando[5] = '\0';
                            comando[6] = '\0';
                            i = 0;
                            RPA = 'a';
                            abre = true;
                        }
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'M') && (comando[3] == 'F'))
                    {
                        ASN1 = comando[4];
                        ASN2 = comando[5];
                        ASN3 = comando[6];
                        uart_puts(UART_ID1, "SMFok");
                        indice = -34;
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[3] = 'Y';
                        i = 0;
                    }
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'N') && (comando[3] == 'E') && (comando[4] != '\0') && (comando[5] != '\0'))
                    {
                        SNE = ((comando[4] - 48) * 10) + comando[5] - 48;
                        uart_puts(UART_ID1, "SNEok");
                        if ((SNE >= 0) && (SNE <= 99))
                        {
                            dezena_SNE = (SNE / 10) + 48;

                            unidade_SNE = (SNE % 10) + 48;
                        }
                        indice = -34;
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[3] = 'Y';
                        i = 0;
                    }
                }

                if (resultado == 0xE7) //   Comando: #GTL Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'T') && (comando[3] == 'L'))
                    {
                        exibe_GTL++;
                        if (exibe_GTL == 2)
                        {
                            uart_puts(UART_ID1, "STL");
                            uart_putc(UART_ID1, STL[0]);
                            uart_putc(UART_ID1, STL[1]);
                            uart_putc(UART_ID1, STL[2]);
                            zera_comando();
                            i = 0;
                            exibe_GTL = 0;
                        }
                    }

                if (resultado == 0xE9) //   Comando: #ASU Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'A') && (comando[2] == 'S') && (comando[3] == 'U'))
                    {
                        uart_puts(UART_ID1, "ASUok");
                        comando[0] = '#';
                        comando[1] = 'I';
                        comando[2] = 'D';
                        comando[2] = 'C';
                        i = 0;
                    }

                if (resultado == 0xEA) //   Comando: #SPG Status: Em Implementação :)
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'G') && ((comando[4] == 'p') || (comando[4] == 'n')))
                    {
                        uart_puts(UART_ID1, "SPGok");
                        SPG = comando[4];
                        zera_comando();
                        i = 0;
                    }

                if (resultado == 0xEC) //   Comando: #SIP / #SPI / #STE Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'I') && (comando[3] == 'P'))
                        if ((comando[7] == '.') && (comando[11] == '.') && (comando[15] == '.'))
                        {
                            if (salva_e_reinicia < 2)
                            {
                                uart_puts(UART_ID1, "SIPok");
                            }

                            primeira_faixa_ip = (((comando[4]) - 48) * 100) + (((comando[5]) - 48) * 10) + ((comando[6]) - 48);
                            segunda_faixa_ip = (((comando[8]) - 48) * 100) + (((comando[9]) - 48) * 10) + ((comando[10]) - 48);
                            terceira_faixa_ip = (((comando[12]) - 48) * 100) + (((comando[13]) - 48) * 10) + ((comando[14]) - 48);
                            quarta_faixa_ip = (((comando[16]) - 48) * 100) + (((comando[17]) - 48) * 10) + ((comando[18]) - 48);

                            CH9121_LOCAL_IP[0] = primeira_faixa_ip;
                            CH9121_LOCAL_IP[1] = segunda_faixa_ip;
                            CH9121_LOCAL_IP[2] = terceira_faixa_ip;
                            CH9121_LOCAL_IP[3] = quarta_faixa_ip;

                            DEV_Delay_ms(20);                             // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                            gpio_put(CFG_PIN, 0);                         // Entra no modo de configuração
                            CH9121_TX_7_bytes(CH9121_LOCAL_IP, LOCAL_IP); // Seta o endereço IP do chip
                            salva_e_reinicia++;
                            i++;
                        }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'I') && ((comando[4] == '2') || (comando[4] == '3')))
                    {
                        uart_puts(UART_ID1, "SPIok");
                        SPI = comando[4];
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'T') && (comando[3] == 'E') && (comando[4] != '\0') && (comando[5] != '\0'))
                    {
                        uart_puts(UART_ID1, "STEok");
                        STE = ((comando[4] - 48) * 10) + comando[5] - 48;
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xEE) //   Comando: #SSH / #STG Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'S') && (comando[3] == 'H') && ((comando[4] == 'h') || (comando[4] == 'a')))
                    {
                        uart_puts(UART_ID1, "SSHok");
                        SSH = comando[4];
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'T') && (comando[3] == 'G') && ((comando[4] == 'm') || (comando[4] == 'i')))
                    {
                        uart_puts(UART_ID1, "STGok");
                        STG = comando[4];
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xEF) //   Comando: #GPX / #SIS Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'G') && (comando[2] == 'P') && (comando[3] == 'X'))
                    {
                        uart_puts(UART_ID1, "SPX");
                        uart_putc(UART_ID1, SPX);
                        zera_comando();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'I') && (comando[3] == 'S'))
                        if ((comando[7] == '.') && (comando[11] == '.') && (comando[15] == '.'))
                        {
                            if (salva_e_reinicia < 2)
                            {
                                uart_puts(UART_ID1, "SISok");
                            }

                            primeira_faixa_is = (((comando[4]) - 48) * 100) + (((comando[5]) - 48) * 10) + ((comando[6]) - 48);
                            segunda_faixa_is = (((comando[8]) - 48) * 100) + (((comando[9]) - 48) * 10) + ((comando[10]) - 48);
                            terceira_faixa_is = (((comando[12]) - 48) * 100) + (((comando[13]) - 48) * 10) + ((comando[14]) - 48);
                            quarta_faixa_is = (((comando[16]) - 48) * 100) + (((comando[17]) - 48) * 10) + ((comando[18]) - 48);

                            CH9121_TARGET_IP[0] = primeira_faixa_is;
                            CH9121_TARGET_IP[1] = segunda_faixa_is;
                            CH9121_TARGET_IP[2] = terceira_faixa_is;
                            CH9121_TARGET_IP[3] = quarta_faixa_is;

                            DEV_Delay_ms(20);                                // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                            gpio_put(CFG_PIN, 0);                            // Entra no modo de configuração
                            CH9121_TX_7_bytes(CH9121_TARGET_IP, TARGET_IP2); // Seta o endereço IP de destino da porta 1 do chip
                            salva_e_reinicia++;
                            i++;
                        }
                }

                if (resultado == 0xF1) //   Comando: #BUZ / #SGW Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'B') && (comando[2] == 'U') && (comando[3] == 'Z') && (comando[4] == '0') && (comando[5] == '0') && (comando[6] == '0') && (comando[7] == '5'))
                    {
                        uart_puts(UART_ID1, "BUZok");
                        DEV_Delay_ms(10); // Tempo necessario para que occora a quebra de linha ao enviar diversos buzzers de uma só vez
                        zera_comando();
                        gpio_put(S_BUZZ, 1);
                        DEV_Delay_ms(12);
                        gpio_put(S_BUZZ, 0);
                        Regula_Tensao();
                        i = 0;
                    }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'G') && (comando[3] == 'W'))
                        if ((comando[7] == '.') && (comando[11] == '.') && (comando[15] == '.'))
                        {
                            if (salva_e_reinicia < 2)
                            {
                                uart_puts(UART_ID1, "SGWok");
                            }

                            primeira_faixa_gateway = (((comando[4]) - 48) * 100) + (((comando[5]) - 48) * 10) + ((comando[6]) - 48);
                            segunda_faixa_gateway = (((comando[8]) - 48) * 100) + (((comando[9]) - 48) * 10) + ((comando[10]) - 48);
                            terceira_faixa_gateway = (((comando[12]) - 48) * 100) + (((comando[13]) - 48) * 10) + ((comando[14]) - 48);
                            quarta_faixa_gateway = (((comando[16]) - 48) * 100) + (((comando[17]) - 48) * 10) + ((comando[18]) - 48);

                            CH9121_GATEWAY[0] = primeira_faixa_gateway;
                            CH9121_GATEWAY[1] = segunda_faixa_gateway;
                            CH9121_GATEWAY[2] = terceira_faixa_gateway;
                            CH9121_GATEWAY[3] = quarta_faixa_gateway;

                            DEV_Delay_ms(30);                           // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                            gpio_put(CFG_PIN, 0);                       // Entra no modo de configuração
                            CH9121_TX_7_bytes(CH9121_GATEWAY, GATEWAY); // Seta o gateway do chip
                            salva_e_reinicia++;
                            i++;
                        }
                }

                if (resultado == 0xF2) //   Comando: #SMR / #SSL Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'M') && (comando[3] == 'R'))
                        if ((comando[7] == '.') && (comando[11] == '.') && (comando[15] == '.'))
                        {
                            if (salva_e_reinicia < 2)
                            {
                                uart_puts(UART_ID1, "SMRok");
                            }

                            primeira_faixa_mascara_de_rede = (((comando[4]) - 48) * 100) + (((comando[5]) - 48) * 10) + ((comando[6]) - 48);
                            segunda_faixa_mascara_de_rede = (((comando[8]) - 48) * 100) + (((comando[9]) - 48) * 10) + ((comando[10]) - 48);
                            terceira_faixa_mascara_de_rede = (((comando[12]) - 48) * 100) + (((comando[13]) - 48) * 10) + ((comando[14]) - 48);
                            quarta_faixa_mascara_de_rede = (((comando[16]) - 48) * 100) + (((comando[17]) - 48) * 10) + ((comando[18]) - 48);

                            CH9121_SUBNET_MASK[0] = primeira_faixa_mascara_de_rede;
                            CH9121_SUBNET_MASK[1] = segunda_faixa_mascara_de_rede;
                            CH9121_SUBNET_MASK[2] = terceira_faixa_mascara_de_rede;
                            CH9121_SUBNET_MASK[3] = quarta_faixa_mascara_de_rede;

                            DEV_Delay_ms(30);                                   // Tempo ocioso necessario para a comparação od dados seriais com {0x55,0xaa,0x5a} que faz entrar no modo de configuração.
                            gpio_put(CFG_PIN, 0);                               // Entra no modo de configuração
                            CH9121_TX_7_bytes(CH9121_SUBNET_MASK, SUBNET_MASK); // Seta a máscara de rede do chip
                            salva_e_reinicia++;
                            i++;
                        }

                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'S') && (comando[3] == 'L') && (((comando[4] == 'o') && (comando[5] == 'n') && (comando[6] == '\0')) || ((comando[4] == 'o') && (comando[5] == 'f') && (comando[6] == 'f'))))
                    {
                        uart_puts(UART_ID1, "SSLok");
                        SSL[0] = comando[4];
                        SSL[1] = comando[5];
                        SSL[2] = comando[6];
                        zera_comando();
                        i = 0;
                    }
                }

                if (resultado == 0xF3) //   Comando: #SPP Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'P') && (comando[4] == 'a') && (comando[5] == 'v'))
                    {
                        uart_puts(UART_ID1, "SPPok");
                        // indice = 0;
                        zera_comando();
                        i = 0;
                        SPP = true;
                    }

                    if ((comando[0] == '#') && ((comando[1] == 'G') && (comando[2] == 'S') && (comando[3] == 'Y')))
                    {
                        help_gsh = 2;
                        help_gsl = 2;
                    }

                    if (((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'T') && (comando[3] == 'L')) && (((comando[4] == '2') && (comando[5] == '3') && (comando[6] == '2')) || ((comando[4] == 'u') && (comando[5] == 's') && (comando[6] == 'b'))))
                    {
                        uart_puts(UART_ID1, "STLok");
                        STL[0] = comando[4];
                        STL[1] = comando[5];
                        STL[2] = comando[6];
                        // indice = 0;
                        zera_comando();
                        i = 0;
                    }
                }
                if (resultado == 0xF9) //   Comando: #RST Status: Funcionando :)
                    if ((comando[0] == '#') && (comando[1] == 'R') && (comando[2] == 'S') && (comando[3] == 'T'))
                    {
                        uart_puts(UART_ID1, "RSTok");
                        zera_comando();
                        DEV_Delay_ms(30);              // Tempo necessario para exibição da mensagem de confirmação antes do reset
                        gpio_put(CFG_PIN, 0);          // Entra no modo de configuração
                        CH9121_TX_3_bytes(RESET_CHIP); // Reseta o chip
                        gpio_put(RESET_PIN, 0);        // Realiza um reset na Raspberry
                        i++;
                    }

                if (resultado == 0xFB) //   Comando: #SPX Status: Em Implementação :)
                {
                    if ((comando[0] == '#') && (comando[1] == 'S') && (comando[2] == 'P') && (comando[3] == 'X') && ((comando[4] == 'w') || (comando[4] == 'a')))
                    {
                        uart_puts(UART_ID1, "SPXok");
                        SPX = comando[4];
                        zera_comando();
                        i = 0;
                    }
                }
                /*
                if ((resultado != 0xD0) && (resultado != 0xD4) && (resultado != 0xD5) && (resultado != 0xD8) && (resultado != 0xDC) && (resultado != 0xDE) && (resultado != 0xE0) && (resultado != 0xE2) && (resultado != 0xE3) && (resultado != 0xE4) && (resultado != 0xE5) && (resultado != 0xE6) && (resultado != 0xE9) && (resultado != 0xEA) && (resultado != 0xEC) && (resultado != 0xEE) && (resultado != 0xEF) && (resultado != 0xF1) && (resultado != 0xF2) && (resultado != 0xF3) && (resultado != 0xF9) && (resultado != 0xFB))
                {
                    uart_puts(UART_ID1, "Comando nao Reconhecido ;(");
                    indice = -39;
                    comando[0] = '#';
                    comando[1] = 'I';
                    comando[2] = 'D';
                    comando[3] = 'Y';
                    i = 0;
                }
                */
            }

            if (i == 1)
            {
                tx[2] = 0x0d;
                DEV_Delay_ms(30);
                uart_puts(UART_ID0, tx);
                i++;
            }

            if (i == 2)
            {
                tx[2] = 0x0e;
                DEV_Delay_ms(30);
                uart_puts(UART_ID0, tx);
                i++;
            }

            if (i == 3)
            {
                tx[2] = 0x5e;
                DEV_Delay_ms(30); // 17 Milissegundos maximo
                uart_puts(UART_ID0, tx);
                if (salva_e_reinicia == 8)
                {
                    gpio_put(CFG_PIN, 0);          // Entra no modo de configuração
                    CH9121_TX_3_bytes(RESET_CHIP); // Reseta o chip
                    DEV_Delay_ms(2000);
                    gpio_put(CFG_PIN, 1);   // Sai do modo de configuração
                    gpio_put(RESET_PIN, 0); // Realiza um reset na Raspberry
                }
                valida_parametros = 1;
                i = 0;
            }

            if (uart_is_writable(UART_ID1))
            {
                if (((ch0 != 0xAA) && (ch0 != 0x40) && (ch0 != 0x114) && (ch0 != 0x5e) && (comando[1] != 'V') && (comando[1] != 'O') && (comando[1] != 'L') && (comando[2] != 'V') && (comando[2] != 'O') && (comando[2] != 'L') && (comando[3] != 'V') && (comando[3] != 'O') && (comando[3] != 'L') && (comando[4] != 'V') && (comando[4] != 'O') && (comando[4] != 'L') && (comando[5] != 'V') && (comando[5] != 'O') && (comando[5] != 'L') && (comando[6] != 'V') && (comando[6] != 'O') && (comando[6] != 'L') && (comando[0] != '\0') && (comando[1] != '\0') && (comando[2] != '\0') && (comando[3] != '\0') && (comando[4] != '\0') && (comando[5] != '\0') && (comando[6] != '\0') && (resultado != 0xD0)) || (valida_parametros == 1)) //&& ( i > 0) (ch0 != 50000) &&
                {
                    if ((GMA_valido == 1) && (le_e_salva == 2) && (comando[3] != 'A'))
                    {
                        get_dados++;
                        dados_6_bytes = 1;
                        if (get_dados == 1)
                        {
                            uart_puts(UART_ID1, "SMA");
                        }
                    }

                    if ((GIP_valido == 1) && (le_e_salva == 2) && (comando[3] != 'P'))
                    {
                        get_dados++;
                        dados_4_bytes = 1;
                        if (get_dados > 1)
                        {
                            uart_puts(UART_ID1, "SIP");
                        }
                    }

                    if ((GIS_valido == 1) && (le_e_salva == 2) && (comando[3] != 'S'))
                    {
                        get_dados++;
                        dados_4_bytes = 1;
                        if (get_dados > 1)
                        {
                            uart_puts(UART_ID1, "SIS");
                        }
                    }

                    if ((GMR_valido == 1) && (le_e_salva == 2) && (comando[3] != 'R'))
                    {
                        get_dados++;
                        dados_4_bytes = 1;
                        if (get_dados == 1)
                        {
                            uart_puts(UART_ID1, "SMR");
                        }
                    }

                    if ((GGW_valido == 1) && (le_e_salva == 2) && (comando[3] != 'W'))
                    {
                        get_dados++;
                        dados_4_bytes = 1;
                        if (get_dados > 1)
                        {
                            uart_puts(UART_ID1, "SGW");
                        }
                    }

                    if (dados_4_bytes == 1)
                    {
                        uint8_t hex_bytes[] = {comando[1], comando[2], comando[3], comando[4]}; // Substitua esses valores pelos seus próprios bytes hexadecimais

                        if (comando[1] == 0x23)
                        {
                            hex_bytes[0] = 0;
                        }

                        if (comando[2] == 0x23)
                        {
                            hex_bytes[1] = 0;
                        }

                        if (comando[3] == 0x23)
                        {
                            hex_bytes[2] = 0;
                        }

                        if (comando[4] == 0x23)
                        {
                            hex_bytes[3] = 0;
                        }

                        if (((hex_bytes[3] != 0) && (GMR_valido != 1)) || (GMR_valido == 1))
                        {
                            char buffer[4];
                            snprintf(buffer, sizeof(buffer), "%03d", hex_bytes[0]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, '.');
                            snprintf(buffer, sizeof(buffer), "%03d", hex_bytes[1]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, '.');
                            snprintf(buffer, sizeof(buffer), "%03d", hex_bytes[2]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, '.');
                            snprintf(buffer, sizeof(buffer), "%03d", hex_bytes[3]);
                            uart_puts(UART_ID1, buffer);
                            gpio_put(CFG_PIN, 1); // Sai do modo de configuração
                            // gpio_put(CFG_PIN, 0);          // Entra no modo de configuração
                            // CH9121_TX_3_bytes(RESET_CHIP); // Reseta o chip
                            //  DEV_Delay_ms(2000);
                            // gpio_put(CFG_PIN, 1);   // Sai do modo de configuração
                            // gpio_put(RESET_PIN, 0); // Realiza um reset na Raspberry
                            zera_comando();
                        }
                    }

                    if (dados_6_bytes == 1)
                    {
                        uint8_t hex_bytes[] = {comando[1], comando[2], comando[3], comando[4], comando[5], comando[6]}; // Substitua esses valores pelos seus próprios bytes hexadecimais

                        if (comando[1] == 0x23)
                        {
                            hex_bytes[0] = 0;
                        }

                        if (comando[2] == 0x23)
                        {
                            hex_bytes[1] = 0;
                        }

                        if (comando[3] == 0x23)
                        {
                            hex_bytes[2] = 0;
                        }

                        if (comando[4] == 0x23)
                        {
                            hex_bytes[3] = 0;
                        }

                        if (comando[5] == 0x23)
                        {
                            hex_bytes[4] = 0;
                        }

                        if (comando[6] == 0x23)
                        {
                            hex_bytes[5] = 0;
                        }

                        if (hex_bytes[5] != 0)
                        {
                            char buffer[6];
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[0]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, ':');
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[1]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, ':');
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[2]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, ':');
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[3]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, ':');
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[4]);
                            uart_puts(UART_ID1, buffer);
                            uart_putc(UART_ID1, ':');
                            snprintf(buffer, sizeof(buffer), "%02X", hex_bytes[5]);
                            uart_puts(UART_ID1, buffer);
                            gpio_put(CFG_PIN, 1); // Sai do modo de configuração
                            zera_comando();
                        }
                    }
                }
            }
        }

        // QR code While
        while (uart_is_readable(UART_ID1))
        {
            if (mensagem_inicializacao == 0)
            {
                UBYTE ch1 = uart_getc(UART_ID1);

                caractere[posicao] = ch1;

                // if (posicao == 1) //&& (caractere[0] != '\0'))
                // {
                //     uart_puts(UART_ID1, "EVA01");
                // }

                // uart_putc(UART_ID1, caractere[posicao]);
                printf("%c\n", caractere[posicao]);
                posicao++;

                // printf("leitura %02d\n", posicao);
            }
        }
        // novo
        if (SSL[1] != antSSL || SSH != antSSH || SPG != antSPG || dezena_SNE != antSNE || unidade_SNE != antSNEu)
        {
            // printf("Foi mussarelo!: %c", SSL[1]);
            antSSL = SSL[1];
            antSSH = SSH;
            antSPG = SPG;
            antSNE = dezena_SNE;
            antSNEu = unidade_SNE;
            sleep_ms(10);
            storageInit();

            stringConfig[8] = (char)dezena_SNE;
            stringConfig[9] = (char)unidade_SNE;
            storageLog(stringConfig);
            if (SPG == 'p')
            {
                stringConfig[6] = '0';
                storageLog(stringConfig);
            }
            else if (SPG == 'n')
            {
                stringConfig[6] = '1';
                storageLog(stringConfig);
            }
            if (SSL[1] == 'f')
            {
                stringConfig[4] = '0';
                storageLog(stringConfig);
            }
            else if (SSL[1] == 'n')
            {
                stringConfig[4] = '1';
                storageLog(stringConfig);
            }
            if (SSH == 'a')
            {
                stringConfig[2] = '0';
                storageLog(stringConfig);
            }
            else if (SSH == 'h')
            {
                stringConfig[2] = '1';
                storageLog(stringConfig);
            }
            if (STG == 'i')
            {
                stringConfig[0] = '0';
                storageLog(stringConfig);
            }
            else if (STG == 'm')
            {
                stringConfig[0] = '1';
                storageLog(stringConfig);
            }

            valorConfig2 = readLog();
            // printf("\nValor da leitura: %d\n", valorConfig2);
        }
    }
}
void loop(void)
{
    if ((!gpio_get(FACE_ID_1)) && (!gpio_get(FACE_ID_2)))
    {
        if (RPA == 'e')
        {
            RPA = 'e';
            abre = true;
        }

        if (RPA == 's')
        {
            RPA = 's';
            abre = true;
        }

        if ((RPA == 'a') || (RPA == '\0'))
        {
            RPA = 'a';
            abre = true;
        }
    }

    if (RPA != 'a')
    {
        if ((!gpio_get(FACE_ID_1)) && (RPA == 'e'))
        {
            RPA = 'e';
            abre = true;
        }

        if ((!gpio_get(FACE_ID_1)) && (RPA == '\0'))
        {
            Trpa = 10;
            RPA = 'e';
            abre = true;
        }

        if ((!gpio_get(FACE_ID_2)) && (RPA == 's'))
        {
            RPA = 's';
            abre = true;
        }

        if ((!gpio_get(FACE_ID_2)) && (RPA == '\0'))
        {
            Trpa = 10;
            RPA = 's';
            abre = true;
        }
    }

    if (SSL[1] == 'f')
    {
        if (SPG == 'p')
        {
            if (!gpio_get(SENSOR2) && !gpio_get(SENSOR1))
            {
                sleep_us(1540);
                gpio_put(SOL1, 0);
                gpio_put(SOL2, 0);
                gpio_put(S_BLOCK, 0);
                gpio_put(S_BUZZ, 0);
                gpio_put(S_STANDBY, 1);
                sinalMisto = false;
            }
        }

        if (SPG == 'n')
        {
            if (gpio_get(SENSOR2) && gpio_get(SENSOR1))
            {
                sleep_us(1540);
                gpio_put(SOL1, 0);
                gpio_put(SOL2, 0);
                gpio_put(S_BLOCK, 0);
                gpio_put(S_BUZZ, 0);
                gpio_put(S_STANDBY, 1);
                sinalMisto = false;
            }
        }
    }

    bool stateButton = !gpio_get(FACE_ID_2);
    int liberar = liberado(contador);
    int fimG = liberado(1);

    // ----------------------- Inicia a verificação ------------------------------
    // Estagio liberado
    if (stateButton == true || abre == true)
    {
        autoriza = 1;
        inTemp++;
    }
    if (auxTemp > Trpa && abre)
    {
        auxTemp = Trpa * 480; // Antes estava em 47
    }

    if (inTemp >= Trpa * 480 && !trava_RPAn) // Antes estava em 47
    {
        auxTemp = inTemp;
        inTemp = 0;
        abre = false;

        // printf("|a%d|",auxTemp);
        // printf("|i%d|",inTemp);
        // printf("|t%d|",tempo);
        // printf("|L%d|\n",autoriza);
    }
    if (stateButton == true)
    {
        if (inTemp >= 1500)
        {
            auxTemp = inTemp;
            inTemp = 0;
            abre = false;
            // RX(5);
            //  RX(5);
            //   RX(5);
            // printf("|a%d|",auxTemp);
            // printf("|i%d|",inTemp);
            // printf("|t%d|",tempo);
            // printf("|L%d|\n",autoriza);
        }
    }

    // Tempo de liberação em ms
    if ((tempo == auxTemp) && (giroGirado == 0)) // Adicionado  && (giroGirado == 0))
    {
        autoriza = 0;
        abre = false;
        inTemp = 0;
        tempo = 0;
        RX(5);
        // Trpa = 0;
        trava_RPAn = false;
        printf("|FimG%d|", fimG);
        printf("|L%d|", liberar);
        printf("|LB%d|", libBlok);
        printf("|G%d|\n", giroGirado);
        printf("|C%d|\n", contador);
        // printf("|A%d|\n", abre);
    }
    // printf("|a%d|",auxTemp);
    // printf("|i%d|",inTemp);
    // printf("|t%d|",tempo);
    // printf("|L%d|",abre);
    // printf("|RPA%d|\n",Trpa);

    if (autoriza == 1)
    {
        tempo++;
        // printf("Tempo %d\n", tempo);
        contador = 1;
        liberar = liberado(contador);
        liberado(contador);

        if (STG == 'i')
        {
            if (SSH == 'h')
            {
                if (RPA == 'e')
                {
                    if (liberar == 1 && RPA == 'e')
                    {
                        naoBlock = true;
                    }
                    if (liberar == 2 && naoBlock == false)
                    {
                        bloqueado(0);
                        naoBlock = false;
                    }
                }
                if (RPA == 's')
                {
                    if (liberar == 1 && naoBlock == false)
                    {
                        bloqueado(0);
                        naoBlock = false;
                    }
                    if (liberar == 2 && RPA == 's')
                    {
                        naoBlock = true;
                    }
                }
                if (RPA == 'a')
                {
                    if (liberar == 1)
                    {
                        naoBlock = true;
                    }
                    if (liberar == 2)
                    {
                        naoBlock = true;
                    }
                }
            }
            if (SSH == 'a')
            {
                if (RPA == 'e')
                {
                    if (liberar == 1 && naoBlock == false)
                    {
                        bloqueado(0);
                        naoBlock = false;
                    }
                    if (liberar == 2 && RPA == 'e')
                    {
                        naoBlock = true;
                    }
                }
                if (RPA == 's')
                {
                    if (liberar == 1 && RPA == 's')
                    {
                        naoBlock = true;
                    }
                    if (liberar == 2 && naoBlock == false)
                    {
                        bloqueado(0);
                        naoBlock = false;
                    }
                }
                if (RPA == 'a')
                {
                    if (liberar == 1)
                    {
                        naoBlock = true;
                    }
                    if (liberar == 2)
                    {
                        naoBlock = true;
                    }
                }
            }
        }
        // Bloqueia giro > 1
        if (liberar == 3 && libBlok == 0)
        {
            libBlok = 1;
            giroGirado = 1;
            trava_RPAn = true;
        }
        if (libBlok == 1)
        {
            if (SSH == 'a')
            {
                if (RPA == 'e')
                {
                    if (liberar == 1 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 3;
                    }
                    // if (liberar == 2 && naoBlock == true)
                    // {

                    //     libBlok = 2;
                    //     giroGirado = 1;
                    //     sentido = 4;
                    // }
                }
                if (RPA == 's')
                {
                    // if (liberar == 1 && naoBlock == true)
                    // {

                    //     libBlok = 2;
                    //     giroGirado = 1;
                    //     sentido = 3;
                    // }
                    if (liberar == 2 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 4;
                    }
                }
                if (RPA == 'a')
                {
                    if (liberar == 1 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 3;
                    }
                    if (liberar == 2 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 4;
                    }
                }
            }
            // Sentido anti-horário
            if (SSH == 'h')
            {
                if (RPA == 'e')
                {
                    if (liberar == 1 && naoBlock == true)
                    {

                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 3;
                    }
                    // if (liberar == 2 && naoBlock == true)
                    // {
                    //     libBlok = 2;
                    //     giroGirado = 1;
                    //     sentido = 4;
                    // }
                }
                if (RPA == 's')
                {
                    // if (liberar == 1 && naoBlock == true)
                    // {

                    //     libBlok = 2;
                    //     giroGirado = 1;
                    //     sentido = 3;
                    // }
                    if (liberar == 2 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 4;
                    }
                }
                if (RPA == 'a')
                {
                    if (liberar == 1 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 4; // manda para a função RX_TX no '¹'
                    }
                    if (liberar == 2 && naoBlock == true)
                    {
                        libBlok = 2;
                        giroGirado = 1;
                        sentido = 3;
                    }
                }
            }
        }
        if (libBlok == 2)
        {
            if (liberar == 0)
            {
                libBlok = 3;
                giroGirado = 1;
            }
        }
        if (libBlok == 3)
        {
            // autoriza = 0;
            bloqueado(0);
            giroGirado = 0;
            gpio_put(S_ESQ, 0);
            gpio_put(S_DIR, 0);
            autoriza = 0;
            tempo = 0;
            abre = false;
            RX(sentido);
            trava_RPAn = false;
        }
    }

    // if(STG == 'm')
    // {

    // }

    // bloqueado no stand by
    if (autoriza == 0 && giroGirado == 0)
    {
        gpio_put(S_LIBERADO, 0);
        gpio_put(S_ESQ, 0);
        gpio_put(S_DIR, 0);
        contador = 0;
        if (contador == 0)
        {
            bloqueado(contador);
            libBlok = 0;
            blokLib = 0;
            trava_RPAn = 0;
        }
    }

    if (autoriza == 1 && sinalMisto == false)
    {
        gpio_put(S_LIBERADO, 1);
        // meio do giro no liberado, mas travou
        if (autoriza == 0 && giroGirado == 1 && fimG >= 1)
        {
            gpio_put(S_LIBERADO, 0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            if (SSH == 'a' && RPA == 'e')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            else if (SSH == 'a' && RPA == 's')
            {
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            if (SSH == 'h' && RPA == 'e')
            {
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            else if (SSH == 'h' && RPA == 's')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            if (RPA == 'a')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 1);
            }
            gpio_put(S_STANDBY, 0);
            if (giroGirado == 1 && (fimG == 1 || fimG == 2))
            {
                blokLib = 1;
            }
        }
        if (blokLib == 1 && fimG == 0)
        {
            gpio_put(S_LIBERADO, 0);
            bloqueado(0);
            giroGirado = 0;
            gpio_put(S_ESQ, 0);
            gpio_put(S_DIR, 0);
            // gpio_put(S_DIR, 1);
            trava_RPAn = false;
            autoriza = 0;
            abre = false;
        }
        if (contador == 0 && giroGirado == 1)
        {
            gpio_put(S_LIBERADO, 0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            if (SSH == 'a' && RPA == 'e')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            else if (SSH == 'a' && RPA == 's')
            {

                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            if (SSH == 'h' && RPA == 'e')
            {
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            else if (SSH == 'h' && RPA == 's')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            if (RPA == 'a')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 1);
            }
            gpio_put(S_STANDBY, 0);
        }
        if (contador == 1 && libBlok == 0)
        {
            gpio_put(S_LIBERADO, 0);
            // Acende o Pictograma verde enquanto não houver terminado o giro ou parar a liberação
            if (SSH == 'a' && RPA == 'e')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            else if (SSH == 'a' && RPA == 's')
            {
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            if (SSH == 'h' && RPA == 'e')
            {
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 1);
            }
            else if (SSH == 'h' && RPA == 's')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 0);
            }
            if (RPA == 'a')
            {
                gpio_put(S_ESQ, 1);
                gpio_put(S_DIR, 1);
            }
            gpio_put(S_STANDBY, 0);
            if (libBlok == 3 && contador == 1)
            {
                gpio_put(S_LIBERADO, 0);
                gpio_put(S_ESQ, 0);
                gpio_put(S_DIR, 0);
                autoriza = 0;
            }
        }
    }
    if (SPP == true)
    {
        // Faz o piscapisca
        gpio_put(S_STANDBY, 1);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 1);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 1);
        sleep_ms(400);
        gpio_put(S_STANDBY, 1);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 1);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 1);
        sleep_ms(400);
        gpio_put(S_STANDBY, 1);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 1);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 1);
        sleep_ms(400);
        gpio_put(S_STANDBY, 1);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 1);
        gpio_put(S_ESQ, 0);
        sleep_ms(400);
        gpio_put(S_STANDBY, 0);
        gpio_put(S_BLOCK, 0);
        gpio_put(S_ESQ, 1);
        SPP = false;
    }
}
// Controla a liberação (Na pratica o sinal de giro)
int liberado(int var)
{
    // gpio_put(S_LIBERADO, 1);
    int aux = 0;
    int gaveta = 0;
    if (var == 1 && aux == 0)
    {
        // Novo
        if (SPG == 'p')
        {
            // S1 = 0 && S2 = 0
            if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2))
            {
                aux = 0;
                // printf("%d", aux);
                naoBlock = false;
                return aux;
            }

            // S1 = 1 and S2 = 0
            if (gpio_get(SENSOR1) && !gpio_get(SENSOR2))
            {
                aux = 2;
                // printf("S1 %d", aux);
                return aux;
            }
            // S1 = 0 and S2 = 1
            if (!gpio_get(SENSOR1) && gpio_get(SENSOR2))
            {
                aux = 1;
                // printf("S2 %d", aux);
                return aux;
            }

            // S1 = 1 and S2 = 1
            if (gpio_get(SENSOR1) && gpio_get(SENSOR2))
            {
                aux = 3;
                // printf("S1 + S2 %d", aux);
                return aux;
            }
        }
        if (SPG == 'n')
        {
            // S1 = 0 && S2 = 0
            if (gpio_get(SENSOR1) && gpio_get(SENSOR2))
            {
                aux = 0;
                // printf("%d", aux);
                naoBlock = false;
                return aux;
            }

            // S1 = 1 and S2 = 0
            if (!gpio_get(SENSOR1) && gpio_get(SENSOR2))
            {
                aux = 2;
                // printf("S1 %d", aux);
                return aux;
            }
            // S1 = 0 and S2 = 1
            if (gpio_get(SENSOR1) && !gpio_get(SENSOR2))
            {
                aux = 1;
                // printf("S2 %d", aux);
                return aux;
            }

            // S1 = 1 and S2 = 1
            if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2))
            {
                aux = 3;
                // printf("S1 + S2 %d", aux);
                return aux;
            }
        }
    }
}

// controla bloqueio
int bloqueado(int blok)
{
    printf("Bloqueado %02d\n", blok);
    if (blok == 0)
    {
        if (SSL[1] == 'f')
        {
            if (SPG == 'p')
            {
                // Acionamento S1
                if ((gpio_get(SENSOR1)) && (!gpio_get(SENSOR2)))
                {
                    // sleep_us(1885); // 2180
                    gpio_put(SOL2, 1);
                    // pictograma Vermelho
                    gpio_put(S_BLOCK, 1);
                    gpio_put(S_ESQ, 0);
                    gpio_put(S_DIR, 0);
                    gpio_put(S_BUZZ, 1);
                    gpio_put(S_STANDBY, 0);
                    // Regula_Tensao();
                    sinalMisto = true;
                    blokLib = 0;
                }

                // Acionamento S2
                if ((!gpio_get(SENSOR1)) && (gpio_get(SENSOR2)))
                {
                    // sleep_us(1885); // 2180
                    gpio_put(SOL1, 1);
                    // pictograma Vermelho
                    gpio_put(S_BLOCK, 1);
                    gpio_put(S_ESQ, 0);
                    gpio_put(S_DIR, 0);
                    gpio_put(S_BUZZ, 1);
                    gpio_put(S_STANDBY, 0);
                    // Regula_Tensao();
                    sinalMisto = true;
                    blokLib = 0;
                }

                if (gpio_get(SENSOR2) && gpio_get(SENSOR1))
                {
                    // sleep_us(1540);
                    gpio_put(SOL1, 0);
                    gpio_put(SOL2, 0);
                    gpio_put(S_BLOCK, 0);
                    gpio_put(S_BUZZ, 0);
                    gpio_put(S_STANDBY, 1);
                    sinalMisto = false;
                }
            }
            if (SPG == 'n')
            {
                // Acionamento S1
                if ((!gpio_get(SENSOR1)) && (gpio_get(SENSOR2)))
                {
                    // sleep_us(1885); // 2180
                    gpio_put(SOL2, 1);
                    // pictograma Vermelho
                    gpio_put(S_BLOCK, 1);
                    gpio_put(S_ESQ, 0);
                    gpio_put(S_DIR, 0);
                    gpio_put(S_BUZZ, 1);
                    gpio_put(S_STANDBY, 0);
                    // Regula_Tensao();
                    sinalMisto = true;
                    blokLib = 0;
                }

                // Acionamento S2
                if ((gpio_get(SENSOR1)) && (!gpio_get(SENSOR2)))
                {
                    // sleep_us(1885); // 2180
                    gpio_put(SOL1, 1);
                    // pictograma Vermelho
                    gpio_put(S_BLOCK, 1);
                    gpio_put(S_ESQ, 0);
                    gpio_put(S_DIR, 0);
                    gpio_put(S_BUZZ, 1);
                    gpio_put(S_STANDBY, 0);
                    // Regula_Tensao();
                    sinalMisto = true;
                    blokLib = 0;
                }

                if (!gpio_get(SENSOR2) && !gpio_get(SENSOR1))
                {
                    // sleep_us(1540);
                    gpio_put(SOL1, 0);
                    gpio_put(SOL2, 0);
                    gpio_put(S_BLOCK, 0);
                    gpio_put(S_BUZZ, 0);
                    gpio_put(S_STANDBY, 1);
                    sinalMisto = false;
                }
            }
        }
        // O modo SSLon irá utilizar apenas um solenoide, o solenoide a ser acionado será o 1.
        if (SSL[1] == 'n')
        {
            if (SPG == 'p')
            {
                if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2))
                {
                    sslOn = 0;
                    // sslOff = false;
                }

                if (SSH == 'h')
                {
                    if (!gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOn == 0)
                    {
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        sslOff = false;
                        blokLib = 0;
                    }
                    else
                    {
                        gpio_put(SOL1, 0);
                        gpio_put(S_BLOCK, 0);
                        gpio_put(S_BUZZ, 0);
                        gpio_put(S_STANDBY, 1);
                        sinalMisto = false;
                    }
                    if (gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == true)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 0;
                        sslOff = false;
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        blokLib = 0;
                    }
                }

                if (SSH == 'a')
                {
                    if (gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOn == 0)
                    {
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        sslOff = false;
                        blokLib = 0;
                    }
                    else
                    {
                        gpio_put(SOL1, 0);
                        gpio_put(S_BLOCK, 0);
                        gpio_put(S_BUZZ, 0);
                        gpio_put(S_STANDBY, 1);
                        sinalMisto = false;
                    }
                    if (!gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == true)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 0;
                        sslOff = false;
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        blokLib = 0;
                    }
                }
            }
            if (SPG == 'n')
            {
                if (gpio_get(SENSOR1) && gpio_get(SENSOR2))
                {
                    sslOn = 0;
                    // sslOff = false;
                }

                if (SSH == 'h')
                {
                    if (gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOn == 0)
                    {
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        sslOff = false;
                        blokLib = 0;
                    }
                    else
                    {
                        gpio_put(SOL1, 0);
                        gpio_put(S_BLOCK, 0);
                        gpio_put(S_BUZZ, 0);
                        gpio_put(S_STANDBY, 1);
                        sinalMisto = false;
                    }
                    if (!gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == true)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 0;
                        sslOff = false;
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        blokLib = 0;
                    }
                }

                if (SSH == 'a')
                {
                    if (!gpio_get(SENSOR1) && gpio_get(SENSOR2) && sslOn == 0)
                    {
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        sslOff = false;
                        blokLib = 0;
                    }
                    else
                    {
                        gpio_put(SOL1, 0);
                        gpio_put(S_BLOCK, 0);
                        gpio_put(S_BUZZ, 0);
                        gpio_put(S_STANDBY, 1);
                        sinalMisto = false;
                    }
                    if (gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == true)
                    {
                        sslOn = 1;
                        sslOff = true;
                    }
                    if (!gpio_get(SENSOR1) && !gpio_get(SENSOR2) && sslOff == false)
                    {
                        sslOn = 0;
                        sslOff = false;
                        gpio_put(SOL1, 1);
                        // pictograma Vermelho
                        gpio_put(S_BLOCK, 1);
                        gpio_put(S_ESQ, 0);
                        gpio_put(S_DIR, 0);
                        gpio_put(S_BUZZ, 1);
                        gpio_put(S_STANDBY, 0);
                        Regula_Tensao();
                        sinalMisto = true;
                        blokLib = 0;
                    }
                }
            }
        }
    }
}
// Controle de mensagem
void RX(int mensagem)
{
    switch (mensagem)
    {
    case 1:
        uart_puts(UART_ID1, "Placa FACE_ID Iniciada!");
        sleep_ms(1);
        mensagem = 0;
        break;
    case 2:
        mensagem = 0;
        break;
    case 3:
        uart_puts(UART_ID1, "RPAe");
        sleep_ms(1);
        mensagem = 0;
        apaga_qr();
        segura_segunda_leitura_qrcode = 0;
        RPA = '\0';
        abre = false;
        break;
    case 4:
        uart_puts(UART_ID1, "RPAs");
        sleep_ms(1);
        mensagem = 0;
        apaga_qr();
        segura_segunda_leitura_qrcode = 0;
        RPA = '\0';
        abre = false;
        break;
    case 5:
        uart_puts(UART_ID1, "RPAn");
        sleep_ms(1);
        mensagem = 0;
        apaga_qr();
        segura_segunda_leitura_qrcode = 0;
        RPA = '\0';
        abre = false;
        break;
    }
}
// Regula a tensão de saida
void Regula_Tensao()
{
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