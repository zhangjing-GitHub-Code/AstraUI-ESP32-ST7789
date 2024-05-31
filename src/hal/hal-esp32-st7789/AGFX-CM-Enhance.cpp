#include <Arduino_GFX_Library.h>
#include "AGFX-CM-Enhance.h"
#include "mutex.h"

#define NO_GDB 0

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
	_callSet=1;
	__asm__ ("":::"memory");
	if(!_doXORdraw){
		*fb=idx;
		return;
	}
	//Serial.printf("SC ");
	//if(_isFillScr){
	//	*fb=idx;
	//	return;
	//}
	/*
	_colC++;
	if(_colC>2000){
		_colC=0;
		if(NO_GDB)Serial.printf("SCL %d@%x\n",idx,fb);
	}
	*/
	// Serial.printf("FB %u chk\n",*fb);
	// uint16_t currcolor=_color_index[*fb];//get_index_color(uint8_t(*fb));
	if(_color_index[*fb]){
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
	if(_recordMOD)_modified[y/ZONE_UNIT]=1;
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
// Usages: font, middle of RBOX, FillRect
void Arduino_Canvas_Idx_Enhanced::writeFillRectPreclipped(int16_t x, int16_t y,
                                                     int16_t w, int16_t h, uint16_t color)
{
	//if((!_isFillScr)&&(w>50&&h>10))return;
	_callSet=0;
	// Serial.printf("FRP %d+%d,%d+%d\n",x,w,y,h);
	if(_recordMOD)for(int ty=y;ty<=y+h;ty+=ZONE_UNIT-(ty%ZONE_UNIT))_modified[ty/ZONE_UNIT]=1;
	__asm__ ("":::"memory");
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
  __asm__ ("":::"memory");
  // if(!_callSet)Serial.printf("NC %d+%d,%d+%d\n",x,w,y,h);
}

void Arduino_Canvas_Idx_Enhanced::writeFastVLineCore(int16_t x, int16_t y,
                                                int16_t h, uint8_t idx)
{
	__asm__ ("":::"memory");
	if(_recordMOD)for(int ty=y;ty<=y+h;ty+=ZONE_UNIT-(ty%ZONE_UNIT))_modified[ty/ZONE_UNIT]=1;
	if(_recordMOD)_modified[y/ZONE_UNIT]=1;
	//Serial.printf("ovr VL %d,%d+%d\n",x,y,h);
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
	__asm__ ("":::"memory");
  if(_recordMOD)_modified[y/ZONE_UNIT]=1;
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
          setColorCheck(fb,idx);
		  fb++;
        }
      }
    }
  }
}
// void Arduino_Canvas_Idx_Enhanced::setDrawMode(uint8_t mode){
	// _drawMode=(eDrawMode)mode;
// }
void Arduino_Canvas_Idx_Enhanced::normFillScr(uint16_t color){
	bool tmp=_doXORdraw;
	//bool tmod[ZONE_SPLIT+2];
	//for(uint8_t i=0;i<ZONE_SPLIT;++i)tmod[i]=_modified[i];
	_recordMOD=0;
	_doXORdraw=0;
	_isFillScr=1;
	__asm__ ("":::"memory");
	fillScreen(color);
	_recordMOD=1;
	//for(uint8_t i=0;i<ZONE_SPLIT;++i)_modified[i]=tmod[i];
	_doXORdraw=tmp;
	_isFillScr=0;
}

void Arduino_Canvas_Idx_Enhanced::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
	if(_recordMOD)for(int ty=y;ty<=y+h;ty+=ZONE_UNIT-(ty%ZONE_UNIT))_modified[ty/ZONE_UNIT]=1;
	if(_recordMOD)_modified[y/ZONE_UNIT]=1;
	//        w
	// x,y ******
	// h *
	//   *
  if (_rotation > 0)
  {
    if (!_isDirectUseColorIndex)
    {
      Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, w, h, x_skip);
    }
    else
    {
      int32_t offset = 0;
      for (int16_t j = 0; j < h; j++, y++)
      {
        for (int16_t i = 0; i < w; i++)
        {
          writePixel(x + i, y, bitmap[offset++]);
        }
        offset += x_skip;
      }
    }
  }
  else
  {
    if (
        ((x + w - 1) < 0) || // Outside left
        ((y + h - 1) < 0) || // Outside top
        (x > _max_x) ||      // Outside right
        (y > _max_y)         // Outside bottom
    )
    {
      return;
    }
    else
    {
      if ((y + h - 1) > _max_y)
      {
        h -= (y + h - 1) - _max_y;
      }
      if (y < 0)
      {
        bitmap -= y * (w + x_skip);
        h += y;
        y = 0;
      }
      if ((x + w - 1) > _max_x)
      {
        x_skip += (x + w - 1) - _max_x;
        w -= (x + w - 1) - _max_x;
      }
      if (x < 0)
      {
        bitmap -= x;
        x_skip -= x;
        w += x;
        x = 0;
      }
      uint8_t *row = _framebuffer;
      row += y * _width;
      row += x;
      int16_t i;
      int16_t wi;
      if (_isDirectUseColorIndex)
      {
        while (h--)
        {
          i = 0;
          wi = w;
          while (wi >= 4)
          {
            *((uint32_t *)&row[i]) = *((uint32_t *)bitmap);
            i += 4;
            wi -= 4;
            bitmap += 4;
          }
          while (i < w)
          {
            row[i++] = *bitmap++;
          }
          bitmap += x_skip;
          row += _width;
        }
      }
      else
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            setColorCheck(row+i,get_color_index(color_index[*bitmap]));
			bitmap++;
          }
          bitmap += x_skip;
          row += _width;
        }
      }
    }
  }
}

