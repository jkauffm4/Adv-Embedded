#include <msp430.h> 
#define DUTY_CYCLE_MS 15

void initTimer_A(void);
void initPort1(void);

char state = 0;                                             // Create state bit to keep track of state
int color_value[7] = {2, 8, 10, 32, 34, 40, 42};            // Array to keep track of the colors
char prev_color;
int OFCount;                                                // Value to keep track of our timer counts

/**
 * Flashing LED Cycle
 * Code by Joshua Kauffman and Patrick Cur
 * Program to cycle through an LED at 1.5 second interval at 50% duty cycle
 * Using only ISR's
 * 09/17/2024
 */
void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	                            // stop watchdog timer
	P2DIR = 42;                                             // Set Pin 2 pins to output
	BCSCTL1 = CALBC1_1MHZ;                                  //
	DCOCTL = CALDCO_1MHZ;                                   //
	initTimer_A();
	initPort1();
	_enable_interrupt();                                    // Enabling interrupts
	prev_color = color_value[0];
	OFCount = 0;                                            //
	TACCR0 = 12500 - 1;                                     // Set Timer to 50 ms, based off of 1 MHZ / 4 = 250 KHZ clock
	while(1);                                               // Almighty while loop
}

/*
 * initTimer_A - Initialization of timer A
 * By Joshua Kauffman and Patrick Cur
 *
*/
void initTimer_A(void){
    TACCR0 = 0;                                             // Set up timer
    TACCTL0 |= CCIE;                                        // Enable interrupt at CCRO
    TACTL = TASSEL_2 + ID_2 + MC_1;                         // Select SMCLK, SMCLK/4, Up Mode
}

/*
 *
*/
void initPort1(void){
    P1DIR = 0;                                              // Set Pin 1 to input for P1.3
    P1REN |= BIT3;                                          // Set Pin 1.3 resistor
    P1OUT |= BIT3;                                          // Pull up resistor
    P1IE |= BIT3;                                           // Enable Pin 1 interrupt
}

/*
 * Timer_A_CCR0_ISR - Timer Interrupt Service Routine
 * Everytime entered (1 ms), count the number of milliseconds
 * defined in OFCount
 * By Joshua Kauffman and Patrick Cur
*/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void) {
    OFCount++;
    if(OFCount >= DUTY_CYCLE_MS){
        if(P2OUT == 0) prev_color = color_value[state];
        P2OUT ^= prev_color;
        OFCount = 0;
    }
}

/*
 * Port1 ISR - Interrupt Service routine for Port 1
 * If selected pin goes low, trigger ISR
 * By Joshua Kauffman and Patrick Cut
 */
#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void){
    if(state < 6) state++;                                  // Increment state machine if not at max of array
    else state = 0;                                         // Reset to first state if at max of array
    P1IFG &= ~BIT3;                                         // Clear interrupt flag on bit 3
}
