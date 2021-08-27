#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mosquitto.h>

#include <inttypes.h>
#include <gtk/gtk.h>

#include "json.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"

#define MQTT_SERVER     "localhost"
#define MQTT_PORT       1883
#define KEEP_ALIVE      60
#define MQTT_PUB_TOPIC  "/test0095"
#define MSG_MAX_SIZE    512

bool is_connected = false;
bool clean_session = true;
struct mosquitto *mosq = NULL;

char fname[ MSG_MAX_SIZE];
char lname[ MSG_MAX_SIZE];

int result;
void on_connect_cb( struct mosquitto *mosq, void *userdata, int result );

json_object *json_obj;
const char *json_str;

GtkWidget *window;
GtkWidget *box;
GtkWidget *input1,*label1,*input2,*label2,*button1, *button2;

static uint32_t counter = 0;
static gboolean counter_enabled = FALSE;

static gboolean timeout( gpointer data ) {
  static struct timeval tv;
	gettimeofday( &tv, NULL );
	g_print( "timestamp: %d.%06d\n", (int)tv.tv_sec, (int)tv.tv_usec );	
	if (counter_enabled) {
		counter++;
	}
	g_print( "tick: counter = %u\n", counter );
	return FALSE; // note: return FALSE to remove the timeout
}

static void button1_callback( GtkWidget *widget, gpointer data ) {
	const gchar *fname_input = gtk_entry_get_text(GTK_ENTRY(input1));
	const gchar *lname_input = gtk_entry_get_text(GTK_ENTRY(input2));
	
	sprintf( fname , "%s", fname_input);
	sprintf( lname , "%s", lname_input);
	
	g_print("First Name : %s\n", fname);
	g_print("Last Name : %s\n", lname);

    json_obj = json_tokener_parse( "{ }" );
    json_object_object_add(json_obj,"First name", json_object_new_string(fname));
    json_object_object_add(json_obj,"Last name", json_object_new_string(lname));
    
    
    json_str = json_object_to_json_string_ext( json_obj, JSON_C_TO_STRING_PRETTY);
    printf( "%s\n\n", json_str );
   
	json_object_put( json_obj);
}

static void button2_callback( GtkWidget *widget, gpointer data ) {
	g_print( "Exit..." );
}

static void activate( GtkApplication *app, gpointer user_data ) {

	// create a GTK application window (main window)
	window = gtk_application_window_new( app );
	// set the title of the application window
	gtk_window_set_title( GTK_WINDOW(window), "GTK Demo" );
	// set the default window size (width x height) in pixels
	gtk_window_set_default_size( GTK_WINDOW(window), 200, 160 );

	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	// set window border width
	gtk_container_set_border_width( GTK_CONTAINER(window), 4 );

	box = gtk_box_new( GTK_ORIENTATION_VERTICAL, 0 /*spacing*/ );
	// add the box to the application window
	gtk_container_add( GTK_CONTAINER(window), box );
  

	label1 = gtk_label_new("First Name : ");
	input1 = gtk_entry_new();
	label2 = gtk_label_new("Last Name : ");
	input2 = gtk_entry_new();
	
	// create a button for enable/disable counter
	button1 = gtk_button_new_with_label( "Enter" );
	g_signal_connect( button1, "clicked",G_CALLBACK(button1_callback), button1 );

	// create a button for exit 
	button2 = gtk_button_new_with_label( "Exit" );
	g_signal_connect( button2, "clicked",G_CALLBACK(button2_callback), NULL );
	g_signal_connect_swapped( button2, "clicked", G_CALLBACK( gtk_widget_destroy), window );
 
	// add the buttons to the box 
	gtk_box_pack_start( GTK_BOX(box), label1,TRUE /*exapnd*/, TRUE /*fill*/, 4/*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), input1,TRUE /*exapnd*/, TRUE /*fill*/, 4/*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), label2,TRUE /*exapnd*/, TRUE /*fill*/, 4/*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), input2,TRUE /*exapnd*/, TRUE /*fill*/, 4/*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), button1,TRUE /*exapnd*/, TRUE /*fill*/, 4 /*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), button2,TRUE /*exapnd*/, TRUE /*fill*/, 4 /*padding*/ );
  
	gtk_widget_show_all( window );
}

int main( int argc, char *argv[]) {

    printf( "Mosquitto - MQTT Client...\n" );

     // Initialize the libmosquito routines
    mosquitto_lib_init();
    
    // Create a mosquitto client
    mosq = mosquitto_new( NULL, clean_session, NULL );
    if( !mosq ) {
        printf( "Create client failed..\n");
        mosquitto_lib_cleanup( );
        return 1;
    }

    // Set callback functions if necessary
    mosquitto_connect_callback_set( mosq, on_connect_cb );

    // Connect to server
    if (mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) {
        fprintf( stderr, "Unable to connect.\n" );
        return 1;
    }

    result = mosquitto_loop_start(mosq); 
    if (result != MOSQ_ERR_SUCCESS) {
        printf("mosquitto loop error\n");
        return 1;
    }

    while (!is_connected) {
		usleep(100000); // sleep for 0.1 seconds
    }

	GtkApplication *app;
	app = gtk_application_new( NULL, G_APPLICATION_FLAGS_NONE );

	g_timeout_add_seconds( 1 /*sec*/, G_SOURCE_FUNC(timeout), NULL );
  
	g_signal_connect( app, "activate", G_CALLBACK(activate), NULL );
	// start the application main loop (blocking call)
	g_application_run( G_APPLICATION(app), argc, argv );
	// decrease the reference count to the object
	g_object_unref( app );

	return 0;
}

void on_connect_cb( struct mosquitto *mosq, void *userdata, int result ) {
    if (result!=0) {
        fprintf( stderr, "Connect failed\n" );
    } else{
		is_connected = true;
        printf( "Connect OK\n" );
    }
}

