/**************************  P R E L I M I N A R  ***************************/
/* DISPLAY.C - C Display Subroutines                            06/fev/2014 */
/* Created by Fernando da Cunha Luiz                                        */
/* This functions is to be used in C-51 only                                */
/****************************************************************************/

// 16/out/2021 Convertendo para MCus genéricos
// 20/out/2015 versão para barramento do LCD de 4 bits
// 06/fev/2014 versão original

/****************************************************************************/
#include <stdio.h>
/****************************************************************************/

#include "DISPLAY.H"

#ifndef __MSDOS__ /* constant definided by Borland C, 8051 family */
   void delay(uint delay_t);
   void delayloop(uint delay_t);
#endif

#if TEM_LCD
   static char Lcd_bus_4 = false;
   static char Count = 0, Addr = 0, Linha = 0, Coluna = 0, ConfigLinhas = 2;
   static char Linhas = 1, Colunas = 16; // Total de linhas e de colunas
#endif // TEM_LCD

/****************************************************************************/

#ifndef __MSDOS__ /* constant definided by Borland C, 8051 family */
 #if (__C51__) /* Keil C ou Old Archimedes */
  #if ELEKTOR /* 1 = Teste de MM na placa da revista ELEKTOR */
   // LCD mapeado em memória externa:
   //#define LCD_BUS4  // Barramento de 4 bits para LCD mapeado em pinos de I/O
   //#define LCD_BUS8  // Barramento de 8 bits para LCD mapeado em pinos de I/O
   #define LCD_BUS8X // Barramento de 8 bits para LCD mapeado em memória externa
   #ifdef LCD_BUS8X  // Usando barramento 8-bit para LCD mapeado em memória externa:
      #define lcd_wr_cmd_en()             /* Desnecessário para LCD mapeado em memória externa */
      #define lcd_read_sts()           \
         read_XDATA(LCD_STATUS)
      #define lcd_wait()               \
         if (lcd_read_sts() & 0x80){   \
            delayloop(1); /* 1ms */    \
          /*delay(20);    era 20ms */  \
         }
      #define lcd_read_dat()           \
         read_XDATA(LCD_DATA_RD);   nop()
