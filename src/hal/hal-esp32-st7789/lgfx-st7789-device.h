#pragma once
#include "LovyanGFX.hpp"
#include "esp_attr.h"
#include "lgfx/v1/misc/common_function.hpp"
#include "lgfx/v1/misc/enum.hpp"
#include "pinConfig.h"
using namespace lgfx::v1;

class LGFX_ST7789 : public lgfx::LGFX_Device
{
	/*
 命名方案可以自由决定，但万一配置的数量增加，请将构造函数的名称改为与面板的名称相同。
 例如，如果你在ESP32 DevKit-C中设置了一个ILI9341 SPI连接，你可以使用
	LGFX_DevKitC_SPI_ILI9341
 并将文件名与类名相匹配，这样在使用时就不容易混淆了。
//*/
// 为要连接的屏幕类型准备一个实例。
//lgfx::Panel_ST7735      _panel_instance;
//lgfx::Panel_ST7735S     _panel_instance;
	lgfx::Panel_ST7789      _panel_instance;
//lgfx::Panel_ST7796      _panel_instance;

// 为屏幕所连接的总线类型准备一个实例。
	lgfx::Bus_SPI        _bus_instance;   // SPI
//lgfx::Bus_I2C        _bus_instance;   // I2C
//lgfx::Bus_Parallel8  _bus_instance;   // 8 Parallel

// 如果进行背光控制，请提供一个实例。 (如果不需要则删除）。
	lgfx::Light_PWM     _light_instance;
// 为触摸屏类型准备一个实例。 (如果不需要则删除）。
// lgfx::Touch_FT5x06           _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436
//lgfx::Touch_GSL1680E_800x480 _touch_instance; // GSL_1680E, 1688E, 2681B, 2682B
//lgfx::Touch_GSL1680F_800x480 _touch_instance;
//lgfx::Touch_GSL1680F_480x272 _touch_instance;
//lgfx::Touch_GSLx680_320x320  _touch_instance;
//lgfx::Touch_GT911            _touch_instance;
//lgfx::Touch_STMPE610         _touch_instance;
//lgfx::Touch_TT21xxx          _touch_instance; // TT21100
//lgfx::Touch_XPT2046          _touch_instance;
public:
	// 创建一个构造函数并在这里配置各种设置。
	// 如果你改变了类的名称，请为构造函数指定相同的名称。
	LGFX_ST7789(void);
};
class LGFX_SpriteMX;
// Sprite with Mono and XOR
struct Panel_SpriteMX : public lgfx::Panel_Sprite{
    friend LGFX_Sprite;
    friend Panel_Sprite;
	friend LGFX_SpriteMX;
	bool bXOR=0;
	Panel_SpriteMX(void) { _start_count = INT32_MAX; }
	IRAM_ATTR void setXORdraw(bool val){ bXOR=val; };
	IRAM_ATTR void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    IRAM_ATTR void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t raw_color) override;
};
#define LGFX_INLINE __attribute__ ((always_inline)) inline
// Sprite with Mono and XOR
class LGFX_SpriteMX : public LGFX_Sprite{
	protected:
	Panel_SpriteMX _panel_sprite;
	public:
	LGFX_SpriteMX(LovyanGFX* parent):LGFX_Sprite(parent){
		Serial.println("MX init");
		// Serial.printf("PanSp addr: %x, from par:%x\n",&_panel_sprite);
		auto spr=LGFX_Sprite::_panel_sprite;
		Serial.printf("PanSp addr: %x,old addr:%x\n",&_panel_sprite,&spr);
		_panel=&_panel_sprite;
	}
//    , _bitwidth(0)

	IRAM_ATTR void setXORdraw(bool val){ _panel_sprite.bXOR=val; };
	IRAM_ATTR bool getXORdraw(){ return _panel_sprite.bXOR; };
	LGFX_INLINE void* getBuffer(void) const { return _panel_sprite.getBuffer(); }
    uint32_t bufferLength(void) const { return _panel_sprite.bufferLength(); }
    void deleteSprite(void)
    {
//      _bitwidth = 0;
      _clip_l = 0;
      _clip_t = 0;
      _clip_r = -1;
      _clip_b = -1;

      _panel_sprite.deleteSprite();
      _img = nullptr;
    }

    void setPsram( bool enabled )
    {
      if (_psram == enabled) return;
      _psram = enabled;
      deleteSprite();
    }

