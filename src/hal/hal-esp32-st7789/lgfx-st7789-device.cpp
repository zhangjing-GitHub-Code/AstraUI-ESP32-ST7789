// Comments and ... Copyright: www.lingshunlab.com


#include "lgfx-st7789-device.h"


LGFX_ST7789 :: LGFX_ST7789(void)
	{
	  { // Bus Configuration
	    auto cfg = _bus_instance.config();

	// SPI设定
	    cfg.spi_host = VSPI_HOST;     // 选择要使用的SPI ESP32-S2,C3 : SPI2_HOST 或 SPI3_HOST / ESP32 : VSPI_HOST 或 HSPI_HOST
	     // * 随着ESP-IDF版本的升级，VSPI_HOST , HSPI_HOST的描述被废弃了，所以如果发生错误，请使用SPI2_HOST , 
	    cfg.spi_mode = 0;             //设置SPI通信模式(0 ~ 3)
	    cfg.freq_write = 65000000;    // 发送时的SPI时钟（最大80MHz，四舍五入为80MHz的整数）。
	    cfg.freq_read  =  6000000;    // 接收时的SPI时钟
	    cfg.spi_3wire  = false;        // 如果用MOSI引脚进行接收，则设置为true
	    cfg.use_lock   = true;        //如果使用交易锁则设置为true
	    cfg.dma_channel = SPI_DMA_CH_AUTO; // 设置要使用的DMA通道（0=不使用DMA/1=1ch/2=ch/SPI_DMA_CH_AUTO=auto设置）。
	    // *随着ESP-IDF版本的升级，现在推荐使用SPI_DMA_CH_AUTO（自动设置）作为DMA通道，1ch和2ch被弃用。
	    cfg.pin_sclk = P_SCL;            // 设置SPI SCLK引脚编号
	    cfg.pin_mosi = P_SDA;            // 设置SPI的MOSI引脚编号
	    cfg.pin_miso = -1;            // 设置SPI的MISO针脚编号（-1 = 禁用）。
	    cfg.pin_dc   = P_DC;            // 设置SPI的D/C针脚编号（-1 = 禁用）。
	   // 当与SD卡共同使用SPI总线时，必须无遗漏地设置MISO。

	    _bus_instance.config(cfg);    // //反映总线上的配置值。
	    _panel_instance.setBus(&_bus_instance);      /// 设置屏幕总线。
	  }

	  { // 配置显示面板控制设置。
	    auto cfg = _panel_instance.config();    // 获取屏幕配置的结构。。

	    cfg.pin_cs           =    P_CS;  // 连接CS的引脚编号（-1 = 禁用）。
	    cfg.pin_rst          =    P_RST;  // 连接RST的引脚编号 (-1 = 禁用)
	    cfg.pin_busy         =    -1;  // 连接BUSY的引脚编号 (-1 = 禁用)

	      // * 下面的设置对每个面板都有一般的默认值，如果你对某个项目不确定，可以把它注释出来并试一试。

	    // cfg.panel_width      =   240;  // 实际可显示的宽度
	    cfg.panel_height     =   240;  // 实际可显示的高度
	    cfg.offset_x         =     0;  // 在屏幕的X方向上的偏移量
	    cfg.offset_y         =     0;  // 在屏幕的Y方向上的偏移量
	    cfg.offset_rotation  =     0;  // 旋转方向的偏移量为0~7（4~7为倒置）。
	    // cfg.dummy_read_pixel =     8;  // 读取像素前的假读位数量
	    cfg.dummy_read_bits  =     1;  // 读取非像素数据前的虚拟读取位数
	    cfg.readable         =  false;  // 如果可以读取数据，则设置为true。
	    cfg.invert           = true;   // 设定 是否反色，有些屏幕需要设置这个值才能获取正确的颜色
	    cfg.rgb_order        = false;  // true 为 RGB false 为 BGR
	    cfg.dlen_16bit       = false;  // 如果面板在16位并行或SPI中以16位单位传输数据长度，则设置为true。
	    cfg.bus_shared       = false;  // SDカー如果与SD卡共享总线，则设置为true（总线控制由drawJpgFile等执行）。
	    _panel_instance.config(cfg);
	  }
	  {
		auto BLcfg=_light_instance.config();
		BLcfg.pin_bl=P_BL;
		BLcfg.freq=10000;
		BLcfg.pwm_channel=1;
		_light_instance.config(BLcfg);
	  }
	  //_panel_instance.light(_light_instance);

	  setPanel(&_panel_instance); // 设置要使用的面板。
	}
