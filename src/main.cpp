#include <Arduino.h>
#include <LifecycleManager.h>

LifecycleManager* lifecycleManager;

void setup() {
    lifecycleManager = new LifecycleManager();
}

void loop() {
	lifecycleManager->iterate();
}