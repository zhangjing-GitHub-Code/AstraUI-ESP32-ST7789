#include "astra_rocket.h"
#include <esp_wifi.h>
// #include "esp-gdbstub/gdbstub.h"
// #include "astra_rocket.cpp"
void start(void *PR){
	astraCoreInit();
	// esp_wifi_80211_tx();
	// l64a -> l6a wronttyp
	// Serial.println("Init Done");
	astraCoreStart(PR);
	//astraCoreStart();
	// Serial.println("Exit start?");
	//ESPHAL *h=new ESPHAL();
	//h->init();
}
void setup(){
	//astraCoreInit();
	// l64a -> l6a wronttyp
	//Serial.println("Init Done");
	BaseType_t UItaskID=xTaskCreatePinnedToCore(start,"astraMain",240*240+1145,NULL,3,NULL,ARDUINO_RUNNING_CORE);
	// BaseType_t UItaskID=xTaskCreatePinnedToCore(start,"astraFLSH",240*11,NULL,3,NULL,ARDUINO_EVENT_RUNNING_CORE);
	//astraCoreStart();
	Serial.println("Created Main Task");
	// gdbstub_init();
	//ESPHAL *h=new ESPHAL();
	//h->init();
}
bool fuck=1;
void loop(){
	/*
	Serial.print("P27 t=");
	Serial.println(touchRead(27));
	Serial.print("P13 t=");
	Serial.println(touchRead(13));
	delay(500);
	return;
	*/
	//astraCoreStart();
	if(fuck){
		Serial.println("FUCK");
		fuck=0;
		return;
	}
}