#include "main.h"
#include "driverlib/driverlib.h"
#include "hal_LCD.h"

char *x ;
int counter = 0;
char recieved_data[10];
char current_number[3];
int current_is_negative = 0;
int coordinates[10] = {0,0,0,0,0,0,0,0,0,0};
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

void clear_lcd()
{

    showChar(' ', pos1);
    showChar(' ', pos2);
    showChar(' ', pos3);
    showChar(' ', pos4);
    showChar(' ', pos5);
    showChar(' ', pos6);

}

char ADCState = 0; //Busy state of the ADC
int16_t ADCResult = 0; //Storage for the ADC conversion result

void main(void)
{

    char buttonState = 0; //Current button press state (to allow edge detection)
    x = malloc(sizeof(char)*8);

    //Turn off interrupts during initialization
    __disable_interrupt();

    //Stop watchdog timer unless you plan on using it
    WDT_A_hold(WDT_A_BASE);

    // Initializations - see functions for more detail
    //nit_GPIO();    //Sets all pins to output low as a default
    //Init_PWM();     //Sets up a PWM output
    // Init_ADC();     //Sets up the ADC to sample
    //Init_Clock();   //Sets up the necessary system clocks
    Init_UART();    //Sets up an echo over a COM port
    //Init_LCD();     //Sets up the LaunchPad LCD display

    PMM_unlockLPM5(); //Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

    //All done initializations - turn interrupts back on.
    __enable_interrupt();

    displayScrollText("ECE 298");
    x = "Hello";
    //Init_UART();


    while (coordinate_counter<3){
        if (coordinate_counter%2 == 0){
            showChar('X', pos1);
        }
        else{
            showChar('Y', pos1);
        }
    }



        int x_1 = recieved_data[0] - '0';
        int x_2 = atoi(recieved_data[1]);




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
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);

    /*
     * UART Configuration Parameter. These are the configuration parameters to
     * make the eUSCI A UART module to operate with a 9600 baud rate. These
     * values were calculated using the online calculator that TI provides at:
     * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
     */

    //SMCLK = 1MHz, Baudrate = 9600
    //UCBRx = 6, UCBRFx = 8, UCBRSx = 17, UCOS16 = 1
    EUSCI_A_UART_initParam param = {0};
        param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
        param.clockPrescalar    = 6;
        param.firstModReg       = 8;
        param.secondModReg      = 17;
        param.parity            = EUSCI_A_UART_NO_PARITY;
        param.msborLsbFirst     = EUSCI_A_UART_LSB_FIRST;
        param.numberofStopBits  = EUSCI_A_UART_ONE_STOP_BIT;
        param.uartMode          = EUSCI_A_UART_MODE;
        param.overSampling      = 1;

    if(STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param))
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
    uint8_t RxStatus = EUSCI_A_UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, RxStatus);

    if (RxStatus && counter<10)
    {

        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, EUSCI_A_UART_receiveData(EUSCI_A0_BASE));

        //current_number[digitCounter] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
        //int display = atoi(recieved_data[counter]);

        if (digit_counter == 0 ){
            if (EUSCI_A_UART_receiveData(EUSCI_A0_BASE) == 45){
                showChar('N', pos2);
                current_is_negative = 1;
                digit_counter ++;

            }
            else{
                current_number[0] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
                showChar(current_number[0],pos3);
                digit_counter = digit_counter+2;
            }

        }
        else if (digit_counter == 1){
            //negative number
            current_number[0] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            showChar(current_number[0],pos3);
            digit_counter++;
        }
        else if (digit_counter == 2){
            current_number[1] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            showChar(current_number[1],pos4);
            digit_counter++;


        }
        else if (digit_counter == 3){
            current_number[2] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            showChar(current_number[2],pos5);
            __delay_cycles(250000);
            digit_counter++;


        }


        if (digit_counter == 4){
            //store the number as an integer

            //digit1 = atoi(&current_number[0]) ;
            //digit2 = atoi(&current_number[1]) ;
            //digit3 = atoi(&current_number[2]);
            digit1 = atoi(&current_number[0]);
            coordinates[coordinate_counter] = digit1 ;
            if (current_is_negative == 1){
                coordinates[coordinate_counter] = digit1*(-1) ;
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
    uint8_t ADCStatus = ADC_getInterruptStatus(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);

    ADC_clearInterrupt(ADC_BASE, ADCStatus);

    if (ADCStatus)
    {
        ADCState = 0; //Not busy anymore
        ADCResult = ADC_getResults(ADC_BASE);
    }
}
