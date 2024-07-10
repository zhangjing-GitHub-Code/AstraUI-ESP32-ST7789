#include <Arduino_GFX_Library.h>
#include "AGFX-CM-Enhance.h"
#include "mutex.h"

#define NO_GDB 1

/** /
Arduino_Canvas_Mono_Enhanced::Arduino_Canvas_Mono_Enhanced(){
	Arduino_Canvas_Mono::Arduino_Canvas_Mono(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, bool verticalByte)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y), _verticalByte(verticalByte),
      _canvas_width(w), _canvas_height(h)
}
*/

//uint8_t ACME::idx_black=0;
//uint8_t ACME::idx_white=1;

// 回调实现，在 ISR 上下文中运行
static bool recv_DMAmemcpy(async_memcpy_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    xSemaphoreGiveFromISR(sem, &high_task_wakeup); // 如果解锁了一些高优先级任务，则将 high_task_wakeup 设置为 pdTRUE
    return high_task_wakeup == pdTRUE;
}


void IRAM_ATTR ACME::setXORdraw(bool isXOR){
	_doXORdraw=isXOR;
	// Serial.printf("Setting XOR to %d\n",isXOR);
}

void ACME::setColorCheck(uint8_t *fb,uint8_t idx){
	_callSet=1;
	// __asm__ ("":::"memory");
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
	//if((_color_index[*(uint8_t *)((int)fb-(int)_framebuffer+(int)_fb_ref)])&WHITE){
		// WHITE on WHITE or BLACK ON BLACK
	//	*fb=idx_black;// get_color_index(BLACK);
	//}else{
	//	*fb=idx_white;// get_color_index(RED);
	//}
}


