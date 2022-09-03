#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <digital_out.h>
#include <digital_in.h>
#include <encoder.h>

Digital_out led(5);
Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out indicator(4);
Encoder location;


// CODE FOR UART TRANSMISSION
// Could have used SERIAL, but that was wayy too slow so this was
// used instead
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


// PART 1
// int main(){
// 	encoder_c1.init();
// 	encoder_c2.init();
// 	indicator.init();
// 	led.init();

// 	bool previous_c1_state = encoder_c1.is_hi();
// 	while (1){
// 		_delay_us(50);
// 		if (previous_c1_state == false && encoder_c1.is_hi()){ // if current state of c1 is high and previous state is low
// 			indicator.set_hi();
// 			_delay_us(10);

// 			if(encoder_c2.is_hi()){ // check state of c2 and increment or decrement 
// 				location.increment();
// 			}
// 			else{
// 				location.decrement();
// 			}

// 			indicator.set_lo();
// 			led.toggle();
// 			}
// 			previous_c1_state = encoder_c1.is_hi();
// 	}

// 	return 0;
// }




// PART 2

ISR(INT1_vect)
{
  
  if (encoder_c1.is_lo() && encoder_c2.is_hi()){
    location.decrement();
  }
}

ISR(INT0_vect)
{
	
  if (encoder_c1.is_hi() && encoder_c2.is_lo()){
    location.increment();
  }
	
	indicator.set_hi();
	_delay_us(10);
    indicator.set_lo();

	led.toggle();

}

int main(){
	Init_Uart();
	encoder_c1.init();
	encoder_c2.init();
	indicator.init();
	led.init();


	while (1){
		
		_delay_ms(1); // delay 1 ms for uart transmit

		UART_load_charVal_in_TxBuffer(location.position()); // load encoder value to transmit
		UART_transmit_TxBuffer(); // transmit encoder value over UART
		reset_TxBuffer(); // reset transmit buffer
	}

	return 0;
}
