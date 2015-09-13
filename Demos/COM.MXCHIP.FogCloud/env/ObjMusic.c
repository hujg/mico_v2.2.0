
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
    ObjectCreate(ObjMusic, strlen(ObjMusic));
    ObjectAddDist(ObjMusic, strlen(ObjMusic), DistTrack, strlen(DistTrack), EDistType_String);
    ObjectAddDist(ObjMusic, strlen(ObjMusic), DistUrlPath, strlen(DistUrlPath), EDistType_String);
    ObjectAddDist(ObjMusic, strlen(ObjMusic), DistVolume, strlen(DistVolume), EDistType_Data);
}

ObjectErr_t GetTrackString(char* str)
{
    return ObjectGetString(ObjMusic, strlen(ObjMusic), DistTrack, strlen(DistTrack), str);
}

ObjectErr_t SetTrackString(char* str)
{
    return ObjectSetString(ObjMusic, strlen(ObjMusic), DistTrack, strlen(DistTrack), str);
}

ObjectErr_t GetUrlPathString(char* str)
{
    return ObjectGetString(ObjMusic, strlen(ObjMusic), DistUrlPath, strlen(DistUrlPath), str);
}

ObjectErr_t SetUrlPathString(char* str)
{
    return ObjectSetString(ObjMusic, strlen(ObjMusic), DistUrlPath, strlen(DistUrlPath), str);
}

ObjectErr_t GetVolumeValue(u16* value)
{
    return ObjectGetValue(ObjMusic, strlen(ObjMusic), DistVolume, strlen(DistVolume), value);
}

ObjectErr_t SetVolumeValue(u16 value)
{
    return ObjectSetValue(ObjMusic, strlen(ObjMusic), DistVolume, strlen(DistVolume), value);
}



// end of file


