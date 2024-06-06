#pragma once
#include <U8g2lib.h>
#include<cstring>
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "hal-st7789-agfx.h"
#include <HardwareSerial.h>
#include "mutex.h"

using namespace mutex;

#define XORCPY if(tgfx->_doXORdraw)tgfx->copyFBref()

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
		gtgfx->flushXtask();
		gtgfx->flush_dur=::millis()-tm;
		// Serial.printf("F:%d\n",::millis()-tm);
	// IDB Serial.println("Giving Mutex in flush");
		ulock=CLEARING;
		gtgfx->clearFrameBuffer();
		vTaskDelay(3);
		// memset(gtgfx->_fr);
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
	tfdev=new Arduino_ST7789(
	  bus,
	  P_RST /* RST */,
	  0 /* rotation */,
	  true /* IPS */,
	  240 /* width */,
	  240 /* height */,
	  0 /* col offset 1 */,
	  0 /* row offset 1 */,
	  0 /* col offset 2 */,
	  0 /* row offset 2 */
	);
	tgfx=new ACME(240 /* width */, 240 /* height */, tfdev,
	                                            0 /* output_x */, 0 /* output_y */);
	if(NO_GDB)Serial.print("Inited Serial\nFree RAM:");
	if(NO_GDB)Serial.println(ESP.getFreeHeap());
	ledcSetup(0,1000,10);
	ledcAttachPin(GFX_BL,0);
	// mutex::Fmutex=xSemaphoreCreateMutex();
	gtgfx=tgfx;
	bool beg=tgfx->begin(65000000);
	assert(1==beg);
	this->backLight(_backLight);
	if(NO_GDB)Serial.println("Inited gfx freq 70 000 000");
	tgfx->setFont(LOG_FONT); //u8g2_font_10x20_me);
	if(NO_GDB)Serial.println("[GFX] before fillS");
	tgfx->fillScreen(0);
	if(NO_GDB)Serial.println("[GFX bef dev fS");
	::delay(500);
	tfdev->fillScreen(0);
	if(NO_GDB)Serial.println("[GFX] before flush Mono");
	::delay(100);
	tgfx->flush();
	tgfx->setCursor(10,20);
	tgfx->println("Astra UI (C) WLZW");
	tgfx->setCursor(10,tgfx->getCursorY());
	tgfx->println("ESP32 ver By\n  Jade_zhang233");
	tgfx->setFont(u8g2_font_unifont_t_chinese3);
	dum_u8g2->setFont(u8g2_font_unifont_t_chinese3);
	tgfx->setUTF8Print(true);
	dum_u8g2->enableUTF8Print();
	tgfx->println("测试一二三我爱中国");
	if(NO_GDB)Serial.println("[GFX] before flush");
	tgfx->flush();
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
	tgfx->drawPixel(I16 x,I16 y,dumCol);
}
void ESPHAL::_drawBox(FL x,FL y,FL w,FL h){
	//tgfx->copyFBref();
	XORCPY;
	// int16_t tx=x,ty=y,th=h,tw=w;
	tgfx->fillRect(I16 x,I16 y,I16 w,I16 h,dumCol);
}
void ESPHAL::_canvasClear(){
	XORCPY;
	tgfx->normFillScr(0);
	// tgfx->copyFBref();
	tgfx->calc_start=::millis();
}
uint8_t *ESPHAL::_getCanvasBuffer(){
	return tgfx->getFramebuffer();
}
uint8_t ESPHAL::_getBufferTileHeight(){
	return 0;
}
void ESPHAL::_screenOn(){
	tfdev->displayOn();
	this->backLight(_backLight);
}
void ESPHAL::_screenOff(){
	this->backLight(0);
	tfdev->displayOff();
}
void ESPHAL::_canvasUpdate(){
	//tfdev->displayOff();
	//tgfx->displayOff();
	//this->backLight(_backLight-50);
	// Serial.println("Update Pre");
	tgfx->setCursor(10,190);
	tgfx->printf("R:%d Fp:%d ms",::millis()-tgfx->calc_start,tgfx->flush_dur);
	WAIT_LOCK(HELD);
	mutex::ulock=FREE; // allow thread to flush
	vTaskDelay(3);
	while(mutex::ulock<CLEARING){
		// Serial.println("Waiting Lock");
		vTaskDelay(1);
	}
	vTaskDelay(2);
	tgfx->swapFB();
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
	//tgfx->drawFastVLine(I16 x,I16 y,I16 h,dumCol);
	XORCPY;
	for(int cy=y;cy<=y+h;++cy) tgfx->drawPixel(x,cy,dumCol);
	// tgfx->drawLine(_x,_y,_x,_h+_y,dumCol);
}
// *S*S* h=5?
// * h=1
// Current Y value:
void ESPHAL::_drawVDottedLine(FL x,FL y,FL h){
	XORCPY;
	int16_t _x=x,_y=y,_h=h;
	for(int cy=_y;cy<_y+_h;cy+=2){
		tgfx->drawPixel(_x,cy,dumCol);
	}
}
void ESPHAL::_drawHLine(FL x,FL y,FL w){
	XORCPY;
	// int16_t _x=x,_y=y,_w=w;
	// tgfx->drawFastHLine(I16 x,I16 y,I16 w,dumCol);
	for(int cx=x;cx<=x+w;++cx) tgfx->drawPixel(cx,y,dumCol);
	// tgfx->drawLine(_x,_y,_w+_x,_y,dumCol);
}
void ESPHAL::_drawHDottedLine(FL x,FL y,FL w){
	// int16_t _x=x,_y=y,_w=w;
	// *S*S* h=5?
	// * h=1
	// Current X value:
	XORCPY;
	for(int cx=x;cx<=x+w;cx+=2){
		tgfx->drawPixel(cx,y,dumCol);
	}
}
void ESPHAL::_drawRBox(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	//tgfx->copyFBref();
	XORCPY;
	tgfx->fillRoundRect(I16 x,I16 y,I16 w,I16 h,I16 r,dumCol);
}
void ESPHAL::_drawFrame(FL x,FL y,FL w,FL h){
	int16_t _x=x,_y=y,_w=w,_h=h;
	tgfx->drawRect(I16 x,I16 y,I16 w,I16 h,dumCol);
	//tgfx->draw(_x,_y,_w,_h,dumCol);
}
void ESPHAL::_drawRFrame(FL x,FL y,FL w,FL h,FL r){
	int16_t _x=x,_y=y,_w=w,_h=h,_r=r;
	XORCPY;
	//tgfx->copyFBref();
	tgfx->drawRoundRect(_x,_y,_w,_h,_r,dumCol);
}
void ESPHAL::_drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap){
	int16_t x=_x,y=_y,w=_w,h=_h;
	tgfx->drawBitmap(x,y,_bitMap,w,h,dumCol);
}
void ESPHAL::_drawChinese(float _x, float _y, const std::string &_text){
	// tgfx->setUTF8Print(1);
	//tgfx->copyFBref();
	XORCPY;
	tgfx->setCursor(I16 _x,I16 _y);
	tgfx->print(_text.c_str());
}
void ESPHAL::_drawEnglish(float _x, float _y, const std::string &_text){
	// tgfx->setUTF8Print(1);
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
	return ::millis()*1000;
}
UL ESPHAL::_getTick(){
	return ::millis()*10;
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
	return dum_u8g2->getUTF8Width(textp);
}
void ESPHAL::_setDrawType(uint8_t type){
	dum_u8g2->setDrawColor(type);
	// tgfx->setDrawMode(type);
	if(type==0){
		tgfx->setTextColor(0);
		dumCol=BLACK;
	}else if(type==1){
		tgfx->setTextColor(WHITE);
		dumCol=WHITE;
	}
	if(type==2){ // Draw XOR
		// tgfx->setTextColor(0);
		tgfx->setXORdraw(1);
		// dumCol=WHITE;
		// _printInfo("SETTING 2");
		// dumCol=BLACK;
	}else{
		tgfx->setXORdraw(0);
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
	if(NO_GDB)Serial.println(_msg.c_str());
}
void ESPHAL::dumpInfo(){
	Serial.printf("COLOR: %x\n",dumCol);
	tgfx->dumpDrawArgs();
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
