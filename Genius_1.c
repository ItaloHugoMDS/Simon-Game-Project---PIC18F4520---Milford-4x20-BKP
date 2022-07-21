#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plib.h>                   //
#include <usart.h>                      //
#include <USART/ubaud.c>                    //
#include <USART/ubusy.c>                        //
#include <USART/uclose.c>                           //
#include <USART/udefs.c>                                // Bibliotecas de comuni-
#include <USART/udrdy.c>                            //      cação serial com o
#include <USART/ugets.c>                        //          LCD;
#include <USART/uopen.c>                    //
#include <USART/uputrs.c>               //
#include <USART/uputs.c>            //
#include <USART/uread.c>        //
#include <USART/uwrite.c>   //

#define _XTAL_FREQ 4000000  // Frequência usada para o PIC18F4520;
#define BAUD_RATE 9600      // Definição da frequência BAUD como 9600, ou seja, 
                            //  9600 bits de tranferência de dados por segundo;

#pragma config WDT  = OFF    // Watchdog desligado;
#pragma config MCLRE = OFF  // Mast erclear desligado;

#define Button_1x1  PORTCbits.RC3    //
#define Button_1x2  PORTCbits.RC4       //
#define Button_1x3  PORTCbits.RC5          //
#define Button_1x4  PORTBbits.RB7             //
#define Button_2x1  PORTCbits.RC7                //
#define Button_2x2  PORTDbits.RD0                   //
#define Button_2x3  PORTDbits.RD1                      //
#define Button_2x4  PORTDbits.RD2                         //
                                                             // Definição de por-
#define Button_3x1  PORTDbits.RD3                         //     tas dos botões;
#define Button_3x2  PORTDbits.RD4                      //        (Entrada de dados)
#define Button_3x3  PORTDbits.RD5                   //    
#define Button_3x4  PORTDbits.RD6                //
#define Button_4x1  PORTDbits.RD7             //
#define Button_4x2  PORTEbits.RE0          //
#define Button_4x3  PORTEbits.RE1       //
#define Button_4x4  PORTEbits.RE2    //


#define Led_1x1 PORTBbits.RB3   //
#define Led_1x2 PORTBbits.RB4      //
#define Led_1x3 PORTBbits.RB5         //
#define Led_1x4 PORTBbits.RB6            //
#define Led_2x1 PORTAbits.RA6               //
#define Led_2x2 PORTBbits.RB0                  //
#define Led_2x3 PORTBbits.RB1                     //
#define Led_2x4 PORTBbits.RB2                        //
                                                        //  Definição de portas
#define Led_3x1 PORTAbits.RA1                        //      dos Leds;
#define Led_3x2 PORTAbits.RA2                     //         (Saída de dados)
#define Led_3x3 PORTAbits.RA3                  //
#define Led_3x4 PORTAbits.RA5               //
#define Led_4x1 PORTCbits.RC2            //
#define Led_4x2 PORTCbits.RC1         //
#define Led_4x3 PORTCbits.RC0      //
#define Led_4x4 PORTAbits.RA0   // 

#define flag_timer_0 INTCONbits.TMR0IF  // Definição da flag do Timer0;
#define flag_timer_1 PIR1bits.TMR1IF    // Definição da flag do Timer1;
#define flag_transmiter PIR1bits.TXIF   // Definção da flag de transmissão de da-
                                        //  dos;

#define Level_Max 128   // Definição dos tamanhos de inicialização dos arrays;

//*******************************[Escopo Global]********************************

int sequence_array [Level_Max];    // Array de armazenamento de sequências;
int active_button_array [Level_Max];    // Array de armazenamento de entrada de
                                        //  dados
int size_sa = 1;    // Pseudo tamanho do array de armazenamento de sequências;
int init = 0;   // Variável de checagem da primeira execução do código;
long int time_sec = 5;  // Tempo de espera para entrada de dados [s], tempo inic-
                        //  ial 5 segundos;
