#pragma once
#include <U8g2lib.h>
#include<cstring>
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "hal-st7789-agfx.h"
#include <HardwareSerial.h>
#define GFX_BL 25

#define FL float
#define UL unsigned long

// u8g2f = new U8G2_NULL(U8G2_R0);

void ESPHAL::init(){ // hz 1 KHZ 1000 MHZ 1000000
	Serial.begin(115200);
	Serial.print("Inited Serial\nFree RAM:");
	Serial.println(ESP.getFreeHeap());
	bool beg=tgfx->begin(70000000);
	assert(1==beg);
	Serial.println("Inited gfx freq 75 000 000");
	tgfx->setFont(LOG_FONT); //u8g2_font_10x20_me);
	Serial.println("[GFX] before fillS");
	tgfx->fillScreen(0);
	tgfx->flush();
	tgfx->setCursor(10,20);
	tgfx->println("Astra UI (C) WLZW");
	tgfx->setCursor(10,tgfx->getCursorY());
	tgfx->println("ESP32 ver - Jade_zhang233");
	Serial.println("[GFX] before flush");
	tgfx->flush();
	Serial.println("Inited gfx Copyright");
	EEPROM.begin(2048); // No FS support, raw io
	Serial.println("Inited EEPROM(flash) size 4096");
	pinMode(34,INPUT);
	pinMode(EC_A,INPUT);
	pinMode(EC_B,INPUT);
	Serial.println("Inited Pin IN mode.\nDone HAL init.");
	::delay(500);
	//attachInterrupt(EC_A,enc_resolv,FALLING);
}

// tft
void ESPHAL::_drawPixel(FL x,FL y){
	// int16_t tx=x,ty=y;
	tgfx->drawPixel(I16 x,I16 y,dumCol);
}
void ESPHAL::_drawBox(FL x,FL y,FL w,FL h){
	// int16_t tx=x,ty=y,th=h,tw=w;
	tgfx->drawRect(I16 x,I16 y,I16 w,I16 h,dumCol);
}
void ESPHAL::_canvasClear(){
	tgfx->fillScreen(0);
}
uint8_t *ESPHAL::_getCanvasBuffer(){
	return tgfx->getFramebuffer();
}
uint8_t ESPHAL::_getBufferTileHeight(){
	return 0;
}
void ESPHAL::_screenOn(){
	tgfx->displayOn();
}
void ESPHAL::_screenOff(){
	tgfx->displayOff();
}
void ESPHAL::_canvasUpdate(){
	tgfx->flush();
}
void ESPHAL::_drawVLine(FL x,FL y,FL h){
	int16_t _x=x,_y=y,_h=h;
	tgfx->drawLine(_x,_y,_x,_h+_y,dumCol);
}
// *S*S* h=5?
// * h=1
// Current Y value:
void ESPHAL::_drawVDottedLine(FL x,FL y,FL h){
	int16_t _x=x,_y=y,_h=h;
	for(int cy=_y;cy<_y+_h;cy+=2){
		tgfx->drawPixel(_x,cy,dumCol);
	}
}
void ESPHAL::_drawHLine(FL x,FL y,FL w){
	int16_t _x=x,_y=y,_w=w;
	tgfx->drawLine(_x,_y,_w+_x,_y,dumCol);
}
void ESPHAL::_drawHDottedLine(FL x,FL y,FL w){
	int16_t _x=x,_y=y,_w=w;
	// *S*S* h=5?
	// * h=1
	// Current X value:
	for(int cx=_x;cx<_x+_w;cx+=2){
		tgfx->drawPixel(cx,_y,dumCol);
	}
}
void ESPHAL::_drawRBox(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	tgfx->fillRoundRect(_x,_y,_w,_h,_r,dumCol);
}
void ESPHAL::_drawFrame(FL x,FL y,FL w,FL h){
	int16_t _x=x,_y=y,_w=w,_h=h;
	//tgfx->draw(_x,_y,_w,_h,dumCol);
}
void ESPHAL::_drawRFrame(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	tgfx->drawRoundRect(_x,_y,_w,_h,_r,dumCol);
}
void ESPHAL::_drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap){
	int16_t x=_x,y=_y,w=_w,h=_h;
	tgfx->drawBitmap(x,y,_bitMap,w,h,dumCol);
}
void ESPHAL::_drawChinese(float _x, float _y, const std::string &_text){
	tgfx->setUTF8Print(1);
	tgfx->setCursor(I16 _x,I16 _y);
	tgfx->print(_text.c_str());
}
void ESPHAL::_drawEnglish(float _x, float _y, const std::string &_text){
	tgfx->setUTF8Print(0);
	tgfx->setCursor(I16 _x,I16 _y);
	tgfx->print(_text.c_str());
}
UL ESPHAL::_getRandomSeed(){
	return analogRead(34)*2+analogRead(33)+analogRead(38)*3;
}
void ESPHAL::_beep(float freq){
	;
}
void ESPHAL::_beepStop(){}
void ESPHAL::_setBeepVol(uint8_t vol){}
void ESPHAL::_delay(UL millis){
	::delay(millis);
}
UL ESPHAL::_millis(){
	return ::millis();
}
UL ESPHAL::_getTick(){
	return ::millis()*100;
}
// uint8_t ESPHAL::_getBufferTileHeight(){return 0;}
uint8_t ESPHAL::_getBufferTileWidth(){return 0;}
void ESPHAL::_setFont(const uint8_t *_font){
	tgfx->setFont(_font);
	dum_u8g2->setFont(_font);
	_fontSet=1;
}
uint8_t ESPHAL::_getFontHeight(){
	if(!_fontSet)return 40; // dangerous, may be imcompatible
	uint8_t hei=abs(dum_u8g2->getFontAscent())+abs(dum_u8g2->getFontDescent());
	return hei;
}
uint8_t ESPHAL::_getFontWidth(std::__cxx11::string &_text){
	const char *textp=_text.c_str();
	return dum_u8g2->getStrWidth(textp);
}
void ESPHAL::_setDrawType(uint8_t type){
	dum_u8g2->setDrawColor(type);
	// tgfx->setDrawMode(type);
	if(type==0){
	}else if(type==1){
		tgfx->setTextColor(WHITE);
		dumCol=WHITE;
	}else if(type==2){
		tgfx->setTextColor(0);
		dumCol=BLACK;
	}
}
void ESPHAL::_updateConfig(){
	EEPROM.put(CFG_START_POS,config);
}
bool ESPHAL::_getKey(key::KEY_INDEX _keyIndex) {
  if (_keyIndex == key::KEY_0) return touchRead(TOUCH_KEY0)<=80;
  if (_keyIndex == key::KEY_1) return touchRead(TOUCH_KEY1)<=80;
  return false;
}
void ESPHAL::_printInfo(std::__cxx11::string _msg){
	Serial.println(_msg.c_str());
}
// ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
/*bool ESPHAL::_getAnyKey() {
  for (int i = 0; i < key::KEY_NUM; i++) {
    if (getKey(static_cast<key::KEY_INDEX>(i))) return true;
  }
  return false;
}*/

