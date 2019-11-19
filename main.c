#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define pos1 4                                                 // Digit A1 - L4
#define pos2 6                                                 // Digit A2 - L6
#define pos3 8                                                 // Digit A3 - L8
#define pos4 10                                                // Digit A4 - L10
#define pos5 2                                                 // Digit A5 - L2
#define pos6 18     // Digit A6 - L18
// Define word access definitions to LCD memories
#define LCDMEMW ((int*)LCDMEM)
#define LCDBMEMW ((int*)LCDBMEM)
#define STEPPER_PIN_1 1
#define STEPPER_PIN_2 2
#define STEPPER_PIN_3 3
#define STEPPER_PIN_4 4

volatile int step_number_x = 0;
volatile int step_number_y = 0;


void setPin(int pinNumber, bool isOn)
{
    if (isOn)
    {
        switch (pinNumber)
        {
        case 1:
            P5OUT |= BIT1; // Set P5.1 to output direction A
            break;
        case 2:
            P2OUT |= BIT5; // Set P2.5 to output direction B
            break;
        case 3:
            P8OUT |= BIT2;  // Set P8.2 to output direction
            break;
        case 4:
            P8OUT |= BIT3;  // Set P8.3 to output direction
            break;
        }
    }
    else
    {
        switch (pinNumber)
        {
        case 1:
            P5OUT &= ~BIT1;
            break;
        case 2:
            P2OUT &= ~BIT5;
            break;
        case 3:
            P8DIR &= ~BIT2;
            break;
        case 4:
            P8OUT &= ~BIT3;
            break;
        }
    }
}


void OneStep(bool dir)
{
    if (dir)
    {
        switch (step_number_x)
        {
        case 0:
            setPin(STEPPER_PIN_1, true);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, false);
            break;
        case 1:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, true);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, true);
            setPin(STEPPER_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, true);
            break;
        }
    }
    else
    {
        switch (step_number_x)
        {
        case 0:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, true);
            break;
        case 1:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, true);
            setPin(STEPPER_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_PIN_1, false);
            setPin(STEPPER_PIN_2, true);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_PIN_1, true);
            setPin(STEPPER_PIN_2, false);
            setPin(STEPPER_PIN_3, false);
            setPin(STEPPER_PIN_4, false);

        }
    }
    step_number_x++;
    if (step_number_x > 3)
    {
        step_number_x = 0;
    }
}


const char digit_7seg[12] = { 0xFC,                                       // "0"
        0x60,                                                      // "1"
        0xDB,                                                      // "2"
        0xF3,                                                      // "3"
        0x67,                                                      // "4"
        0xB7,                                                      // "5"
        0xBF,                                                      // "6"
        0xE4,                                                      // "7"
        0xFF,                                                      // "8"
        0xF7,                                                      // "9"
        0x0,                                                        // " "
        0X03                                                        //"-"
        };

const char digit[11][2] = { { 0xFC, 0x28 }, /* "0" LCD segments a+b+c+d+e+f+k+q */
                            { 0x60, 0x20 }, /* "1" */
                            { 0xDB, 0x00 }, /* "2" */
                            { 0xF3, 0x00 }, /* "3" */
                            { 0x67, 0x00 }, /* "4" */
                            { 0xB7, 0x00 }, /* "5" */
                            { 0xBF, 0x00 }, /* "6" */
                            { 0xE4, 0x00 }, /* "7" */
                            { 0xFF, 0x00 }, /* "8" */
                            { 0xF7, 0x00 }, /* "9" */
                            { 0x03, 0x00 } /* "-" */

};

const char alphabetBig[26][2] = { { 0xEF, 0x00 }, /* "A" LCD segments a+b+c+e+f+g+m */
                                  { 0xF1, 0x50 }, /* "B" */
                                  { 0x9C, 0x00 }, /* "C" */
                                  { 0xF0, 0x50 }, /* "D" */
                                  { 0x9F, 0x00 }, /* "E" */
                                  { 0x8F, 0x00 }, /* "F" */
                                  { 0xBD, 0x00 }, /* "G" */
                                  { 0x6F, 0x00 }, /* "H" */
                                  { 0x90, 0x50 }, /* "I" */
                                  { 0x78, 0x00 }, /* "J" */
                                  { 0x0E, 0x22 }, /* "K" */
                                  { 0x1C, 0x00 }, /* "L" */
                                  { 0x6C, 0xA0 }, /* "M" */
                                  { 0x6C, 0x82 }, /* "N" */
                                  { 0xFC, 0x00 }, /* "O" */
                                  { 0xCF, 0x00 }, /* "P" */
                                  { 0xFC, 0x02 }, /* "Q" */
                                  { 0xCF, 0x02 }, /* "R" */
                                  { 0xB7, 0x00 }, /* "S" */
                                  { 0x80, 0x50 }, /* "T" */
                                  { 0x7C, 0x00 }, /* "U" */
                                  { 0x0C, 0x28 }, /* "V" */
                                  { 0x6C, 0x0A }, /* "W" */
                                  { 0x00, 0xAA }, /* "X" */
                                  { 0x00, 0xB0 }, /* "Y" */
                                  { 0x90, 0x28 } /* "Z" */
};



