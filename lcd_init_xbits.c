/****************************************************************************/
/*                                                                          */
/* Driver para Displays LCDs baseados no chip Hitachi HD44780               */
/*                                                                          */
/* Versão 1.0.7     (V20211016)                                             */
/*                                                                          */
/* Autores: Fernando da Cunha Luiz                                          */
/*          Halysson Carvalho Junior                                        */
/*                                                                          */
/* Criado em : 12 de outubro de 2021                                        */
/* Revisão em: 16 de outubro de 2021                                        */
/*                                                                          */
/****************************************************************************/

#define DEPURACAO   1

/** BIBLIOTECAS: ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
//#include <stdint.h> // ANSI C99 (int8_t, uint8_t, int16_t, uint16_t, ...)

#define min(a,b)    ((a) > (b) ? (a) : (b))

/** Estilo ANSI C99: ********************************************************/

#define int8_t    signed   char
#define uint8_t   unsigned char
#if 0 // (sizeof(int) == 4)
#   define int16_t   signed   short
#   define uint16_t  unsigned short
#else // (sizeof(int) == 2)
#   define int16_t   signed   int
#   define uint16_t  unsigned int
#endif // (sizeof(...
#define int32_t   signed   long
#define uint32_t  unsigned long

/** FUNÇÕES PÚBLICAS: *******************************************************/

void lcd_init(void);         // Inicializa o display LCD
void lcd_en_pulse(void);     // Pulsa pino EN do display LCD

// Envia comando ou dado para o display LCD, 0 = comando, 1 = dado:
void lcd_envia_comando_dado(uint8_t comando, uint8_t tipo_comando_ou_dado);

// Envia frase para display LCD a partir da posição atual do cursor:
void lcd_print(char *frase, int8_t linha);

/** CONFIGURAÇÕES DO DISPLAY LCD: *******************************************/

#define LCD_MODO_4_BITS 0x20 // Modo de interface de dados 4-bits do display
//#define LCD_MODO_8_BITS 0x30 // Modo de interface de dados 8-bits do display

#define LCD_TAM_LINHA   16   // Quantidade de colunas numa linha do display LCD

#define LCD_TIPO_COMANDO    0
#define LCD_TIPO_DADO       1

#define LCD_BIT_I_D ((uint8_t)(HIGH << 1)) // HIGH => incrementa, LOW => decrementa
#define LCD_BIT_S   ((uint8_t)(HIGH << 0)) // HIGH => desloca,    LOW => não desloca

#define LCD_OFF     0x08                             // Comando "Display off"
#define LCD_LIMPA   0x01                             // Comando "Display clear"
#define LCD_MODO    (0x04 | LCD_BIT_I_D | LCD_BIT_S) // Comando "Display clear"

/** DEFINIÇÕES DAS CONFIGURAÇÕES DO HARDWARE (PINO DA GPIO OU DA PCB: *******/

#define LOW   0
#define HIGH  1

// Pinos para Franzininho DIY: PB0, PB1, PB2, PB3, PB5 e PB4

//#define LCD_PINO_WR    PB? // Define para pino ligado ao GND

//#define LCD_PINO_EN    PB0 // Define para pino 4 do conector PB0 só p/ teste
#define LCD_PINO_EN    PB5 // Define para pino 4 do conector PB5 é RESET na DIY?
#define LCD_PINO_RS    PB4 // Define para pino 5 do conector

#   define LCD_BIT_7   PB0 // Define para pino 0 do conector
#   define LCD_BIT_6   PB1 // Define para pino 1 do conector
#   define LCD_BIT_5   PB2 // Define para pino 2 do conector
#   define LCD_BIT_4   PB3 // Define para pino 3 do conector
#ifdef LCD_MODO_8_BITS
#   define LCD_BIT_3   3 // Define para pino ?
#   define LCD_BIT_2   2 // Define para pino ?
#   define LCD_BIT_1   1 // Define para pino ?
#   define LCD_BIT_0   0 // Define para pino ?
#endif // LCD_MODO_8_BITS

