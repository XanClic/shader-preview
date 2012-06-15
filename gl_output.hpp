#ifndef GL_OUTPUT_HPP
#define GL_OUTPUT_HPP

#include <GL/gl.h>

#include <QGLWidget>
#include <QString>
#include <QWidget>

#include "uniforms.hpp"


class gl_output: public QGLWidget
{
    Q_OBJECT

    public:
        int tmus;

        gl_output(QWidget *parent);

        void upload_vsh(const QString &src);
        void upload_fsh(const QString &src);
        void bind_tex_to(unsigned tex_id, unsigned tmu);
        void update_uniform(uniform *u);

    public slots:
        void idle(void);

    protected:
        void initializeGL(void);
        void resizeGL(int w, int h);
        void paintGL(void);
        void mousePressEvent(QMouseEvent *evt);
        void mouseReleaseEvent(QMouseEvent *evt);
        void mouseMoveEvent(QMouseEvent *evt);

    private:
        GLuint quad_vbo;
        GLuint vsh, fsh, prg;
        bool rotate_object;
        int rot_l_x, rot_l_y;

        bool compile_shader(GLuint id, const QString &src);
        void link_shaders(void);
};

#endif
