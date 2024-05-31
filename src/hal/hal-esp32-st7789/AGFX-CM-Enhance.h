#pragma once
#include <Arduino_GFX_Library.h>
#define ZONE_SPLIT 16
const int ZONE_UNIT=240/ZONE_SPLIT;
class Arduino_Canvas_Idx_Enhanced:public Arduino_Canvas_Indexed{
	private:
	enum eDrawMode{
		NORMAL,
		INVERSE,
		XOR
	};
	eDrawMode _drawMode=NORMAL;
	bool _doXORdraw=false;
	uint8_t idx_white,idx_black;
	bool _modified[ZONE_SPLIT+2]={0};
	bool _modifiedPrev[ZONE_SPLIT+2]={0};
	public:
	Arduino_Canvas_Idx_Enhanced(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, bool verticalByte=0)
    : Arduino_Canvas_Indexed(w, h,output,output_x,output_y){
		idx_black=get_color_index(BLACK);
		idx_white=get_color_index(WHITE);
	};
	~Arduino_Canvas_Idx_Enhanced(){
		if (_framebuffer){
			free(_framebuffer);
		}
	}
	// void _setBufTo(int32_t pos,uint16_t x,uint16_t y);
	void writePixelPreclipped(int16_t x, int16_t y, uint16_t color);
	void writeFastVLineCore(int16_t x,int16_t y,int16_t h,uint8_t idx);
	void writeFastHLineCore(int16_t x, int16_t y, int16_t w, uint8_t idx);
	void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
	void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip = 0) override;
	void setColorCheck(uint8_t *fb, uint8_t idx);
	void setXORdraw(bool isXOR);
	void normFillScr(uint16_t color);
	void flush() override;
	void flushXTaskRecv(void *taskParams);
	void flushXtask();
};