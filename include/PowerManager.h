#include <GyverPower.h>

class PowerManager
{
public:
    PowerManager();
    void deepSleep(int seconds);
    void wakeUp();

    void changeSensorsPower(bool enable);
    void changeGsmPower(bool enable);

    bool isSensorsEnabled = false;
    bool isGsmEnabled = false;
};