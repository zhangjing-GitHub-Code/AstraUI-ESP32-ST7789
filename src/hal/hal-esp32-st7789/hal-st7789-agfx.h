#define ESP32SPIDMA_MAX_PIXELS_AT_ONCE 256
#include <U8g2lib.h>
#include<cstring>
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "AGFX-CM-Enhance.h"
#include <EEPROM.h>
#include <HardwareSerial.h>
#define GFX_BL 25
#define I16 (uint16_t)
#define LOG_FONT u8g2_font_chargen_92_tf
// # dont include "hal-st7789-agfx.cpp"

#define FL float
#define UL unsigned long
// ``
#include "../hal.h"
#include "AGFX-CM-Enhance.h"

#ifndef HAL_ESP_ST7789_H
#define HAL_ESP_ST7789_H

// U8G2_NULL *u8g2f;
const int
  P_DC = 32, // 25
  P_CS = 5,  // 5
  P_SCL = 18,
  P_SDA = 23,
  P_RST = 15,
  EC_A=35,
  EC_B=36;
const int CFG_START_POS=0;
const int TOUCH_KEY0=13,
		  TOUCH_KEY1=27; // T7
class ESPHAL:public HAL{
	private:
	// key::KEY_TYPE keyFlag;
	bool _fontSet=false;
	int8_t _backLight=255;
	TaskHandle_t flTk=0;
	U8G2_NULL *dum_u8g2=new U8G2_NULL(U8G2_R0);
	Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
	  P_DC /* DC */,
	  P_CS /* CS */,
	  P_SCL /* SCK */,
	  P_SDA /* MOSI */,
	  GFX_NOT_DEFINED /* MISO */,
	  VSPI
	);
	Arduino_GFX *tfdev = new Arduino_ST7789(
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
	Arduino_Canvas_Idx_Enhanced *tgfx = new Arduino_Canvas_Idx_Enhanced(240 /* width */, 240 /* height */, tfdev,
	                                            0 /* output_x */, 0 /* output_y */);
	int dumCol;
	void enc_resolv(){
		;
	}
	void init() override;
	void setColorCompat(int col) {
	  // 0 -> blank
	  // 1 -> draw
	  // 2 -> XOR (tmp as 1)
		if (col == 0) {
			dumCol = BLACK;
		} else if (col == 1) {
			dumCol = WHITE;
		} else if (col == 2) {
			dumCol = WHITE;
		}
	}
	// tft
	public:

  uint8_t *_getCanvasBuffer() override;
  uint8_t _getBufferTileHeight() override;
  uint8_t _getBufferTileWidth() override;
  void _canvasUpdate() override;
  void backLight(uint8_t value);
  void _canvasClear() override;
  void _setFont(const uint8_t *_font) override;
  uint8_t _getFontWidth(std::string &_text) override;
  uint8_t _getFontHeight() override;
  void _setDrawType(uint8_t _type) override;
  void _drawPixel(float _x, float _y) override;

  //notice: _x和_y是字体左下角的坐标 _x and _y is the coordinate the lower left corner of the font
  //todo 检查所有的draw函数的坐标是否是左下角的坐标  void _drawEnglish(float _x, float _y, const std::string &_text) override;
  void _drawEnglish(float _x, float _y, const std::string &_text) override;
  void _drawChinese(float _x, float _y, const std::string &_text) override;
  void _drawVDottedLine(float _x, float _y, float _h) override;
  void _drawHDottedLine(float _x, float _y, float _l) override;
  void _drawVLine(float _x, float _y, float _h) override;
  void _drawHLine(float _x, float _y, float _l) override;

  void _drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap) override;
  void _drawBox(float _x, float _y, float _w, float _h) override;
  void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
  void _drawFrame(float _x, float _y, float _w, float _h) override;
  void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;
  void _printInfo(std::string _msg);
  public:
  void _delay(unsigned long _mill) override;
  unsigned long _millis() override;
  unsigned long _getTick() override;
  /**optional**/
  unsigned long _getRandomSeed() override;

  /**
   * @brief buzzer.
   * */
public:  void _beep(float _freq) override;
  void _beepStop() override;
  void _setBeepVol(uint8_t _vol) override;
  void _screenOn() override;
  void _screenOff() override;

  /**
   * @brief key.
   */
public:  bool _getKey(key::KEY_INDEX _keyIndex) override;
  // bool _getAnyKey();

protected:
  key::keyAction key[key::KEY_NUM] = {static_cast<key::keyAction>(0)};

public: //  void _keyScan();
  // void _keyTest();

  /**
   * @brief system config.
   */
public:
  void _updateConfig() override;
};

#endif // HAL_ESP_ST7789_H