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

void setSegmentsFormatTemperature(int num) {
  uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
  if (num < 0 & abs(num) > 100){
    data[0] = 0x40;
    num = abs(num);
    for (int i = 3; i >= 1; --i) {
    uint8_t digit = num % 10;

    if (digit == 0 && num == 0)
    data[i] = 0x3f;
    else
    data[i] = display.encodeDigit(digit);
    if (i == 2){
      data[i] = display.encodeDigit(digit) + 0x80;
    }
    num /= 10;
    }    
  }else if(num < 0 &abs(num) > 1){
    data[0] = 0x40;
    data[3] = 0x63;
    num = abs(num);
    for (int i = 2; i >= 1; --i) {
    uint8_t digit = num % 10;

    if (digit == 0 && num == 0)
    data[i] = 0x3f;
    else
    data[i] = display.encodeDigit(digit);
    if (i == 1){
      data[i] = display.encodeDigit(digit) + 0x80;
    }
    num /= 10;    
  }
  
  }else if(num > 0 & num > 100){
    data[3] = 0x63;
    num = abs(num);
    for (int i = 2; i >= 0; --i) {
    uint8_t digit = num % 10;

    if (digit == 0 && num == 0)
    data[i] = 0x3f;
    else
    data[i] = display.encodeDigit(digit);
    if (i == 1){
      data[i] = display.encodeDigit(digit) + 0x80;
    }
    num /= 10;    
  }
  }else if(num > 0 & num > 10){
    data[0] = 0x00;
    data[3] = 0x63;
    num = abs(num);
    for (int i = 2; i >= 1; --i) {
    uint8_t digit = num % 10;

    if (digit == 0 && num == 0)
    data[i] = 0x00;
    else
    data[i] = display.encodeDigit(digit);
    if (i == 1){
      data[i] = display.encodeDigit(digit) + 0x80;
    }
    num /= 10;    
    }
  }else{
    data[0] = 0x00;
    data[3] = 0x63;
    num = abs(num);
    for (int i = 2; i >= 1; --i) {
    uint8_t digit = num % 10;

    if (digit == 0 && num == 0)
    data[i] = 0x3f;
    else
    data[i] = display.encodeDigit(digit);
    if (i == 1){
      data[i] = display.encodeDigit(digit) + 0x80;
    }
    num /= 10;    
    }
  }
  display.setSegments(data);
}

void printTemperature(DeviceAddress deviceAddress)
{
  //косяк с отображением температуры от -1 до 0 и от 0 +1 надо исправить
  int tempC = sensors.getTempC(deviceAddress) *10; //Домножаем на 10, чтобы не потерять дробную часть
  if(tempC == DEVICE_DISCONNECTED_C){
    display.setSegments(SEG_ERR);
    return;
  }
  //display.showNumberDecEx(tempC, (0x80 >> 2), false); //и ставим точку на предпоследнем разряде.
  setSegmentsFormatTemperature((int) tempC);
}//Таким образом получаем точность в пределах погрешности датчика

void loop(void)
{ 
  sensors.requestTemperatures();
  printTemperature(insideThermometer);
}
