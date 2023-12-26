/*
EES 208 1.0 Data Acquisition and Signal Processing Lab
Lab 4.2 Activity 1
AS2019494
*/

#define BAUD 115200                       // BAUD Rate
#define F_CPU 16000000                    // CPU frequency
#define TPS   64                          // Timer1 pre-scalar
#define AREF  5.05                        // Analog reference voltage
#define SAMPLE_RATE 200
#define UBRR ((F_CPU/(8 * BAUD)) -1) / 2  // BAUD settings

void sendUSART(char *);     // Transfer char array
uint16_t readADC(uint8_t);  // A/D conversion

char * strVolt0 = NULL;

int main(){

  cli();  // Disable all interrupts

  // Save BAUD settings in UBRR0 register
  UBRR0L = UBRR;
  UBRR0H = UBRR >> 8;
  UCSR0B |= (1 << TXEN0); // Enable USART transmission
  
  ADMUX |= (1 << REFS0);  // Default reference
  ADCSRA |= 0x07;         // 128 pre-scalar
  ADCSRA |= (1 << ADEN);  // Enable ADC
  
  TCNT1 = 0;              // Counter
  OCR1A = 1250;           // Compare match register
  TCCR1A = 0;
  TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);   // CTC mode & 64 pre-scalar
  TIMSK1 |= (1 << OCIE1A);                              // Enable timer compare interrupt
  
  strVolt0 = (char *)malloc(sizeof(char)* 8);

  sei();  // Enable interrupts

  while(1);

}

void sendUSART(char * str){

  for (int i = 0; i< strlen(str); i++){
    while (!(UCSR0A & (1 << UDRE0)));   // Waiting for end previous transfer
    UDR0 = str[i];                      // Transfer new byte
  }

}

uint16_t readADC(uint8_t ch){

  ADMUX |= (ch & 0x0F);             // Select ADC Channel
  ADCSRA |= (1 << ADSC);            // Start conversion
  while ((ADCSRA & (1 << ADSC)));   // Waiting for conversion complete
  return ADC;

}

ISR(TIMER1_COMPA_vect){
  
  uint16_t adc0 = readADC(0);       // A/D conversion
  float volt0 = adc0 * AREF / 1023; // ADC to voltage
  dtostrf(volt0, 4, 2, strVolt0);   // Convert float to char array
  sendUSART(strVolt0);              // USART output
  sendUSART("\n");
  
}