    void setBuffer(void* buffer, int32_t w, int32_t h, uint8_t bpp = 0)
    {
      deleteSprite();
      if (bpp != 0) _write_conv.setColorDepth((color_depth_t)bpp, hasPalette());

      _panel_sprite.setBuffer(buffer, w, h, &_write_conv);
      _img = _panel_sprite.getBuffer();

      _sw = w;
      _clip_r = w - 1;
      _xpivot = w >> 1;

      _sh = h;
      _clip_b = h - 1;
      _ypivot = h >> 1;
    }

    void* createSprite(int32_t w, int32_t h)
    {
      _img = _panel_sprite.createSprite(w, h, &_write_conv, _psram);
      if (_img) {
        if (getColorDepth() & color_depth_t::has_palette)
        {
          createPalette();
        }
      }
      setRotation(getRotation());

      _sw = width();
      _clip_r = _sw - 1;
      _xpivot = _sw >> 1;

      _sh = height();
      _clip_b = _sh - 1;
      _ypivot = _sh >> 1;

      _clip_l = _clip_t = _sx = _sy = 0;


      return _img;
    }
	//protected: uint_fast16_t getPW(){
	//	return 
	//}
    LGFX_INLINE void* setColorDepth(uint8_t bpp)
    {
      _write_conv.setColorDepth(bpp, bpp < 8);
      return setColorDepth(_write_conv.depth);
    }
    void* setColorDepth(color_depth_t depth)
    {
      _write_conv.setColorDepth(depth);
      _read_conv = _write_conv;

      _panel_sprite.setColorDepth(_write_conv.depth);

      if (_panel_sprite.getBuffer() == nullptr) return nullptr;
      auto w = _panel_sprite._panel_width;
      auto h = _panel_sprite._panel_height;
      deleteSprite();
      deletePalette();
      return createSprite(w, h);
    }
    template<typename T>
    LGFX_INLINE void pushSprite(                int32_t x, int32_t y, const T& transp) { push_sprite(_parent, x, y, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T>
    LGFX_INLINE void pushSprite(LovyanGFX* dst, int32_t x, int32_t y, const T& transp) { push_sprite(    dst, x, y, _write_conv.convert(transp) & _write_conv.colormask); }
    LGFX_INLINE void pushSprite(                int32_t x, int32_t y) { push_sprite(_parent, x, y); }
    LGFX_INLINE void pushSprite(LovyanGFX* dst, int32_t x, int32_t y) { push_sprite(    dst, x, y); }
	void push_sprite(LovyanGFX* dst, int32_t x, int32_t y, uint32_t transp = pixelcopy_t::NON_TRANSP){
		// Serial.println("OVERRIDEN PUSH");
      pixelcopy_t p(_img, dst->getColorDepth(), getColorDepth(), dst->hasPalette(), _palette, transp);
      dst->pushImage(x, y, _panel_sprite._panel_width, _panel_sprite._panel_height, &p, _panel_sprite.getSpriteBuffer()->use_dma()); // DMA disable with use SPIRAM
    }
    void push_rotate_zoom(LovyanGFX* dst, float x, float y, float angle, float zoom_x, float zoom_y, uint32_t transp = pixelcopy_t::NON_TRANSP){
      dst->pushImageRotateZoom(x, y, _xpivot, _ypivot, angle, zoom_x, zoom_y, _panel_sprite._panel_width, _panel_sprite._panel_height, _img, transp, getColorDepth(), _palette.img24());
    }
    void push_rotate_zoom_aa(LovyanGFX* dst, float x, float y, float angle, float zoom_x, float zoom_y, uint32_t transp = pixelcopy_t::NON_TRANSP){
      dst->pushImageRotateZoomWithAA(x, y, _xpivot, _ypivot, angle, zoom_x, zoom_y, _panel_sprite._panel_width, _panel_sprite._panel_height, _img, transp, getColorDepth(), _palette.img24());
    }
    void push_affine(LovyanGFX* dst, const float matrix[6], uint32_t transp = pixelcopy_t::NON_TRANSP){
      dst->pushImageAffine(matrix, _panel_sprite._panel_width, _panel_sprite._panel_height, _img, transp, getColorDepth(), _palette.img24());
    }
    void push_affine_aa(LovyanGFX* dst, const float matrix[6], uint32_t transp = pixelcopy_t::NON_TRANSP){
      dst->pushImageAffineWithAA(matrix, _panel_sprite._panel_width, _panel_sprite._panel_height, _img, transp, getColorDepth(), _palette.img24());
    }


    uint32_t readPixelValue(int32_t x, int32_t y) { return _panel_sprite.readPixelValue(x, y); }

};