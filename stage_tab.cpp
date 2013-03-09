#include <gtkmm.h>

#include "mtk.hpp"

#include "hlcstr.hpp"
#include "shader_edit.hpp"
#include "stage_tab.hpp"


using namespace Gtk;


stage_tab::stage_tab(int stage_number, renderer *rnd, Notebook &parent)
{
    Button *gsh_enable = new Button("Enable _geometry shader", true);


    VBox *gsh_layout = new VBox;

    gsh_layout->add(gsh_edit);
    gsh_layout->pack_end(*gsh_enable, false, false);


    Mtk::HPaned editables;

    editables.add(vsh_edit);
    editables.add(*gsh_layout);
    editables.add(fsh_edit);


    Button *sh_apply = new Button("Appl_y shaders", true);


    VBox *edit_apply = new VBox;

    edit_apply->add(editables);
    edit_apply->pack_end(*sh_apply, false, false);

    add(*edit_apply);


    CellRenderer *text_renderer = new CellRendererText;

    uniforms.append_column("Uniform", *text_renderer);
    uniforms.append_column("Value",   *text_renderer);

    uniforms.set_headers_visible(true);
    uniforms.set_size_request(100, 30);

    add(uniforms);


    HBox *tab_label = new HBox;
    tab_label->pack_start(*new Label("Stage %i"_hl % stage_number), true, true);
    tab_label->pack_end(*new Image(StockID("gtk-close"), ICON_SIZE_MENU), false, false);

    tab_label->show_all();


    parent.append_page(*this, *tab_label);
}
