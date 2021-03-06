#ifndef DIYBMS_ATTINY841_H // include guard
#define DIYBMS_ATTINY841_H

//Show error is not targeting ATTINY841
#if !(defined(__AVR_ATtiny841__))
#error Written for ATTINY841 chip
#endif

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

/*
This class wraps the hardware pins of DIYBMS away from the core logic/code
if you are porting to another chipset, clone this class and modify it.
*/
class DiyBMSATTiny841
{
public:
   static void ConfigurePorts();

   static void ResumePWM()
   {
      //Switch ON Bit 1 – OCIE1A: Timer/Counter, Output Compare A Match Interrupt Enable
      TIMSK1 |= _BV(OCIE1A);
      interrupts();
   }

   static void StopTimer1()
   {
      //Switch OFF Bit 1 – OCIE1A: Timer/Counter, Output Compare A Match Interrupt Enable
      TIMSK1 &= (~_BV(OCIE1A));
   }

   static void StartTimer1()
   {
      //Normal port operation, OC1A/OC1B disconnected
      //Bits 1:0 – WGMn[1:0]: Waveform Generation Mode
      //COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10
      //        CCCC--WW
      TCCR1A = B00000000;

      //CTC (Clear Timer on Compare) = mode 4 = 0100
      //TOP = OCR1A
      //Bits 2:0 – CSn[2:0]: Clock Select = Prescale 64
      //ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
      //        II-WWCCC
      TCCR1B = B00001011;

      // Prescaler 8Mhz/64 = 125000 counts per second
      // Call ISR 1000 times per second
      OCR1A = 125;

      ResumePWM();
   }

   static void PausePWM()
   {
      StopTimer1();
      DumpLoadOff();
   }

   static void DumpLoadOn()
   {
      PORTA |= _BV(PORTA3);
   }

   static void DumpLoadOff()
   {
      PORTA &= (~_BV(PORTA3));
   }

   static void ReferenceVoltageOn()
   {
      //When to switch external voltage reference on or off. Connected to Pin 6, PA7
      PORTA |= _BV(PORTA7);
   }

   static void ReferenceVoltageOff()
   {
      //When to switch external voltage reference on or off. Connected to Pin 6, PA7
      PORTA &= (~_BV(PORTA7));
   }

   static void GreenLedOn()
   {
      PORTA |= _BV(PORTA6);
   }

   static void GreenLedOff()
   {
      PORTA &= (~_BV(PORTA6));
   }

#if defined(DIYBMSMODULEVERSION) && DIYBMSMODULEVERSION < 430
   static void SparePinOn()
   {
      PORTB |= _BV(PORTB1);
   }

   static void SparePinOff()
   {
      PORTB &= (~_BV(PORTB1));
   }
#endif

#if defined(DIYBMSMODULEVERSION) && DIYBMSMODULEVERSION < 430
   static void BlueLedOn()
   {
      PORTA |= _BV(PORTA5);
   }

   static void BlueLedOff()
   {
      PORTA &= (~_BV(PORTA5));
   }
#endif

   static void FlushSerial0()
   {
      Serial.flush();

      //while (bit_is_set(UCSR0B, UDRIE0) || bit_is_clear(UCSR0A, TXC0))       {      }
   }

   static void DisableSerial0TX()
   {
      UCSR0B &= ~_BV(TXEN0); //disable transmitter (saves 6mA)
   }

   static void EnableSerial0TX()
   {
      UCSR0B |= (1 << TXEN0); // enable transmitter
   }

   static void DisableSerial0()
   {
      //Disable serial0
      UCSR0B &= ~_BV(RXEN0); //disable receiver
      UCSR0B &= ~_BV(TXEN0); //disable transmitter
   }

   static void EnableSerial0()
   {
      UCSR0B |= (1 << RXEN0); // enable RX Serial0
      UCSR0B |= (1 << TXEN0); // enable TX Serial0
   }

   static void DisableSerial1()
   {
      UCSR1B &= ~_BV(RXEN1); //disable receiver
      UCSR1B &= ~_BV(TXEN1); //disable transmitter
   }

   static void EnableSerial1()
   {
      UCSR1B |= (1 << RXEN1); // enable RX Serial1
      UCSR1B |= (1 << TXEN1); // enable TX Serial1
   }

   static void EnableStartFrameDetection()
   {
      noInterrupts();

      // Enable Start Frame Detection
      UCSR0D = (1 << RXSIE0) | (1 << SFDE0);

      interrupts();
   }

   static void SetWatchdog8sec();

   static uint16_t ReadADC();

   static void BeginADCReading();

   static void Sleep();

   static void SelectCellVoltageChannel()
   {
      //PB2 = ADC8 PIN 5 ARDUINO PIN 2/A8 = VOLTAGE reading
      //ADMUXA – ADC Multiplexer Selection Register A
      //ADC8 (single end) MUX[5:0] 00 1000
      ADMUXA = (0 << MUX5) | (0 << MUX4) | (1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
   }

   static void SelectInternalTemperatureChannel()
   {
      //PA4
      //ADMUXA – ADC Multiplexer Selection Register A
      //ADC4 (single end) MUX[5:0] 00 0100
      ADMUXA = (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
   }

   static void SelectExternalTemperatureChannel()
   {
      //External sensor
      //ADMUXA – ADC Multiplexer Selection Register A

#if defined(DIYBMSMODULEVERSION) && DIYBMSMODULEVERSION < 430
      //ADC11 (single end) MUX[5:0] 00 1011
      ADMUXA = (0 << MUX5) | (0 << MUX4) | (1 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0);
#else
      //V4.3 boards ADC5 (single end) MUX[5:0] 00 0101
      ADMUXA = (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0);
#endif
   }

   static void double_tap_green_led();
#if defined(DIYBMSMODULEVERSION) && DIYBMSMODULEVERSION < 430
   static void double_tap_blue_led();
#endif
};

#endif
