#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <gtkmm.h>
#include <gdk/gdk.h>
#include <gdk/gdkgl.h>

#include "texture_management.hpp"
#include "types.hpp"


class renderer:
    public Gtk::DrawingArea
{
    public:
        renderer(void);

        void initialize_gl(void);

        inline void begin_gl(void) { gdk_gl_drawable_gl_begin(drawable, context); }
        inline void end_gl(void) { gdk_gl_drawable_gl_end(drawable); }
        inline void swap_buffers(void) { gdk_gl_drawable_swap_buffers(drawable); }

        void fbo_display_setting_changed(bool new_state);

        int tmus, vattrs;
        int width, height;

        mat4 *modelview, *projection, *it_modelview;
        mat3 *normal_mat;

        managed_texture *tex_bound;


    protected:
        virtual bool on_configure_event(GdkEventConfigure *evt);
        virtual bool on_expose_event(GdkEventExpose *evt);


    private:
        GdkGLConfig *glconf;

        GdkGLDrawable *drawable;
        GdkGLContext *context;

        void *mat_mem;

        vec4 tex_draw_data[4];
        unsigned tex_draw_buf, tex_draw_prg, tex_draw_tex_uniform, tex_draw_vtx_attrib;

        bool scale_display_fbo;
};


class stage_tab;

class render_stage
{
    public:
        render_stage(void);
        ~render_stage(void);

        bool update_shaders(stage_tab *st);
        void update_fbo(stage_tab *st);
        void update_uniforms(stage_tab *st);
        void update_vertex_buffers(stage_tab *st);

        unsigned prg, fbo;

        int fbo_cb_bindings;
};

#endif
