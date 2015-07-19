/////////////////////////////////////////////////////////////////////
//
//  GTK Installer for vJoy (Install.exe)
//
//	GUI Described by GLADE file (install.glade) - 
//	It consists of the following widgets:
//		   
//		main_window		-	Main application window
//		install_button	-	Button (Install vJoy)
//		remove_button	-	Button (Remove vJoy)
//		refresh_button	-	Button (Remove then re-install vJoy)
//		cancel_button	-	Cancel button
//		log_textview	-	Text View area used for messages
//		gen_progressbar	-	Prograss bar
//	These widgets are packed into structure: ChData
//	Since there is only one such structure, it is global.
//
//	Callback functions:
//	Each function button calles a corresponding callback function:
//	> Pressing install_button calls function on_install_button_clicked()
//	> Pressing remove_button calls function on_remove_button_clicked()
//	> Pressing refresh_button calls function on_refresh_button_clicked()
//	Every callback function starts a worker thread where the actual processing is done
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "../../../inc/public.h"
#include "install.h"
#include "wrapper.h"


////////////////////////////////////////////////////////////////////
// Main GTK function - quite standard
int main(int argc, char **argv )
{
    GtkBuilder *builder;
    GError     *error = NULL;
	GtkWidget  *window;
	GtkTextBuffer *buffer;
	BOOL	ppjoy_installed, vjoy_installed;

	/* init threads */
	g_thread_init(NULL);
	gdk_threads_init();

	/* Init GTK+ */
	gtk_init( &argc, &argv );

	/* Create new GtkBuilder object */
	builder = gtk_builder_new();
    if( ! gtk_builder_add_from_file( builder, UI_FILE, &error ) )
    {
        g_warning( "%s", error->message );
        g_free( error );
        return( 1 );
    }
 
    /* Allocate data structure */
    g_data = g_slice_new( ChData );
 
    /* Get objects from UI */
#define GW( name ) CH_GET_WIDGET( builder, name, g_data )
    GW( main_window );
    GW( refresh_button );
    GW( install_button );
    GW( remove_button );
    GW( cancel_button );
	GW( log_textview );
	GW( gen_progressbar );
#undef GW
 
    /* Connect signals */
    gtk_builder_connect_signals( builder, g_data );
 
    /* Destroy builder, since we don't need it anymore */
    g_object_unref( G_OBJECT( builder ) );

	/* If PPJoy installed - warn the user and desensetise install & refresh buttons */
	ppjoy_installed = is_ppjoy_installed();

	/* Test if (this version of) vJoy installed */
	vjoy_installed = is_vjoy_installed();

	/* Here we initialize widgets */
	init_control(ppjoy_installed, vjoy_installed);

	/*** Log View - Display welcome message */
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (g_data->log_textview));
	gtk_text_buffer_set_text (buffer, DSP_OPEN, -1);
	if (ppjoy_installed)
		ppjoy_warning(buffer);

	/* Show window. All other widgets are automatically shown by GtkBuilder */
	gtk_widget_show( g_data->main_window );

	/* Set the status message function (callback) */
	SetStatusMessageFunc(status_message);
	status_message( (void *) g_data->log_textview, NULL,  APP_INFO);

	/* enter the GTK main loop */
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave(); 

	/* Free any allocated data */
	g_slice_free( ChData, g_data );

	return( 0 );
}


/////// Helper Functions ///////////////////////////////////////////////////////////////////
/*
	Initialize controls
	
	Buttons (All buttons are NOT pressed):
		Remove:		Sensetive if vJoy IS installed
		Install:	Sensitive if vJoy NOT installed and if PPJoy NOT installed.
		Refresh:	Sensitive if vJoy IS installed and if PPJoy NOT installed.
		Cancel:		Sensitive

	Progress bar:
		Set to 1%

	Input:
		ppjoy_installed:	True is PPJoy IS installed.
		vjoy_installed:		True is vJoy IS installed.
*/
void init_control(BOOL ppjoy_installed, BOOL vjoy_installed)
{
	if (!g_data)
		return;

	/*** Normal cursor */
	set_std_cursor(GDK_ARROW);

	/*** Making action possible/impossible */
	gtk_widget_set_sensitive(g_data->install_button, !ppjoy_installed && !vjoy_installed);
	gtk_widget_set_sensitive(g_data->remove_button, vjoy_installed);
	gtk_widget_set_sensitive(g_data->refresh_button, !ppjoy_installed && vjoy_installed);
	gtk_widget_set_sensitive(g_data->cancel_button, TRUE);

}
/*
	Change the mouse pointer (cursor) to one of the standard cursors
	as described here: http://library.gnome.org/devel/gdk/stable/gdk-Cursors.html#gdk-cursor-new-for-display

	Input:
		cursor:	One of the standard cursors (NULL = default)
		
*/
void set_std_cursor(GdkCursorType cursor_type)
{
	GdkDisplay * def_display;
	GdkCursor  * cursor;


	def_display = gdk_display_get_default();
	cursor = gdk_cursor_new_for_display(def_display, cursor_type);
	
	gdk_window_set_cursor(gtk_widget_get_window((GtkWidget *)g_data->main_window), cursor);
	gdk_window_set_cursor(gtk_text_view_get_window((GtkTextView *)g_data->log_textview, GTK_TEXT_WINDOW_TEXT), cursor);

	gdk_display_sync(def_display);
    gdk_cursor_unref(cursor); 

}