const int keypad_value[4][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 0, 0,
                                                                          1 } };


void set_output_mode()
{
    showChar('O', pos1);
    showChar('U', pos2);
    showChar('T', pos3);
    showChar('P', pos4);
    showChar('U', pos5);
    showChar('T', pos6);

    //Disable pulldown resistors
    P5REN &= ~BIT3;
    P1REN &= ~BIT3;
    P1REN &= ~BIT4;
    P1REN &= ~BIT5;

    // 5.1,2.5,8.2,8.3 J1 MOTORX
    P5DIR |= BIT1; //A
    P2DIR |= BIT5; //B
    P8DIR |= BIT2; //C
    P8DIR |= BIT3; //D

    //5.3 1.3 1.4 1.5 J2 MOTORY
    P5DIR |= BIT3; //A
    P1DIR |= BIT3; //B
    P1DIR |= BIT4; //C
    P1DIR |= BIT5; //D

    //enable motor drivers
    //set pin5.2 to high
    P5DIR |= BIT2;
    P5OUT |= BIT2;

}
void showChar(char c, int position)
{
    if (c == ' ')
    {
        // Display space
        LCDMEMW[position / 2] = 0;
    }
    else if (c >= '0' && c <= '9')
    {
        // Display digit
        LCDMEMW[position / 2] = digit[c - 48][0] | (digit[c - 48][1] << 8);
    }
    else if (c == '-')
    {
        // Display digit
        LCDMEMW[position / 2] = digit[10 - 48][0] | (digit[10 - 48][1] << 8);
    }
    else if (c >= 'A' && c <= 'Z')
    {
        // Display alphabet
        LCDMEMW[position / 2] = alphabetBig[c - 65][0]
                | (alphabetBig[c - 65][1] << 8);
    }
    else
    {
        // Turn all segments on if character is not a space, digit, or uppercase letter
        LCDMEMW[position / 2] = 0xFFFF;
    }
}

void set_input_mode()
{

    //    LCDMEMCTL |= LCDCLRM; //clear the LCD

    //set rows to inputs
    P5DIR &= ~BIT3;
    P1DIR &= ~BIT3;
    P1DIR &= ~BIT4;
    P1DIR &= ~BIT5;

    //set columns to outputs
    P5DIR |= BIT1;
    P2DIR |= BIT5;
    P8DIR |= BIT2;

    //enable pulldown
    P5REN |= BIT3;
    P1REN |= BIT3;
    P1REN |= BIT4;
    P1REN |= BIT5;

    //disable motor drivers
    //set pin5.2 to low
    P5DIR |= BIT2;
    P5OUT &= ~BIT2;

}

void display_on_lcd(int x, int y)
{
    if (x < 0)
    {
        x = -x;
        //showChar('-', pos1);
        LCDMEM[pos1] = digit_7seg[11];
    }
    else
    {
        showChar(' ', pos1);
    }
    int x1 = x / 10;
    int x2 = x % 10;

    if (y < 0)
    {
        y = -y;
        //showChar('-', pos4);
        LCDMEM[pos4] = digit_7seg[11];
    }
    else
    {
        showChar(' ', pos4);
    }
    int y1 = y / 10;
    int y2 = y % 10;

    showChar(x1 + '0', pos2);
    showChar(x2 + '0', pos3);
    showChar(y1 + '0', pos5);
    showChar(y2 + '0', pos6);

    LCDCTL0 |= LCD4MUX | LCDON;  // Turn on LCD, 4-mux selected

}

void clear_lcd()
{

    showChar(' ', pos1);
    showChar(' ', pos2);
    showChar(' ', pos3);
    showChar(' ', pos4);
    showChar(' ', pos5);
    showChar(' ', pos6);

}

