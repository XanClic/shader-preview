#include <gtkmm.h>
#include <pangomm.h>

#include "shader_edit.hpp"


using namespace Gtk;
using namespace Pango;


static void populate_popup(Menu *menu);

shader_edit::shader_edit(void)
{
    set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
    set_border_width(2);

    FontDescription font("monospace 8");

    edit.set_wrap_mode(WRAP_NONE);
    edit.modify_font(font);

    edit.signal_populate_popup().connect(sigc::ptr_fun(populate_popup));


    add(edit);
}


static void populate_popup(Menu *menu)
{
    MenuItem *items[4];

    items[0] = new SeparatorMenuItem;

    items[1] = new MenuItem("Load file...");
    items[2] = new MenuItem("Save");
    items[3] = new MenuItem("Save file...");

    for (MenuItem *item: items)
    {
        menu->items().push_back(*item);
        item->show();
    }
}
