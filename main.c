/*
 Task 1: As in assignment 3, hook up actuators to visualize the state of a register (e.g. 8 LEDs) to pin bank B. Use a variable to count from 0 to 255. 
  Increment the variable each second by one. If you reach 255, start over. Write the value to the register of pin bank B every time it changes.

 Task 2: Use a variable named "direction" to change the direction of number counting. If "direction" is "1", count from 0 up to 255 (and repeat that forever). If "direction" is "0", count from 255 down to 0 (and repeat forever).

 Task 3: Hook up a button to PD3 (Yes, it has to be PD3). Activate the appropriate interrupt for PD3. Set the trigger for the interrupt to "low level".
  Activate all interrupts. Create the interrupt service routine that is appropriate for the interrupt you are using. In that interrupt service routine: set each bit in pin bank B to "1" and change the "direction" variable to the opposite("1" -> "0" or "0" -> "1").
  Remember to debounce the button.



 
 Task 4: Create a Statechart diagram of the whole system (hardware & software) of this assignment (Astah is recommended; drawing on paper and uploading a picture is fine too). 
 You can substitute changing bit patterns in the Statechart with "CurrentRegisterValue", if necessary. CurrentRegisterValue is a substitute for e.g. "bit1, ^bit2, bit3, ^bit4, ^bit5, bit6, bit7, bit8".
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//definitions
#define FORWARD 1
#define BACKWARD 0
#define ON 1
#define OFF 0
#define BUTTON   
#define DEBOUNCE_TIME 1000
//macros
#define max_count_check(count) (count > 255) ? 0 : count
#define min_count_check(count) (count < 0) ? 255 : count
#define check_count(count) min_count_check(max_count_check(count))

//Global Declarations
volatile int direction = FORWARD;   //interupt flag
volatile int power = ON;    //interrupt flag

//prototypes

//inits
void init_leds();   //PIN BANK B
void init_buttons(); //
void init_interrupts();

//helper functions
int debounce(int buttonPin);

//interupt service routines
ISR(INT1_vect)
{
    while(!debounce(PD3)){}
    PORTB = 0XFF;
    direction = direction == FORWARD ? BACKWARD : FORWARD;
}



//Main
int main(void)
{
    //INITS
    init_leds();
    init_buttons();
    init_interrupts();

    
    //MAIN LOGIC
    //while the on/off button is in an on state
    while(power == ON)
    {
        //init count
        int count = 0;

        //Forward State
        //Execute the forward state at least once then poll the direction flag
        do
        {
            //light leds
            PORTB = count;
            
            //increment count
            count = check_count(++count); //check count keeps the count variable between 0 and 255 (inclusive)
        
        //check direction flag
        }while((direction == FORWARD) && (power == ON) );
        
        //check that the power button has not been pressed
        if(power == OFF)
        {
            break;
        }

        //Backward State
        //Execute the backwards state at leaste once and the poll direction flag
        do
        {
            //decrement count
            count = check_count(--count);
            
            //light leds
            PORTB = count;
        
        //check direction flag
        }while((direction == BACKWARD) && (power == ON) );
    }
}

//method declarations

void init_leds()
{
    DDRB |= 0XFF;

    for(int i = 0 ; i < 3 ; i++)
    {
        PORTB = 0XFF;
        _delay_ms(500);
        PORTB = 0X00;
    }
}

void init_buttons()
{
    DDRD &= (0 << PD3);//explicitly set to input
    PORTD |= (1 << PD3); //activate pullup
}

void init_interrupts()
{
    EIMSK |= (1 << INT0); //use 0 bit mask
    EICRA |= (1 << ISC00); //activate interrupt on falling edge
    sei(); //enable interrupts
}

//helper functions
int debounce(int buttonPin)
{
    short debounced = 0;
    if(bit_is_clear(PIND,buttonPin))
    {
        _delay_us(DEBOUNCE_TIME);
        
        if(bit_is_clear(PIND,buttonPin))
        {
            debounced = 1;
        }
    }
    
    return debounced;
}

