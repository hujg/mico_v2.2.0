
/***

History:
2015-09-11: Ted: Create

*/


#include <Object.h>
#include <string.h>
#include <ObjLights.h>


// Object: Lights-1
const char ObjLights[] = "Lights-1";
const char DistLights[] = "lights";


void ObjLightsInit()
{
    ObjectCreate(ObjLights, strlen(ObjLights));
    ObjectAddDist(ObjLights, strlen(ObjLights), DistLights, strlen(DistLights), EDistType_Data);
}

ObjectErr_t GetLightsValue(u16* value)
{
    return ObjectGetValue(ObjLights, strlen(ObjLights), DistLights, strlen(DistLights), value);
}

ObjectErr_t SetLightsValue(u16 value)
{
    return ObjectSetValue(ObjLights, strlen(ObjLights), DistLights, strlen(DistLights), value);
}


// end of file