//    #define lcd_write_cmd_bits(c)    write_XDATA(LCD_CMD, c);   nop() /* => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */
//    #define lcd_write_cmd_4MSb(c)    lcd_write_cmd_bits(c)   /* MS4 e LS4 */
      #define lcd_write_cmd_4MSb(c)       /* MS4 e LS4 */ \
         write_XDATA(LCD_CMD, c);   nop() /* => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */
      #define lcd_wait_write_cmd(c)    \
         lcd_wait();                   \
         write_XDATA(LCD_CMD, c);   nop()
      #define lcd_wait_write_dat(c)    \
         lcd_wait();                   \
         write_XDATA(LCD_DATA_WR, c);  nop()
   #endif // LCD_BUS8X
   #ifdef LCD_BUS4   // Usando barramento 4-bit para LCD mapeado em pinos de I/O:
      uchar lcd_read_sts(){
         static uchar c;
         c = (read_XDATA(LCD_STATUS) /*& 0x0f*/);  nop();   // MS4
         c <<= 4; // MS4
         c |= (read_XDATA(LCD_STATUS) & 0x0f);     nop();   // LS4
         return (c);
      }
      uchar lcd_read_dat(){
         static uchar c;
         c = (read_XDATA(LCD_DATA_RD) /*& 0x0f*/);  nop();   // MS4
         c <<= 4; // MS4
         c |= (read_XDATA(LCD_DATA_RD) & 0x0f);     nop();   // LS4
         return (c);
      }
      static void lcd_wait(){
         if (lcd_read_sts() & 0x80){   \
            delayloop(1); /* 1ms */    \
          //delay(20); /* era 20ms */
         }
      }
      #define lcd_write_cmd_bits(c)    \
         write_XDATA(LCD_CMD, c);   nop() /* => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */
      #define lcd_write_cmd_4MSb(c)    \
         lcd_write_cmd_bits((c) >> 4)     // MS4
      #define lcd_write_cmd_4LSb(c)    \
         lcd_write_cmd_bits((c) & 0x0f)   // LS4
      static void lcd_no_wait_write_cmd(uchar c){
         lcd_write_cmd_4MSb(c);           // MS4
         lcd_write_cmd_4LSb(c);           // LS4
      }
      #define lcd_wait_write_cmd(c)    \
         lcd_wait();                   \
         lcd_no_wait_write_cmd(c)
      #define lcd_write_dat_bits(c)    \
         write_XDATA(LCD_DATA_WR, c);  nop() /* => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */
      #define lcd_write_dat_4MSb(c)    \
         lcd_write_dat_bits((c) >> 4)     // MS4
      #define lcd_write_dat_4LSb(c)    \
         lcd_write_dat_bits((c) & 0x0f)   // LS4
      static void lcd_no_wait_write_dat(uchar c){
         lcd_write_dat_4MSb(c);           // MS4
         lcd_write_dat_4LSb(c);           // LS4
      }
      #define lcd_wait_write_dat(c)    \
         lcd_wait();                   \
         lcd_no_wait_write_dat(c)
   #endif // LCD_BUS4
  #else // ELEKTOR
   // LCD usando alguns pinos de I/O (LCD_CMD_DAT, LCD_WR_RD, LCD_EN e dados em P1.0-P1.3):
   // Usando barramento 4-bit para o Medidor TRIMONO:
   //#define LCD_BUS4 // Barramento de 4 bits
   //#define LCD_BUS8 // Barramento de 8 bits
   // Usando barramento 4-bit ou 8-bit:
   #ifdef LCD_BUS8   // Usando barramento 8-bit:
      #define lcd_set_for_input()   output(P1, 0xff) // LCD_D7 a LCD_D0 = 1
      #define lcd_set_output(c)     output(P1, c)    // LCD_D7 a LCD_D0 = c
   #endif // LCD_BUS8
   #ifdef LCD_BUS4   // Usando barramento 4-bit:
      #define lcd_set_for_input()   output(P1, 0xbf) // ADE_DIN = 1, ADE_SCLK = 0, ADE_DOUT = x, ADE_CS_ = 1, LCD_D7 a LCD_D4 = 1
      #define lcd_set_output(c)     output(P1, (c) | 0xb0) // LCD_D7 a LCD_D4 = MS4 ou LS4 + ADE_DIN = 1, ADE_SCLK = 0, ADE_DOUT = x = 0, ADE_CS_ = 1
   #endif // LCD_BUS4
      #define lcd_rd_sts_en()       \
         clear_bit(LCD_CMD_DAT);    \
         set_bit(LCD_WR_RD);        \
         lcd_set_for_input()
      #define lcd_rd_dat_en()       \
         set_bit(LCD_CMD_DAT);      \
         set_bit(LCD_WR_RD);        \
         lcd_set_for_input()
   #ifdef LCD_BUS8   // Usando barramento 8-bit:
      static uchar lcd_read(){
         static uchar c;
         set_bit(LCD_EN);     nop(); // => Pulse with PWEH > 450ns, tDDR < 360ns
         c = (input(P1));            // MS4 e LS4
         clear_bit(LCD_EN);   nop(); // => tcycE > 1000ns
         return (c);
      }
   #endif // LCD_BUS8
   #ifdef LCD_BUS4   // Usando barramento 4-bit:
      static uchar lcd_read(){
         static uchar c;
         set_bit(LCD_EN);     nop(); // => Pulse with PWEH > 450ns, tDDR < 360ns
         c = (input(P1) /*& 0x0f*/); // MS4
         clear_bit(LCD_EN);   nop(); // => tcycE > 1000ns
         c <<= 4; // MS4
         set_bit(LCD_EN);     nop(); // => Pulse with PWEH > 450ns, tDDR < 360ns
         c |= (input(P1) & 0x0f);    // LS4
         clear_bit(LCD_EN);   nop(); // => tcycE > 1000ns
         return (c);
      }
   #endif // LCD_BUS4
   #if TEM_LCD
      #define lcd_read_sts_bits     lcd_read
      #define lcd_read_dat_bits     lcd_read
      uchar lcd_read_sts(){
         lcd_rd_sts_en();
         return (lcd_read_sts_bits());
      }
      uchar lcd_read_dat(){
         lcd_rd_dat_en();
         return (lcd_read_dat_bits());
      }
      static void lcd_wait(){
         lcd_rd_sts_en();
       //if (lcd_read() & 0x80)
         while (lcd_read_sts_bits() & 0x80)
         {
            delayloop(1); /* era 1ms */
         }
      }
      #define lcd_wr_cmd_en()       \
         clear_bit(LCD_CMD_DAT);    \
         clear_bit(LCD_WR_RD)
      #define lcd_wr_dat_en()       \
         set_bit(LCD_CMD_DAT);      \
         clear_bit(LCD_WR_RD)
      #define lcd_write_cmd_bits(c) \
         set_bit(LCD_EN);           \
         lcd_set_output(c);         \
       /*set_bit(LCD_EN);     nop()*/  /*  => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */ \
         clear_bit(LCD_EN);   nop()    /*  => Pulse with PWEH > 450ns, tDSW > 195ns, tcycE > 1000ns */
      #define lcd_write_dat_bits       lcd_write_cmd_bits
      #ifdef LCD_BUS4   // Usando barramento 4-bit:
         #define lcd_write_cmd_4MSb(c) \
            lcd_write_cmd_bits((c) >> 4)     // MS4
         #define lcd_write_cmd_4LSb(c) \
            lcd_write_cmd_bits((c) & 0x0f)   // LS4
         static void lcd_no_wait_write_cmd(uchar c){
            lcd_write_cmd_4MSb(c);           // MS4
            lcd_write_cmd_4LSb(c);           // LS4
         }
         #define lcd_write_dat_4MSb       lcd_write_cmd_4MSb
       //#define lcd_write_dat_4LSb       lcd_write_cmd_4LSb
         #define lcd_no_wait_write_dat    lcd_no_wait_write_cmd
      #endif // LCD_BUS4
      #ifdef LCD_BUS8   // Usando barramento 8-bit:
         #define lcd_write_cmd_4MSb(c) \
            lcd_write_cmd_bits(c)            // MS4 e LS4
         #define lcd_no_wait_write_cmd    lcd_write_cmd_bits
         #define lcd_no_wait_write_dat    lcd_write_dat_bits
      #endif // LCD_BUS8
      #define lcd_wait_write_cmd(c) \
         lcd_wait();                \
         lcd_wr_cmd_en();           \
         lcd_no_wait_write_cmd(c)
      #define lcd_wait_write_dat(c) \
         lcd_wait();                \
         lcd_wr_dat_en();           \
         lcd_no_wait_write_dat(c)
   #endif // TEM_LCD
  #endif // ELEKTOR
  #if TEM_LCD
   void lcd_write_cmd(uchar c){
      if (Lcd_bus_4 == true){ // Barramendo de 4 bits só pode operar em 4 bits:
         if ((c & 0xe0) == 0x20){ // Function Set Command
            c &= ~0x10; // DL = 0, operando com barramento do LCD de 4 bits
         }
      } // Barramendo de 8 bits também pode operar em 4 bits
      lcd_wait_write_cmd(c);
   }

   void lcd_write_dat(uchar c){
      lcd_wait_write_dat(c);
   }

   void lcd_init(char c){ // Assegura que a inicialização seja correta:
      lcd_wr_cmd_en(); // Define endereço de comando
      #ifdef LCD_BUS8X // Usando barramento mapeado em memória externa
         nop(); nop(); // => tcycE > 1000ns
      #else            // Usando barramento mapeado em pinos de I/O
         clear_bit(LCD_EN);   nop(); // => tcycE > 1000ns
      #endif // LCD_BUS8X

      delayloop(50); /* 50ms >= 40ms after power on (VCC > 2,7V) or 15ms after power on (VCC > 4,5V) */

      lcd_write_cmd_4MSb(0x30); // MS4
      delayloop(15); /* 15ms >= 4,1ms */

      lcd_write_cmd_4MSb(0x30); // MS4
      delayloop(15); /* 15ms >= 100us */

      lcd_write_cmd_4MSb(0x30); // MS4
      delayloop(15); /* 15ms >= ??? */

      #ifdef LCD_BUS4   // Usando barramento 4-bit:
         c = 4; // Force switch to 4-bit mode
      #endif //LCD_BUS4
      if (c == 4){ // Switch to 4-bit mode:
         Lcd_bus_4 = true;
         lcd_write_cmd_4MSb(0x20); // MS4
         delayloop(15); /* 15ms >= ??? */
      }
   }

   void putchar_lcd(char c){ /* Escreve caracter no display LCD */
//delay(50);
      if ((c == CR) || (c == LF) || (c == FF)){ // "\r", "\n" ou "\f"
         switch (c){
            case CR : // Vai para coluna 0 da mesma linha "\r"
               lcd_wait_write_cmd(0x02); // Return home and set DDRAM to address 0x00
               if (Coluna == Colunas){
                  if ((ConfigLinhas == 2) && (Linhas == 2)){
                     if (Linha){
                        Linha = (Linha - 1) & 0x01; // Linha 0 ou 1, vai para coluna 0 da linha anterior
                        lcd_wait_write_cmd(0x80 | ((Linha == 0) ? 0x00 : 0x40)); // Set DDRAM to address 0x00 ou 0x40
                     }
                  }
               }
               Coluna = 0;
               break;
            case LF : // Vai para coluna 0 da próxima linha "\n"
               Coluna = 0;
               if (Linhas == 2){
                  Linha = 1; // Linha 1
               }
               lcd_wait_write_cmd(0x80 | ((Linha == 0) ? 0x00 : 0x40)); // Set DDRAM to address 0x00 ou 0x40
               break;
            case FF : // "\f"
               Coluna = 0;
               Linha = 0;
               lcd_wait_write_cmd(0x01); // Clear display and set DDRAM to address 0x00
               break;
            default:
               ;
         }
         Count = Linha * Colunas + Coluna;
         delayloop(2); /* 2ms >= 1,6ms */
         return;
      }
      if (Coluna == (Colunas / ConfigLinhas)){                                         // coluna 8 ?
         if ((Linhas == 1) && (ConfigLinhas == 2)){                                    //
            Addr = 0x40;                                                               //
         }                                                                             //
         if (Linhas == 2){                                                             //
            Linha = (Linha + 1) & 0x01; // Linha 0 ou 1, vai para coluna 0 da linha    //
            Addr = (Linha ? Coluna | (Linha * 0x40) :  Coluna);                        // coluna ?
         }                                                                             //
         lcd_wait_write_cmd(0x80 | Addr); // Set DDRAM to address Addr                 //
      }                                                                                //
      else if (Coluna == 0){                                                           //
         if ((Linhas == 1) && (ConfigLinhas == 2)){                                    // coluna 0
            Addr = 0x00;                                                               //
         }                                                                             //
         lcd_wait_write_cmd(0x80 | Addr); // Set DDRAM to address Addr                 //
      }                                                                                //
      lcd_wait_write_dat(c);                                                           //
      Coluna = (Coluna + 1) % Colunas;                                                 // coluna 0 .. 15
   }                                                                                   //
  #endif // TEM_LCD
 #endif /* __C51__ */
 #if TEM_LCD
   void gotoxy_lcd(char x, char y){ /* Vai para a coluna x e linha y do display LCD */
      if ((x < 1) || (x >= Colunas)){
         return;
      }
      if ((y < 1) || (y >= Linhas)){
         return;
      }
      Coluna = x - 1;
      Linha = y - 1;
      if (Coluna == (Colunas / ConfigLinhas)){                                         // coluna 8 ?
         if ((Linhas == 1) && (ConfigLinhas == 2)){                                    //
            Addr = 0x40;                                                               //
         }                                                                             //
         if (Linhas == 2){                                                             //
            Linha = (Linha + 1) & 0x01; // Linha 0 ou 1, vai para coluna 0 da linha    //
            Addr = (Linha ? Coluna | (Linha * 0x40) :  Coluna);                        // coluna ?
         }                                                                             //
         lcd_wait_write_cmd(0x80 | Addr); // Set DDRAM to address Addr                 //
      }                                                                                //
      else if (Coluna == 0){                                                           //
         if ((Linhas == 1) && (ConfigLinhas == 2)){                                    // coluna 0
            Addr = 0x00;                                                               //
         }                                                                             //
         lcd_wait_write_cmd(0x80 | Addr); // Set DDRAM to address Addr                 //
      }                                                                                //
      Count = Linha * Colunas + Coluna;
   }

   void puts_lcd(const char *c){ /* Escreve texto no display LCD */
      while (*c){
         putchar_lcd(*(c++));
      }
   }
 #endif // TEM_LCD
#endif // __MSDOS__

/***************************    F     I     M    ****************************/
