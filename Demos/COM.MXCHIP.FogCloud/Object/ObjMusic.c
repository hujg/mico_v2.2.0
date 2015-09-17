
/***

History:
2015-09-11: Ted: Create

*/

#include <Object.h>
#include <string.h>
#include <ObjMusic.h>


// Object: Music-1
char ObjMusic[] = "Music-1";
char DistTrack[] = "track";
char DistUrlPath[] = "urlPath";
char DistVolume[] = "volume";


void ObjMusicInit()
{
    ObjectCreate(ObjMusic);
    ObjectAddDist(ObjMusic, DistTrack, EDistType_String);
    ObjectAddDist(ObjMusic, DistUrlPath, EDistType_String);
    ObjectAddDist(ObjMusic, DistVolume, EDistType_Data);
    
    SetVolumeValue(20);
}

OSStatus GetTrackString(char* str)
{
    return ObjectGetString(ObjMusic, DistTrack, str);
}

OSStatus SetTrackString(char* str)
{
    return ObjectSetString(ObjMusic, DistTrack, str);
}

OSStatus GetUrlPathString(char* str)
{
    return ObjectGetString(ObjMusic, DistUrlPath, str);
}

OSStatus SetUrlPathString(char* str)
{
    return ObjectSetString(ObjMusic, DistUrlPath, str);
}

OSStatus GetVolumeValue(u16* value)
{
    return ObjectGetValue(ObjMusic, DistVolume, value);
}

OSStatus SetVolumeValue(u16 value)
{
    return ObjectSetValue(ObjMusic, DistVolume, value);
}



// end of file