/*
	Print status message to the log view
	Used as callback function - called by vJoyInstall.dll

	Initialization:
		GUI application calls this fubction once.
		It passes pointer to output buffer (=log view) that will be kept as static.

	Input:
		buffer:	Pointer to log buffer of type GtkTextBuffer. Used only for initialization.
		msg:	Pointer to text message buffer allocated and distroyed by caller.
		level:	Level of severity. Can be INFO, ERR, WARN or FATAL.
				Any other value is treated as plain text.

*/
BOOL  WINAPI status_message( void * textview, TCHAR * msg,  enum ERRLEVEL level)
{
	static GtkWidget *  container;
	static GtkTextBuffer * out_buf;
	static GtkTextTag *tag_info, *tag_warn, *tag_err, *tag_fatal, *tag_unknn, *tag_app_info, *tag_app_err, *tag_app_warn;
	GtkTextTag *tag_sel;
	GtkTextIter start, end;
	int line_before;
	TCHAR array_prefix[20];
	const TCHAR *prefix;
	GtkTextTagTable *buf_tag_table=NULL;



	/* Set output buffer */
	if (textview)
	{
		container = (GtkWidget *)textview;
		out_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW (container));
	}
	else
	{
		if (!out_buf)
			return FALSE;
	};

	/* Sanity chack */
	if (!msg)
		return FALSE;

	gdk_threads_enter(); // Enter thread-safe zone

	/* Create tags */
	buf_tag_table = gtk_text_buffer_get_tag_table(out_buf);
	if (!tag_info)
		tag_info  = gtk_text_buffer_create_tag (out_buf, "lightgrey_foreground", "foreground", "#b2b2b2", "family","monospace", NULL);
	if (!tag_unknn)
		tag_unknn = gtk_text_buffer_create_tag (out_buf, "standard_foreground", "family","monospace", NULL);
	if (!tag_warn)
		tag_warn = gtk_text_buffer_create_tag (out_buf, "orange_foreground", "foreground", "#f9c87f", "family","monospace", NULL);
	if (!tag_err)
		tag_err= gtk_text_buffer_create_tag (out_buf, "red_foreground", "foreground", "red", "family","monospace", NULL);
	if (tag_fatal)
		tag_fatal = gtk_text_buffer_create_tag (out_buf, "boldred_foreground", "foreground", "red", "family","monospace", "weight", "bold", NULL);
	if (!tag_app_info)
		tag_app_info = tag_unknn;
	if (!tag_app_warn)
		tag_app_warn = tag_warn;
	if (!tag_app_err)
		tag_app_err= tag_err;

	/* Create message prefix  and select tag */
	prefix = &(array_prefix[0]);
	switch (level)
	{
	case INFO:
		prefix = (TEXT("[I] "));
		tag_sel = tag_info;
		break;
	case WARN:
		prefix = ("[W] ");
		tag_sel = tag_warn;
		break;
	case ERR:
		prefix = ("[E] ");
		tag_sel = tag_err;
		break;
	case FATAL:
		prefix = ("[F] ");
		tag_sel = tag_fatal;
		break;
	case APP_INFO:
		prefix = ("");
		tag_sel = tag_app_info;
		break;
	case APP_WARN:
		prefix = ("Warning: ");
		tag_sel = tag_app_warn;
		break;
	case APP_ERR:
		prefix = ("Error: ");
		tag_sel = tag_app_err;
		break;
	default:
		prefix = ("[]");
		tag_sel = tag_unknn;
	}


	/* format message */
	line_before = gtk_text_buffer_get_line_count(out_buf); // Get the number of lines (before printing message)
	gtk_text_buffer_insert_at_cursor(out_buf,prefix, -1); // Print message level prefix
	gtk_text_buffer_insert_at_cursor(out_buf,msg, -1); // Print message
	gtk_text_buffer_insert_at_cursor(out_buf,"\n", -1); // add New line

	gtk_text_buffer_get_iter_at_line (out_buf, &start, line_before-1);// Create iterator at Beginning of the warning
	gtk_text_buffer_get_iter_at_offset (out_buf, &end, -1); // Create iterator at End of buffer
	gtk_text_buffer_apply_tag (out_buf, tag_sel, &start, &end); // Apply tag 

	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW (container), &start, 0, TRUE, 0, 1); 

	gdk_threads_leave(); // Leave thread-safe zone

	gtk_widget_show_now((GtkWidget *)container);
	while (g_main_context_iteration(NULL, FALSE));

	return TRUE;
}


