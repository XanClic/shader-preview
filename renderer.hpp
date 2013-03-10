#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QGLWidget>
#include <QMenu>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "texture_management.hpp"
#include "types.hpp"

extern int ogl_maj, ogl_min;

class main_window;
class stage_tab;

class renderer: public QGLWidget
{
    Q_OBJECT

    public:
        renderer(const QGLFormat &format, main_window *parent);
        ~renderer(void);

        int tmus, vattrs;
        int width, height;

    public slots:
        void set_bound_texture(void);
        void fbo_display_setting_changed(int new_state);

    protected:
        void initializeGL(void);
        void resizeGL(int w, int h);
        void paintGL(void);
        void mousePressEvent(QMouseEvent *evt);
        void mouseReleaseEvent(QMouseEvent *evt);
        void mouseMoveEvent(QMouseEvent *evt);

    private:
        QGLFormat gl_format;

        QMenu popup_menu;
        QTimer refresh_timer;
        bool rotate_object;
        int rot_l_x, rot_l_y;

        void *mat_mem;

        mat4 *modelview, *projection, *it_modelview;
        mat3 *normal_mat;

        int frame_counter;
        float elapsed_time;

        QTime meas_time;

        vec4 tex_draw_data[4];
        unsigned tex_draw_buf, tex_draw_prg, tex_draw_tex_uniform, tex_draw_vtx_attrib;
        managed_texture *tex_bound;

        bool scale_display_fbo;

        friend class main_window;
        friend class stage_tab;
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
