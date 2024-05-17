#include <Arduino_GFX_Library.h>
class Arduino_Canvas_Mono_Enhanced:public Arduino_Canvas_Mono{
	private:
	enum eDrawMode{
		NORMAL,
		INVERSE,
		XOR
	};
	eDrawMode _drawMode=NORMAL;
	public:
	Arduino_Canvas_Mono_Enhanced(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, bool verticalByte=0)
    : Arduino_Canvas_Mono(w, h,output,output_x,output_y,verticalByte){};
	~Arduino_Canvas_Mono_Enhanced(){
		if (_framebuffer){
			free(_framebuffer);
		}
	}
	void _setBufTo(int32_t pos,uint16_t x,uint16_t y);
	void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
	void setDrawMode(uint8_t mode);
};