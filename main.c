/*
  ATmega32 @ 8MHz
  Plays an 8bit/8000 sample PCM audio on OC1A output
*/


#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "0.h"
#include "1.h"
#include "2.h"
#include "3.h"
#include "4.h"
#include "5.h"
#include "6.h"
#include "7.h"
#include "8.h"
#include "9.h"
#include "point.h"
#include "V.h"
#include "F.h"
#include "LCD.H"
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>
#include"LCD.h"
#include"vr.h"

#define SAMPLE_RATE 8000;
int flag=0;
int x1,l;
int m;
	uint16_t temp;
	float x=3.98,y;
float v2=0,i2=0,r2=0,h2=0,f2=0;

volatile uint16_t sample;
int sample_count;

void uart_init()
{
	UCSR0A |= (1<<U2X0);
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	UBRR0L=103;
}

void adc_init()
{
	ADCSRA |= (1<<ADEN) | (1<<ADPS2);
	ADMUX |= (1<<REFS0);
	DDRF = 0b10011100;

}


int read()
{
while(!( UCSR0A & (1<<RXC0)));

return UDR0;
}

void write(uint8_t c)
{
while(!( UCSR0A & (1<<UDRE0)));

UDR0 = c;
}

uint16_t adc_val(int mux)
{
	ADMUX = ADMUX & 0b11100000;
	ADMUX = ADMUX + mux;
	//ADMUX |= (1<<MUX0);
	_delay_us(5);
	ADCSRA |= (1<<ADSC);
	while(!(ADCSRA & (1<<ADIF)));
	return ADC;

}

float check_voltage()
{
	PORTF |= (1<<PF2);//set for 100V first
	PORTF &= ~(1<<PF3);
	PORTF &= ~(1<<PF4);
	uint16_t v=adc_val(voltage);
	/*if(v < 300 ) //some value
	{
		PORTF &= ~(1<<PF2);//then set for 30V
		PORTF |= (1<<PF3);
		PORTF &= ~(1<<PF4);
		float v=adc_val(voltage);
		if(v < 300 ) //some value
		{
			PORTF &= ~(1<<PF2);//then set for 10V
			PORTF &= ~(1<<PF3);
			PORTF |= (1<<PF4);
			uint16_t v=adc_val(voltage);
			v2=(v/1023.)*10;
			//v2=(v/1024)*10;
			//v2=v;
			
			return v2;
		}
		else
		{
			
			v2=(v/1023.)*30;
			return v2;
		}
	}
	else
	{
		
		v2=(v/1023.)*100;
		return v2;
	
	}*/
	return v;
	
	
}

float check_current()
{
	uint16_t i=adc_val(current);
	i2=((i/1023.)*500.+1 );
	return i2;

}

int check_beeta()
{
	uint16_t h=adc_val(beeta);
	h2=(h/1023.)*5;
	h2=1/h2;
	h2=h2/62.3;
	return h2;

}

int check_resistance()
{
	PORTF |= (1<<PF7);
	uint16_t r=adc_val(resistance);
	//if(r<210)
	//{
		//PORTF &= ~(1<<PF7);
		//uint16_t r=adc_val(resistance);
		//r2=(r/1023.)*5;
		//r2=((10/r2)-2.)*1000;
		//return r2;
	
	//}
	//else
	//{
	//r2=(r/1023.)*5;
	//r2=((10/r2)-2.)*1000;
	r2=r;
	return r2;
	//}
}

uint32_t check_frequency()
{
	if(!(PINE & (1<<PE4)))
   {
      while(!(PINE & (1<<PE1)));   
   }

   while(PINF & (1<<PE4));    

   TCNT1=0;

   TCCR1B=(1<<CS10);
   while(!(PINE & (1<<PE4)));   
    while(PINE & (1<<PE4));

   TCCR1B=0x00;

   return ((float)8000000UL/TCNT1);

}


void pwm_init(void)
{

	
	DDRF=0x00;
	PORTF=0x00;
    DDRB = _BV(PB4) | _BV(PB5);

    TCCR1A = _BV(COM1A1) | _BV(WGM10);
  
    TCCR1B = _BV(WGM12) | _BV(CS10);

    OCR1A = 0;

    TCCR0|=(1<<CS00);
    TCNT0=0;
    TIMSK|=(1<<TOIE0);
    sample_count = 4;
    sei();
}