void Arduino_Canvas_Idx_Enhanced::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip)
{
	if(_recordMOD)for(int ty=y;ty<=y+h;ty+=ZONE_UNIT-(ty%ZONE_UNIT))_modified[ty/ZONE_UNIT]=1;
  if (_rotation > 0)
  {
    if (!_isDirectUseColorIndex)
    {
      Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, chroma_key, w, h, x_skip);
    }
    else
    {
      int32_t offset = 0;
      uint8_t color_key;
      for (int16_t j = 0; j < h; j++, y++)
      {
        for (int16_t i = 0; i < w; i++)
        {
          color_key = bitmap[offset++];
          if (color_key != chroma_key)
          {
            writePixel(x + i, y, color_key);
          }
        }
        offset += x_skip;
      }
    }
  }
  else
  {
    if (
        ((x + w - 1) < 0) || // Outside left
        ((y + h - 1) < 0) || // Outside top
        (x > _max_x) ||      // Outside right
        (y > _max_y)         // Outside bottom
    )
    {
      return;
    }
    else
    {
      if ((y + h - 1) > _max_y)
      {
        h -= (y + h - 1) - _max_y;
      }
      if (y < 0)
      {
        bitmap -= y * (w + x_skip);
        h += y;
        y = 0;
      }
      if ((x + w - 1) > _max_x)
      {
        x_skip += (x + w - 1) - _max_x;
        w -= (x + w - 1) - _max_x;
      }
      if (x < 0)
      {
        bitmap -= x;
        x_skip -= x;
        w += x;
        x = 0;
      }
      uint8_t *row = _framebuffer;
      row += y * _width;
      row += x;
      uint8_t color_key;
      if (_isDirectUseColorIndex)
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            color_key = *bitmap++;
            if (color_key != chroma_key)
            {
              setColorCheck(row+i,color_key);
            }
          }
          bitmap += x_skip;
          row += _width;
        }
      }
      else
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            color_key = *bitmap++;
            if (color_key != chroma_key)
            {
              setColorCheck(row+i,get_color_index(color_index[color_key]));
            }
          }
          bitmap += x_skip;
          row += _width;
        }
      }
    }
  }
}
void Arduino_Canvas_Idx_Enhanced::flush()
{
	// Prev 1 This 0 -> this keep0 but update (Update to black)
	// This 1 -> update
	// Prev 0 This 0 -> skip
	__asm__ ("":::"memory");
  int perfCnt=0,sTime=::millis();
  for(int zidx=0;zidx<ZONE_SPLIT;++zidx){
	uint8_t tm=_modified[zidx];
     if( tm ||
         (_modifiedPrev[zidx]&&!tm)
     ){
      // ++perfCnt;
      _modified[zidx]=0;
      _output->drawIndexedBitmap(_output_x, _output_y+zidx*ZONE_UNIT, _framebuffer+240*zidx*15, _color_index, WIDTH, ZONE_UNIT);
     }
	 _modifiedPrev[zidx]=tm;
  }
  // memset(_modified,0,sizeof(_modified));
  sTime=millis()-sTime;
  if(perfCnt>10){
    // Serial.printf("Flush %d chunk %d mil\n",perfCnt,sTime);
  }
}
void Arduino_Canvas_Idx_Enhanced::flushXTaskRecv(void *taskParams){
	this->flush();
}
void Arduino_Canvas_Idx_Enhanced::flushXtask(){
	this->flush();
}
void Arduino_Canvas_Idx_Enhanced::dumpDrawArgs(){
	Serial.printf("doXOR: %d\nWHi: %d; BLi: %d\nCalled: %d;rec: %d\n",_doXORdraw,idx_white,idx_black,_callSet,_recordMOD);
}