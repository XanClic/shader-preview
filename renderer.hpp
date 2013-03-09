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

        int ogl_maj, ogl_min;


    protected:
        virtual bool on_configure_event(GdkEventConfigure *evt);
        virtual bool on_expose_event(GdkEventExpose *evt);


    private:
        GdkGLConfig *glconf;

        GdkGLDrawable *drawable;
        GdkGLContext *context;

        void *mat_mem;

        mat4 *modelview, *projection, *it_modelview;
        mat3 *normal_mat;

        vec4 tex_draw_data[4];
        unsigned tex_draw_buf, tex_draw_prg, tex_draw_tex_uniform, tex_draw_vtx_attrib;
        managed_texture *tex_bound;

        bool scale_display_fbo;
};

#endif
