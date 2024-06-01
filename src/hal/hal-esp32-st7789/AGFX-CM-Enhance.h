#pragma once
#include <Arduino_GFX_Library.h>
#define ZONE_SPLIT 16
const int ZONE_UNIT=240/ZONE_SPLIT;
class ACME:public Arduino_Canvas_Mono{
	protected:
	enum eDrawMode{
		NORMAL,
		INVERSE,
		XOR
	};
	size_t fbsz;
	volatile bool _callSet=false;
	uint8_t *_fb_ref;
	eDrawMode _drawMode=NORMAL;
	public:
	unsigned long calc_start,flush_dur;
	volatile bool _doXORdraw=false,_recordMOD=true,_isFillScr=0;
	uint16_t _colC=0;
	public:
	bool _modified[ZONE_SPLIT+2]={0};
	bool _modifiedPrev[ZONE_SPLIT+2]={0};
	public:
	ACME(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y)
    : Arduino_Canvas_Mono(w, h,output,output_x,output_y){
		//idx_black=get_color_index(BLACK);
		//idx_white=get_color_index(WHITE);
		//Serial.printf("SET B:%d RB%d W:%d,RW%d\n",idx_black,get_color_index(BLACK),idx_white,get_color_index(WHITE));
	};
	~ACME(){
		if (_framebuffer){
			free(_framebuffer);
		}
	}
	bool begin(int32_t speed);
	void copyFBref();
	// void _setBufTo(int32_t pos,uint16_t x,uint16_t y);
	void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
	//void writeFastVLineCore(int16_t x,int16_t y,int16_t h,uint8_t idx);
	//void writeFastHLineCore(int16_t x, int16_t y, int16_t w, uint8_t idx);
	//void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	// void drawMonoBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
	// void drawMonoBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip = 0) override;
	void setColorCheck(uint8_t *fb, uint8_t idx);
	void setXORdraw(bool isXOR);
	void normFillScr(uint16_t color);
	void flush() override;
	void flushXTaskRecv(void *taskParams);
	void flushXtask();
	void dumpDrawArgs();
};