
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
ObjectErr_t GetEnergyValue(u16* value);
ObjectErr_t SetEnergyValue(u16 value);
ObjectErr_t GetIntervalValue(u16* value);
ObjectErr_t SetIntervalValue(u16 value);
ObjectErr_t GetRemindValue(u16* value);
ObjectErr_t SetRemindValue(u16 value);


#ifdef __cplusplus
}
#endif

#endif // _OBJDEVICE_H

// end of file