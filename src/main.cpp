#include "astra_rocket.h"
// #include "astra_rocket.cpp"
void setup(){
	astraCoreInit();
	// l64a -> l6a wronttyp
	Serial.println("Init Done");
	astraCoreStart();
	Serial.println("Exit start?");
	//ESPHAL *h=new ESPHAL();
	//h->init();
}
bool fuck=1;
void loop(){
	astraCoreStart();
	if(fuck){
		Serial.println("FUCK");
		fuck=0;
		return;
	}
}