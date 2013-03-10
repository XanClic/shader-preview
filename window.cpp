#include <gtkmm.h>

#include "renderer.hpp"
#include "stage_tab.hpp"
#include "window.hpp"


using namespace Gtk;


main_window::main_window(void)
{
    set_title("Shader Preview");
    set_border_width(5);


    add(tab_container);


    build_render_tab();

    stage_tabs.push_back(new stage_tab(0, &rnd, tab_container));


    show_all();


    rnd.initialize_gl();
}


main_window::~main_window(void)
{
    for (stage_tab *st: stage_tabs)
        delete st;
}


void main_window::fbo_display_setting_changed_wrapper(CheckButton *checkbox)
{
    rnd.fbo_display_setting_changed(checkbox->get_active());
}

void main_window::build_render_tab(void)
{
    render_page.add(rnd);


    CheckButton *scale_display_fbo = manage(new CheckButton("Render displayed FBO in appropriate resolution"));
    scale_display_fbo->signal_toggled().connect(sigc::bind<CheckButton *>(sigc::mem_fun(*this, &main_window::fbo_display_setting_changed_wrapper), scale_display_fbo));
    scale_display_fbo->set_active(true);
    scale_display_fbo->set_size_request(100, 25);

    render_page.pack_end(*scale_display_fbo, false, false);


    tab_container.append_page(render_page, "Result");
}
