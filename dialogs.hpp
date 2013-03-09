#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <gtkmm.h>

#include "window.hpp"


extern main_window *main_wnd;


class message_dialogs
{
    public:
        static inline void error(const char *title, const char *text)
        {
            Gtk::MessageDialog dialog(*main_wnd, text, Gtk::MESSAGE_ERROR);
            dialog.set_title(title);

            dialog.run();
        }
};

#endif
