
/***

History:
2015-09-11: Ted: Create

*/

#ifndef _OBJMUSIC_H
#define _OBJMUSIC_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include <Object_int.h>


void ObjMusicInit();
ObjectErr_t GetTrackString(char* str);
ObjectErr_t SetTrackString(char* str);
ObjectErr_t GetUrlPathString(char* str);
ObjectErr_t SetUrlPathString(char* str);
ObjectErr_t GetVolumeValue(u16* value);
ObjectErr_t SetVolumeValue(u16 value);


#ifdef __cplusplus
}
#endif

#endif // _OBJMUSIC_H

// end of file