#include "LoRaWan_APP.h"
#include "HT_SSD1306Wire.h"
#include <Wire.h>

// ------------------ OLED ------------------
SSD1306Wire factory_display(0x3c, 500000, SDA_OLED, SCL_OLED,                           GEOMETRY_128_64, RST_OLED);

void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void showTemperatureOnScreen(int temperature) {
  factory_display.clear();
  factory_display.setFont(ArialMT_Plain_16);
  factory_display.setTextAlignment(TEXT_ALIGN_CENTER);
  factory_display.drawString(64, 0, "LoRaWAN TX");
  factory_display.setFont(ArialMT_Plain_24);
  factory_display.drawString(64, 25, String(temperature) + " \xC2\xB0" + "C");
  factory_display.display();
}

// ------------------ LoRaWAN original ------------------

uint8_t devEui[8] = { 0x85, 0xE0, 0xA5, 0x21, 0x0C, 0x23, 0xDE, 0x80 };
uint8_t appEui[8] = { 0x97, 0x7C, 0xAA, 0x78, 0x70, 0x09, 0x9F, 0xC1 };
uint8_t appKey[16] = { 0x22, 0x1F, 0x28, 0xA5, 0xC3, 0x23, 0xD1, 0x8F, 0xF3, 0xBB, 0x9C, 0x83, 0x37, 0xBE, 0xA7, 0x55 };

uint32_t devAddr = 0x00000000;
uint8_t nwkSKey[16] = { 0 };
uint8_t appSKey[16] = { 0 };

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t  loraWanClass = CLASS_A;

uint32_t appTxDutyCycle = 15000;
bool overTheAirActivation = true;
bool loraWanAdr = false;
bool isTxConfirmed = false;
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 8;

// ------------------ TX FRAME ------------------
static void prepareTxFrame(uint8_t port)
{
    int temperature = random(0, 101);
    appData[0] = (temperature >> 8) & 0xFF;
    appData[1] = temperature & 0xFF;
    appDataSize = 2;

    Serial.print("Enviando temperatura: ");
    Serial.println(temperature);
    // Mostrar en pantalla
    showTemperatureOnScreen(temperature);
}

void setup() {
  Serial.begin(115200);

  // Iniciar OLED
  VextON();
  delay(100);
  factory_display.init();
  factory_display.clear();
  factory_display.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
}

void loop()
{
  switch (deviceState)
  {
    case DEVICE_STATE_INIT:
      #if(LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
      #endif
      
      LoRaWAN.init(loraWanClass, loraWanRegion);
      LoRaWAN.setDefaultDR(3);
      break;

    case DEVICE_STATE_JOIN:
      LoRaWAN.join();
      break;

    case DEVICE_STATE_SEND:
      VextON();              
      delay(100);
      factory_display.init(); 
      prepareTxFrame(appPort);
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;

    case DEVICE_STATE_CYCLE:
      txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;

    case DEVICE_STATE_SLEEP:
      LoRaWAN.sleep(loraWanClass);
      break;

    default:
      deviceState = DEVICE_STATE_INIT;
      break;
  }
}