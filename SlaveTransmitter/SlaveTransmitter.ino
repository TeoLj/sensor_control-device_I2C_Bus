// I2C Slave Transmitter
// I2C Pins
// SDA = PC4 = Arduino A4
// SCL = PC5 = Arduino A5
// Poti = PC1 = Arduino A1


#include <util/twi.h>
#include <avr/io.h>
#include <util/delay.h>

int result;

int main() {
  init();
  Serial.begin(9600);

  // Slave Address
  byte SLA_W = 2;
  TWAR = SLA_W << 1 ;


  while (1) {
    
    read_poti();
    uint8_t data_out = result / 1023.*255.;

    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Check Status: Own address received
    //TW_ST_SLA_ACK 0xA8
    Status();
    if (TW_STATUS != TW_ST_SLA_ACK) ERROR();

    //Send data, expect NACK (because last packet)
    TWDR = data_out;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait
    while (!(TWCR & (1 << TWINT)));
    //Master Receiver has responded wit NACK
    Status();     Serial.println(""); //TW_ST_DATA_NACK 0xC0
    if (TW_STATUS != TW_ST_DATA_NACK) ERROR();

  }
}



void ERROR() {
  Serial.print("TWCR: ");
  Serial.println(TWCR, BIN);
  Serial.print("TW_STATUS: ");
  Serial.println(TW_STATUS, HEX);
  Serial.println("");
}


void Status(void) {
  Serial.print("TW_STATUS: ");
  Serial.println(TW_STATUS, HEX);
}



void read_poti() {
  // Activate ADC
  ADCSRA |= (1 << ADEN);
  //Set up Prescaler to 128
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  ADMUX |= (1 << MUX0);      //Channel of Multiplexe
  ADMUX |= (1 << REFS0);    //Use Internal 5V as reference voltaghe

  // Read analog Input at channel
  ADCSRA |= (1 << ADSC);    // start conversion
  while (ADCSRA & (1 << ADSC)); // Wait for conversion end...
  result = ADC;         //Save result
  ADCSRA &= ~(1 << ADEN);   // Deactivate ADC
}