using namespace lgfx;
using namespace lgfx::v1;
IRAM_ATTR void Panel_SpriteMX::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
	//Serial.println("IM");
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); }
    }
    auto bits = _write_bits;
    uint32_t index = x + y * _bitwidth;
    if (bits >= 8)
    {
      if (bits == 8)
      {
        if(bXOR){
			// if(_img.img8()[index])
			_img.img8()[index] ^= rawcolor & 0xFF;
		}else _img.img8()[index] = rawcolor;
      }
      else if (bits == 16)
      {
        if(bXOR){
			// if(_img.img8()[index])
			_img.img16()[index] ^= rawcolor & 0xFFFF;
		}else _img.img16()[index] = rawcolor;
      }
      else
      {
        if(bXOR){
			// if(_img.img8()[index])
			_img.img24()[index] = (uint32_t)_img.img24()[index] ^ rawcolor;
		}else _img.img24()[index] = rawcolor;
      }
    }
    else
    {
      index *= bits;
      uint8_t* dst = &_img.img8()[index >> 3];
      uint8_t mask = (uint8_t)(~(0xFF >> bits)) >> (index & 7);
	  // Msk: 0 0 0 1 1 1 1 1 [ TC 1 1 1 0 0 0 0 0
	  // NMK: 1 1 1 0 0 0 0 0
	  // col: 0 0 0 1 1 0 1 0 [ TC 1 1 0 0 0 0 0 0

	  // so msk 0 0 1 1 1 1 1 1
	  // col    x x 1 1 0 1 1 0
	  //
	  if(bXOR) *dst = (*dst & ~mask) ^ (rawcolor & mask);
	  else     *dst = (*dst & ~mask) | (rawcolor & mask);
    }
  }

IRAM_ATTR void Panel_SpriteMX::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
	for(uint8_t j=y;j<y+h;++j){
		for(uint8_t i=x;i<x+w;++i){
			drawPixelPreclipped(i,j,rawcolor);
		}
	}
	return;
	/*
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + h); }
      if (r & 2)                  { x = _width  - (x + w); }
      if (r & 1) { std::swap(x, y);  std::swap(w, h); }
    }

    uint_fast8_t bits = _write_bits;
    if (bits >= 8)
    {
      if (w > 1)
      {
        uint_fast8_t bytes = bits >> 3;
        uint_fast16_t bw = _bitwidth;
        uint8_t* dst = &_img[(x + y * bw) * bytes];
        uint8_t* src = dst;
        uint_fast16_t add_dst = bw * bytes;
        uint_fast32_t len = w * bytes;
        uint_fast32_t w32 = w;

        if (_img.use_memcpy())
        {
          if (w32 != bw)
          {
            dst += add_dst;
          }
          else
          {
            w32 *= h;
            h = 1;
          }
        }
        else
        {
          src = (uint8_t*)alloca(len);
          ++h;
        }
        lgfx::memset_multi(src, rawcolor, bytes, w32);
        while (--h)
        {
          memcpy(dst, src, len);
          dst += add_dst;
        }
      }
      else
      {
        uint_fast16_t bw = _bitwidth;
        uint32_t index = x + y * bw;
        if (bits == 8)
        {
          auto img = &_img[index];
          do { *img = rawcolor;  img += bw; } while (--h);
        }
        else if (bits == 16)
        {
          auto img = &_img.img16()[index];
          do { *img = rawcolor;  img += bw; } while (--h);
        }
        else if (bits == 32)
        {
          auto img = &_img.img32()[index];
          do { *img = rawcolor; img += bw; } while (--h);
        }
        else
        {
          auto img = &_img.img24()[index];
          do { *img = rawcolor; img += bw; } while (--h);
        }
      }
    }
    else
    {
      x *= bits;
      w *= bits;
      uint32_t add_dst = _bitwidth * bits >> 3;
      uint8_t* dst = &_img[y * add_dst + (x >> 3)];
      uint32_t len = ((x + w) >> 3) - (x >> 3);
      uint8_t mask = 0xFF >> (x & 7);
      if (len)
      {
        if (mask != 0xFF)
        {
          --len;
          auto d = dst++;
          uint8_t mc = rawcolor & mask;
          auto i = h;
          do { *d = (*d & ~mask) | mc; d += add_dst; } while (--i);
        }
        mask = ~(0xFF>>((x + w) & 7));
        if (len)
        {
          auto d = dst;
          auto i = h;
          do { memset(d, rawcolor, len); d += add_dst; } while (--i);
          dst += len;
        }
        if (mask == 0) return;
      }
      else
      {
        mask ^= mask >> w;
      }
      rawcolor &= mask;
      do { *dst = (*dst & ~mask) | rawcolor; dst += add_dst; } while (--h);
    }
	*/
}
