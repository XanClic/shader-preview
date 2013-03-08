#include <cstdio>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkgl.h>

#include "window.hpp"


main_window *main_wnd;


int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    gdk_gl_init(&argc, &argv);

    main_wnd = new main_window;


    gtk_main();

    return 0;
}