/*
	Issue a warning in red that PPJoy is already installed
*/
void ppjoy_warning(GtkTextBuffer *buffer)
{
	GtkTextTag *tag_redtext = NULL;
	GtkTextIter start, end;
	int line_before;

	
	tag_redtext = gtk_text_buffer_create_tag (buffer, "red_foreground", "foreground", "red", NULL);//Create tag "red_foreground"
	line_before = gtk_text_buffer_get_line_count(buffer); // Get the number of lines (before printing warning)
	
	gtk_text_buffer_insert_at_cursor(buffer,DSP_PPJOY_WARN, -1); // Print warning
	gtk_text_buffer_get_iter_at_line (buffer, &start, line_before-1);// Create iterator at Beginning of the warning
	gtk_text_buffer_get_iter_at_offset (buffer, &end, -1); // Create iterator at End of buffer
	gtk_text_buffer_apply_tag (buffer, tag_redtext, &start, &end); // Apply tag "red_foreground" on warning
}
/* 
	Put the GUI in hold
	All buttons are grey (insensitive), cursor in wait (hourglass)
	Start the progress bar pulses
*/
guint wait_state(void)
{

	// Buttons are insensitive
	gtk_widget_set_sensitive(g_data->install_button, FALSE);
	gtk_widget_set_sensitive(g_data->remove_button, FALSE);
	gtk_widget_set_sensitive(g_data->refresh_button, FALSE);
	gtk_widget_set_sensitive(g_data->cancel_button, FALSE);

	// Set cursor to wait+arrow
	set_std_cursor(GDK_CLOCK);

	// Sets function wait_progress() (that pulses the progress bar) to be called every 100 milliseconds, 
	// with the given priority (G_PRIORITY_DEFAULT).
	// The function is called repeatedly until it returns FALSE,
	// at which point the timeout is automatically destroyed and the function will not be called again.
	// Function wait_progress_done() (that resets the progress bar) is called when the timeout is destroyed.
	return (g_timeout_add_full(G_PRIORITY_DEFAULT, 100,(GSourceFunc)wait_progress, g_data, (GDestroyNotify)wait_progress_done));
}

/*
	Callback from wait_state()
	This function is called 10 times a second.
	When global parameter 'processing' becomes FALSE it is called for the LAST time.

	> Pulse the progress bar
*/
static gboolean wait_progress(gpointer data)
{
	gtk_progress_bar_pulse((GtkProgressBar *)(((ChData *)g_data)->gen_progressbar));
	return processing;
}

/*
	Callback from wait_state()
	This function is called ONCE when global parameter becomes processing FALSE

	> Progress bar to 100%
	> Cursor restored to original value (GDK_ARROW)
*/
static gboolean wait_progress_done(void)
{
	gtk_progress_bar_set_fraction((GtkProgressBar *)(g_data->gen_progressbar), 1);
	set_std_cursor(GDK_ARROW);
	return TRUE;
}





////// Callback (and related) Functions //////////////////////////////////////////////

// Called when the Install button is clicked
// Creates a worker thread and exits
G_MODULE_EXPORT gboolean on_install_button_clicked(GtkWidget *widget, ChData *data)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	GError     *error = NULL;

	/* Start installation in a new thread */
	GThread * thread_install = g_thread_create(do_install,data, FALSE, &error);

	/* Error handling */
	if (!thread_install)
	{
		g_warning( " Thread was not created: %s", error->message );
		g_free( error );
		return FALSE;
	};	

	return TRUE;
}


/* 
	This function runs in a worker thread and handles all the GUI related issues
	as well as the actual job of installing vJoy
*/

void * do_install(void * gui_data)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	BOOL vjoy_installed, ppjoy_installed;
	
	/* Put the GUI in wait-state*/
	processing=TRUE;
	wait_state();

	/* Operation - may be very long */
	get_inf_file(inf);
	get_dev_hwid(dev);
	installation(dev, inf);
	/* Installation ended */

	/* If PPJoy installed - warn the user and desensetise install & refresh buttons */
	ppjoy_installed = is_ppjoy_installed();

	/* Test if (this version of) vJoy installed */
	vjoy_installed = is_vjoy_installed();


	/* Getting GUI out of wait state */
	processing=FALSE;

	/* Here we reset widgets */
	init_control(ppjoy_installed, vjoy_installed);

	/* Print results */
	if (vjoy_installed)
		status_message( NULL, (TCHAR *)INSTALL_SUCCESS,  APP_INFO);
	else
		status_message( NULL, (TCHAR *)INSTALL_FAILED,  APP_ERR);


	return NULL;
}



