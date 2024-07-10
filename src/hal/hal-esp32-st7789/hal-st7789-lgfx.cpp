#pragma once
#include <U8g2lib.h>
#include<cstring>
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "hal-st7789-lgfx.h"
#include <HardwareSerial.h>
#include "mutex.h"

using namespace mutex;

#define XORCPY ; // if(lgfx->_doXORdraw)lgfx->copyFBref()

#define NO_GDB 1

#define SDEBUG 1
#if (SDEBUG)
#define IDB
#else
#define IDB if(0)
#endif

#define GFX_BL 25

#define FL float
#define UL unsigned long

// u8g2f = new U8G2_NULL(U8G2_R0);
void flushXTaskRecv(void *taskParams){
	UL tm=0;
	// bool tval;
	// xMutex
	// IDB Serial.println("F:Getting mutex");
	while(1){
		// lockStat st=FREE;
		WAIT_LOCK(FREE);
		ulock=FLUSHING;
	//this->flush();
		tm=::millis();
		glgfx->flush();
		// glgfx->flush_dur=::millis()-tm;
		// Serial.printf("F:%d\n",::millis()-tm);
	// IDB Serial.println("Giving Mutex in flush");
		ulock=CLEARING;
		glgfx->clear();
		vTaskDelay(3);
		// memset(glgfx->_fr);
		//mutex::ulock=0;
		vTaskDelay(3);
		ulock=HELD;
	// Serial.println("Given");
	//TaskHandle_t hand;
	}
	vTaskDelete(NULL);
}

void ESPHAL::init(){ // hz 1 KHZ 1000 MHZ 1000000
	Serial.begin(115200);
	//async_memcpy_config_t AMcfg=ASYNC_MEMCPY_DEFAULT_CONFIG();
	//AMcfg.backlog=8;
	// async_memcpy_t dri=NULL;
	//ESP_ERROR_CHECK(esp_async_memcpy_install(&AMcfg,&mutex::AMdri));
	//Arduino_Canvas_Idx_Enhanced::idx_white=0;
	//Arduino_Canvas_Idx_Enhanced::idx_black=1;
	lgdev=new LGFX_ST7789;
	lgfx=new SPR_TYPE(lgdev);
	if(NO_GDB)Serial.print("Inited Serial\nFree RAM:");
	if(NO_GDB)Serial.println(ESP.getFreeHeap());
	ledcSetup(0,1000,10);
	ledcAttachPin(GFX_BL,0);
	// mutex::Fmutex=xSemaphoreCreateMutex();
	glgfx=lgfx;
	lgdev->initDMA();
	lgdev->initBus();
	bool beg=lgdev->init();
	assert(1==beg);
	this->backLight(_backLight);
	lgdev->setTextColor(WHITE);
	lgdev->print("DEVICE INIT--");
	lgfx->setColorDepth(8);
	if(NO_GDB)Serial.println("Inited gfx freq 70 000 000");
	lgfx->createSprite(240,240);
	// lgfx->clear();
	// lgfx->fillScreen(0);
	// lgfx->setColorDepth(8);
	lgfx->setColor(TFT_WHITE);
	lgfx->setTextColor(TFT_WHITE);
	lgfx->setRawColor(TFT_WHITE);
	lgfx->setFont(&LOG_FONT); //u8g2_font_10x20_me);
	if(NO_GDB)Serial.println("[GFX] before fillS");
	lgfx->fillScreen(0);
	if(NO_GDB)Serial.println("[GFX bef dev fS");
	::delay(500);
	lgdev->fillScreen(TFT_DARKGREY);
	lgdev->display();
	lgdev->startWrite();
	if(NO_GDB)Serial.println("[GFX] before flush Mono");
	::delay(100);
	lgfx->flush();
	lgfx->setCursor(10,20);
	lgfx->println("Astra UI (C) WLZW");
	lgfx->setCursor(10,lgfx->getCursorY());
	lgfx->println("ESP32 ver By\n  Jade_zhang233");
	lgfx->setFont(&unifontCN);
	dum_u8g2->setFont(u8g2_font_unifont_t_chinese3);
	// Not exist: lgfx->setUTF8Print(true);
	dum_u8g2->enableUTF8Print();
	lgfx->println("测试一二三我爱中国");
	lgfx->fillRectAlpha(100,100,20,10,100,TFT_YELLOW);
	if(NO_GDB)Serial.println("[GFX] before flush");
	// lgfx->flush();
	lgfx->pushSprite(lgdev,0,0);
	// lgfx->pushRotated(0);
	lgdev->endWrite();
	lgdev->display();
	if(NO_GDB)Serial.println("Inited gfx Copyright");
	EEPROM.begin(2048); // No FS support, raw io
	if(NO_GDB)Serial.println("Inited EEPROM(flash) size 4096");
	pinMode(34,INPUT);
	pinMode(EC_A,INPUT);
	pinMode(EC_B,INPUT);
	if(NO_GDB)Serial.println("Inited Pin IN mode.\nDone HAL init.");
	::delay(500);
	mutex::ulock=HELD;
	xTaskCreatePinnedToCore(flushXTaskRecv,"GFXflushD",4000,NULL,2,&flTk,ARDUINO_EVENT_RUNNING_CORE);
	//attachInterrupt(EC_A,enc_resolv,FALLING);
}

