
/***

History:
2015-09-10: Ted: Create

*/

#include <Object.h>


SObjectMng object[MAX_OBJECT_NUM];


// Object initialization
void ObjectInit(void)
{
    u8 index;
    
    memset(object, 0, MAX_OBJECT_NUM * sizeof(SObjectMng));
}
// end of Object initialization

// Create Object
ObjectErr_t ObjectCreate(char* obj_name, u8 size_of_name)
{
    u8 index = 0;
    
    do {
        if(object[index].valid == false) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrExceed;
    }
    
    object[index].valid = true;
    strncpy(object[index].objName, obj_name, size_of_name);
    
    return NoErr;
}
// end of Create Object

// Add Dist into Object
ObjectErr_t ObjectAddDist(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name)
{
    u8 index;
    u8 dist_index;
    
    index = 0;
    
    do {
        if((object[index].valid == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrNotExist;
    }
    
    dist_index = 0;
    
    do {
        if(object[index].dist[dist_index].valid == false) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        return ErrDistFull;
    }
    
    object[index].dist[dist_index].valid = true;
    strncpy(object[index].dist[dist_index].distName, dist_name, size_of_dist_name);
    
    return NoErr;
}

// Subscribe/Unsubscribe Interface
ObjectErr_t ObjectSubscribe(char* obj_name, u8 size_of_obj_name, threadId_t suber_id)
{
    u8 index;
    u8 sub_index;
    
    index = 0;
    
    do {
        if((object[index].valid == true)
            && (strncmp(object[index].name, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrNotExist;
    }
    
    sub_index = 0;
    
    do {
        if(object[index].subscribe[sub_index].valid == false) {
            break;
        }
        
        sub_index++;
    }while(sub_idnex < MAX_SUBSCRIBE_NUM_IN_OBJECT);
    
    if(sub_index >= MAX_SUBSCRIBE_NUM_IN_OBJECT) {
        return ErrExceed;
    }

    object[index].subscribe[sub_index].valid = true;
    object[index].subscribe[sub_index].subscriber_thread_id = suber_id;
    
    return NoErr;
}

ObjectErr_t ObjectUnsubscribe(char* obj_name, u8 size_of_obj_name, threadId_t unsuber_id)
{
    u8 index;
    u8 sub_index;
    
    index = 0;
    
    do {
        if((object[index].valid == true)
            && (strncmp(object[index].name, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrNotExist;
    }
    
    sub_index = 0;
    
    do {
        if((object[index].subscribe[sub_index].valid == true)
            && (object[index].subscribe[sub_index].subscriber_thread_id == unsuber_id)) {
            break;
        }
        
        sub_index++;
    }while(sub_idnex < MAX_SUBSCRIBE_NUM_IN_OBJECT);
    
    if(sub_index >= MAX_SUBSCRIBE_NUM_IN_OBJECT) {
        return ErrNotExist;
    }
    
    object[index].subscribe[sub_index].valid = false;
    object[index].subscribe[sub_index].subscriber_thread_id = 0;
    
    return NoErr;
}
// end of Subscribe/Unsubscribe Interface

// get Object dist value
u16 ObjectGetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name)
{
    u8 index;
    u8 dist_index;
    
    index = 0;
    
    do {
        if((object[index].valid == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrNotExist;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].valid == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        return ErrNotExist;
    }
    
    return object[index].dist[dist_index].data;
}

// set Object dist value
ObjectErr_t ObjectSetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name, u16 value)
{
    u8 index;
    u8 dist_index;
    
    index = 0;
    
    do {
        if((object[index].valid == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        return ErrNotExist;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].valid == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        return ErrNotExist;
    }
    
    object[index].dist[dist_index].data = value;
    
    return NoErr;
}

// end of file


