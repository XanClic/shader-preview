#include <gtk/gtk.h>

#include "renderer.hpp"
#include "window.hpp"


main_window::main_window(void)
{
    wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(wnd), "Shader Preview");

    g_signal_connect(wnd, "destroy", G_CALLBACK(gtk_main_quit), NULL);


    tab_container = gtk_notebook_new();

    gtk_container_add(GTK_CONTAINER(wnd), tab_container);
    gtk_widget_show(tab_container);


    build_render_tab();


    gtk_widget_show(wnd);


    rnd->initialize_gl();
}


static void fbo_display_setting_changed_wrapper(GtkWidget *checkbox, GdkEvent *evt, renderer *rnd)
{
    (void)evt;

    rnd->fbo_display_setting_changed(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox)));
}

void main_window::build_render_tab(void)
{
    render_page = gtk_vbox_new(FALSE, 0);
    render_page_label = gtk_label_new("Result");

    gtk_notebook_append_page(GTK_NOTEBOOK(tab_container), render_page, render_page_label);
    gtk_widget_show(render_page);
    gtk_widget_show(render_page_label);


    rnd = new renderer;

    gtk_widget_set_size_request(rnd->output, 320, 200);
    gtk_container_add(GTK_CONTAINER(render_page), rnd->output);
    gtk_widget_show(rnd->output);


    scale_display_fbo = gtk_check_button_new_with_label("Render displayed FBO in appropriate resolution");

    g_signal_connect(scale_display_fbo, "toggled", G_CALLBACK(fbo_display_setting_changed_wrapper), rnd);

    gtk_container_add_with_properties(GTK_CONTAINER(render_page), scale_display_fbo, "expand", FALSE, NULL);
    gtk_widget_show(scale_display_fbo);
}
