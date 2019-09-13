//I2C Pins
//SDA = PC4
//SCL = PC5

#include <util/twi.h>

void ERROR();
uint8_t tankstand = 0;

uint8_t prozentangabe_tank = (tankstand / 1023.) * 100; // Potentiometer values between 0-1023


int main()
{

  init();
  Serial.begin(9600);

  //internal Pull Up
  PORTC |= (1 << PORTC4) | (1 << PORTC5);

  //
  TWBR = 72;

  while (1)
  {
    delay(1000);
    Serial.println("---");

    //Start senden
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    //Warten
    while (!(TWCR & (1 << TWINT)));

    //Status: Start erzeugt?
    if (TW_STATUS != TW_START) ERROR();

    //Adresse senden des Slave Arduino
    uint8_t SLA = 2;
    TWDR = SLA << 1; //letztes Bit für R/W reserviert
    TWDR |= (1 << 0); //Master im Read-Modus->MT
    TWCR = (1 << TWINT) | (1 << TWEN); //Adressbits auf Bus setzen

    //Warten
    while (!(TWCR & (1 << TWINT)));

    //Status: Adresse bestätigt?
    if (TW_STATUS != TW_MR_SLA_ACK) ERROR();

    //Sensor-Datenmpfang starten, ACK (da vorletztes Paket)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    //Warten
    while (!(TWCR & (1 << TWINT)));

    //Status:Daten empfangen?
    if (TW_STATUS != TW_MR_DATA_ACK) ERROR();

    //Datenpaket 0 auslesen
    uint8_t tankstand_0 = TWDR;

    //Sensor-Datenmpfang starten, NACK (da letztes Paket)
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Warten
    while (!(TWCR & (1 << TWINT)));

    //Status:Daten empfangen?
    if (TW_STATUS != TW_MR_DATA_NACK) ERROR();

    //Datenpaket 0 auslesen
    uint8_t tankstand_1 = TWDR;

    //Stop senden
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    //Auswertung
    tankstand = ((uint16_t)tankstand_1 << 8) | tankstand_0;
    Serial.print("Tankfüllstand: ");
    Serial.println(tankstand);
    Serial.println(prozentangabe_tank);
  }
}

void ERROR()
{
  Serial.print ("TWCR: ");
  Serial.println (TWCR, BIN);
  Serial.print ("TW_STATUS: ");
  Serial.println (TW_STATUS, HEX);
}