int take_input()
{
    int digit1 = 0;
    int digit2 = 0;
    int stored = 0;

    printf("before while loop \n");
    int column = 0;
    while (1)
    {
        if (column >= 3)
        {
            column = 0;
        }
        //set P8.2(COL3) and p2.5(COL2) to high ( to disable these columns ) and P5.1 to low ( to enable this column)
        int row = -1;

        printf("entered while loop \n");
        //Column 1 case:

        set_column(column);
        if ((P5IN & BIT3) == 0)/* pins for row 0*/
        {

            row = 0;
            stored++;

        }

        if ((P1IN & BIT3) == 0) /* pins for row 1*/
        {
            row = 1;
            stored++;

        }

        if ((P1IN & BIT4) == 0) /* pins for row 2*/
        {
            row = 2;
            stored++;

        }

        if ((P1IN & BIT5) == 0) /* pins for row 3*/
        {
            row = 3;
            stored++;

        }

        if (stored == 1)
        {
            digit1 = keypad_value[row][column];
            LCDMEM[pos3] = digit_7seg[digit1];
            stored++;
            __delay_cycles(250000);
        }
        else if (stored == 3)
        {
            digit2 = keypad_value[row][column];
            LCDMEM[pos4] = digit_7seg[digit2];
            __delay_cycles(500000);
            break;
        }
        column++;
    }

    return (digit1 * 10) + digit2;
}

int check_negative()
{

    int is_negative = 0;
    int stored = 0;
    int column = 0;

    while (1)
    {

        if (column >= 3)
        {
            column = 0;
        }
        int row = -1;
        set_column(column);
        if ((P1IN & BIT5) == 0) /* pins for row 3*/
        {
            row = 3;
            stored++;

        }

        if (stored == 1)
        {
            is_negative = keypad_value[row][column];
            __delay_cycles(500000);
            break;
        }
        column++;

    }
    return is_negative;

}

void set_column(int column)
{

    if (column == 0)
    {
        P8OUT |= BIT2;
        P2OUT |= BIT5;
        P5OUT &= ~BIT1;

    }
    else if (column == 1)
    {
        P8OUT |= BIT2;
        P2OUT &= ~BIT5;
        P5OUT |= BIT1;
    }
    else if (column == 2)
    {
        P8OUT &= ~BIT2;
        P2OUT |= BIT5;
        P5OUT |= BIT1;
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                             // Stop watchdog timer

    // Configure XT1 oscillator
    P4SEL0 |= BIT1 | BIT2;                            // P4.2~P4.1: crystal pins
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);           // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }
    while (SFRIFG1 & OFIFG);                       // Test oscillator fault flag
    CSCTL6 = (CSCTL6 & ~(XT1DRIVE_3)) | XT1DRIVE_2; // Higher drive strength and current consumption for XT1 oscillator

//     Disable the GPIO power-on default high-impedance mode
//     to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LCD pins
    SYSCFG2 |= LCDPCTL;             // R13/R23/R33/LCDCAP0/LCDCAP1 pins selected

    LCDPCTL0 = 0xFFFF;
    LCDPCTL1 = 0x07FF;
    LCDPCTL2 = 0x00F0;                         // L0~L26 & L36~L39 pins selected

    LCDCTL0 = LCDSSEL_0 | LCDDIV_7;                    // flcd ref freq is xtclk

    // LCD Operation - Mode 3, internal 3.08v, charge pump 256Hz
    LCDVCTL = LCDCPEN | LCDREFEN | VLCD_6
            | (LCDCPFSEL0 | LCDCPFSEL1 | LCDCPFSEL2 | LCDCPFSEL3);

    LCDMEMCTL |= LCDCLRM;                                    // Clear LCD memory

    LCDCSSEL0 = 0x000F;                               // Configure COMs and SEGs
    LCDCSSEL1 = 0x0000;                              // L0, L1, L2, L3: COM pins
    LCDCSSEL2 = 0x0000;

    LCDM0 = 0x21;                                        // L0 = COM0, L1 = COM1
    LCDM1 = 0x84;                                        // L2 = COM2, L3 = COM3

    //turn on lcd
    LCDCTL0 |= LCD4MUX | LCDON;
    //TAKE INPUTS FROM USER
    set_input_mode();

    //take input X from user
    clear_lcd();

    showChar('N', pos1);
    int is_negative_x = check_negative();

    showChar('X', pos1);
    int x = take_input();
    if (is_negative_x == 1)
    {
        x = -x;
    }

    clear_lcd();
    showChar('N', pos1);
    int is_negative_y = check_negative();
    showChar('Y', pos1);
    int y = take_input();
    if (is_negative_y == 1)
    {
        y = -y;
    }

    clear_lcd();

    //clear particularly the first bit
    showChar(' ', pos1);

    display_on_lcd(x, y);

    __delay_cycles(1000000);

    set_output_mode();

    while (1)
    {
        __delay_cycles(100000);
        OneStep(1);
    }

    /*INPUT MODE DONE AT THIS POINT START OUTPUT MODE */

    // step motor X, X number of times
    // step motor Y, Y number of times
    //if value reaches over a current value

    PMMCTL0_H = PMMPW_H;                         // Open PMM Registers for write
    PMMCTL0_L |= PMMREGOFF_L;                               // and set PMMREGOFF

    __bis_SR_register(LPM3_bits | GIE);                        // Enter LPM3.5
    __no_operation();                                          // For debugger
}

