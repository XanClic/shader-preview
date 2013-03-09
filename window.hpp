#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <list>

#include <gtkmm.h>

#include "renderer.hpp"
#include "stage_tab.hpp"


class main_window:
    public Gtk::Window
{
    public:
        main_window(void);

        std::list<stage_tab *> stage_tabs;


    private:
        void build_render_tab(void);
        void fbo_display_setting_changed_wrapper(Gtk::CheckButton *checkbox);

        Gtk::Notebook tab_container;

        renderer rnd;
        Gtk::VBox render_page;
};

#endif
