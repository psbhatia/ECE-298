#include "main.h"
#include "driverlib/driverlib.h"
#include "hal_LCD.h"
#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//#include <math.h>

#define STEPPER_X_PIN_1 1
#define STEPPER_X_PIN_2 2
#define STEPPER_X_PIN_3 3
#define STEPPER_X_PIN_4 4
#define STEPPER_Y_PIN_1 5
#define STEPPER_Y_PIN_2 6
#define STEPPER_Y_PIN_3 7
#define STEPPER_Y_PIN_4 8

#define X 0
#define Y 1

#define STEP_TIME 5000

#define CCW 0
#define CW 1

#define LIMIT_SWITCH_X_MIN 1
#define LIMIT_SWITCH_X_MAX 2
#define LIMIT_SWITCH_Y_MIN 3
#define LIMIT_SWITCH_Y_MAX 4
#define NO_LIMIT 0

volatile int step_number_x = 0;
volatile int step_number_y = 0;

//These are the global x and y values
int currentX = 0; // Assuming the plotter starts at x-origin
int currentY = 0; // Assuming the plotter starts at y-origin

typedef struct Point
{
    float x;
    float y;
} Point;

char *x;
int counter = 0;
char recieved_data[10];
char current_number[3];
int current_is_negative = 0;
int coordinates[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int coordinate_counter = 0;
int digit_counter = 0;
int digit1 = 0;
int digit2 = 0;
int digit3 = 0;

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

uint8_t y;

const int keypad_value[4][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 0, 0,
                                                                          1 } };

/*returns
 LIMIT_SWITCH_X_MIN
 LIMIT_SWITCH_X_MAX
 LIMIT_SWITCH_Y_MIN
 LIMIT_SWITCH_Y_MAX
 NO_LIMIT*/
int readLimitSwitches()
{
    //TODO: turn on LED if success turn off if failure.
    if ((P2IN & BIT7) == 0)
    {
        //Turn on the limit LED.
        P8OUT |= BIT1;
        return LIMIT_SWITCH_X_MIN;
    }
    else if ((P8IN & BIT0) == 0)
    {
        //Turn on the limit LED.
        P8OUT |= BIT1;
        return LIMIT_SWITCH_X_MAX;
    }
    else if ((P1IN & BIT6) == 0)
    {
        //Turn on the limit LED.
        P8OUT |= BIT1;
        return LIMIT_SWITCH_Y_MIN;
    }
    else if ((P5IN & BIT0) == 0)
    {
        //Turn on the limit LED.
        P8OUT |= BIT1;
        return LIMIT_SWITCH_Y_MAX;
    }
    else
    {
        //Turn off the limit LED.
        P8OUT &= ~BIT1;
        return NO_LIMIT;
    }
}

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
        case 5:
            P5OUT |= BIT3; // Set P5.1 to output direction A
            break;
        case 6:
            P1OUT |= BIT3; // Set P2.5 to output direction B
            break;
        case 7:
            P1OUT |= BIT4;  // Set P8.2 to output direction
            break;
        case 8:
            P1OUT |= BIT5;  // Set P8.3 to output direction
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
            P8OUT &= ~BIT2;
            break;
        case 4:
            P8OUT &= ~BIT3;
            break;
        case 5:
            P5OUT &= ~BIT3;
            break;
        case 6:
            P1OUT &= ~BIT3;
            break;
        case 7:
            P1OUT &= ~BIT4;
            break;
        case 8:
            P1OUT &= ~BIT5;
            break;
        }
    }
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

// 0 is CCW
void OneStepX(bool dir)
{
    if (dir)
    {
        switch (step_number_x)
        {
        case 0:
            setPin(STEPPER_X_PIN_1, true);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, false);
            break;
        case 1:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, true);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, true);
            setPin(STEPPER_X_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, true);
            break;
        }
    }
    else
    {
        switch (step_number_x)
        {
        case 0:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, true);
            break;
        case 1:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, true);
            setPin(STEPPER_X_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_X_PIN_1, false);
            setPin(STEPPER_X_PIN_2, true);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_X_PIN_1, true);
            setPin(STEPPER_X_PIN_2, false);
            setPin(STEPPER_X_PIN_3, false);
            setPin(STEPPER_X_PIN_4, false);

        }
    }
    step_number_x++;
    if (step_number_x > 3)
    {
        step_number_x = 0;
    }
    //TODO: display x and y values to the LCD upon success of stepping the motor.
    display_on_lcd(currentX / 100, currentY / 100);
}

// 0 is CCW
void OneStepY(bool dir)
{
    if (dir)
    {
        switch (step_number_y)
        {
        case 0:
            setPin(STEPPER_Y_PIN_1, true);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, false);
            break;
        case 1:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, true);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, true);
            setPin(STEPPER_Y_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, true);
            break;
        }
    }
    else
    {
        switch (step_number_y)
        {
        case 0:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, true);
            break;
        case 1:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, true);
            setPin(STEPPER_Y_PIN_4, false);
            break;
        case 2:
            setPin(STEPPER_Y_PIN_1, false);
            setPin(STEPPER_Y_PIN_2, true);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, false);
            break;
        case 3:
            setPin(STEPPER_Y_PIN_1, true);
            setPin(STEPPER_Y_PIN_2, false);
            setPin(STEPPER_Y_PIN_3, false);
            setPin(STEPPER_Y_PIN_4, false);

        }
    }
    step_number_y++;
    if (step_number_y > 3)
    {
        step_number_y = 0;
    }
    //TODO: display x and y values to the LCD upon success of stepping the motor.
    display_on_lcd(currentX / 100, currentY / 100);
}