long int repet_time = 1;    // Repetição da contagem de tempo;
long int count;    // Contagem de repetições;
char level[];   // Buffer para armazenamento da string convertida (Nível do joga-
                //  dor);
char level_cd;  // Variável necessária para a conversão do nível do jogador;
char time[];    //Buffer para armazenamento da string convertida (Tempo para en-
                //  trada de dados);
char time_cd;   // Variável necessária para a conversão do tempo de entrada de
                //  dados;
int multi = 450;    // Multiplicador para contagem do tempo;

//*******************************[Escopo Global]********************************

int LCD_Initialize(void)    // Inicialização do LCD;
{
    __delay_ms(500);
    
    int config  = USART_TX_INT_ON & USART_RX_INT_OFF & USART_BRGH_HIGH & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_ADDEN_ON;
    int spbrg   = 25;
    int baud    = BAUD_IDLE_CLK_HIGH & BAUD_8_BIT_RATE & BAUD_WAKEUP_OFF & BAUD_AUTO_OFF;
    
    baudUSART(baud);
    OpenUSART(config, spbrg);
    
    __delay_ms(500);
}

void LCD_Clear(void)    // Limpagem da tela do LCD;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B00000001);
    __delay_ms(10);
}

void LCD_Second_Line(void)    // Colocar o curso na Segunda linha do LCD;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B11000000);
    __delay_ms(10);
}

void LCD_Third_Line(void)    // Colocar o curso na Terceira linha do LCD;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B10010100);
    __delay_ms(10);
}

void LCD_Fourth_Line(void)   // Colocar o curso na Quarta linha do LCD;
{
    WriteUSART(0B11111110);
    WriteUSART(0B11010100);
}

int random_sequence(void)    // Gerar números aleatórios entre 0 e 15;
{
    srand(TMR0);
    return rand() % 16;
}

void adding_to_array(int num)    // Adicionar os números aleatórios ao array da
                                 //  SEQUENCIA A SER REPETIDA ("SEQUENCIA CERTA");
{
    if(((size_sa - 1) == 0) && (init == 0))
    {
        sequence_array[size_sa - 1] = first_seq();
        init = 1;
        __delay_ms(1500);
    }
    else
    {
        sequence_array[size_sa - 1] = num;
    }
}

int level_time(void)    // Realizar a contagem de tempo entre cada nível;
{
    count = (time_sec * multi);
    flag_timer_1 = 0;
    TMR1H = 0x0B;
    TMR1L = 0xDB;
    
    T1CONbits.TMR1ON = 1;
    
    while(1)
    {
        if((repet_time % multi) == 0)
        {
            time_cd = itoa(time, (repet_time / multi), 10);
            LCD_Fourth_Line();
            putsUSART(time_cd);
            putsUSART("s        ");
            putsUSART(itoa(time, time_sec, 10));
            putsUSART("s");
        }
        
        if(flag_timer_0 == 1)
        {
            flag_timer_0 = 0;
        }
        else
        {
            TMR0++;
        }
        
        if(flag_timer_1 == 1);
        {     
            if(repet_time == count)
            {
                T1CONbits.TMR1ON  = 0;
                return 1;
            }
            else
            {
                T1CONbits.TMR1ON  = 0;
                repet_time++;
                return 0;
            }
            
        }
        
    }
    
}

void led_act_desact (int button_act, int state)    // Ativar e desativar os LEDS;
{
    switch(button_act)
    {
        case 0:
        {
            Led_1x1 = state;
            break;
        }
        case 1:
        {
            Led_1x2 = state;
            break;
        }
        case 2:
        {
            Led_1x3 = state;
            break;
        }
        case 3:
        {
            Led_1x4 = state;
            break;
        }
        case 4:
        {
            Led_2x1 = state;
            break;
        }
        case 5:
        {
            Led_2x2 = state;
            break;
        }
        case 6:
        {
            Led_2x3 = state;
            break;
        }
        case 7:
        {
            Led_2x4 = state;
            break;
        }
        case 8:
        {
            Led_3x1 = state;
            break;
        }
        case 9:
        {
            Led_3x2 = state;
            break;
        }
        case 10:
        {
            Led_3x3 = state;
            break;
        }
        case 11:
        {
            Led_3x4 = state;
            break;
        }
        case 12:
        {
            Led_4x1 = state;
            break;
        }
        case 13:
        {
            Led_4x2 = state;
            break;
        }
        case 14:
        {
            Led_4x3 = state;
            break;
        }
        case 15:
        {
            Led_4x4 = state;
            break;
        }
    }
}

