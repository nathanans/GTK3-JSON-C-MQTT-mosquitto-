// $ sudo apt install libjson-c-dev
// $ gcc main.c -o main -I/usr/include/json-c -ljson-c

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "json.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"

int main(int argc, char **argv) {
    json_object *json_obj;

    json_obj = json_tokener_parse( 
        "{\"key\": \"number\", \"value\": 30095 }" );
    
    // Convert JSON object to string
    const char *json_str;
    json_str = json_object_get_string( json_obj );
    printf( "%s\n\n", json_str );

    //------------------------------------------------------
    char buf[128];
    time_t now = time(NULL); // get current timestamp
    struct tm tm = *localtime( &now ); 

    snprintf( buf, sizeof(buf), "%d-%02d-%02d", 
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday );

    json_object_object_add( json_obj, "date", 
        json_object_new_string( buf ) );

    snprintf( buf, sizeof(buf), "%02d:%02d:%02d", 
        tm.tm_hour, tm.tm_min, tm.tm_sec );

    json_object_object_add( json_obj, "time",
        json_object_new_string(buf) );

    // convert JSON object to pretty JSON string
    json_str = json_object_to_json_string_ext( 
        json_obj, JSON_C_TO_STRING_PRETTY );
    printf( "%s\n", json_str );

    //------------------------------------------------------
    // decrement the reference count of json_obj
    // and free if it reaches zero
    json_object_put( json_obj );
    return 0;
}

/* sample output:
{ "key": "number", "value": 1234 }
{
  "key":"number",
  "value":1234,
  "date":"2021-08-27",
  "time":"08:52:12"
}
*/
