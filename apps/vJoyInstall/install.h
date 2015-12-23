#ifndef __SUPPORT_H__
#define __SUPPORT_H__
 
#include <gtk/gtk.h>


/* Convenience macros for obtaining objects from UI file */
#define CH_GET_OBJECT( builder, name, type, data ) \
    data->name = type( gtk_builder_get_object( builder, #name ) )
#define CH_GET_WIDGET( builder, name, data ) \
    CH_GET_OBJECT( builder, name, GTK_WIDGET, data )

/* User Interface definition file - GLADE to edit the file below */
#define UI_FILE "install.glade"

/* Main data structure definition */
typedef struct _ChData ChData;
struct _ChData
{
    /* Widgets */
    GtkWidget *main_window;		/* Main application window */
	/*** Buttons */
	GtkWidget *install_button;
	GtkWidget *remove_button;
	GtkWidget *refresh_button;
	GtkWidget *cancel_button;

	/*** Log Display */
	GtkWidget		*log_textview;
	GtkWidget		*gen_progressbar;
};
 
/* Expand  definition of ERRLEVEL*/
#define	APP_INFO	APP
#define	APP_ERR		(enum ERRLEVEL)(APP+1)
#define	APP_WARN	(enum ERRLEVEL)(APP+2)
#define	APP_FATAL	(enum ERRLEVEL)(APP+3)

/* Strings */
/*** Buttons ***/
#define BTN_CAPT_INSTALL "install"
#define BTN_CAPT_REMOVE "uninstall"
#define BTN_CAPT_REPAIR "refresh"

/*** Dispaly View ***/
#define DSP_OPEN	"\
************************************************************************************************\n\
 vJoy Installation Wizard\n\
 - Press button "BTN_CAPT_INSTALL" to install vJoy Device Driver\n\
 - Press button "BTN_CAPT_REMOVE" to Remove vJoy Device Driver from your system\n\
 - Press button "BTN_CAPT_REPAIR" to Remove vJoy Device Driver from your system then re-install it\n\
************************************************************************************************\n\
\n"

#define DSP_PPJOY_WARN "\
************************************************************************************************\n\
  PPJoy is already installed on this computer\n\
  Currently, it is impossible for both vJoy AND PPJoy to cohabite\n\
  Please exit vJoy installation, uninstall PPJoy then install vJoy\n\
************************************************************************************************\n\
\n"

#define INSTALL_SUCCESS "vJoy successfully installed"
#define INSTALL_FAILED  "vJoy was not installed"
#define REMOVE_SUCCESS "vJoy successfully removed"
#define REMOVE_FAILED  "vJoy was not removed"
#define REFRESH_SUCCESS "vJoy successfully refreshed"
#define REFRESH_FAILED  "vJoy was not refreshed"

/* Dialog box */
#define DLG_TTL_CLEAN_INF	"Uninstall vJoy"
#define DLG_TEXT_CLEAN_INF	"Would you like to delete the driver software for vJoy?"

enum TO_CLEAN {YES, NO, INVALID};

/*** Function Prototypes ***/
void ppjoy_warning(GtkTextBuffer *buffer);
void init_control(BOOL ppjoy_installed, BOOL vjoy_installed);
void set_std_cursor(GdkCursorType cursor);
guint wait_state(void);
BOOL  WINAPI status_message( void * buffer, TCHAR * msg,  enum ERRLEVEL level);
void * do_install(void * data);
void * do_remove(void * data, gboolean clean_inf);
void * do_refresh(void * data);

void * do_remove_clean(void * data);
void * do_remove_no_clean(void * data);

static gboolean wait_progress(gpointer data);
static gboolean wait_progress_done(void);

enum TO_CLEAN is_clean_inf(void * data);

/*** Global Variables ***/
ChData     *g_data=NULL;		// Pointer to GUI data structure
gboolean	processing=FALSE;	// True if worker thread is processing


#endif /* __SUPPORT_H__ */
