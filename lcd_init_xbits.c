/****************************************************************************/
/*                                                                          */
/* Driver para Displays LCDs baseados no chip Hitachi HD44780               */
/*                                                                          */
/* Versão 1.0.0     (V20211012)                                             */
/*                                                                          */
/* Autor: Fernando da Cunha Luiz                                            */
/*                                                                          */
/* Criado em : 12 de outubro de 2021                                        */
/* Revisão em: 12 de outubro de 2021                                        */
/*                                                                          */
/****************************************************************************/

//#define MODO_8_BITS // Modo de dados do display LCD

#define LCD_TIPO_COMANDO    0
#define LCD_TIPO_DADO       1

void lcd_init(void);         // Inicializa o display LCD
void lcd_config_4bits(void); // Inicializa o display LCD no modo interface 4-bits
void lcd_config_8bits(void); // Inicializa o display LCD no modo interface 8-bits
void lcd_en_pulse(void);     // Pulsa pino EN do display LCD

// Envia comando ou dado para o display LCD, 0 = comando, 1 = dado:
void lcd_envia_comando_dado(unsigned char comando, unsigned char tipo_comando_ou_dado);

/****************************************************************************/

// Definições das configurações do hardware:

//#define LCD_PINO_WR    9 // Define pino da GPIO ou na PCB  Está ligado ao GND?
#define LCD_PINO_EN    9 // Define pino da GPIO ou na PCB
#define LCD_PINO_RS    8 // Define pino da GPIO ou na PCB

#   define LCD_BIT_7   7 // Define pino da GPIO ou na PCB
#   define LCD_BIT_6   6 // Define pino da GPIO ou na PCB
#   define LCD_BIT_5   5 // Define pino da GPIO ou na PCB
#   define LCD_BIT_4   4 // Define pino da GPIO ou na PCB
#ifdef MODO_8_BITS
#   define LCD_BIT_3   3 // Define pino da GPIO ou na PCB
#   define LCD_BIT_2   2 // Define pino da GPIO ou na PCB
#   define LCD_BIT_1   1 // Define pino da GPIO ou na PCB
#   define LCD_BIT_0   0 // Define pino da GPIO ou na PCB
#endif // MODO_8_BITS

/****************************************************************************/

void setup(void); // Setup do hardware

/****************************************************************************/

void main(void){

    setup(); // Setup do hardware

    return;
} // main()

/****************************************************************************/

void setup(void){ // Setup do hardware:

    lcd_init();

    return;
} // setup()

/****************************************************************************/

void lcd_init(void){

    unsigned char linhas_fonte = 0;

    delay(40); // Mínimo de 15ms para 5V ou 40ms para 2,7V, após alimentado.

    // Iniciando EN e RS do display LCD:
    LCD_PINO_EN = LOW; // Desabilitando pulso do EN
    delay(1);
    LCD_PINO_RS = LOW; // Enviando comando
    delay(1);

    // Display esta no modo interface 8-bits.

    // Iniciando modo interface 8-bits:
    LCD_BIT_7 = LOW; // 0x30, bits 7 e 6 em baixo + bits 5 e 4 em alto
    LCD_BIT_6 = LOW;
    LCD_BIT_5 = HIGH;
    LCD_BIT_4 = HIGH;

    // Envia 0x30 3 vezes:
    lcd_en_pulse(); // Pulsa pino EN do display
    delay(5); // Mínimo de 4,1ms !!!

    lcd_en_pulse(); // Pulsa pino EN do display
    delay(1); // Mínimo de 100uS !!!

    lcd_en_pulse(); // Pulsa pino EN do display

    // Display esta no modo interface 8-bits.

#ifdef MODO_8_BITS
    lcd_config_8bits(); // Inicializa o display no modo interface 8-bits
    // DL = Bit 4 = 1 => Modo interface 8-bits
    linhas_fonte = ((HIGH << 5) | (HIGH << 4)) ; // 0x30, bit 5 e bit 4 em alto
    // Display esta no modo interface 8-bits.
#else
    lcd_config_4bits(); //  Inicializa o display no modo interface 4-bits
    // DL = Bit 4 = 0 => Modo interface 4-bits
    linhas_fonte = ((HIGH << 5) | (LOW << 4)) ; // 0x20, bit 5 em alto e bit 4 em baixo
    // Display esta no modo interface 4-bits.
#endif

    // Configura para 2 linhas e fonte 5 X 8:
    linhas_fonte |= (1 << 3); // N, Linhas, N = 1 => 2 linhas, N = 0 => 1 linha
    linhas_fonte |= (0 << 2); // F, Fonte, F = 1 => 5 X 10, F = 0 => 5 X 8

    lcd_envia_comando_dado(linhas_fonte, LCD_TIPO_COMANDO); // Envia comando para o display LCD

} // lcd_init()

