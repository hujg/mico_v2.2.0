
/***

History:
2015-09-11: Ted: Create

*/

#ifndef _OBJLIGHTS_H
#define _OBJLIGHTS_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include <Object_int.h>


void ObjLightsInit();
ObjectErr_t GetLightsValue(u16* value);
ObjectErr_t SetLightsValue(u16 value);


#ifdef __cplusplus
}
#endif

#endif // _OBJLIGHTS_H

// end of file