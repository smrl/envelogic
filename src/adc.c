//interrupt-based analog reading -
//objective is to cyclically (round-robin) sample each analog pin and update a global variable.
//using background conversion, and interrupts so reading doesn't block.

// Note, many macro values are defined in <avr/io.h> and
// <avr/interrupts.h>, which we need to include for dev in platform.io

#include <avr/io.h>
#include <avr/interrupts.h>

#DEFINE ADC_CHANNELS 6

volatile uint8_t adc[ADC_CHANNELS];  //simple to stick to 8 bit resolution

// Initialization
void setup(){

  // set ADLAR in ADMUX (0x7C) to left-adjust the result
  // ADCH will contain upper 8 bits, ADCH LSB results we will discard.
  ADMUX |= B00100000;
//ADMUX &= B11011111;  // right-adjust

  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  ADMUX |= B01000000;

  // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
  // input
  ADMUX &= B11110000;

  // Set MUX3..0 in ADMUX (0x7C) to read from AD8 (Internal temp)
  // Do not set above 15! You will overrun other parts of ADMUX. A full
  // list of possible inputs is available in Table 24-4 of the ATMega328
  // datasheet
  ADMUX |= 8;
  // ADMUX |= B00001000; // Binary equivalent

  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;

  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering. ?? want cyclic
  ADCSRA |= B00100000;

  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;

  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.  - a conversion takes 13 ADC cycles.
  ADCSRA |= B00000111;

  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;

  // Enable global interrupts
  // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
  // supplies by default.
  sei();

  // Set ADSC in ADCSRA (0x7A) to start the first ADC conversion
  ADCSRA |=B01000000;
}



ISR(ADC_vect)
  {
  static int i;
  cli();
  (ADC_RESULT[i]) = ADCH;

  if (++i >= ADC_CHANNELS)
      {i=0;}

  ADMUX = (1<<ADLAR) | (1<<REFS0) | i; //Select ADC Channel
  ADCSRA |= (1 << ADSC);    // Start next A/D Conversion by setting ADSC in ADCSRA (0x7A)
  // ADCSRA |= B01000000; equivalent?
  sei();
  }