/** FUNÇÕES PRIVADAS: *******************************************************/

static void setup(void); // Setup do hardware
static void loop(void); // Laço infinito (big loop)
static void delay_ms(uint16_t delay_time); // Laço infinito (big loop)
#define delay  delay_ms // define que o delay é em milissegundos

#if DEPURACAO
#   define MAX_COUNT_1MS   5 // Ajustar para loop de 1 milissegundo
#else
#   define MAX_COUNT_1MS   5000 // Ajustar para loop de 1 milissegundo
#endif // DEPURACAO

/** FUNÇÕES PARA DEPURAÇÃO NA CONSOLE DO PC: ********************************/

#define printf_depuracao    printf

#if DEPURACAO

void digitalWrite(unsigned char pino, unsigned char valor){

    //printf_depuracao("Pino/GPIO: %2d = %02X = %3u\n", pino, valor);
    ;  // "Null statement"
}

#else

#   define printf_depuracao(a,...) {} // ; "Null statement"

#endif // DEPURACAO

/****************************************************************************/

static void setup(void){ // Setup do hardware:

#if DEPURACAO
    printf_depuracao("\nsetup...\n");
#endif // DEPURACAO
    lcd_init();

    return;
} // setup()

/****************************************************************************/

static void loop(void){ // Laço infinito (big loop)

    // Comando "Display clear":
    lcd_envia_comando_dado(LCD_LIMPA, LCD_TIPO_COMANDO); // Envia comando para o display LCD
    delay(500); // Espera 500 milissegundos

    lcd_print("Escrevendo na linha 1", 1); // Escreve na linha 1 do display
    delay(500); // Espera 500 milissegundos

    lcd_print("Escrevendo na linha 2", 2); // Escreve na linha 1 do display
    delay(500); // Espera 500 milissegundos

    return;
} // loop()

/****************************************************************************/

int main(void){

    setup(); // Setup do hardware

    while(1){
        loop(); // Laço infinito (big loop)
#if DEPURACAO
        return 1;
#endif
    }

    return 0;

} // main()

/****************************************************************************/

static void delay_ms(uint16_t delay_time){ // Laço infinito (big loop)

    for (int8_t i = 0; i < delay_time; i++){
        for (int8_t j = 0; j < MAX_COUNT_1MS; j++){ // Laço de 1 milissegundo
            ; // "Null statement"
        } // for (int8_t j
    } // for (int8_t i
    return;

} // delay_ms(...

/****************************************************************************/

// Envia frase para display LCD a partir da posição atual do cursor:
void lcd_print(char *frase, int8_t linha){

    int8_t tamanho_linha = (int8_t)min(strlen(frase), LCD_TAM_LINHA);
    // Obs.: Apesar de LCD_TAM_LINHA estar definido, a posição atual do cursor
    // não está sendo verificada => implementação futura.

#if DEPURACAO
    printf_depuracao("\n   lcd_print:  \"");
#endif // DEPURACAO

    for (int8_t i = 0; i < tamanho_linha; i++){
        // Envia dado para o display LCD:
        lcd_envia_comando_dado((uint8_t)frase[i], LCD_TIPO_DADO);
#if DEPURACAO
        printf_depuracao("%c", (uint8_t)frase[i]);
#endif // DEPURACAO
    } // for (int8_t i

#if DEPURACAO
    printf_depuracao("\"");
#endif // DEPURACAO
} // lcd_print(...

/****************************************************************************/

