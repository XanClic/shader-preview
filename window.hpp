#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <list>

#include <gtk/gtk.h>

#include "renderer.hpp"
#include "stage_tab.hpp"


class main_window
{
    public:
        main_window(void);

        std::list<stage_tab *> stage_tabs;


    private:
        void build_render_tab(void);

        GtkWidget *wnd;
        GtkWidget *tab_container;

        renderer *rnd;
        GtkWidget *render_page, *render_page_label;

        GtkWidget *scale_display_fbo;
};

#endif
