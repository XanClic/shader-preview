#ifndef GL_OUTPUT_HPP
#define GL_OUTPUT_HPP

#include <GL/gl.h>

#include <QGLWidget>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "uniforms.hpp"


class gl_output: public QGLWidget
{
    Q_OBJECT

    public:
        int tmus;

        gl_output(QWidget *parent = NULL);

        void upload_vsh(const QString &src);
        void upload_fsh(const QString &src);
        void bind_tex_to(unsigned tex_id, unsigned tmu);
        void update_uniform(uniform *u);

    protected:
        void initializeGL(void);
        void resizeGL(int w, int h);
        void paintGL(void);
        void mousePressEvent(QMouseEvent *evt);
        void mouseReleaseEvent(QMouseEvent *evt);
        void mouseMoveEvent(QMouseEvent *evt);

    private:
        QTimer refresh_timer;
        GLuint quad_vbo;
        GLuint vsh, fsh, prg;
        GLint inv_mat_uniform;
        bool rotate_object;
        int rot_l_x, rot_l_y;

        bool compile_shader(GLuint id, const QString &src);
        void link_shaders(void);
};

#endif
