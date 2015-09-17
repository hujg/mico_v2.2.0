
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
    ObjectCreate(ObjLights);
    ObjectAddDist(ObjLights, DistLights, EDistType_Data);
    
    SetLightsValue(1);
}

OSStatus GetLightsValue(u16* value)
{
    return ObjectGetValue(ObjLights, DistLights, value);
}

OSStatus SetLightsValue(u16 value)
{
    return ObjectSetValue(ObjLights, DistLights, value);
}


// end of file