bool ACME::begin(int32_t speed)
{
  if (speed != GFX_SKIP_OUTPUT_BEGIN)
  {
    if (!_output->begin(speed))
    {
      return false;
    }
  }

  if (!_framebuffer)
  {
    // size_t s;

    // allocate memory by full bytes.
    if (_verticalByte)
    {
      fbsz = _canvas_width * (_canvas_height + 7) / 8;
    }
    else
    {
      fbsz = (_canvas_width + 7) / 8 * _canvas_height;
    }

#if defined(ESP32)
    if (psramFound())
    {
      // _framebuffer = (uint8_t *)ps_malloc(fbsz);
      _fb0    = (uint8_t *)ps_malloc(fbsz);
      _fb1    = (uint8_t *)ps_malloc(fbsz);
	  _fb_ref = (uint8_t *)ps_malloc(fbsz);
    }
    else
    {
      _fb0    = (uint8_t *)malloc(fbsz);
      _fb1    = (uint8_t *)malloc(fbsz);
	  _fb_ref = (uint8_t *)malloc(fbsz);
    }
#else
    _framebuffer = (uint8_t *)malloc(s);
#endif
    if (!(_fb_ref&&_fb0&&_fb1))
    {
      return false;
    }
	memset(_fb0,0,fbsz);
	memset(_fb1,0,fbsz);
	_framebuffer=_fb0;
  }

  return true;
}
void IRAM_ATTR ACME::swapFB(){
	if(_useFB0){
		_framebuffer=_fb1;
	}else{
		_framebuffer=_fb0;
	}
	_useFB0^=1;
}
void IRAM_ATTR ACME::clearFrameBuffer(){
	if(_useFB0)memset(_fb0,0,fbsz);
	else memset(_fb1,0,fbsz);
}
void IRAM_ATTR ACME::copyFBref(){
	/*uint8_t *fb=_framebuffer;
	uint8_t *fbr=_fb_ref;
	uint8_t shif=0;
	memset(_fb_ref,0,_width*_height/8);
	for(uint8_t x=0;x<240;++x){
		for(uint8_t y=0;y<240;++y){
			if(_color_index[*fb]&WHITE){
				*fbr|=0b10000000>>shif;
			}
			if(++shif>7){
				shif=0;
				++fbr;
			}
			++fb;
		}
	}*/
	// memcpy(_fb_ref,_framebuffer,fbsz);
	memset(_fb_ref,0,fbsz);
// 创建一个信号量，在异步 memcpy 完成时进行报告
	//SemaphoreHandle_t semphr = xSemaphoreCreateBinary();

// 从用户的上下文中调用
// ESP_ERROR_CHECK(esp_async_memcpy(driver_handle, to, from, copy_len, my_async_memcpy_cb, my_semaphore));
// 其他事项
	//ESP_ERROR_CHECK(esp_async_memcpy(mutex::AMdri,_fb_ref,_framebuffer,fbsz,NULL,semphr));
	//xSemaphoreTake(semphr, portMAX_DELAY); // 等待 buffer 复制完成
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
void IRAM_ATTR ACME::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{

	if(_recordMOD)_modified[y/ZONE_UNIT]=1;
	//if(_recordMOD)_modified[y/ZONE_UNIT]=1;
	// if(x>230&&y>230)Serial.printf("Enter Pixel %d,%d<-%x\n",x,y,color);
  uint8_t idx;
  /*// change the pixel in the original orientation of the bitmap buffer
  if (_verticalByte) // = false, don't look
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
  {*/
    // horizontal buffer layout: 1 byte in the buffer contains 8 horizontal pixels
    int16_t w = (_canvas_width + 7) / 8;
    int32_t pos = (y * w) + (x / 8);
#define mask (0x80 >> (x & 7))
	if(_doXORdraw){
		if(_fb_ref[pos]&mask)return;
		_fb_ref[pos]|=mask;
		// if(_recordMOD)_modified[y/ZONE_UNIT]=1;
		_framebuffer[pos]= _framebuffer[pos]&mask ? _framebuffer[pos]& (~mask) : _framebuffer[pos]|mask ;
		/* replaced:
		if(_framebuffer[pos]&mask){
			_framebuffer[pos] &= ~mask;
		}else{
			// this is white
			_framebuffer[pos] |= mask;
		}
		*/
		return;
	}
	// Original Flavor
    if (	(color & 0b1000010000010000) // || // Draw White
			// (_doXORdraw&&!(_framebuffer[pos]&mask)) // XOR read black
	){
		// to draw a white
		
	// if(_recordMOD)_modified[y/ZONE_UNIT]=_framebuffer[pos]&mask?0:1;
      _framebuffer[pos] |= mask;
    }
    else // NOT white NOR XOR read white
    {
	// if(_recordMOD)_modified[y/ZONE_UNIT]=_framebuffer[pos]&mask?1:0;
      _framebuffer[pos] &= ~mask;
    }
  //}
}
void ACME::normFillScr(uint16_t color){
	bool tmp=_doXORdraw;
	//bool tmod[ZONE_SPLIT+2];
	//for(uint8_t i=0;i<ZONE_SPLIT;++i)tmod[i]=_modified[i];
	_recordMOD=_doXORdraw=0;
	_isFillScr=1;
	// __asm__ ("":::"memory");
	// fillScreen(color);
	clearFrameBuffer();
	_recordMOD=1;
	//for(uint8_t i=0;i<ZONE_SPLIT;++i)_modified[i]=tmod[i];
	_doXORdraw=tmp;
	_isFillScr=0;
}
/*
void ACME::drawMonoBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
	if(_recordMOD)for(int ty=y;ty<=y+h;ty+=ZONE_UNIT-(ty%ZONE_UNIT))_modified[ty/ZONE_UNIT]=1;
	if(_recordMOD)_modified[y/ZONE_UNIT]=1;
	//        w
	// x,y ******
	// h *
	//   *
}*/
void ACME::flush()
{
	// Prev 1 This 0 -> this keep0 but update (Update to black)
	// This 1 -> update
	// Prev 0 This 0 -> skip
	__asm__ ("":::"memory");
	//if (_output)
    //	_output->drawBitmap(_output_x, _output_y, _framebuffer, _canvas_width, _canvas_height, WHITE, BLACK);
	
  // int perfCnt=0,sTime=::millis();
  for(int zidx=0;zidx<ZONE_SPLIT;++zidx){
	uint8_t tm=_modified[zidx];
     if( tm ||
         (_modifiedPrev[zidx]&&!tm)
     ){
      // ++perfCnt;
      _modified[zidx]&=0;
      _output->drawBitmap(_output_x, _output_y+zidx*ZONE_UNIT, _framebuffer+240*zidx*15/8, _canvas_width,ZONE_UNIT,WHITE,BLACK);
     }
	 _modifiedPrev[zidx]=tm;
  }
  // memset(_modified,0,sizeof(_modified));
  // sTime=millis()-sTime;
  // if(perfCnt>10){
    // Serial.printf("Flush %d chunk %d mil\n",perfCnt,sTime);
  // }
}
void ACME::flushXTaskRecv(void *taskParams){
	this->flush();
}
void ACME::flushXtask(){
	this->flush();
}
void ACME::dumpDrawArgs(){
	// Serial.printf("doXOR: %d\nWHi: %d; BLi: %d\nCalled: %d;rec: %d\n",_doXORdraw,idx_white,idx_black,_callSet,_recordMOD);
}