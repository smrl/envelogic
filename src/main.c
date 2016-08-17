#include <avr/io.h>
#include <avr/interrupts.h>
#include <stdint.h>
#include <logic.h>

// PORTC are the analog inputs.
#define ADC_CHANNELS 6

#define LMODEA_PIN A0 // logic mode "a" in
#define EMODEA_PIN A2 // envelope mode "a" in
#define SKEWA_PIN A1 // skew / ctrl "a" in

#define LMODEB_PIN A3 // logic mode "b" in
#define EMODEB_PIN A5 // envelope mode "b" in
#define SKEWB_PIN A4 // skew /ctrl "b" in

// Arduino pin numbers for logic inputs
#define IN1A 2
#define IN2A 4
#define IN3A 7

#define IN1B 8
#define IN2B 12
#define IN3B 13

//PD3, PD5, PD6 PWM capable outs
#define OUTA 3
#define INVOUTA 5
#define ENVOUTA 6

//PB1, PB2, PB3 PWM capable outs
#define OUTB 9
#define INVOUTB 10
#define ENVOUTB 11

static logicOutPin[2] = [OUTA, OUTB];

//fast read/write macros
#define portOfPin(P) (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P) (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P) (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P) ((uint8_t)(P>13?P-14:P&7))
#define pinMask(P) ((uint8_t)(1<<pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P) ((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P) ((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P) ((uint8_t)isHigh(P))

void setup()
{
  pinAsInput(IN1A);
  pinAsInput(IN1B);
  pinAsInput(IN1C);

  pinAsInput(IN2A);
  pinAsInput(IN2B);
  pinAsInput(IN2C);

  pinAsOutput(OUTA);
  pinAsOutput(OUTB);

  pinMode(INVOUTA, output); // used pinMode to ensure setup of PWM/analogWrite()
  pinMode(ENVOUTA, output);

  pinMode(INVOUTB, output);
  pinMode(ENVOUTB, output);

  /* *********** LOGIC INTERRUPT SETUP *************

An interrupt is fired each time a logic input changes.  The interrupt will call
the appropriate function based on the logic mode, calculate the new logic state
and if changed take appropriate measures to note the time of the change for
later timing calculations.  These might happen after an envelope finishes,
during envelopes or reset and recalculate for a new envelope.

*/

  //Pin Change Interrupt mask register PCINT0 (PORTB)
  //PB0, B4, B5 = digital 8, 12, 13 = PCINT0, 4, 5
  PCMSK0 |= (1<<PCINT0);
  PCMSK0 |= (1<<PCINT4);
  PCMSK0 |= (1<<PCINT5);

  //Pin Change Interrupt mask register on PCINT2 (PORTD)
  //PD2, D4, D7 = digital 2, 4, 7 = PCINT18, 20, 23
  PCMSK2 |= (1<<PCINT18);
  PCMSK2 |= (1<<PCINT20);
  PCMSK2 |= (1<<PCINT23);

  //Then enable the interrupt for the corresponding vector
  PCICR |= (1<<PCIE0);  // PORTB interrupt vector
  PCICR |= (1<<PCIE2);  // PORTD interrupt vector

  /* *********** ADC INTERRUPT SETUP *************

  interrupt-based analog reading:
  objective is to cyclically (round-robin) sample each analog pin and update a global variable.
  using background conversion and interrupts so reading doesn't block.
  Logic out won't change with mode change until a pin changes.
  Envelope functions and mode-setting should take into account the possibility
  for ADC values to be changing mid-envelope.  Envelopes may preserve state
  until they are finished and then switch to a new mode/param setting (or not).

  */

  // set ADLAR in ADMUX (0x7C) to left-adjust the result
  // ADCH will contain upper 8 bits, ADCL LSB results we will discard.
  ADMUX |= B00100000;
  //ADMUX &= B11011111;  // right-adjust

  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  // NOTE: 01 =   AVcc with external capacitor on AREF pin
  // NOTE: 00 = 	AREF, Internal Vref turned off
  ADMUX |= B01000000;

  // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
  // input
  ADMUX &= B11110000;

  // Set MUX3..0 in ADMUX (0x7C) to read from AD0 (AIN1)
  // We will cycle through 0-5. A full list of possible inputs
  //  is available in Table 24-4 of the ATMega328 datasheet
  ADMUX |= 0;
  // ADMUX |= B00000000; // Binary equivalent

  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;

  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering. ?? want cyclic
  // ADCSRA |= B00100000;  // not needed, triggered in interrupt

  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  // ADCSRB &= B11111000;  // NO NO NO NO NO we want single conversion these are irrelevant

  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.
  // ADCSRA |= B00000111; // NO NO NO NO NO 8 bit will do fine thx
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_16; // x16 prescaler for 1mhz ADC clock - sacrificing accuracy for speed. - could fuck with. x8?

  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;

  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  // ADCSRA |= (1 << ADSC);    // or this
  ADCSRA |= B01000000;

  // set global interrupts:
  sei();
}

