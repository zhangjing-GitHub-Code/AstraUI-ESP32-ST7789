#include <Arduino_GFX_Library.h>
#include "AGFX-CM-Enhance.h"

/** /
Arduino_Canvas_Mono_Enhanced::Arduino_Canvas_Mono_Enhanced(){
	Arduino_Canvas_Mono::Arduino_Canvas_Mono(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, bool verticalByte)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y), _verticalByte(verticalByte),
      _canvas_width(w), _canvas_height(h)
}
*/

void Arduino_Canvas_Idx_Enhanced::setXORdraw(bool isXOR){
	_doXORdraw=isXOR;
	// Serial.printf("Setting XOR to %d\n",isXOR);
}
void Arduino_Canvas_Idx_Enhanced::setColorCheck(uint8_t *fb,uint8_t idx){
	if(!_doXORdraw){
		*fb=idx;
		return;
	}
	// Serial.printf("FB %u chk\n",*fb);
	// uint16_t currcolor=_color_index[*fb];//get_index_color(uint8_t(*fb));
	if(_color_index[*fb]==WHITE){
		// WHITE on WHITE or BLACK ON BLACK
		*fb=idx_black;// get_color_index(BLACK);
	}else{
		*fb=idx_white;// get_color_index(RED);
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
void Arduino_Canvas_Idx_Enhanced::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

  uint8_t *fb = _framebuffer;
  switch (_rotation)
  {
  case 1:
    fb += (int32_t)x * _height;
    fb += _max_y - y;
    setColorCheck(fb,idx);
    break;
  case 2:
    fb += (int32_t)(_max_y - y) * _width;
    fb += _max_x - x;
    setColorCheck(fb,idx);
    break;
  case 3:
    fb += (int32_t)(_max_x - x) * _height;
    fb += y;
    setColorCheck(fb,idx);
    break;
  default: // case 0:
    fb += (int32_t)y * _width;
    fb += x;
    setColorCheck(fb,idx);
  }
}

void Arduino_Canvas_Idx_Enhanced::writeFillRectPreclipped(int16_t x, int16_t y,
                                                     int16_t w, int16_t h, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

  if (_rotation > 0)
  {
    int16_t t = x;
    switch (_rotation)
    {
    case 1:
      x = WIDTH - y - h;
      y = t;
      t = w;
      w = h;
      h = t;
      break;
    case 2:
      x = WIDTH - x - w;
      y = HEIGHT - y - h;
      break;
    case 3:
      x = y;
      y = HEIGHT - t - w;
      t = w;
      w = h;
      h = t;
      break;
    }
  }
  // log_i("adjusted writeFillRectPreclipped(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
  uint8_t *row = _framebuffer;
  row += y * WIDTH;
  row += x;
  for (int j = 0; j < h; j++)
  {
    for (int i = 0; i < w; i++)
    {
      //row[i]=idx;
      setColorCheck(&row[i],idx);
    }
    row += WIDTH;
  }
}

void Arduino_Canvas_Idx_Enhanced::writeFastVLineCore(int16_t x, int16_t y,
                                                int16_t h, uint8_t idx)
{
  if (_ordered_in_range(x, 0, MAX_X) && h)
  { // X on screen, nonzero height
    if (h < 0)
    {             // If negative height...
      y += h + 1; //   Move Y to top edge
      h = -h;     //   Use positive height
    }
    if (y <= MAX_Y)
    { // Not off bottom
      int16_t y2 = y + h - 1;
      if (y2 >= 0)
      { // Not off top
        // Line partly or fully overlaps screen
        if (y < 0)
        {
          y = 0;
          h = y2 + 1;
        } // Clip top
        if (y2 > MAX_Y)
        {
          h = MAX_Y - y + 1;
        } // Clip bottom

        uint8_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (h--)
        {
          setColorCheck(fb,idx);
          fb += WIDTH;
        }
      }
    }
  }
}
void Arduino_Canvas_Idx_Enhanced::writeFastHLineCore(int16_t x, int16_t y,
                                                int16_t w, uint8_t idx)
{
  if (_ordered_in_range(y, 0, MAX_Y) && w)
  { // Y on screen, nonzero width
    if (w < 0)
    {             // If negative width...
      x += w + 1; //   Move X to left edge
      w = -w;     //   Use positive width
    }
    if (x <= MAX_X)
    { // Not off right
      int16_t x2 = x + w - 1;
      if (x2 >= 0)
      { // Not off left
        // Line partly or fully overlaps screen
        if (x < 0)
        {
          x = 0;
          w = x2 + 1;
        } // Clip left
        if (x2 > MAX_X)
        {
          w = MAX_X - x + 1;
        } // Clip right

        uint8_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (w--)
        {
          setColorCheck(fb++,idx);
        }
      }
    }
  }
}
// void Arduino_Canvas_Idx_Enhanced::setDrawMode(uint8_t mode){
	// _drawMode=(eDrawMode)mode;
// }