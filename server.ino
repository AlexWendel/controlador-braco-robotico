#include <RadioHead.h>
#include <Servo.h>
#include <SPI.h>
#include <RH_RF95.h>


#define pinBase 3
#define pinOmbro 4
#define pinCotovelo 5
#define pinGarra 7  

#define ESQUERDA 0
#define DIREITA 1

#define BASE 0
#define OMBRO 1
#define COTOVELO 2
#define GARRA 3

RH_RF95 rf95;

float frequency = 904.5;
long sbw = 125000;
uint8_t denominator = 5;
uint8_t sf = 7;
int TxPower = 10;

Servo Base;
Servo Ombro;
Servo Cotovelo;
Servo Garra;
int posicao[] = { 90, 90, 85, 178};
int posicaoInicial[] = { 90, 90, 90, 178 };
int limsup = 90;
//OBJETOS A SEREM CONTROLADOS

void setup() {

  Serial.begin(9600);
  while (!Serial);  // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("Inicio Falhou");

  rf95.setFrequency(frequency);
  rf95.setSignalBandwidth(sbw);
  rf95.setCodingRate4(denominator);
  rf95.setSpreadingFactor(sf);
  rf95.setTxPower(TxPower);

  Serial.println("Servidor Iniciado");
  Base.attach(pinBase);
  Ombro.attach(pinOmbro);
  Cotovelo.attach(pinCotovelo);
  Garra.attach(pinGarra);

  Base.write(posicao[0]);
  Ombro.write(posicao[1]);
  Cotovelo.write(posicao[2]);
  Garra.write(posicao[3]);
}

void loop() {

  // Esperando uma resposta
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  rf95.waitPacketSent();
  rf95.waitAvailableTimeout(3000);

  if (rf95.recv(buf, &len)) {
    // Serial.println("Received:");
    // for (int i = 0; i < len; i++) {
    //   Serial.print(buf[i]);
    //   Serial.print(" ");
    // }
    // Serial.println("");
    switch (buf[0]) {
      case 0x01:
        switch (buf[1]) {
          case BASE:
            Serial.print("BASEEEEE:");
            if (posicao[buf[1]] > 15 && buf[2] == DIREITA) {
              posicao[buf[1]] -= 10;

            } else if (posicao[buf[1]] < 170 && buf[2] == ESQUERDA) {
              posicao[buf[1]] += 10;
            }
            if (posicao[buf[1]] > 170) { posicao[buf[1]] = 170; }
            if (posicao[buf[1]] < 15) { posicao[buf[1]] = 15; }
            Base.write(posicao[buf[1]]);
            Serial.println(posicao[buf[1]]);
          break;

           case OMBRO:
          Serial.print("OMBROOO:");
             if (posicao[buf[1]] > 10 && buf[2] == ESQUERDA) {
               posicao[buf[1]] -= 3;
             } else if (posicao[buf[1]] < 120 && buf[2] == DIREITA) {
               posicao[buf[1]] += 3;
             }
             if (posicao[buf[1]] > 120) { posicao[buf[1]] = 120; }
             if (posicao[buf[1]] < 10) { posicao[buf[1]] = 10; }
             Ombro.write(posicao[buf[1]]);

             limsup = 85;
             if (posicao[OMBRO] > 85) {
               limsup = 170 - posicao[OMBRO];
             }
             if (posicao[COTOVELO] > limsup) {
               posicao[COTOVELO] = limsup;
               Cotovelo.write(posicao[COTOVELO]);
             }
          Serial.println(posicao[buf[1]]);
          Serial.print("COTOVELOOO:");
          Serial.println(posicao[COTOVELO]);
             break;

           case COTOVELO:
            Serial.print("COTOVELOOO:");

             limsup = 85;
             if (posicao[OMBRO] > 85) {
               limsup = 170 - posicao[OMBRO];
             }
             if(limsup > 85){
              limsup = 85;
             }
             if(limsup < 10){
              limsup = 10;
             }

             if (posicao[buf[1]] > 10 && buf[2] == ESQUERDA) {
               posicao[buf[1]] -= 7;
             } else if (posicao[buf[1]] < limsup && buf[2] == DIREITA) {
               posicao[buf[1]] += 7;
             }
             if (posicao[buf[1]] > limsup) { posicao[buf[1]] = limsup; }
             if (posicao[buf[1]] < 10) { posicao[buf[1]] = 10; }
             Cotovelo.write(posicao[buf[1]]);
//            rf95.send(posicao, 4);
            Serial.println(posicao[buf[1]]);
            break;

          case GARRA:
            Serial.print("GARRAAAAA:");
            if (posicao[GARRA] > 155 && buf[2] == DIREITA) { posicao[buf[1]] = 155; }  // Abrir
            else if (posicao[GARRA] < 178 && buf[2] == ESQUERDA) {
              posicao[buf[1]] = 178;
            }  // Fechar
            if (posicao[GARRA] > 178) { posicao[GARRA] = 178; }
            if (posicao[GARRA] < 155) { posicao[GARRA] = 155; }
            Garra.write(posicao[GARRA]);
            Serial.println(posicao[buf[1]]);
            break;
        }
        break;
    }
  }
//  delay(200);
}
