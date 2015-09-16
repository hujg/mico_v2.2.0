
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
    ObjectCreate(ObjDevice, strlen(ObjDevice));
    ObjectAddDist(ObjDevice, strlen(ObjDevice), DistEnergy, strlen(DistEnergy), EDistType_Data);
    ObjectAddDist(ObjDevice, strlen(ObjDevice), DistInterval, strlen(DistInterval), EDistType_Data);
    ObjectAddDist(ObjDevice, strlen(ObjDevice), DistRemind, strlen(DistRemind), EDistType_Data);
}

ObjectErr_t GetEnergyValue(u16* value)
{
    return ObjectGetValue(ObjDevice, strlen(ObjDevice), DistEnergy, strlen(DistEnergy), value);
}

ObjectErr_t SetEnergyValue(u16 value)
{
    return ObjectSetValue(ObjDevice, strlen(ObjDevice), DistEnergy, strlen(DistEnergy), value);
}

ObjectErr_t GetIntervalValue(u16* value)
{
    return ObjectGetValue(ObjDevice, strlen(ObjDevice), DistInterval, strlen(DistInterval), value);
}

ObjectErr_t SetIntervalValue(u16 value)
{
    return ObjectSetValue(ObjDevice, strlen(ObjDevice), DistInterval, strlen(DistInterval), value);
}

ObjectErr_t GetRemindValue(u16* value)
{
    return ObjectGetValue(ObjDevice, strlen(ObjDevice), DistRemind, strlen(DistRemind), value);
}

ObjectErr_t SetRemindValue(u16 value)
{
    return ObjectSetValue(ObjDevice, strlen(ObjDevice), DistRemind, strlen(DistRemind), value);
}



// end of file