G_MODULE_EXPORT gboolean on_refresh_button_clicked(GtkWidget *widget, ChData *data)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	GError     *error = NULL;

	/* Start installation in a new thread */
	GThread * thread_install = g_thread_create(do_refresh,data, FALSE, &error);

	/* Error handling */
	if (!thread_install)
	{
		g_warning( " Thread was not created: %s", error->message );
		g_free( error );
		return FALSE;
	};	

	return TRUE;
}

void * do_refresh(void * data)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	BOOL vjoy_installed, ppjoy_installed;
	static guint wait_progress_id = 0;
	
	/* Put the GUI in wait-state*/
	processing=TRUE;
	wait_state();

	/* Operation - may be very long */
	get_inf_file(inf);
	get_dev_hwid(dev);
	repair(dev, inf);

	/* If PPJoy installed - warn the user and desensetise install & refresh buttons */
	ppjoy_installed = is_ppjoy_installed();

	/* Test if (this version of) vJoy installed */
	vjoy_installed = is_vjoy_installed();

	/* Getting GUI out of wait state */
	processing=FALSE;

	/* Here we reset widgets */
	init_control(ppjoy_installed, vjoy_installed);

	/* Print results */
	if (vjoy_installed)
		status_message( NULL, REFRESH_SUCCESS,  APP_INFO);
	else
		status_message( NULL, REFRESH_FAILED,  APP_ERR);


	return NULL;
}


G_MODULE_EXPORT gboolean on_remove_button_clicked(GtkWidget *widget, ChData *data)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	GError     *error = NULL;
	BOOL clean_inf=FALSE;
	GThread * thread_install;

	/* Display a Clean/Remove pop-up window */
	clean_inf = is_clean_inf(data);

	/* Start removal in a new thread */
	if (clean_inf == YES)
		thread_install = g_thread_create(do_remove_clean,data, FALSE, &error);
	else if (clean_inf == NO)
		thread_install = g_thread_create(do_remove_no_clean,data, FALSE, &error);
	else
	{
		processing=FALSE;
		return FALSE;
	};

	/* Error handling */
	if (!thread_install)
	{
		g_warning( " Thread was not created: %s", error->message );
		g_free( error );
		return FALSE;
	};	

	return TRUE;
}

void * do_remove_clean(void * data)
{
	return do_remove(data, TRUE);
}
void * do_remove_no_clean(void * data)
{
	return do_remove(data, FALSE);
}

/* 
	This function runs in a worker thread and handles all the GUI related issues
	as well as the actual job of removing vJoy
*/
void * do_remove(void * gui_data, gboolean clean_inf)
{
	TCHAR inf[MAX_PATH], dev[MAX_PATH];
	BOOL vjoy_installed, ppjoy_installed;
	
	/* Put the GUI in wait-state*/
	processing=TRUE;
	wait_state();

	/* Operation - may be very long */
	get_inf_file(inf);
	get_dev_hwid(dev);
	removal(dev, inf,clean_inf);

	/* If PPJoy installed - warn the user and desensetise install & refresh buttons */
	ppjoy_installed = is_ppjoy_installed();

	/* Test if (this version of) vJoy installed */
	vjoy_installed = is_vjoy_installed();

	/* Getting GUI out of wait state */
	processing=FALSE;

	/* Here we reset widgets */
	init_control(ppjoy_installed, vjoy_installed);


	/* Print results */
	if (!vjoy_installed)
		status_message( NULL, REMOVE_SUCCESS,  APP_INFO);
	else
		status_message( NULL, REMOVE_FAILED,  APP_ERR);


	return NULL;
}


/*
	Display pop-up dialog box asking the user if to clean OEMxx.INF file
*/
enum TO_CLEAN is_clean_inf(void * data)
{
	ChData *d;
	GtkWidget *dialog, *label, *content_area;
	gint result;

	d = (ChData *)data;

	/* Dialog box will be Modal, with YES/NO Buttons */
	dialog = gtk_message_dialog_new (
								 GTK_WINDOW(d->main_window),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_QUESTION,
                                 GTK_BUTTONS_YES_NO,
                                 DLG_TEXT_CLEAN_INF);

	/* Display dialog box - wait for reply */
	result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	if (result == GTK_RESPONSE_YES)
		return YES;
	if (result == GTK_RESPONSE_NO)
		return NO;
	else
		return INVALID;
}
