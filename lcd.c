/****************************************************************************/
/*                                                                          */
/* Protótipo de Driver para Displays LCDs baseados no chip Hitachi HD44780  */
/*                                                                          */
/* Versão 1.0.6     (V20191216)                                             */
/*                                                                          */
/* Autor: Fernando da Cunha Luiz                                            */
/*                                                                          */
/* Criado em : 16 de dezembro de 2019                                       */
/* Revisão em: 16 de outubro de 2021                                        */
/*                                                                          */
/****************************************************************************/
/* Rotinas para o LCD                                                       */
/****************************************************************************/

#define delay_ms  delay
#define delay_us  delayMicroseconds

// Este é o bloco com as rotinas necessárias para manipular o LCD

/*--------------------------------------------------------------------------*/
/* Envio de "Nibble" para o LCD                                             */
/* -------------------------------------------------------------------------*/

//Esta rotina lê o "Nibble" inferior de uma variável e envia para o LCD.
void envia_nibble_lcd(int dado)
{
  //Carrega as vias de dados (pinos) do LCD de acordo com o nibble lido
  output_bit(lcd_db4, bit_test(dado, 0));   //Carrega DB4 do LCD com o bit DADO<0>
  output_bit(lcd_db5, bit_test(dado, 1));   //Carrega DB5 do LCD com o bit DADO<1>
  output_bit(lcd_db6, bit_test(dado, 2));   //Carrega DB6 do LCD com o bit DADO<2>
  output_bit(lcd_db7, bit_test(dado, 3));   //Carrega DB7 do LCD com o bit DADO<3>

  //Gera um pulso de enable
  output_high(lcd_enable);      // ENABLE = 1
  delay_us(1);                  // Recomendado para estabilizar o LCD
  output_low(lcd_enable);       // ENABLE = 0
  return;                       // Retorna ao ponto de chamada da função
}

/*--------------------------------------------------------------------------*/
/* Envio de Byte para o LCD                                                 */
/* -------------------------------------------------------------------------*/

// Esta rotina irá enviar um dado ou um comando para o LCD conforme abaixo:
// ENDEREÇO = 0 -> a variável DADO será uma instrução
// ENDEREÇO = 1 -> a variável DADO será um caractere

void envia_byte_lcd(boolean endereco, int dado)
{
  output_bit(lcd_rs, endereco); // Seta o bit RS para instrução ou caractere
  delay_us(100);                // Aguarda 100 us para estabilizar o pino do LCD
  output_low(lcd_enable);       // Desativa a linha ENABLE
  envia_nibble_lcd(dado >> 4);  // Envia a parte ALTA do dado/comando
  envia_nibble_lcd(dado & 0x0f);// Limpa a parte ALTA e envia a parte BAIXA do
                                // dado/comando
  delay_us(40);                 // Aguarda 40us para estabilizar o LCD com 2,7V
  return;                       // Retorna ao ponto de chamada da função
}

/*--------------------------------------------------------------------------*/
/* Envio de caractere para o LCD                                            */
/* -------------------------------------------------------------------------*/
// Esta rotina serve apenas como uma forma mais fácil de escrever um caractere
// no display. Ela pode ser eliminada e ao invés dela usar diretamente a
// função envia_byte_lcd(1, "<caractere a ser mostrado no LCD>"); ou
// envia_byte_lcd(1, <código do caractere a ser mostrado no LCD>);

void escreve_lcd(char c)  // envia caractere para o display
{
  envia_byte_lcd(1, c);
}

/*--------------------------------------------------------------------------*/
/* Função para limpar o LCD                                                 */
/* -------------------------------------------------------------------------*/
// Como esta operação pode ser muito utilizada, transformando-a em função
// faz com que o código compilado seja menor.

void limpa_lcd(void)
{
  envia_byte_lcd(0, 0x01);         // Envia instrução para limpar o LCD
  delay_ms(2);                     // Aguarda 2ms para estabilizar o LCD
  return;                          // Retorna ao ponto de chamada da função
}

/*--------------------------------------------------------------------------*/
/* Inicializa o LCD                                                         */
/* -------------------------------------------------------------------------*/

void inicializa_lcd(void)
{
  output_low(lcd_db4);     // Garante que o pino DB4 está em 0 (LOW)
  output_low(lcd_db5);     // Garante que o pino DB5 está em 0 (LOW)
  output_low(lcd_db6);     // Garante que o pino DB6 está em 0 (LOW)
  output_low(lcd_db7);     // Garante que o pino DB7 está em 0 (LOW)
  output_low(lcd_rs);      // Garante que o pino RS estão em 0 (LOW)
  output_low(lcd_enable);  // Garante que o pino ENABLE estão em 0 (LOW)
  delay_ms(15);            // Aguarda 15ms para estabilizar o LCD
  envia_nibble_lcd(0x03);  // Envia comando para inicializar o display
  delay_ms(5);             // Aguarda 5ms para estabilizar o LCD
  envia_nibble_lcd(0x03);  // Envia comando para inicializar o display
  delay_ms(5);             // Aguarda 5ms para estabilizar o LCD
  envia_nibble_lcd(0x03);  // Envia comando para inicializar o display
  delay_ms(5);             // Aguarda 5ms para estabilizar o LCD
  envia_nibble_lcd(0x02);  // CURSOR HOME - Envia comando para zerar o
                           //contador de caracteres e retornar à posição
                           // inicial (0x80).
  delay_ms(1);             // Aguarda 1ms para estabilizar o LCD
  envia_byte_lcd(0, 0x28); // FUNCTION SET - Configura o LCD para 4 bits,
                           // 2 linhas, fonte 5X7.
  envia_byte_lcd(0, 0x0c); // DISPLAY CONTROL - Display ligado, sem cursor
  limpa_lcd();             // Limpa o LCD
  envia_byte_lcd(0, 0x06); // ENTRY MODE SET - Desloca o cursor para a direita
  return;                  // Retorna ao ponto de chamada da função
}

/*--------------------------------------------------------------------------*/
/* Define inicio da escrita                                                 */
/* -------------------------------------------------------------------------*/
//Esta função foi adicionada e serve para se definir em que posição do
//LCD deseja-se iniciar a escrita. Para isto basta chamar a Função
//"caracter_Inicio()" indicando a linha e a coluna onde o cursor será
// posicionado antes de se mandar escrever

void caracter_inicio(int linha, int coluna)//define a posicão de inicio da frase
{
  int16 posicao = 0;
     if (linha == 1)
     {
        posicao = 0x80;   //Se setado linha 1, end incial 0x80
     }
     if (linha == 2)
     {
        posicao=0xc0;     //Se setado linha 2, end incial 0xc0
     }

  posicao = posicao + coluna; //soma ao end inicial, o número da coluna
  posicao--;                  //subtrai 1 para corrigir posição

  envia_byte_lcd(0, posicao);
  return;
}
/****************************************************************************/
/* Final das rotinas para o LCD                                             */
/****************************************************************************/
