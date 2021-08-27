#include <Arduino.h>
#include <LifecycleManager.h>
#include "GyverWDT.h"

//#define DEBUG

LifecycleManager* lifecycleManager;

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

void setup() {
   #ifdef DEBUG
   Serial.begin(9600);
   Serial.println("START");
   #endif
   Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024);
   lifecycleManager = new LifecycleManager();
}

void loop() {  
   #ifdef DEBUG
   Serial.print("Free memory: ");Serial.println(memoryFree());
   #endif
	lifecycleManager->iterate();
   Watchdog.reset();
}