#include <cstdio>

#include <gtkmm.h>
#include <gdk/gdk.h>
#include <gdk/gdkgl.h>

#include "window.hpp"


using namespace Gtk;


main_window *main_wnd;


int main(int argc, char *argv[])
{
    Main gtk(argc, argv);

    gdk_gl_init(&argc, &argv);


    main_wnd = new main_window;


    gtk.run(*main_wnd);

    return 0;
}