/****************************************************************************/

// Somente inicializa o display no modo interface 4-bits:
void lcd_config_4bits(void){

    // Display esta no modo interface 8-bits.
    // Bits 7 e 6 em baixo + bit 5 em alto + bit 4 em alto

    // DL = Bit 4 = 0 => Troca para modo interface 4-bits
    LCD_BIT_4 = LOW; // 0x20, bit 4 em baixo

    // Envia 0x20:
    lcd_en_pulse(); // Pulsa pino EN do display

    /*-------------*/
    // Display agora esta no modo interface 4-bits
    /*-------------*/

    // Envia nibble mais significativo:
    // Envia 0x20:
    lcd_en_pulse(); // Pulsa pino EN do display

    // Envia nibble menos significativo:
    // 2 linhas e fonte 5 X 8:
    // Envia 0x28:
    LCD_BIT_7 = (1 << 3); // 0x28 = 0x20 + N, N = 1 => 2 linhas, N = 0 => 1 linha
    LCD_BIT_6 = (0 << 2); // 0x28 = 0x28 + F, Fonte, F = 1 => 5 X 10, F = 0 => 5 X 8


    lcd_en_pulse(); // Pulsa pino EN do display
    delay(1); // Mínimo de 37us

} // lcd_config_4bits()

/****************************************************************************/

// Somente inicializa o display no modo interface 8-bits:
void lcd_config_8bits(void){

    // Display esta no modo interface 8-bits.
    // Bits 7 e 6 em baixo + bit 5 em alto + bit 4 em alto

    // DL = Bit 4 = 1 => Mantem modo interface 8-bits
    LCD_BIT_4 = (HIGH << 4) ; // 0x30, bit 4 em alto

    // Fonte 5 X 8 e 2 linhas:
    LCD_BIT_3 = (1 << 3); // 0x38 = 0x30 + N, N = 1 => 2 linhas, N = 0 => 1 linha
    LCD_BIT_2 = (0 << 2); // 0x38 = 0x38 + F, Fonte, F = 1 => 5 X 10, F = 0 => 5 X 8

    // Bit 4 em baixo + bit 3 com N + bit 2 com F
    // Envia 0x38:
    lcd_en_pulse(); // Pulsa pino EN do display
    delay(1); // Mínimo de 37us

} // lcd_config_8bits

/****************************************************************************/

void lcd_en_pulse(void){ // Pulsa pino EN do display

    delay(1); // Mínimo de 1us
    LCD_PINO_EN = HIGH;
    delay(1); // Mínimo de 1us
    LCD_PINO_EN = LOW;

} // lcd_en_pulse()

/****************************************************************************/

// Envia comando ou dado para o display LCD, 0 = comando, 1 = dado:
void lcd_envia_comando_dado(unsigned char comando, unsigned char tipo_comando_ou_dado){

    if (tipo_comando_ou_dado == LCD_TIPO_COMANDO){
        LCD_PINO_RS = LOW; // Enviando comando
    }
    else{
        LCD_PINO_RS = HIGH; // Enviando dado
    }

    LCD_BIT_7 = !(!(comando & (1 < 7)));
    LCD_BIT_6 = !(!(comando & (1 < 6)));
    LCD_BIT_5 = !(!(comando & (1 < 5)));
    LCD_BIT_4 = !(!(comando & (1 < 4)));
#ifdef MODO_8_BITS
    LCD_BIT_3 = !(!(comando & (1 < 3)));
    LCD_BIT_2 = !(!(comando & (1 < 2)));
    LCD_BIT_1 = !(!(comando & (1 < 1)));
    LCD_BIT_0 = !(!(comando & (1 < 0)));
    // Envia 8 bits para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display
#else
    // Envia o nibble mais significativo para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display
    LCD_BIT_7 = !(!(comando & (1 < 3)));
    LCD_BIT_6 = !(!(comando & (1 < 2)));
    LCD_BIT_5 = !(!(comando & (1 < 1)));
    LCD_BIT_4 = !(!(comando & (1 < 0)));
    // Envia o nibble menos significativo para o LCD:
    lcd_en_pulse(); // Pulsa pino EN do display
#endif    

} // lcd_envia_comando_dado()

/****************************************************************************/
/*  FIM                       */
/****************************************************************************/

