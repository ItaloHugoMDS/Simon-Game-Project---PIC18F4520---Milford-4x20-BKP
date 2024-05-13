#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>                  //
#include <usart.h>                      //
#include <USART/ubaud.c>                    //
#include <USART/ubusy.c>                        //
#include <USART/uclose.c>                           //
#include <USART/udefs.c>                                // Serial Communication 
#include <USART/udrdy.c>                            //     Library (Used for the
#include <USART/ugets.c>                        //         LCD communication)
#include <USART/uopen.c>                    //
#include <USART/uputrs.c>               //
#include <USART/uputs.c>            //
#include <USART/uread.c>        //
#include <USART/uwrite.c>   //

#define _XTAL_FREQ 4000000  // Frequency used for PIC18F4520;
#define BAUD_RATE 9600      // Definying the BAUD frequency reate as 9600,
                            // therefore 9600 bits of data transfer per second;

#pragma config WDT  = OFF    // Turning off Watchdog;
#pragma config MCLRE = OFF  // Turning off Masterclear;

#define Button_1x1  PORTCbits.RC3    //
#define Button_1x2  PORTCbits.RC4       //
#define Button_1x3  PORTCbits.RC5          //
#define Button_1x4  PORTBbits.RB7             //
#define Button_2x1  PORTCbits.RC7                //
#define Button_2x2  PORTDbits.RD0                   //
#define Button_2x3  PORTDbits.RD1                      //
#define Button_2x4  PORTDbits.RD2                         //
                                                             // Defining input
#define Button_3x1  PORTDbits.RD3                         //    ports
#define Button_3x2  PORTDbits.RD4                      //       (user's input)
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
                                                        //  Definying output
#define Led_3x1 PORTAbits.RA1                        //     ports (LED output);
#define Led_3x2 PORTAbits.RA2                     //        
#define Led_3x3 PORTAbits.RA3                  //
#define Led_3x4 PORTAbits.RA5               //
#define Led_4x1 PORTCbits.RC2            //
#define Led_4x2 PORTCbits.RC1         //
#define Led_4x3 PORTCbits.RC0      //
#define Led_4x4 PORTAbits.RA0   // 

#define flag_timer_0 INTCONbits.TMR0IF  // Definying Timer0 flags;
#define flag_timer_1 PIR1bits.TMR1IF    // Definying Timer1 flags;
#define flag_transmiter PIR1bits.TXIF   // Definying flags for data transmission;

#define Level_Max 128   // Definying the initialization size of the array that
                        // will be used to store the sequences;

//*******************************[Global Escope]********************************

int sequence_array [Level_Max];    // Array which will store the sequence;
int active_button_array [Level_Max];    // Array which will store the user's
                                        //  input;
int size_sa = 1;    // Pseudo array size. This is the size which will be
// incremented or decremented on each level;
int init = 0;   // Variable which will check if that's the first time the code
// is being executed;
long int time_sec = 5;  // Waiting time for user's input, initial time is 5
                        // seconds, it increments in 5 seconds each 2 levels;
long int repet_time = 1;    // Repetition for time count;
long int count;    // Variable to keep track of the loop iterations for the
                // timing procedure;
char level[];   // Buffer to store the converted string (This string shows the
                // current level of the game);
char level_cd;  // Variable necessary to make the conversion of the current
                // level of the game;
char time[];    //Buffer to store the converted string (This string will show
                // the remaining time for the user's input);
char time_cd;   // Variable necessary to make the conversion of the remaining
                // time for user's input;
int multi = 450;    // Multiplier for the operation of counting the remaining
                    // time for user's input. This variable is necessary for
                    // making the games' counting mechanic closer to real time.

//*******************************[Global Escope]********************************

int LCD_Initialize(void)    // LCD booting up;
{
    __delay_ms(500);
    
    int config  = USART_TX_INT_ON & USART_RX_INT_OFF & USART_BRGH_HIGH & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_ADDEN_ON;
    int spbrg   = 25;
    int baud    = BAUD_IDLE_CLK_HIGH & BAUD_8_BIT_RATE & BAUD_WAKEUP_OFF & BAUD_AUTO_OFF;
    
    baudUSART(baud);
    OpenUSART(config, spbrg);
    
    __delay_ms(500);
}

