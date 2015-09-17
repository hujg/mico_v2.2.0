
/***

History:
2015-09-06: Ted: Create

*/

#include "mico.h"
#include "MicoFogCloud.h"
//#include "json_c/json.h"
#include <ObjDevice.h>
#include <ObjMusic.h>
#include <ObjLights.h>

/* User defined debug log functions
 * Add your own tag like: 'USER_DOWNSTREAM', the tag will be added at the beginning of a log
 * in MICO debug uart, when you call this function.
 */
#define user_log(M, ...) custom_log("DOWNSTREAM", M, ##__VA_ARGS__)
#define user_log_trace() custom_log_trace("DOWNSTREAM")


extern bool subscribe;
extern char* track;
extern char* url_path;

/* Handle user message from cloud
 * Receive msg from cloud && do hardware operation
 */
void downstream_thread(void* arg)
{
    user_log_trace();
    OSStatus err = kUnknownErr;
    mico_Context_t *app_context = (mico_Context_t *)arg;
    fogcloud_msg_t *recv_msg = NULL;
    json_object *recv_json_object = NULL;
    
    require(app_context, exit);
    
    while(1) {
        mico_thread_sleep(1);
        if(!MicoFogCloudIsConnect(app_context)) {
            user_log("appStatus.fogcloudStatus.isCloudConnected = false");
            //mico_thread_sleep(1);
            continue;
        }
        
        // recv_msg->data = <topic><data>
        err = MicoFogCloudMsgRecv(app_context, &recv_msg, 1000);
        user_log("err = %d", err);
        if(kNoErr == err){
            user_log("Msg recv: topic[%d]=[%.*s]\tdata[%d]=[%.*s]", 
                    recv_msg->topic_len, recv_msg->topic_len, recv_msg->data, 
                    recv_msg->data_len, recv_msg->data_len, recv_msg->data + recv_msg->topic_len);
            
            recv_json_object = json_tokener_parse((const char*)recv_msg->data + recv_msg->topic_len);
            if(NULL != recv_json_object) {
                user_log("recv_json_object != NULL");
                
                // parse json object
                char *key; struct json_object *val; struct lh_entry *entry;

                for(entry = json_object_get_object(recv_json_object)->head; \
                        (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); \
                        entry = entry->next) {
                    if(!strcmp(key, "energy")){
                        SetEnergyValue(json_object_get_int(val));
                    }
                    else if(!strcmp(key, "interval")) {
                        SetIntervalValue(json_object_get_int(val));
                    }
                    else if(!strcmp(key, "lights")) {
                        SetLightsValue(json_object_get_int(val));
                    }
                    else if(!strcmp(key, "remind")) {
                        SetRemindValue(json_object_get_int(val));
                    }
                    else if(!strcmp(key, "volume")) {
                        SetVolumeValue(json_object_get_int(val));
                    }
                    else if(!strcmp(key, "subscribe")) {
                        subscribe = json_object_get_boolean(val);
                        user_log("subscribe = %d", (int)subscribe);
                    }
                    else if(!strcmp(key, "track")) {
                        //track = json_object_get_string(val);
                    }
                    else if(!strcmp(key, "url_path")) {
                        //url_path = json_object_get_string(val);
                    }
                }
            }
            
            // free memory of json object
            json_object_put(recv_json_object);
            recv_json_object = NULL;
        }
        
        // NOTE: must free msg memory after been used.
        if(NULL != recv_msg){
            free(recv_msg);
            recv_msg = NULL;
        }
    }
    
exit:
    if(kNoErr != err){
        user_log("ERROR: downstream_thread_handle thread exit with err=%d", err);
    }
    
    mico_rtos_delete_thread(NULL);  // delete current thread
}
 
 