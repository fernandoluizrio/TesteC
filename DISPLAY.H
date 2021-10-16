/**************************  P R E L I M I N A R  ***************************/
/* FILE DISPLAY.H                                               06/fev/2014 */
/* Configurations for some MCUs 8051 compatibles                            */
/* Header file containing the Atmel CPU configuration.                      */
/* Created by Fernando da Cunha Luiz                                        */
/* This functions is to be used in C-51 only                                */
/****************************************************************************/

// 16/out/2021 Convertendo para MCus genéricos
// 20/out/2015 versão para barramento do LCD de 4 bits
// 06/fev/2014 versão original


/****************************************************************************/

#ifndef __DISPLAY_H
#define __DISPLAY_H

/****************************************************************************/

void lcd_init(char lcd_bus);
void lcd_write_cmd(uchar c);
void putchar_lcd(char c);           /* Escreve caracter no display LCD */
void gotoxy_lcd(char x, char y);    /* Vai para a coluna x e linha y do display LCD */
void puts_lcd(const char *c);       /* Escreve texto no display LCD */

/****************************************************************************/

#ifdef __C51__ /* Defined on C-51 compiler command line */
   #if (__C51__ > 1) /* Keil C */
      #include "..\Keil\REG52.H"      /* include CPU definition file (for example, 8052)*/
      #include "..\H51\IO51TYP.H"
      #include "..\H51\IO51BITS.H"
      #include "..\H51\PX51BITS.H"
   #elif (__C51__ == 1) /* Old Archimedes C-51 compiler */
      #include <io51.h>
      #include "..\H51\IO51TYP.H"
   #endif /* __C51__ */
#endif /* __C51__ */

/****************************************************************************/

#ifndef DT_VERSAO          /* DT_VERSAO deve estar definido no projeto */
   #error Compilation Error : VERSAO deve estar definido no projeto
#endif

#define AT89S8252          0 /*  8 Kbytes flash */
#define T89C51Rx2          1 /* até 63 Kbytes flash - T89C51RB2/RC2/RD2 */
#define AT89C51Rx2         0 /* 64 Kbytes flash - AT89C51RD2/ED2 */
#ifdef T89C51Rx2
   #define T89C51Rx2_OR_AT89C51Rx2 1 /* T89C51RB2/RC2/RD2 ou AT89C51RD2/ED2 */
#else
 #ifdef AT89C51Rx2
   #define T89C51Rx2_OR_AT89C51Rx2 1 /* T89C51RB2/RC2/RD2 ou AT89C51RD2/ED2 */
 #else
   #define T89C51Rx2_OR_AT89C51Rx2 0 /* T89C51RB2/RC2/RD2 ou AT89C51RD2/ED2 */
 #endif
#endif

#ifndef ARCHIMEDES_C
   #define ARCHIMEDES_C    0 /* Other compilers */
#endif
#ifndef __C51__
   #if ARCHIMEDES_C
      #define __C51__      1 /* Old Archimedes */
   #else
      #define __C51__      0 /* Other compilers */
   #endif
#endif

#ifdef ARCHIMEDES_C /* Old Archimedes */
   #include "..\H51\limits51.h"
#else
   #include <limits.h>
#endif

#include "..\H51\MEM51TYP.H"

#define ON     0xFFFFFFFFU
#define OFF    0x00000000
#define TRUE   0xFFFFFFFFU
#define FALSE  0x00000000
#define true   1
#define false  0

/****************************************************************************/
   #define TEM_LCD   1 /* Com LCD HD44780 */
/****************************************************************************/

#define tx_ASCII        1

#define ESC             0x1B // Escape
#define BS              0x08 // BackSpace "\b"
#define FF              0x0C // FormFeed "\f"
#define CR              0x0D // Carriage Return "\r"
#define LF              0x0A // LineFeed "\n"
#define LOAD            'L'
#define SOH             0x01
#define STX             0x02
#define ETX             0x03
#define ACK             0x06
#define XON             0x11
#define XOFF            0x13
#define NAK             0x15
#define CAN             0x18
/*#define PGM           0x16  * Erro de programação na FLASH/EEPROM */
#define PGM             CAN  /* Erro de programação na FLASH/EEPROM */

#define BOOT_EPROM      0x08 /* era 255 */
#define LIMPA_NOVRAM    0x20 /* novo    */
#define NUMSERIE_SET    0x24 /* novo    */
#define NUMSERIE_GET    0x25 /* novo    */
#define VERSAO_FIRMW    0x28 /* novo    */

/*#ifdef __MSDOS__  * constant definided by Borland C *
*  uchar set_bit(const uchar);
*  uchar clear_bit(const uchar);
*  uchar read_bit(const uchar);
*  uchar read_bit_and_clear(const uchar);
*  uchar read_XCODE(uint);
*  uchar read_XDATA(uint);
*  uchar write_XDATA(uint,uchar);
* #endif*/

/*******************************************/

#ifndef __MSDOS__ /* constant definided by Borland C, 8051 family */

   #define USETIMER2    0

#if MRC                          /* ;Testes de na placa do MRC. */
   #define ADDR_PPI        0x8000U     /* BASE ADDRESS OF 74HCT244 */
   #define RAM_SIZE        0x8000U     /* SIZE OF EXTERNAL RAM */
   #define PROG_SIZE       0x8000U     /* SIZE OF EXTERNAL EPROM */
   #define NOVRAM_ADDR     0xC000U     /* BASE ADDRESS OF EXTERNAL NON VOLATILE RAM */
   #define NOVRAM_SIZE     0x0800U     /* SIZE OF EXTERNAL NON VOLATILE RAM */
#endif   /* MRC */

#if ELEKTOR                      /* Placa de desenvolvimento: */
   #define RAM_SIZE        0x8000U /* SIZE OF EXTERNAL RAM */
   #define PROG_SIZE       0x2000U /* SIZE OF INTERNAL FLASH */
   #define NOVRAM_ADDR     0x0000U /* BASE ADDRESS OF INTERNAL EEPROM */
   #define NOVRAM_SIZE     0x0800U /* SIZE OF INTERNAL EEPROM */
   #define LCD_CMD         0X8000  /* Escrita de comandos */
   #define LCD_STATUS      0X8001  /* Leitura de status */
   #define LCD_DATA_WR     0X8002  /* Escrita de dados */
   #define LCD_DATA_RD     0X8003  /* Leitura de dados */
#else
 #ifndef MM_AT                    /* Outras placas: */
   #define PROG_SIZE       0x2000U /* SIZE OF INTERNAL FLASH */
   #define NOVRAM_ADDR     0x0000U /* BASE ADDRESS OF INTERNAL EEPROM */
   #define NOVRAM_SIZE     0x0800U /* SIZE OF INTERNAL EEPROM */
 #endif /* MM_AT */
#endif /* ELEKTOR */

/****************************************************************************/

#if (__C51__ == 1) /* Old Archimedes */
   #ifndef nop
      #define  nop()           set_bit(P0_0_bit)   /* 1 ciclo de instrução */
   #endif
#endif /* __C51__ */

/****************************************************************************/

#endif /* __DISPLAY_H */

/***************************    F     I     M    ****************************/