void LCD_Clear(void)    // Clear LCD screen;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B00000001);
    __delay_ms(10);
}

void LCD_Second_Line(void)    // Move cursor to the second line in the LCD;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B11000000);
    __delay_ms(10);
}

void LCD_Third_Line(void)    // Move cursor to the third line in the LCD;
{
    __delay_ms(10);
    WriteUSART(0B11111110);
    WriteUSART(0B10010100);
    __delay_ms(10);
}

void LCD_Fourth_Line(void)   // Move cursor to the fourth line in the LCD;
{
    WriteUSART(0B11111110);
    WriteUSART(0B11010100);
}

int random_sequence(void)    // Generating random numbers from 0 to 15;
{
    srand(TMR0);
    return rand() % 16;
}

void adding_to_array(int num)   // Adding the random numbers to the array which
                                // contains the correct sequence;
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

int level_time(void)    // Counting the remaining time for user's input for each
                        // level;
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

void led_act_desact (int button_act, int state)     // Activate and Deactivate
                                                    // LEDs;
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

int first_seq(void)    // Reading user's input to START the game;
{
    int flag = 0, button = 0, button_led = 0, first_numb;
    
    LCD_Second_Line();
    
    putsUSART("  Press any Key to");
    
    LCD_Third_Line();
    
    putsUSART("  Start the Game");
    
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

void button_act (void)  // Reading user's input DURING the game and storing
                        // the input into an array for later comparison of the
                        // sequences;
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

int checking_sequence(void)     // Comparing the sequences. Comparing the CORRECT
                                // sequence with the sequence INPUTED by the user;
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

void display_current_sequence(void)     // Displaying the random sequence;
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
    ADCON1  = 0b00001111;   // Configuring all ports as digital ones;
    T0CON   = 0b10111000;   // Configuring Timer0:
                            //      - Prescaler null;
                            //      - External Clock (Counter Mode);
                            //      - Transition High-low level;
                            //      - Prescaler deactivated (Counter Mode);
                            //      - Timer0 activated;
    
    T1CON   = 0b00110000;   // Configuring Timer1:
                            //      - Prescaler 1:8;
                            //      - External Pulse;
                            //      - Sync with external clock;
                            //      - Internal Clock;
                            //      - Timer1 deactivated;
    
    TRISA   = 0b00010000;   // A Ports: All port as output, except RA4 because
                            //              it is configured as the input for
                            //              the external clock of Timer0;
    
    TRISB   = 0b10000000;   // B Ports: All configured ad output, except RB7
                            //              because it is configured as the
                            //              input for the keypad;
    
    TRISC   = 0b10111000;   // C Ports: Half are configured ad output for the
                            //              LEDs and half are configured as
                            //              input for the keypad;
    
    TRISD   = 0b11111111;   // D Ports: All configured as input for the keypad;
    
    TRISE   = 0b00000111;   // E Ports: RE0, RE1 and RE2 are configured ad input
                            //              for the keypad;
 
    TMR0 = 0;   // Initializing Timer0 as 0;
    flag_timer_0 = 0;    // Seeting Timer0's flag as 0;
    int button;
    
    __delay_ms(1000);
        
    LCD_Initialize();    // Booting up LCD;
    
    while(1)    // Begining of the infinity loop;
    {
        adding_to_array(random_sequence());     // Generating random numbers and
                                                // adding them to the sequence
                                                // array;
                
        putsUSART("Level:");    // Seding information to the LCD through serial
                                // communication;
        
        level_cd = itoa(level, size_sa, 10);    // Converting the pseudo value
                                                // of the array size into string;
        
        putsUSART(level_cd);    // Seding information to the LCD through serial
                                // communication;
        
        LCD_Third_Line();   // Setting the cursos in the first column of the
                            // third row on the LCD;
        
        putsUSART("     SEQUENCE...");  // Seding information to the LCD through
                                        // serial communication;
        
        __delay_ms(1000);
        
        display_current_sequence();    // Displaying the random sequence by
                                       // light up the LEDs;
        
        LCD_Third_Line();   // Setting the cursos in the first column of the
                            // third row on the LCD;
        
        putsUSART("     READY???                   ");   // Seding information to
                                                        // the LCD through
                                                        // serial communication;
        
        for(button = 0; button <= 15; button++){led_act_desact(button, 1);}
            // Lighting up LEDs;
        
        __delay_ms(1000);
        
        for(button = 0; button <= 15; button++){led_act_desact(button, 0);}
            // Turning off LEDs;
        LCD_Third_Line();   // Setting the cursos in the first column of the
                            // third row on the LCD;
        
        putsUSART("     GO                ");   // Seding information to the LCD
                                                // through serial communication;
        
        if((size_sa % 2) == 0)  // Checking if the current level is a even
                                // number;
        {
            time_sec = (size_sa / 2) * 5;   // If the value is even, calculating
                                            // the new remaining time for user's
                                            // input;
        }
        else
        {
            time_sec = ((size_sa + 1) / 2) * 5;     // If the value is odd,
                                                    // calculating a new
                                                    // remaining time for user's
                                                    // input;
        }
        
        if(checking_sequence() == 1)    // Checking if the return value of the
                                        // function is TRUE;

        {   // If the value is TRUE, the user got the sequence RIGHT;
            __delay_ms(500);
            LCD_Fourth_Line();  // Setting the cursor in the first line on the
                                // fourth row of the LCD;
            
            putsUSART("                   ");   // Seding information to the LCD
                                                // through serial communication;
            
            LCD_Third_Line();   // Setting the cursos in the first column of the
                                // third row on the LCD;
            
            putsUSART("     PERFECT              ");    // Seding information to
                                                        // the LCD through
                                                        // serial communication;
            
            for(button = 8; button <= 11; button++){led_act_desact(button, 1); __delay_ms(100);}
                // Lighting up the row of green buttons, one by one;
            __delay_ms(125);
            for(button = 8; button <= 11; button++){led_act_desact(button, 0);}
                // Turning off the row of green buttons;
            __delay_ms(125);
            for(button = 8; button <= 11; button++){led_act_desact(button, 1);}
                // Lighting up the entire row of green buttons;
            __delay_ms(250);
            for(button = 11; button >= 8; button--){led_act_desact(button, 0); __delay_ms(100);}
                // Turning off the row of green buttons, one by one;
            
            LCD_Clear();    // Cleaning up the LCD;
            size_sa++;  // Adding one to the pseudo value of the array size;
        }
        else
        {
                // If it's FALSE, the user got the sequence WRONG, didn't push
                // any buttons or ran out of time;
            __delay_ms(500);
            LCD_Fourth_Line();  // Setting the cursor in the first line on the
                                // fourth row of the LCD;
            
            putsUSART("                   ");   // Seding information to the LCD
                                                // through serial communication;
            
            LCD_Third_Line();   // Setting the cursos in the first column of the
                                // third row on the LCD;
            
            putsUSART("     INCORRECT      ");   // Seding information
                                                            // to the LCD
                                                            // through serial
                                                            // communication;
            
            for(button = 0; button <= 3; button++){led_act_desact(button, 1); __delay_ms(100);}
                // Lighting up the row of red buttons, one by one;
            __delay_ms(125);
            for(button = 0; button <= 3; button++){led_act_desact(button, 0);}
                // Turning off the row of red buttons;
            __delay_ms(125);
            for(button = 0; button <= 3; button++){led_act_desact(button, 1);}
                // Lighting up the row of red buttons.
            __delay_ms(250);
            for(button = 3; button >= 0; button--){led_act_desact(button, 0); __delay_ms(100);}
                // Turning off the row of red buttons, one by one;
            
            LCD_Clear();    // Cleaning up the LCD;
            time_sec = 5;    // Resetting the remaining time as 5 seconds;
            size_sa = 1;    // Returning to Level 1;
        }
        
        if(flag_timer_0 == 1)    // Checking if Timer0's flag was activated;
        {
                // Case TRUE:
            flag_timer_0 = 0;    // Reset Timer0's flag;
        }
        else
        {
                // Case FALSE:
            TMR0++;    // Adding one to TIMER0 which is running in Counter Mode;
        }
        
    }
    
}
