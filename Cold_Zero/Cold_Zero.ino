#include <OneWire.h>
#include <DallasTemperature.h>

#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 3
#define DIO 2
#define TEST_DELAY 500

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;

const uint8_t SEG_HI[] = {
  SEG_G,                                    // -
  SEG_B | SEG_C | SEG_E | SEG_F | SEG_G ,   //H
  SEG_B | SEG_C,                            //I
  SEG_G                                     // -
  };

const uint8_t SEG_ERR[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
  SEG_E | SEG_G,                                   // r
  SEG_E | SEG_G,                                   // r
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F    // O
  };

TM1637Display display(CLK, DIO);

void setup(void)
{
  display.setBrightness(7, true);
  //display.clear();
  display.setSegments(SEG_HI);
  delay(TEST_DELAY);
 
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) display.setSegments(SEG_ERR);     
  sensors.setResolution(insideThermometer, 9); 
}

void printTemperature(DeviceAddress deviceAddress)
{
  //косяк с отображением температуры от -1 до 0 и от 0 +1 надо исправить
  uint16_t tempC = sensors.getTempC(deviceAddress)*10; //Домножаем на 10, чтобы не потерять дробную часть
  if(tempC == DEVICE_DISCONNECTED_C){
    display.setSegments(SEG_ERR);
    return;
  }
  display.showNumberDecEx(tempC, (0x80 >> 2), false); //и ставим точку на предпоследнем разряде.
}//Таким образом получаем точность в пределах погрешности датчика

void loop(void)
{ 
  sensors.requestTemperatures();
  printTemperature(insideThermometer); 
}