// UI variables
uint_8t logicMode[2];  // stores current logic mode
uint_8t envMode[2];    // stores current envelope mode
uint_16t envParam[2];  // stores parameter used in an assignable way per-mode
volatile uint8_t adc[6];  // raw 8-bit adc samples

// ENVELOPE variables

uint_8t envStep[4] = 0; // for maintaining current precession through envelope 0-256
uint32_t envLastStep[4]; // time of last step in micros()
volatile int envStage[4] = 0; // 0 disabled, 1 attack, 2 sustain, 3 release

bool calcEnv[2] = FALSE; // flag set to determine whether new timing values need calculation.

uint32_t riseTime[4]; // result of calculated time constants [0] - enva, [1] - envb, [2] - ienva, [3] - ienvb
uint32_t fallTime[4];
//uint32_t iRiseTime[2]; // result of calculated time constants, should use risetime[index+2] instead
//uint32_t iFallTime[2];


// LOGIC variables

volatile bool stateChange[2] = FALSE;

// shared between both sections, updated in ISR just before calling logic function
volatile bool logicIn1 = FALSE;
volatile bool logicIn2 = FALSE;
volatile bool LogicIn3 = FALSE;

volatile bool out[2] = FALSE;
volatile bool islatched[2] = FALSE; // for maintaining state in gated latch mode

volatile uint32_t timeChanged[2];
volatile uint32_t prevTimeChanged[2];
volatile uint32_t timeTrue[2];
volatile uint32_t prevTimeTrue[2];
volatile uint32_t timeFalse[2];
volatile uint32_t prevTimeFalse[2];

uint32_t currentTime; // updated once per main() loop to establish when envs need stepping


ISR(PCINT0_vect)
{
  cli();  // clear global interrupts

  logicIn1 = digitalState(IN1A); //use macros for quick read
  logicIn2 = digitalState(IN2A);
  logicIn3 = digitalState(IN3A);

  switch (modeVal[0]){
    case 0:
    threeinAND(0);
    break;
    case 1:
    threeinOR(0);
    break;
    case 2:
    twoinORinhibit(0);
    break;
    case 3:
    majority(0);
    break;
    case 4:
    twoofthree(0);
    break;
    case 5:
    isodd(0);
    break;
    case 6:
    threeinXOR(0);
    break;
    case 7:
    twoinXOR_inhibit(0);
    break;
    case 8:
    SRlatch_gated(0);
    break;
  }
  sei();  // set global interrupts
}

ISR(PCINT2_vect)
{
  cli();  // clear global interrupts

  logicIn1 = digitalState(IN1B); //use macros for quick read
  logicIn2 = digitalState(IN2B);
  logicIn3 = digitalState(IN3B);

  switch (modeVal[1]){
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
  }

  sei();  // set global interrupts
}

ISR(ADC_vect)
  {
  static int i;
  cli(); // clear global interrupts
  adc[i] = ADCH;

  if (++i >= ADC_CHANNELS)
      {i=0;}

  ADMUX = (1<<ADLAR) | (1<<REFS0) | i; //Select ADC Channel
  ADCSRA |= (1 << ADSC);    // Start next A/D Conversion
  sei();  // set global interrupts
  }