void lcd_init(void){

    uint8_t lcd_comando = 0;

#if DEPURACAO
    printf_depuracao("\nIniciando GPIOs para o Display LCD\n");
#endif // DEPURACAO

    delay(15); // Mínimo de 15ms para 5V ou 40ms para 2,7V, após alimentado

    // Iniciando EN e RS do display LCD:
#if DEPURACAO
    printf_depuracao("\n");
    printf_depuracao("   LCD_PINO_EN = %u\n", LOW);
#endif // DEPURACAO
    digitalWrite(LCD_PINO_EN, LOW); // Desabilitando pulso do EN
    delayMicroseconds(1);

#if DEPURACAO
    printf_depuracao("   LCD_PINO_RS = %u\n", LOW);
#endif // DEPURACAO

    digitalWrite(LCD_PINO_RS, LOW); // Enviando comando
    delayMicroseconds(1);

    // Display esta no modo interface 8-bits.

#if DEPURACAO
    printf_depuracao("\nConfigurando o Display LCD, linha %d\n", __LINE__);
#endif // DEPURACAO

    // Iniciando modo interface 8-bits:
    digitalWrite(LCD_BIT_7, LOW); // 0x30, bits 7 e 6 em baixo + bits 5 e 4 em alto
    digitalWrite(LCD_BIT_6, LOW);
    digitalWrite(LCD_BIT_5, HIGH);
    digitalWrite(LCD_BIT_4, HIGH);

#if DEPURACAO
    printf_depuracao("\n      LCD_BIT_7 LCD_BIT_6 LCD_BIT_5 LCD_BIT_4 = %u %u %u %u \n", LOW, LOW, HIGH, HIGH);
#endif // DEPURACAO

    // Envia 0x30 3 vezes:
    lcd_en_pulse(); // Pulsa pino EN do display
    delay(5); // Mínimo de 4,1ms !!!

    lcd_en_pulse(); // Pulsa pino EN do display
    delayMicroseconds(100); // Mínimo de 100uS !!!

    lcd_en_pulse(); // Pulsa pino EN do display

    // Display esta no modo interface 8-bits.

#ifdef LCD_MODO_4_BITS
#if DEPURACAO
    printf_depuracao("\n   Configurando no modo interface 4-bits... \n");
#endif // DEPURACAO
    // Display esta no modo interface 8-bits.
    // Bits 7 e 6 em baixo + bit 5 em alto + bit 4 em alto
    // DL = Bit 4 = 0 => Troca para modo interface 4-bits
    digitalWrite(LCD_BIT_4, LOW); // 0x20, bit 4 em baixo

#if DEPURACAO
    //printf_depuracao("\n      LCD_BIT_7 LCD_BIT_6 LCD_BIT_5 LCD_BIT_4 = %u %u %u %u \n", LOW, LOW, HIGH, LOW);
#endif // DEPURACAO
    // Envia 0x20:
    lcd_en_pulse(); // Pulsa pino EN do display
#endif // LCD_MODO_4_BITS

#ifdef LCD_MODO_4_BITS
    // DL = Bit 4 = 0 => Modo interface 4-bits
    lcd_comando = ((HIGH << 5) | (LOW << 4)) ; // 0x20, bit 5 em alto e bit 4 em baixo
    // Display esta no modo interface 4-bits.
#if DEPURACAO
    printf_depuracao("\n   Configurado no modo interface 4-bits\n");
#endif // DEPURACAO
#else // LCD_MODO_8_BITS:
    // Display esta no modo interface 8-bits.
    // DL = Bit 4 = 1 => Modo interface 8-bits
    lcd_comando = ((HIGH << 5) | (HIGH << 4)) ; // 0x30, bit 5 e bit 4 em alto
#if DEPURACAO
    // Display esta no modo interface 8-bits.
    printf_depuracao("\n   Configurado no modo interface 8-bits\n");
#endif // DEPURACAO
#endif // LCD_MODO_...
    // Configura para 2 linhas e fonte 5 X 8:
    lcd_comando |= (1 << 3); // N, Linhas, N = 1 => 2 linhas, N = 0 => 1 linha
    lcd_comando |= (0 << 2); // F, Fonte, F = 1 => 5 X 10, F = 0 => 5 X 8
    lcd_envia_comando_dado(lcd_comando, LCD_TIPO_COMANDO); // Envia comando para o display LCD

    lcd_comando = LCD_OFF;                                 // Comando "Display off"
    lcd_envia_comando_dado(lcd_comando, LCD_TIPO_COMANDO); // Envia comando para o display LCD

    lcd_comando = LCD_LIMPA;                               // Comando "Display clear"
    lcd_envia_comando_dado(lcd_comando, LCD_TIPO_COMANDO); // Envia comando para o display LCD

    lcd_comando = LCD_MODO;                                // Comando "Entry mode set"
    lcd_envia_comando_dado(lcd_comando, LCD_TIPO_COMANDO); // Envia comando para o display LCD

    // Fim da Inicialização

} // lcd_init()

