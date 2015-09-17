
/***

History:
2015-09-11: Ted: Create

*/


#include <Object.h>
#include <string.h>
#include <ObjDevice.h>


// Object: Device-1
const char ObjDevice[] = "Device-1";
const char DistEnergy[] = "energy";
const char DistInterval[] = "interval";
const char DistRemind[] = "remind";


void ObjDeviceInit()
{
    ObjectCreate(ObjDevice);
    ObjectAddDist(ObjDevice, DistEnergy, EDistType_Data);
    ObjectAddDist(ObjDevice, DistInterval, EDistType_Data);
    ObjectAddDist(ObjDevice, DistRemind, EDistType_Data);
    
    SetEnergyValue(50);
    SetIntervalValue(30);
    SetRemindValue(10);
}

OSStatus GetEnergyValue(u16* value)
{
    return ObjectGetValue(ObjDevice, DistEnergy, value);
}

OSStatus SetEnergyValue(u16 value)
{
    return ObjectSetValue(ObjDevice, DistEnergy, value);
}

OSStatus GetIntervalValue(u16* value)
{
    return ObjectGetValue(ObjDevice, DistInterval, value);
}

OSStatus SetIntervalValue(u16 value)
{
    return ObjectSetValue(ObjDevice, DistInterval, value);
}

OSStatus GetRemindValue(u16* value)
{
    return ObjectGetValue(ObjDevice, DistRemind, value);
}

OSStatus SetRemindValue(u16 value)
{
    return ObjectSetValue(ObjDevice, DistRemind, value);
}



// end of file