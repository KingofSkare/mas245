#include <avr/io.h> // https://www.nongnu.org/avr-libc/user-manual/group__avr__io.html
#define F_CPU 20000000UL // CPU speed at 20 MHz witch is max at 4.5V - 5.5V (we are using VCC = 4.9V)
#include <util/delay.h> // https://www.nongnu.org/avr-libc/user-manual/group__util__delay.html 

/* ATMega168 Datasheet: https://no.mouser.com/datasheet/2/268/Atmel_2545_8_bit_AVR_Microcontroller_ATmega48_88_1-1315326.pdf
   TCCR0A er konfigurert for FAST PWM som håndterer høyere frekvenser (s. 139).
*/


// PWM output configuration on OC0A (pin PD6/12)
void setupPWM()
{
	DDRD |= (1 << DDD6);
	TCCR0A |= (1 << COM0A1); // Clear OC0A on Compare Match, set OC0B at BOTTOM, (non-inverting mode)
	TCCR0A |= (1 << WGM01);
	TCCR0A |= (1 << WGM00);
	TCCR0B |= (1 << CS00); // No prescaling.
}


int main()
{
	setupPWM();
	while(1)
	{
		for(int i = 255 ; i > 0 ; i--)
		{
			OCR0A = i;
			_delay_us(50);
		}
			
		for(int i = 0 ; i < 255 ; i++)
		{
			OCR0A = i;
			_delay_us(50);
		}
	}
}