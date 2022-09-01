// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <avr/delay.h>
// #include <digital_out.h>

// int encoder = 0;
// Digital_out led(5);


// int main(){
//   	DDRD &= (1<<DDD3); // set pin D3 as input
//     DDRD &= (1<<DDD2); // set pin D2 as input

//     PORTD |= (1<<DDD3); // set PD3 pullup enabled
//     EIMSK |= (1<<INT1); // external interrupt mask register for INT1 interrupt vector
//     EICRA |= (1<<ISC10) | (1<<ISC11); // falling + rising edge interrupt requests
//     led.init();
//     sei();

//     while(1){
//       // if (encoder > 100){
//       //   // led.toggle();
//       //   // _delay_ms(100);/
//       // }
//       // led.set_lo();
//       _delay_ms(1);
//     }

// }

// ISR(INT1_vect)
// {
// 	int b = PIND & (1 << PIND2);
// 	if (encoder >= 700 || encoder <= -700) encoder = 0;
// 	if (b > 0){
// 		encoder++;
// 	}else{
// 		encoder--;
// 	}
//   if (encoder < 300){
//   led.toggle();
//   }
// }





/*
 * main.c
 *
 *  Created on: 31. jan. 2022
 *      Author: Bjarki
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <digital_out.h>
#include <Arduino.h>
#include <digital_in.h>
#include <encoder.h>

Digital_out led(5);
Digital_in encoder_1(DDD2);
Digital_in encoder_2(DDD3);
Encoder location;
char TxBuffer[32];
int indx, len;

void Init_Uart()
{
	// 57600 baudrate
	UBRR0H = 0;
	UBRR0L = 16;
	UCSR0B = (1<<RXEN0) |(1<<TXEN0);// |(1<<TXCIE0);
	UCSR0C = (1<<USBS0) |(3<<UCSZ00);
	indx = len = 0;
}

void reset_TxBuffer(){
	indx = len = 0;
}

void UART_transmit_TxBuffer(){
	while (indx < len){
		while(!(UCSR0A & (1<<UDRE0))){;}
		UDR0 = TxBuffer[indx];
		indx++;
	}
}


int strlen(char s[])
{
	int i=0;
	while ( s[i] != 0 )
	i++;
	return i;
}

void reverse(char s[])
{
	int i,j;
	char c;

	for (i=0, j=strlen(s)-1; i<j; i++, j--){
	c = s[i];
	s[i] = s[j];
	s[j] = c;
	}
}

void UART_itoa(int n, char s[])
{
	int i,sign;

	if ( (sign = n ) < 0 )
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ( ( n /= 10 ) > 0 );
	if (sign < 0 )
	s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void UART_load_string_in_TxBuffer(char s[])
{
	while(s[len] != 0)
	{
		TxBuffer[len] = s[len];
		len++;
	}

	TxBuffer[len] = '\r';
	TxBuffer[len+1] = '\n';
	len += 2;
}

void UART_load_charVal_in_TxBuffer(int data)
{
	char temp[32];
	UART_itoa(data,temp);
	UART_load_string_in_TxBuffer(temp);
}


ISR(INT1_vect)
{
  if (encoder_1.is_lo() && encoder_2.is_hi()){
    location.increment();
  }
}

ISR(INT0_vect)
{
  if (encoder_1.is_hi() && encoder_2.is_lo()){
    location.decrement();
  }
}

int main(){
	Init_Uart();
	encoder_1.init();
  encoder_2.init();
  led.init();

	EIMSK |= (1<<INT1); // external interrupt mask register for INT1 interrupt vector
  EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC10) | (1<<ISC11); // falling + rising edge interrupt requests 
  EICRA |= (1<<ISC00) | (1<<ISC01);
  // EICRA |= (1<<ISC00);
  
	asm("sei"); // enable interrupts

	while (1){
		
		_delay_ms(1); // delay 1 ms
		UART_load_charVal_in_TxBuffer(location.position()); // load encoder value to transmit
		UART_transmit_TxBuffer(); // transmit encoder value over UART
		reset_TxBuffer(); // reset transmit buffer
    if(location.position() > 350){
      led.set_hi();
    }else{
      led.set_lo();
    }
	}

	return 0;
}
