#ifndef SHADER_EDIT_HPP
#define SHADER_EDIT_HPP

#include <gtkmm.h>


class shader_edit:
    public Gtk::ScrolledWindow
{
    public:
        shader_edit(void);

        Glib::RefPtr<Gtk::TextBuffer> buffer(void) { return edit.get_buffer(); }


    private:
        Gtk::TextView edit;
};

#endif
