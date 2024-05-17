#include <Arduino_GFX_Library.h>
#include "AGFX-CM-Enhance.h"

/** /
Arduino_Canvas_Mono_Enhanced::Arduino_Canvas_Mono_Enhanced(){
	Arduino_Canvas_Mono::Arduino_Canvas_Mono(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, bool verticalByte)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y), _verticalByte(verticalByte),
      _canvas_width(w), _canvas_height(h)
}
*/

void Arduino_Canvas_Mono_Enhanced::_setBufTo(int32_t pos,uint16_t x,uint16_t y){
	if(_drawMode==NORMAL){
		// draw WHITE
		//                                    vertical            horiz
		_framebuffer[pos]|=_verticalByte ? (0x80 >> (x & 7)) : (1 << (y & 7));
	}else if(_drawMode==INVERSE){
		// draw BLACK
		//                                     vertical              horiz
		_framebuffer[pos]&=_verticalByte ? ~(0x80 >> (x & 7)) : ~(1 << (y & 7));
		
	}else if(_drawMode==XOR){
		// Draw XORly
		// ?^0=? ?^1=rev ?
		//                                    vertical            horiz
		_framebuffer[pos]^=_verticalByte ? (0x80 >> (x & 7)) : (1 << (y & 7));
	}
}

/*

void Arduino_Canvas_Mono::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  // change the pixel in the original orientation of the bitmap buffer
  if (_verticalByte)
  {
    // vertical buffer layout: 1 byte in the buffer contains 8 vertical pixels
    int32_t pos = x + (y / 8) * _canvas_width;

    if (color & 0b1000010000010000)
    {
      _framebuffer[pos] |= (1 << (y & 7));
    }
    else
    {
      _framebuffer[pos] &= ~(1 << (y & 7));
    }
  }
  else
  {
    // horizontal buffer layout: 1 byte in the buffer contains 8 horizontal pixels
    int16_t w = (_canvas_width + 7) / 8;
    int32_t pos = (y * w) + (x / 8);

    if (color & 0b1000010000010000)
    {
      _framebuffer[pos] |= 0x80 >> (x & 7);
    }
    else
    {
      _framebuffer[pos] &= ~(0x80 >> (x & 7));
    }
  }
}
*/

// Ignoring color arg
void Arduino_Canvas_Mono_Enhanced::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  // change the pixel in the original orientation of the bitmap buffer
  if (_verticalByte){
    // vertical buffer layout: 1 byte in the buffer contains 8 vertical pixels
    int32_t pos = x + (y / 8) * _canvas_width;
    _setBufTo(pos,x,y);
  }else{
    // horizontal buffer layout: 1 byte in the buffer contains 8 horizontal pixels
    int16_t w = (_canvas_width + 7) / 8;
    int32_t pos = (y * w) + (x / 8);
    _setBufTo(pos,x,y);
  }
}
void Arduino_Canvas_Mono_Enhanced::setDrawMode(uint8_t mode){
	_drawMode=(eDrawMode)mode;
}