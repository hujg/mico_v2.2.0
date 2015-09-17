
/***

History:
2015-09-11: Ted: Create

*/

#ifndef _OBJDEVICE_H
#define _OBJDEVICE_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include <Object_int.h>


void ObjDeviceInit();
OSStatus GetEnergyValue(u16* value);
OSStatus SetEnergyValue(u16 value);
OSStatus GetIntervalValue(u16* value);
OSStatus SetIntervalValue(u16 value);
OSStatus GetRemindValue(u16* value);
OSStatus SetRemindValue(u16 value);


#ifdef __cplusplus
}
#endif

#endif // _OBJDEVICE_H

// end of file