#ifndef STAGE_TAB_HPP
#define STAGE_TAB_HPP

#include <gtk/gtk.h>

#include "mtk.hpp"

#include "renderer.hpp"
#include "shader_edit.hpp"


class stage_tab:
    public Mtk::VPaned
{
    public:
        stage_tab(int stage_number, renderer *render, Gtk::Notebook &parent);
        ~stage_tab(void);


    private:
        shader_edit vsh_edit, gsh_edit, fsh_edit;
        Gtk::TreeView uniforms;
};

#endif