/**
 * @brief key scanner default. 默认按键扫描函数
 *
 * @note run per 5 ms.
 * @return key::keyValue
 * /
void ESPHAL::_keyScan() {
  static unsigned char _timeCnt = 0;
  static bool _lock = false;
  static key::KEY_FILTER _keyFilter = key::CHECKING;
  switch (_keyFilter) {
    case key::CHECKING:
      if (getAnyKey()) {
        if (getKey(key::KEY_0)) _keyFilter = key::KEY_0_CONFIRM;
        if (getKey(key::KEY_1)) _keyFilter = key::KEY_1_CONFIRM;
      }
      _timeCnt = 0;
      _lock = false;
      break;

    case key::KEY_0_CONFIRM:
    case key::KEY_1_CONFIRM:
      //filter
      if (getAnyKey()) {
        if (!_lock) _lock = true;
        _timeCnt++;

        //timer
        if (_timeCnt > 100) {
          keyFlag = key::KEY_PRESSED;
          //long press 1s
          if (getKey(key::KEY_0)) {
            key[key::KEY_0] = key::PRESS;
            key[key::KEY_1] = key::RELEASE;
          }
          if (getKey(key::KEY_1)) {
            key[key::KEY_1] = key::PRESS;
            key[key::KEY_0] = key::RELEASE;
          }
          _timeCnt = 0;
          _lock = false;
          _keyFilter = key::RELEASED;
        }
      } else {
        if (_lock) {
          if (_keyFilter == key::KEY_0_CONFIRM) {
            key[key::KEY_0] = key::CLICK;
            key[key::KEY_1] = key::RELEASE;
          }
          if (_keyFilter == key::KEY_1_CONFIRM) {
            key[key::KEY_1] = key::CLICK;
            key[key::KEY_0] = key::RELEASE;
          }
          keyFlag = key::KEY_PRESSED;
          _keyFilter = key::RELEASED;
        } else {
          _keyFilter = key::CHECKING;
          key[key::KEY_0] = key::RELEASE;
          key[key::KEY_1] = key::RELEASE;
        }
      }
      break;

    case key::RELEASED:
      if (!getAnyKey()) _keyFilter = key::CHECKING;
      break;

    default: break;
  }
}
*/

/**
 * @brief default key tester. 默认按键测试函数
 * /
void _keyTest() {
  if (getAnyKey()) {
    for (unsigned char i = 0; i < key::KEY_NUM; i++) {
      if (key[i] == key::CLICK) {
        //do something when key clicked
        if (i == 0) break;
        if (i == 1) break;
      } else if (key[i] == key::PRESS) {
        //do something when key pressed
        if (i == 0) break;
        if (i == 1) break;
      }
    }
    memset(key, key::RELEASE, sizeof(key));
  }
}
*/