int stepMotor(int axis, bool direction)
{
    __delay_cycles(STEP_TIME);
    int limit_switches_value = readLimitSwitches();
    if (axis == X)
    {
        if (limit_switches_value == LIMIT_SWITCH_X_MAX && direction == CW) //If the max limit is pressed and you're moving positively
        {
            return 1;
        }
        else if (limit_switches_value == LIMIT_SWITCH_X_MIN && direction == CCW) //If the MIN limit is pressed and you're moving negatively
        {
            return 1;
        }
        OneStepX(direction);
    }
    else if (axis == Y)
    {
        if (limit_switches_value == LIMIT_SWITCH_Y_MAX && direction == CW) //If the max limit is pressed and you're moving positively
        {
            return 1;
        }
        else if (limit_switches_value == LIMIT_SWITCH_Y_MIN && direction == CCW) //If the MIN limit is pressed and you're moving negatively
        {
            return 1;
        }
        OneStepY(direction);
    }

    return 0;
}

//This is a dumb goto point function that simply goes in the shape of an L to get to a point.
//It's only going to be used for very small x or y increments.
//Tested successfully.
bool gotoPointHelper(const int x, const int y)
{
    int directionX; // Variable helps specify the direction that the motor will always be going, there could be a better way, but for now I have specified an individual direction for both the x and y
    int directionY;
    int axis;
    int changeofX;
    int changeofY;

    if (x > currentX)
    { // This determines the original X-direction of the motor.
        directionX = CW;
        changeofX = 0;
    }
    else if (x < currentX)
    {
        directionX = CCW;
        changeofX = 1;
    }

    if (y > currentY)
    { // This determines the original Y-direction of the motor.
        directionY = CW;
        changeofY = 0;
    }
    else if (y < currentY)
    {
        directionY = CCW;
        changeofY = 1;
    }

    if (currentX == x && currentY == y)
    { // For the scenario of the (x,y) being a single point
        return true;
    }

    if (currentX == x)
    { // For the scenario of the (x,y) resulting in a vertical line
        while (currentY != y)
        {
            axis = Y;
            stepMotor(axis, directionY);
            currentY = currentY + (-2 * changeofY + 1);
        }
        return true;
    }

    if (currentY == y)
    { // For the scenario of the (x,y) resulting in a horizontal line
        while (currentX != x)
        {
            axis = X;
            stepMotor(axis, directionX);
            currentX = currentX + (-2 * changeofX + 1);
        }
        return true;
    }

    while (currentX != x)
    {
        axis = X;
        stepMotor(axis, directionX);
        currentX = currentX + (-2 * changeofX + 1);
    }
    while (currentY != y)
    {
        axis = Y;
        stepMotor(axis, directionY);
        currentY = currentY + (-2 * changeofY + 1);
    }
    return true;
}

int gotoPoint(Point point)
{
    int oldX = currentX; // OldX is the original y-coordinate of the motor
    int oldY = currentY; // OldY is the original y-coordinate of the motor

    float dx = point.x - oldX;
    float dy = point.y - oldY;

    int x = currentX;

    if (x <= point.x)
    {
        for (; x < point.x; x++)
        {
            float y = (float) oldY
                    + (float) dy * ((float) x - (float) oldX) / (float) dx;
            gotoPointHelper(x, (int) y);
        }
    }
    else
    {
        for (; x > point.x; x--)
        {
            float y = (float) oldY
                    + (float) dy * ((float) x - (float) oldX) / (float) dx;
            gotoPointHelper(x, (int) y);
        }
    }

    //Calculate distance using pythagorean theorem:
//    return (float) sqrt(pow(dx, 2) + pow(dy, 2));
    return 0;
}

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

    //Set the limit switches to inputs.
    P2DIR &= ~BIT7; // Xmin L1
    P8DIR &= ~BIT0; // Xmax L2
    P1DIR &= ~BIT6; // Ymin L3
    P5DIR &= ~BIT0; // Ymax L4

    //Set LED to output and turn it off.
    P8DIR |= BIT1;
    P8OUT &= ~BIT1;

    //enable motor drivers
    //set pin5.2 to high
    P5DIR |= BIT2;
    P5OUT |= BIT2;

}

char ADCState = 0; //Busy state of the ADC
int16_t ADCResult = 0; //Storage for the ADC conversion result