// tft
void ESPHAL::_drawPixel(FL x,FL y){
	// int16_t tx=x,ty=y;
	XORCPY;
	lgfx->drawPixel(I16 x,I16 y,dumCol);
}
void ESPHAL::_drawBox(FL x,FL y,FL w,FL h){
	//lgfx->copyFBref();
	XORCPY;
	// int16_t tx=x,ty=y,th=h,tw=w;
	lgfx->fillRect(I16 x,I16 y,I16 w,I16 h,dumCol);
}
void ESPHAL::_canvasClear(){
	XORCPY;
	bool t=lgfx->getXORdraw();
	lgfx->setXORdraw(0);
	lgfx->fillScreen(0);
	lgfx->setXORdraw(t);
	// lgfx->copyFBref();
	// lgfx->calc_start=::millis();
}
void *ESPHAL::_getCanvasBuffer(){
	return lgfx->getBuffer();
}
uint8_t ESPHAL::_getBufferTileHeight(){
	return 0;
}
void ESPHAL::_screenOn(){
	// lgdev->displayOn();
	this->backLight(_backLight);
}
void ESPHAL::_screenOff(){
	this->backLight(0);
	// lgdev->displayOff();
}
void ESPHAL::_canvasUpdate(){
	lgdev->startWrite();
	lgfx->pushSprite(0,0);
	lgdev->endWrite();
	lgdev->display();
	return;
	// BELOW TMP USELESS 2407082226
	//lgdev->displayOff();
	//lgfx->displayOff();
	//this->backLight(_backLight-50);
	// Serial.println("Update Pre");
	lgfx->setCursor(10,190);
	// lgfx->printf("R:%d Fp:%d ms",::millis()-lgfx->calc_start,lgfx->flush_dur);
	WAIT_LOCK(HELD);
	mutex::ulock=FREE; // allow thread to flush
	vTaskDelay(3);
	while(mutex::ulock<CLEARING){
		// Serial.println("Waiting Lock");
		vTaskDelay(1);
	}
	vTaskDelay(2);
	// lgfx->swapFB();
	//mutex::ulock=HELD;
	//vTaskDelay(2);
	// return;
}
void ESPHAL::backLight(uint8_t value){
	if(value<0||value>255)return;
	ledcWrite(0,map(value,0,255,0,1023));
}
void ESPHAL::_drawVLine(FL x,FL y,FL h){
	// int16_t _x=x,_y=y,_h=h;
	//lgfx->drawFastVLine(I16 x,I16 y,I16 h,dumCol);
	XORCPY;
	for(int cy=y;cy<=y+h;++cy) lgfx->drawPixel(x,cy,dumCol);
	// lgfx->drawLine(_x,_y,_x,_h+_y,dumCol);
}
// *S*S* h=5?
// * h=1
// Current Y value:
void ESPHAL::_drawVDottedLine(FL x,FL y,FL h){
	XORCPY;
	int16_t _x=x,_y=y,_h=h;
	for(int cy=_y;cy<_y+_h;cy+=2){
		lgfx->drawPixel(_x,cy,dumCol);
	}
}
void ESPHAL::_drawHLine(FL x,FL y,FL w){
	XORCPY;
	// int16_t _x=x,_y=y,_w=w;
	// lgfx->drawFastHLine(I16 x,I16 y,I16 w,dumCol);
	for(int cx=x;cx<=x+w;++cx) lgfx->drawPixel(cx,y,dumCol);
	// lgfx->drawLine(_x,_y,_w+_x,_y,dumCol);
}
void ESPHAL::_drawHDottedLine(FL x,FL y,FL w){
	// int16_t _x=x,_y=y,_w=w;
	// *S*S* h=5?
	// * h=1
	// Current X value:
	XORCPY;
	for(int cx=x;cx<=x+w;cx+=2){
		lgfx->drawPixel(cx,y,dumCol);
	}
}
void ESPHAL::_drawRBox(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	//lgfx->copyFBref();
	XORCPY;
	lgfx->fillRoundRect(I16 x,I16 y,I16 w,I16 h,I16 r,dumCol);
}
void ESPHAL::_drawFrame(FL x,FL y,FL w,FL h){
	int16_t _x=x,_y=y,_w=w,_h=h;
	lgfx->drawRect(I16 x,I16 y,I16 w,I16 h,dumCol);
	//lgfx->draw(_x,_y,_w,_h,dumCol);
}
void ESPHAL::_drawRFrame(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	XORCPY;
	//lgfx->copyFBref();
	lgfx->drawRoundRect(_x,_y,_w,_h,_r,dumCol);
}
void ESPHAL::_drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap){
	int16_t x=_x,y=_y,w=_w,h=_h;
	lgfx->drawBitmap(x,y,_bitMap,w,h,dumCol);
}
void ESPHAL::_drawChinese(float _x, float _y, const std::string &_text){
	// lgfx->setUTF8Print(1);
	//lgfx->copyFBref();
	XORCPY;
	lgfx->setCursor(I16 _x,I16 _y);
	lgfx->print(_text.c_str());
}
void ESPHAL::_drawEnglish(float _x, float _y, const std::string &_text){
	// lgfx->setUTF8Print(1);
	lgfx->setCursor(I16 _x,I16 _y);
	lgfx->print(_text.c_str());
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
	return ::millis()*1000;
}
UL ESPHAL::_getTick(){
	return ::millis()*10;
}
// uint8_t ESPHAL::_getBufferTileHeight(){return 0;}
uint8_t ESPHAL::_getBufferTileWidth(){return 0;}
void ESPHAL::_setFont(const uint8_t *_font){
	lgfx->setFont(new lgfx::U8g2font(_font));
	dum_u8g2->setFont(_font);
	// u8g2_
	_fontSet=1;
}
uint8_t ESPHAL::_getFontHeight(){
	if(!_fontSet)return 40; // dangerous, may be imcompatible
	uint8_t hei=abs(dum_u8g2->getFontAscent())+abs(dum_u8g2->getFontDescent());
	return hei;
}
uint8_t ESPHAL::_getFontWidth(std::__cxx11::string &_text){
	const char *textp=_text.c_str();
	return dum_u8g2->getUTF8Width(textp);
}
void ESPHAL::_setDrawType(uint8_t type){
	dum_u8g2->setDrawColor(type);
	// lgfx->setDrawMode(type);
	if(type==0){
		lgfx->setTextColor(0);
		lgfx->setRawColor(0);
		lgfx->setBaseColor(0);
		dumCol=BLACK;
	}else if(type==1){
		lgfx->setTextColor(TFT_WHITE);
		lgfx->setRawColor(TFT_WHITE);
		lgfx->setBaseColor(TFT_WHITE);
		dumCol=WHITE;
	}
	if(type==2){ // Draw XOR
		// lgfx->setTextColor(0);
		lgfx->setXORdraw(1);
		// dumCol=WHITE;
		// _printInfo("SETTING 2");
		// dumCol=BLACK;
	}else{
		lgfx->setXORdraw(0);
	}
}
void ESPHAL::_updateConfig(){
	EEPROM.put(CFG_START_POS,config);
}
IRAM_ATTR bool ESPHAL::_getKey(key::KEY_INDEX _keyIndex) {
  if (_keyIndex == key::KEY_0) return touchRead(TOUCH_KEY0)<=80;
  if (_keyIndex == key::KEY_1) return touchRead(TOUCH_KEY1)<=80;
  return false;
}
void ESPHAL::_printInfo(std::__cxx11::string _msg){
	if(NO_GDB)Serial.println(_msg.c_str());
}
void ESPHAL::dumpInfo(){
	// Serial.printf("COLOR: %x\n",dumCol);
	// lgfx->dumpDrawArgs();
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
