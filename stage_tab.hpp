#ifndef STAGE_TAB_HPP
#define STAGE_TAB_HPP

#include <QtGui>

#include "color_buffer.hpp"
#include "popup_tree.hpp"
#include "renderer.hpp"
#include "shader_edit.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


class stage_tab: public QWidget
{
    Q_OBJECT

    public:
        stage_tab(int stage_number, renderer *render, QWidget *parent = NULL);
        ~stage_tab(void);

    public slots:
        void apply_shaders(void);
        void enable_gsh(int enable);
        void assign_uniform(void);
        void bind_builtin(void);
        void different_vertex_rendering_method(int idx);
        void set_displayed(bool isit);
        void set_vertex_attribute(void);
        void add_vertex(void);
        void remove_vertex(void);
        void load_wavefront(void);

    private:
        /// Scans the shaders contained in the edit widgets for variables
        void scan_shaders(void);

        QHBoxLayout sub_layout[2];
        QVBoxLayout main_layout, gsh_layout, vtx_layout;
        shader_edit vsh_edit, gsh_edit, fsh_edit;
        QPushButton sh_apply;
        popup_tree uniform_tree, vertex_tree, buffer_tree;
        QCheckBox gsh_enable;
        QComboBox vtx_type;

        QList<uniform *> *uniforms;
        vertex_info *vertices;
        QList<color_buffer *> *outputs;

        int vertex_rendering_method, stage_number;

        render_stage rpd; // renderer private data
        renderer *render;

        bool ready_to_roll, used_in_display;

        friend class main_window;
        friend class render_stage;
        friend class renderer;
};

#endif