void main(void)
{

//    char buttonState = 0; //Current button press state (to allow edge detection)
    x = malloc(sizeof(char) * 8);

    //Turn off interrupts during initialization
    __disable_interrupt();

    //Stop watchdog timer unless you plan on using it
    WDT_A_hold(WDT_A_BASE);

    Init_UART();    //Sets up an echo over a COM port
    Init_LCD();     //Sets up the LaunchPad LCD display

    PMM_unlockLPM5(); //Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

    //All done initializations - turn interrupts back on.
    __enable_interrupt();

    displayScrollText("ECE 298");
    x = "Hello";
    coordinate_counter = 0;
    //Init_UART();

    while (coordinate_counter < 10)
    {
        if (coordinate_counter % 2 == 0)
        {
            showChar('X', pos1);
        }
        else
        {
            showChar('Y', pos1);
        }
    }

    displayScrollText("OUTPUT");

//    display_on_lcd((int) sqrt(2), 6);

    set_output_mode();
//
    Point point1 = { coordinates[0] * 100, coordinates[1] * 100 };
    Point point2 = { coordinates[2] * 100, coordinates[3] * 100 };
    Point point3 = { coordinates[4] * 100, coordinates[5] * 100 };
    Point point4 = { coordinates[6] * 100, coordinates[7] * 100 };
    Point point5 = { coordinates[8] * 100, coordinates[9] * 100 };
    gotoPoint(point1);
    gotoPoint(point2);
    gotoPoint(point3);
    gotoPoint(point4);
    gotoPoint(point5);
//    gotoPointHelper(2000, 5000);
//    gotoPointHelper(3* 100, 2* 100);
//    gotoPointHelper(0, 0);

}

/* UART Initialization */
void Init_UART(void)
{
    /* UART: It configures P1.0 and P1.1 to be connected internally to the
     * eSCSI module, which is a serial communications module, and places it
     * in UART mode. This let's you communicate with the PC via a software
     * COM port over the USB cable. You can use a console program, like PuTTY,
     * to type to your LaunchPad. The code in this sample just echos back
     * whatever character was received.
     */

    //Configure UART pins, which maps them to a COM port over the USB cable
    //Set P1.0 and P1.1 as Secondary Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN1,
    GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN0,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /*
     * UART Configuration Parameter. These are the configuration parameters to
     * make the eUSCI A UART module to operate with a 9600 baud rate. These
     * values were calculated using the online calculator that TI provides at:
     * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
     */

    //SMCLK = 1MHz, Baudrate = 9600
    //UCBRx = 6, UCBRFx = 8, UCBRSx = 17, UCOS16 = 1
    EUSCI_A_UART_initParam param = { 0 };
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 6;
    param.firstModReg = 8;
    param.secondModReg = 17;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = 1;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param))
    {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable EUSCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

}

//STUFF FOR UART

/* EUSCI A0 UART ISR - Echoes data back to PC host */
#pragma vector=USCI_A0_VECTOR
__interrupt
void EUSCIA0_ISR(void)
{
    uint8_t RxStatus = EUSCI_A_UART_getInterruptStatus(
            EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, RxStatus);

    if (RxStatus && counter < 10)
    {

        EUSCI_A_UART_transmitData(EUSCI_A0_BASE,
                                  EUSCI_A_UART_receiveData(EUSCI_A0_BASE));

        //current_number[digitCounter] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
        //int display = atoi(recieved_data[counter]);

        if (digit_counter == 0)
        {
            if (EUSCI_A_UART_receiveData(EUSCI_A0_BASE) == 45)
            {
//                showChar('N', pos2);
                LCDMEM[pos2] = digit_7seg[11];
                current_is_negative = 1;
                digit_counter++;

            }
            else
            {
                current_number[0] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
                showChar(current_number[0], pos3);
                digit_counter = digit_counter + 2;
            }

        }
        else if (digit_counter == 1)
        {
            //negative number
            current_number[0] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            showChar(current_number[0], pos3);
//            LCDMEM[pos3] = digit_7seg[11];
            digit_counter++;
        }
        else if (digit_counter == 2)
        {
            current_number[1] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            showChar(current_number[1], pos4);
            digit_counter++;

        }
//        else if (digit_counter == 3){
//            current_number[2] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
//            showChar(current_number[2],pos5);
//            __delay_cycles(250000);
//            digit_counter++;
//
//
//        }

        if (digit_counter == 3)
        {
            //store the number as an integer

            //digit1 = atoi(&current_number[0]) ;
            //digit2 = atoi(&current_number[1]) ;
            //digit3 = atoi(&current_number[2]);
            digit1 = atoi(&current_number[0]);
            coordinates[coordinate_counter] = digit1;
            if (current_is_negative == 1)
            {
                coordinates[coordinate_counter] = digit1 * (-1);
            }
            __delay_cycles(250000);
            clear_lcd();
            coordinate_counter++;
            digit_counter = 0;
            current_is_negative = 0;
            current_number[0] = 0;
        }

    }
}

//ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt
void ADC_ISR(void)
{
    uint8_t ADCStatus = ADC_getInterruptStatus(ADC_BASE,
    ADC_COMPLETED_INTERRUPT_FLAG);

    ADC_clearInterrupt(ADC_BASE, ADCStatus);

    if (ADCStatus)
    {
        ADCState = 0; //Not busy anymore
        ADCResult = ADC_getResults(ADC_BASE);
    }
}
