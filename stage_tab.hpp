#ifndef STAGE_TAB_HPP
#define STAGE_TAB_HPP

#include <vector>

#include <gtk/gtk.h>

#include "mtk.hpp"

#include "color_buffer.hpp"
#include "depth_buffer.hpp"
#include "renderer.hpp"
#include "shader_edit.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


class stage_tab:
    public Mtk::VPaned
{
    public:
        stage_tab(int stage_number, renderer *render, Gtk::Notebook &parent);
        ~stage_tab(void);

        void assign_uniform(void);
        void bind_builtin(void);
        void set_displayed(bool isit);


    private:
        class VertexModeColumns:
            public Gtk::TreeModel::ColumnRecord
        {
            public:
                VertexModeColumns(void) { add(gl); add(name); }

                Gtk::TreeModelColumn<int> gl;
                Gtk::TreeModelColumn<Glib::ustring> name;
        };

        class UniformColumns:
            public Gtk::TreeModel::ColumnRecord
        {
            public:
                UniformColumns(void) { add(uni); add(name); add(value); }

                Gtk::TreeModelColumn<uniform *> uni;
                Gtk::TreeModelColumn<Glib::ustring> name, value;
        };

        class VertexColumns:
            public Gtk::TreeModel::ColumnRecord
        {
            public:
                VertexColumns(void) { add(vi); add(va); add(name); add(value); }

                Gtk::TreeModelColumn<int> vi;
                Gtk::TreeModelColumn<vertex_attrib *> va;
                Gtk::TreeModelColumn<Glib::ustring> name, value;
        };

        class BufferColumns:
            public Gtk::TreeModel::ColumnRecord
        {
            public:
                BufferColumns(void) { add(cb); add(name); }

                Gtk::TreeModelColumn<color_buffer *> cb;
                Gtk::TreeModelColumn<Glib::ustring> name;
        };

        void apply_shaders(void);
        void enable_gsh(void);
        void different_vertex_rendering_method(void);
        void scan_shaders(void);

        shader_edit vsh_edit, gsh_edit, fsh_edit;
        Gtk::CheckButton gsh_enable;

        Gtk::ComboBox vtx_mode;
        VertexModeColumns vtx_mode_cols;
        Glib::RefPtr<Gtk::ListStore> vtx_mode_model;

        Gtk::TreeView uniform_tree, vertex_tree, buffer_tree;
        UniformColumns uniform_tree_cols;
        VertexColumns vertex_tree_cols;
        BufferColumns buffer_tree_cols;
        Glib::RefPtr<Gtk::ListStore> uniform_tree_model, buffer_tree_model;
        Glib::RefPtr<Gtk::TreeStore> vertex_tree_model;

        std::vector<uniform *> *uniforms;
        vertex_info *vertices;
        std::vector<color_buffer *> *outputs;
        depth_buffer *depth;

        render_stage rpd; // renderer private data
        renderer *render;

        int vertex_rendering_method, stage_number;

        bool ready_to_roll, used_in_display;


        friend class render_stage;
        friend class renderer;
};

#endif
