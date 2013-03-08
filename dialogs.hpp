#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <gtk/gtk.h>


extern GtkWidget *main_wnd;


class message_dialogs
{
    public:
        static inline void error(const char *title, const char *text)
        {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_wnd), GtkDialogFlags(0), GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, text);
            gtk_window_set_title(GTK_WINDOW(dialog), title);

            gtk_dialog_run(GTK_DIALOG(dialog));
        }
};

#endif
