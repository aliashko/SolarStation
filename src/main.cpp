#include <Arduino.h>
#include <LifecycleManager.h>
#include "GyverWDT.h"

#define DEBUG

LifecycleManager* lifecycleManager;
#ifdef DEBUG
// Переменные, создаваемые процессом сборки,
// когда компилируется скетч
extern int __bss_end;
extern void *__brkval;
 
// Функция, возвращающая количество свободного ОЗУ (RAM)
int memoryFree()
{
   int freeValue;
   if((int)__brkval == 0)
      freeValue = ((int)&freeValue) - ((int)&__bss_end);
   else
      freeValue = ((int)&freeValue) - ((int)__brkval);
   return freeValue;
}
#endif

void setup() {
   #ifdef DEBUG
   Serial.begin(9600);
   Serial.println("START");
   #endif
   lifecycleManager = new LifecycleManager();
   Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024);
}

void loop() {  
   #ifdef DEBUG
   Serial.print("Free memory: ");Serial.println(memoryFree());
   #endif
	lifecycleManager->iterate();
   Watchdog.reset();
}

void safeDelay(unsigned int ms){
	for(unsigned int i = 0; i < ms; i += 1000){
		delay(ms - i > 1000 ? 1000 : (ms - i));
		Watchdog.reset();
	}
}