int first_seq(void)    // Ler os botões para que o usuário dê início ao jogo;
{
    int flag = 0, button = 0, button_led = 0, first_numb;
    
    LCD_Second_Line();
    
    putsUSART(" Pressione Qualquer");
    
    LCD_Third_Line();
    
    putsUSART(" Tecla Para Iniciar");
    
    do
    {
        if(flag_timer_0 == 1)
        {
            flag_timer_0 = 0;
        }
        else
        {
            TMR0++;
        }
        
        first_numb = random_sequence();
        
        int active_button [] = {Button_1x1,Button_1x2,Button_1x3,Button_1x4,\
                                Button_2x1,Button_2x2,Button_2x3,Button_2x4,\
                                Button_3x1,Button_3x2,Button_3x3,Button_3x4,\
                                Button_4x1,Button_4x2,Button_4x3,Button_4x4};

        if(active_button[button] == 1)
        {
            flag = 1;
        }
        else
        {
            if(button == 15)
            {
               button = 0;
            }
            else
            {
                button++;
            }
        }   
    }while(flag == 0);
    
    LCD_Clear();
    
    return first_numb;
}

void button_act (void)    // Ler os botões e armazenar A SEQUÊNCIA QUE O USUÁRIO
                          //  INSERIU (não necessariamente a sequência certa) no
                          //  ARRAY DE BOTÕES ATIVADOS;
{
    int size_aba = size_sa;
    int current_button_array = 0, button = 0, flag = 0;
    repet_time = 1;
    
    for(int init = 0; init < Level_Max; init++)
    {
        active_button_array[init] = -1;
    }
    
        LCD_Fourth_Line();
        putsUSART("0 s        ");
        putsUSART(itoa(time, time_sec, 10));
        putsUSART("s");

        do
        {
            if(flag_timer_0 == 1)
            {
                flag_timer_0 = 0;
            }
            else
            {
                TMR0++;
            }
            
            int active_button [] = {Button_1x1,Button_1x2,Button_1x3,Button_1x4,\
                                    Button_2x1,Button_2x2,Button_2x3,Button_2x4,\
                                    Button_3x1,Button_3x2,Button_3x3,Button_3x4,\
                                    Button_4x1,Button_4x2,Button_4x3,Button_4x4};

            if(active_button[button] == 1)
            {
                led_act_desact(button, 1);
                __delay_ms(250);
                led_act_desact(button, 0);
                active_button_array[current_button_array] = button;
                current_button_array++;

                if(button == 15)
                {
                   button = 0;
                }
                else
                {
                    button++;
                }
    
            }
            else
            {
            
                if(button == 15)
                {
                   button = 0;
                }
                else
                {
                    button++;
                }
            }
            
            for(int elem = 0; elem <= (size_aba -1); elem++)
            {
              if(active_button_array[elem] != (-1))
              {
                  if(elem == (size_aba -1))
                  {
                    flag = 1;
                    break;
                  }
                  else
                  {
                      continue;
                  }
              }
              else
              {
                break;
              }
            }
            
            if(flag == 1)
            {
                break;
            }
            
        }while ((level_time() == 0));       
}

int checking_sequence(void)    // Faz a checagem da sequência de botões QUE O
                               //  USUÁRIO APERTOU com os botões QUE O USUÁRIO
                               //  DEVERIA TER APERTADO;
{
    int size_aba = size_sa;
    int total_size = (size_aba + size_sa)/2;
    
    button_act();
    
    for(int elem_1 = 0; elem_1 <= (total_size -1); elem_1++)
    {
      if(active_button_array[elem_1] == sequence_array[elem_1] )
      {
          if(elem_1 == (total_size -1))
          {
            return 1;
          }
          else
          {
            continue;
          }
      }
      else
      {
        return 0;
      }
    }
}