ISR(TIMER0_OVF_vect)
{
	
	switch(l)
	{
		case 0:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;
			flag=0;          
             OCR1A = pgm_read_byte(&zero_samples[sample++]);
             if(sample>zero_length)sample=0,flag=1,l=1;
            }
		break;
		case 1:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;
			flag=0;             
             OCR1A = pgm_read_byte(&one_samples[sample++]);
             if(sample>one_length)sample=0,flag=1,l=2;
            }
		break;
		case 2:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;
			flag=0;          
             OCR1A = pgm_read_byte(&two_samples[sample++]);
             if(sample>two_length)sample=0,flag=1,l=3;
            }
		break;
		case 3:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;        
			flag=0;  
             OCR1A = pgm_read_byte(&three_samples[sample++]);
             if(sample>three_length)sample=0,flag=1,l=4;
            }
		break;
		case 4:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;     
			flag=0;     
             OCR1A = pgm_read_byte(&four_samples[sample++]);
             if(sample>four_length)sample=0,flag=1,l=5;
            }
		break;
		case 5:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4; 
			flag=0;         
             OCR1A = pgm_read_byte(&five_samples[sample++]);
             if(sample>five_length)sample=0,flag=1,l=6;
            }
		break;
		case 6:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;          
			 flag=0;
             OCR1A = pgm_read_byte(&six_samples[sample++]);
             if(sample>six_length)sample=0,flag=1,l=7;
            }
		break;
		case 7:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;   
			flag=0;       
             OCR1A = pgm_read_byte(&seven_samples[sample++]);
             if(sample>seven_length)sample=0,flag=1,l=8;
            }
		break;
		case 8:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;        
			flag=0;  
             OCR1A = pgm_read_byte(&eight_samples[sample++]);
             if(sample>eight_length)sample=0,flag=1,l=9;
            }
		break;
		case 9:
         sample_count--;
         if (sample_count == 0)
            {
             sample_count = 4;      
			flag=0;    
             OCR1A = pgm_read_byte(&nine_samples[sample++]);
             if(sample>nine_length)sample=0,flag=1,l=0;
            }
		break;
	}
	
		
}



void port_init()
{
	int x1;
	uint16_t temp;
	DDRF=0x00;
	PORTF=0x00;
	//adc_init();

}

int main(void)
{
	port_init();
	pwm_init();
	int group=1;
	float vol,cur;
	int fre,bee,res;
	lcd_init();
	adc_init();
	uart_init();
	PORTF=0;
	DDRE &= ~_BV(4);
   	l=0;
	while(1)
	{
		//setPinOutput(1,4);
		lcd_gotoxy2(0);
		lcd_char('L');
		recognizeCommand(1);
		do
		{
		// can do some processing while waiting for a spoken command
		}
		while (!hasFinished());
	
		lcd_gotoxy2(0);
		lcd_char('D');
		
		 
  idx = getCommand();
	lcd_gotoxy1(8);
	lcd_num1(idx);
  
	switch(idx)
	{
		case 0:
		//voltage
			lcd_gotoxy1(0);
			lcd_string("V=  ");
			vol=check_voltage();
			lcd_gotoxy1(4);
			lcd_showvaluef(vol);
		break;
		case 1:
			lcd_gotoxy1(0);
			lcd_string("I=  ");
			cur=check_current();
			lcd_gotoxy1(4);
			lcd_showvaluef(cur);
			
		break;
		case 2:
			lcd_gotoxy1(0);
			lcd_string("F=  ");
			fre=check_frequency();
		//frequency
			lcd_gotoxy1(4);
			lcd_showvaluef(fre);
		break;
		case 3:
			lcd_gotoxy1(0);
			lcd_string("H=  ");
			bee=check_beeta();
		//beeta
			lcd_gotoxy1(4);
			lcd_showvaluef(bee);
			
		break;
		case 4:
			lcd_gotoxy1(0);
			lcd_string("R=  ");
			res=check_resistance();
		//resistance
			lcd_gotoxy1(4);
			lcd_showvaluef(res);
		break;
		default:
			lcd_gotoxy1(0);
			lcd_string("Give    INPUT  ");
		break;
		
		//do nothing;
		
	}
  
  //say value as per the command.
 // _delay_ms(1000);
	idx=-1;
	_value=-1;
  

}
   
}
