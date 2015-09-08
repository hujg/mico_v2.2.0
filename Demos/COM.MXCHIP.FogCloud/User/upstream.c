
/***

History:
2015-09-06: Ted: Create

*/

#include "mico.h"
#include "MicoFogCloud.h"
//#include "json_c/json.h"

/* User defined debug log functions
 * Add your own tag like: 'USER_UPSTREAM', the tag will be added at the beginning of a log
 * in MICO debug uart, when you call this function.
 */
#define user_log(M, ...) custom_log("UPSTREAM", M, ##__VA_ARGS__)
#define user_log_trace() custom_log_trace("UPSTREAM")

extern int energy;
extern int interval;
extern int lights;
extern int remind;
extern int volume;
extern bool subscribe;
extern char* track;
extern char* url_path;

/* Message upload thread
 * Get device data && upload to cloud
 */
void upstream_thread(void* arg)
{
    user_log_trace();
    
    OSStatus err = kUnknownErr;
    mico_Context_t *app_context = (mico_Context_t *)arg;
    json_object *send_json_object = NULL;
    const char *upload_data = NULL;
    
    require(app_context, exit);
    
    while(1) {
        if(subscribe) {
            if(!MicoFogCloudIsConnect(app_context)) {
                user_log("appStatus.fogcloudStatus.isCloudConnected = false");
                mico_thread_sleep(1);
                continue;
            }
            
            // create json object to format upload data
            send_json_object = json_object_new_object();
            if(NULL == send_json_object) {
                user_log("create json object error!");
                err = kNoMemoryErr;
            }
            else {
                // add device parameter data into json object
                json_object_object_add(send_json_object, "energy", json_object_new_int(energy));
                json_object_object_add(send_json_object, "interval", json_object_new_int(interval));
                json_object_object_add(send_json_object, "lights", json_object_new_int(lights));
                json_object_object_add(send_json_object, "remind", json_object_new_int(remind));
                json_object_object_add(send_json_object, "volume", json_object_new_int(volume));
                json_object_object_add(send_json_object, "subscribe", json_object_new_boolean(subscribe));
                json_object_object_add(send_json_object, "track", json_object_new_string(track));
                json_object_object_add(send_json_object, "url_path", json_object_new_string(url_path));
                
                upload_data = json_object_to_json_string(send_json_object);
                if(NULL == upload_data) {
                    user_log("create upload data string error!");
                    err = kNoMemoryErr;
                }
                else {
                    // upload data string to fogcloud, the seconde param(NULL) means send to defalut topic: '<device_id>/out'
                    MicoFogCloudMsgSend(app_context, NULL, 0, (unsigned char*)upload_data, strlen(upload_data));
                    user_log("ready sended, %s", (unsigned char*)upload_data);
                    err = kNoErr;
                }
                
                // free json object memory
                json_object_put(send_json_object);
                send_json_object = NULL;
                user_log("free json_object_put");
            }
            
            mico_thread_sleep(10);
        }
    }
    
exit:
    if(kNoErr != err){
        user_log("ERROR: upstream_thread_handle thread exit with err=%d", err);
    }
    
    mico_rtos_delete_thread(NULL);  // delete current thread
}
