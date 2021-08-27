// $ sudo apt-get install libgtk-3-dev
/*
 - gcc include flags: `pkg-config --cflags gtk+-3.0`
 - gcc library flags: `pkg-config --libs gtk+-3.0`
 $ gcc -Wall -g main.c -o main \
    `pkg-config --cflags gtk+-3.0` \
    `pkg-config --libs gtk+-3.0`
*/ 
////////////////////////////////////////////////////////////////
#include <inttypes.h>
#include <gtk/gtk.h>

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
	return TRUE; // note: return FALSE to remove the timeout
}

static void button1_callback( GtkWidget *widget, gpointer data ) {
	counter_enabled = !counter_enabled;
	gtk_button_set_label( data,
			counter_enabled ? "Running" : "Paused" );
}

static void button2_callback( GtkWidget *widget, gpointer data ) {
	g_print( "Exit..." );
}

static void activate( GtkApplication *app, gpointer user_data ) {
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button1, *button2;

	// create a GTK application window (main window)
	window = gtk_application_window_new( app );
	// set the title of the application window
	gtk_window_set_title( GTK_WINDOW(window), "GTK Demo" );
	// set the default window size (width x height) in pixels
	gtk_window_set_default_size( GTK_WINDOW(window), 200, 160 );
	// set window position
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	// set window border width
	gtk_container_set_border_width( GTK_CONTAINER(window), 4 );

	// create a box for containing buttons (vertical orientation)
	// see: https://www.manpagez.com/html/gtk3/gtk3-3.24.14/GtkBox.php
	box = gtk_box_new( 
		GTK_ORIENTATION_VERTICAL, 0 /*spacing*/ );
	// add the box to the application window
	gtk_container_add( GTK_CONTAINER(window), box );
  
	// create two buttons: button1 and button2
	// see: https://www.manpagez.com/html/gtk3/gtk3-3.24.14/GtkButton.php

	// create a button for enable/disable counter
	button1 = gtk_button_new_with_label( "Click me!" );
	g_signal_connect( button1, "clicked",
		G_CALLBACK(button1_callback), button1 );

	// create a button for exit 
	button2 = gtk_button_new_with_label( "Exit" );
	g_signal_connect( button2, "clicked",
		G_CALLBACK(button2_callback), NULL );
	g_signal_connect_swapped( button2, "clicked", 
		G_CALLBACK( gtk_widget_destroy), window );
 
	// add the buttons to the box 
	gtk_box_pack_start( GTK_BOX(box), button1,
		TRUE /*exapnd*/, TRUE /*fill*/, 4 /*padding*/ );
	gtk_box_pack_start( GTK_BOX(box), button2, 
		TRUE /*exapnd*/, TRUE /*fill*/, 4 /*padding*/ );
  
	// make the window and its children widgets visible
	gtk_widget_show_all( window );
}

int main( int argc,char **argv ) {
	GtkApplication *app;
	app = gtk_application_new( NULL, G_APPLICATION_FLAGS_NONE );

	g_timeout_add_seconds( 1 /*sec*/, G_SOURCE_FUNC(timeout), NULL );
  
	g_signal_connect( app, "activate", 
		G_CALLBACK(activate), NULL );
	// start the application main loop (blocking call)
	g_application_run( G_APPLICATION(app), argc, argv );
	// decrease the reference count to the object
	g_object_unref( app );

	return 0;
}
////////////////////////////////////////////////////////////////
