#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void ADC_INIT(void);
void USART_INIT(void);
uint16_t ADC_CONVERSION(void);
void DECODE_VALUE(uint16_t value);
void USART_SEND(unsigned char character);
void USART_SEND_ARRAY(uint8_t *arrayPointer);

uint8_t UBRR = 51;							///< Value to be changed to specify the baudrate for the particular operating clock frequency.

unsigned char transmit[4];

int main(void){
	ADC_INIT();
	USART_INIT();
	uint16_t result = 0;
	
	while(1){
		result = ADC_CONVERSION();
		DECODE_VALUE(result);
		USART_SEND_ARRAY(&transmit[0]);
		_delay_ms(100);
	}
}

/*!
 *	@brief Initialize ADC.
 */

void ADC_INIT(void){
	ADMUX = ((1<<REFS0)|(1<<MUX4));					///< Set ADC reference to AVCC and analog input mode to differential input of ADC0 and ADC1.
	ADCSRA = ((1<<ADEN)|(1<<ADPS2)|(1<<ADPS1));			///< Enable ADC and set ADC prescalar to 64.
}

/*!
 *	@brief Initialize USART.
 */

void USART_INIT(void){
	UCSRB |= (1<<TXEN);						///< Enable transmission over USART.
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);			///< Select register UCSRC and set transmission character size to 8 bits.
	UBRRL = UBRR;							///< Set UBRR value for specified baudrate at specified frequency.
}

/*!
 *	@brief Perform an ADC conversion.
 *	@return Result of ADC conversion (uint16_t).
 */

uint16_t ADC_CONVERSION(void){
	ADCSRA |= (1<<ADSC);						///< Start ADC conversion.
	while(ADCSRA & (1<<ADSC));					///< Wait until conversion is complete.
	return ADC;							///< Return result of conversion.
}

/*!
 *	@brief Decode a value into individual digits and save in array transmit.
 *	@param Value to be decoded (uint16_t).
 */

void DECODE_VALUE(uint16_t value){
	transmit[0] = 48 + (value/1000);				///< Decode first digit of the value.
	transmit[1] = 48 + ((value%1000)/100);				///< Decode second digit of the value.
	transmit[2] = 48 + ((value%100)/10);				///< Decode third digit of the value.
	transmit[3] = 48 + (value%10);					///< Decode last digit of the value.
}

/*!
 *	@brief Transmit a character over USART.
 *	@param Character to be transmitted (unsigned char).
 */

void USART_SEND(unsigned char character){
	while(!(UCSRA & (1<<UDRE)));					///< Wait until data register is empty.
	UDR = character;						///< Load character to be transmitted to data register.
	while(!(UCSRA & (1<<TXC)));					///< Wait until transmission is complete.
}

/*!
 *	@brief Transmit an array of variable size over USART.
 *	@param Pointer to data array to be transmitted (uint8_t).	
 */

void USART_SEND_ARRAY(uint8_t *arrayPointer){
	for(uint8_t i=0;i<4;i++){
		USART_SEND(arrayPointer[i]);
	}
	USART_SEND(32);							///< Transmit a space character.
}