void display_current_sequence(void)    // Mostra a sequência aleatória de botões;
{
    for(int seq = 0; seq < (size_sa); seq++)
    {
        int current_array = sequence_array[seq];
        led_act_desact(current_array, 1);
        __delay_ms(500);
        led_act_desact(current_array, 0);
        __delay_ms(500);
    }
}

void main(void)
{
    ADCON1  = 0b00001111;   // Configuração de portas digitais (todas);
    T0CON   = 0b10111000;   // Configuração do Timer0:
                            //      - Prescaler zerado;
                            //      - Clock Externo (Modo Contador);
                            //      - Transição Alto-Baixo nível;
                            //      - Prescaler desabilitado (Modo Contador);
                            //      - Timer0 ativado;
    
    T1CON   = 0b00110000;   // Configuração do Timer1:
                            //      - Prescaler 1:8;
                            //      - Pulso Externo;
                            //      - Sincronizado com o clock;
                            //      - Clock interno;
                            //      - Timer1 desativado;
    
    TRISA   = 0b00010000;    // Portas A: Todas como saída, exceto RA4, pois es-
                             //            tá configurada como entrada para o
                             //            clock externo do timer0;
    TRISB   = 0b10000000;    // Portas B: Todas como saída, exceto RB7, pois es-
                             //            tá configurada como entrada de dados
                             //            do teclado matricial;
    TRISC   = 0b10111000;    // Portas C: Metade como saída, para os leds, e me-
                             //            tade como entrada para o teclado ma-
                             //            tricial;
    TRISD   = 0b11111111;    // Portas D: Todas como entrada para o teclado ma-
                             //            tricial;
    TRISE   = 0b00000111;    // Portas E: RE0, RE1 e RE2 como entrada para o te-
                             //            clado matricial;
 
    TMR0 = 0;       // Iniciando o timer0 com o valor 0;
    flag_timer_0 = 0;    // Zerando a flag do timer0;
    int button;
    
    __delay_ms(1000);
        
    LCD_Initialize();    // Inicializando o LCD;
    
    while(1)    // Início do loop infinito;
    {
        adding_to_array(random_sequence());    // Gerando números aleatórios e a-
                                               //  dicionando ao array de sequên-
                                               //  cia;
                
        putsUSART("Nivel:");    // Enviando informação ao LCD através da comuni-
                                //  cação serial;
        level_cd = itoa(level, size_sa, 10);    // Convertendo o pseudo valor do
                                                //  array em string;
        putsUSART(level_cd);    // Enviando string convertida para o LCD através
                                //  da comunicação serial;
        
        LCD_Third_Line();    // Colocando o cursor na primeira coluna da tercei-
                             //  ra linha do LCD;
        putsUSART("     SEQUENCIA...");    // Enviando informação ao LCD através
                                           //  da comunicação serial;
        
        __delay_ms(1000);
        
        display_current_sequence();    // Mostrando a sequência aleatória de bo-
                                       //  tões através dos acendimento dos LEDS;
        
        LCD_Third_Line();    // Colocando o cursor na primeira coluna da tercei-
                             //  ra linha do LCD;
        putsUSART("       PRONTO ?                 ");    // Enviando informação
                                                          //  ao LCD através da
                                                          //  comunicação serial;
        
        for(button = 0; button <= 15; button++){led_act_desact(button, 1);}
            // Acender todos os leds;
        
        __delay_ms(1000);
        
        for(button = 0; button <= 15; button++){led_act_desact(button, 0);}
            // Apagar todos os leds;
        LCD_Third_Line();    // Colocando o cursor na primeira coluna da tercei-
                             //  ra linha do LCD;
        putsUSART("        AGORA                ");    // Enviando informação ao
                                                       //   LCD através da comu-
                                                       //nicação serial;
        
        if((size_sa % 2) == 0)    // Checar se o nível atual é par
        {
            time_sec = (size_sa / 2) * 5;    // Caso par, usar uma fórmula para
                                             //  calcular o tempo para a entrada
                                             //  de dados;
        }
        else
        {
            time_sec = ((size_sa + 1) / 2) * 5;    // Caso impar usar uma outra
                                                   //  fórmula para calcular o
                                                   //  tempo para a entrada de
                                                   //  dados;
        }
        
        if(checking_sequence() == 1)    // Checar se o valor retornado pela fun-
                                        //  ção de checagem é verdadeiro;
        {
                // Caso verdadeiro (O usuário acertou a sequência):
            __delay_ms(500);
            LCD_Fourth_Line();    // Colocando o cursor na primeira coluna da
                                  //  quarta linha do LCD;
            putsUSART("                   ");    // Enviando informação ao LCD a-
                                                 //  través da comunicação 
                                                 //  serial;
            LCD_Third_Line();    // Colocando o cursor na primeira coluna da ter-
                                 //  ceira linha do LCD;
            putsUSART("       CORRETO            ");    // Enviando informação
                                                        //  ao LCD através da
                                                        //  comunicação serial;
            for(button = 8; button <= 11; button++){led_act_desact(button, 1); __delay_ms(100);}
                // Acender a linha de botões verdes, um a um;
            __delay_ms(125);
            for(button = 8; button <= 11; button++){led_act_desact(button, 0);}
                // Desligar a linha de botões verdes;
            __delay_ms(125);
            for(button = 8; button <= 11; button++){led_act_desact(button, 1);}
                // Acender a linha de botões verdes;
            __delay_ms(250);
            for(button = 11; button >= 8; button--){led_act_desact(button, 0); __delay_ms(100);}
                // Desligar a linha de botões verdes, um a um;
            
            LCD_Clear();    // Limpar a tela do LCD;
            size_sa++;     // Adicionar mais um ao nível/pseudo tamanho do array;
        }
        else
        {
                //Caso Falso (O usuário errou a sequência, não apertou botões su-
                // ficiente ou o tempo esgotou):
            __delay_ms(500);
            LCD_Fourth_Line();    // Colocando o cursor na primeira coluna da
                                  //  quarta linha do LCD;
            putsUSART("                   ");    // Enviando informação ao LCD a-
                                                 //  través da comunicação 
                                                 //  serial;
            LCD_Third_Line();    // Colocando o cursor na primeira coluna da ter-
                                 //  ceira linha do LCD;
            putsUSART("       ERRADO               ");    // Enviando informação
                                                          //  ao LCD através da
                                                          //  comunicação serial;
            for(button = 0; button <= 3; button++){led_act_desact(button, 1); __delay_ms(100);}
                // Acender a linha de botões vermelhos, um a um;
            __delay_ms(125);
            for(button = 0; button <= 3; button++){led_act_desact(button, 0);}
                // Apagar a linha de botões vermelhos;
            __delay_ms(125);
            for(button = 0; button <= 3; button++){led_act_desact(button, 1);}
                // Acender a linha de botões vermelhos;
            __delay_ms(250);
            for(button = 3; button >= 0; button--){led_act_desact(button, 0); __delay_ms(100);}
                // Apagar a linha de botões vermelhos, um a um;
            
            LCD_Clear();    // Limpar tela do LCD;
            time_sec = 5;    // Retornar o tempo entre os níveis para 5 segundos;
            size_sa = 1;    // Retornar para o nível 1;
        }
        
        if(flag_timer_0 == 1)    // Checar se a flag do timer0 foi ativada;
        {
                // Caso verdadeiro:
            flag_timer_0 = 0;    // Desativar a flag do timer0;
        }
        else
        {
                // Caso falso:
            TMR0++;    // Adicionar mais 1 ao timer0 que está no modo contador;
        }
        
    }
    
}