/****************************************************************************/

void lcd_en_pulse(void){ // Pulsa pino EN do display
    delayMicroseconds(1); // Mínimo de 1us
    digitalWrite(LCD_PINO_EN, HIGH);
    delayMicroseconds(1); // Mínimo de 1us
    digitalWrite(LCD_PINO_EN, LOW);
} // lcd_en_pulse()

/****************************************************************************/

// Envia comando ou dado para o display LCD, 0 = comando, 1 = dado:
void lcd_envia_comando_dado(uint8_t comando, uint8_t tipo_comando_ou_dado){

    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
#if DEPURACAO
        printf_depuracao("\n   Comando... ");
#endif // DEPURACAO
        digitalWrite(LCD_PINO_RS, LOW); // Enviando comando
    } //
    else{ // (tipo_comando_ou_dado == LCD_TIPO_DADO)
        digitalWrite(LCD_PINO_RS, HIGH); // Enviando dado
    } // (tipo_comando_ou_dado == LCD_TIPO_...

    digitalWrite(LCD_BIT_7, !(!(comando & (1 < 7))));
    digitalWrite(LCD_BIT_6, !(!(comando & (1 < 6))));
    digitalWrite(LCD_BIT_5, !(!(comando & (1 < 5))));
    digitalWrite(LCD_BIT_4, !(!(comando & (1 < 4))));

#ifdef LCD_MODO_8_BITS
#if DEPURACAO
    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
        printf_depuracao("      LCD 8 bits = ");
        printf_depuracao("0x%1X", (comando & 0xf0) >> 4);
    }
#endif // DEPURACAO

    digitalWrite(LCD_BIT_3, !(!(comando & (1 < 3))));
    digitalWrite(LCD_BIT_2, !(!(comando & (1 < 2))));
    digitalWrite(LCD_BIT_1, !(!(comando & (1 < 1))));
    digitalWrite(LCD_BIT_0, !(!(comando & (1 < 0))));

#if DEPURACAO
    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
        printf_depuracao("%1X\n", (comando & 0x0f));
    }
#endif // DEPURACAO

    // Envia 8 bits para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display
#else // LCD_MODO_4_BITS
#if DEPURACAO
    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
        printf_depuracao("      LCD 4 bits = ");
        printf_depuracao("0x%1X", (comando & 0xf0));
    }
#endif // DEPURACAO

    // Envia o nibble mais significativo para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display

    digitalWrite(LCD_BIT_7, !(!(comando & (1 < 3))));
    digitalWrite(LCD_BIT_6, !(!(comando & (1 < 2))));
    digitalWrite(LCD_BIT_5, !(!(comando & (1 < 1))));
    digitalWrite(LCD_BIT_4, !(!(comando & (1 < 0))));

#if DEPURACAO
    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
        printf_depuracao(" 0x%1X\n", (comando & 0x0f) << 4);
    }
#endif // DEPURACAO

    // Envia o nibble menos significativo para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display
#endif // LCD_MODO_...

} // lcd_envia_comando_dado(...

/****************************************************************************/
/*  FIM                                                                     */
/****************************************************************************/
