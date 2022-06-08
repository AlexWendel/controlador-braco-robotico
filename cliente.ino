#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

float frequency = 904.5;
long sbw = 125000;
uint8_t denominator = 5;
uint8_t sf = 7;
int TxPower = 10;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("Inicio Falhou");

  rf95.setFrequency(frequency);
  rf95.setSignalBandwidth(sbw);
  rf95.setCodingRate4(denominator);
  rf95.setSpreadingFactor(sf);
}

const int BUFFER_SIZE = 3;
byte buf[BUFFER_SIZE];

void loop() {
  
  if (Serial.available() > 0) {
    // lê o dado recebido:
    int rlen = Serial.readBytes(buf, BUFFER_SIZE);

    
//    Serial.println(incomingByte);
    rf95.send(buf, 3);
//    delay(400);
  }
  
//  byte buf[3] = {0x01, 0x00, 0x00};
//
//  for (int i=0; i < 3;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(400);      
//  }
//
//  byte buf[3] = {0x01, 0x01, 0x01};
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//
//  
//  buf[0] = 0x01;
//  buf[1] = 0x01; 
//  buf[2] = 0x00;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }             
//
//  buf[0] = 0x01;
//  buf[1] = 0x03; 
//  buf[2] = 0x00;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//
//  
//  buf[0] = 0x01;
//  buf[1] = 0x03; 
//  buf[2] = 0x01;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }       
//
//  buf[0] = 0x01;
//  buf[1] = 0x00; 
//  buf[2] = 0x00;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//
//  buf[0] = 0x01;
//  buf[1] = 0x00; 
//  buf[2] = 0x01;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//
//  buf[0] = 0x01;
//  buf[1] = 0x02; 
//  buf[2] = 0x00;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//
//  buf[0] = 0x01;
//  buf[1] = 0x02; 
//  buf[2] = 0x01;
//  for (int i=0; i < 20;i++){
//      rf95.send(buf, 3);
//      Serial.println("Enviando");
//      delay(50);      
//  }
//
//  delay(1000);
//

  //delay(00);
}
