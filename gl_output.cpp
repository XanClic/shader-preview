#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "gl_output.hpp"


gl_output::gl_output(QWidget *parent):
    QGLWidget(parent),
    rotate_object(false)
{
    connect(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(idle()));
}

void gl_output::idle(void)
{
    update();
}


void gl_output::upload_vsh(const QString &src)
{
    GLuint nvsh = glCreateShader(GL_VERTEX_SHADER);

    if (!compile_shader(nvsh, src))
        return;

    glDeleteShader(vsh);
    vsh = nvsh;

    if (fsh)
        link_shaders();
}

void gl_output::upload_fsh(const QString &src)
{
    GLuint nfsh = glCreateShader(GL_FRAGMENT_SHADER);

    if (!compile_shader(nfsh, src))
        return;

    glDeleteShader(fsh);
    fsh = nfsh;

    if (vsh)
        link_shaders();
}

void gl_output::bind_tex_to(unsigned tex_id, unsigned tmu)
{
    glActiveTexture(GL_TEXTURE0 + tmu);
    glBindTexture(GL_TEXTURE_2D, tex_id);
}

void gl_output::update_uniform(uniform *u)
{
    if (!u->id)
        u->id = glGetUniformLocation(prg, u->proc_name);

    if (u->id < 0)
        return;

    switch (u->proc_type)
    {
        case uniform::t_integer:
            glUniform1i(u->id, u->proc_value.value<int>());
            break;
        case uniform::t_float:
            glUniform1f(u->id, u->proc_value.value<float>());
            break;
        case uniform::t_vec2:
        {
            QVector2D val = u->proc_value.value<QVector2D>();
            glUniform2f(u->id, val.x(), val.y());
            break;
        }
        case uniform::t_vec3:
        {
            QVector3D val = u->proc_value.value<QVector3D>();
            glUniform3f(u->id, val.x(), val.y(), val.z());
            break;
        }
        case uniform::t_vec4:
        {
            QVector4D val = u->proc_value.value<QVector4D>();
            glUniform4f(u->id, val.x(), val.y(), val.z(), val.w());
            break;
        }
        case uniform::t_unk:
        default:
            break;
    }
}


void gl_output::initializeGL(void)
{
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmus);

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glColor4f(1.f, 1.f, 1.f, 1.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -3.f);

    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

    float vertices[] = {
        1.f, 1.f,  1.f,  1.f, 0.f,
        0.f, 1.f, -1.f,  1.f, 0.f,
        0.f, 0.f, -1.f, -1.f, 0.f,
        1.f, 0.f,  1.f, -1.f, 0.f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glInterleavedArrays(GL_T2F_V3F, 0, NULL);
}

void gl_output::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.f, static_cast<float>(w) / static_cast<float>(h), .1f, 10.f);

    glMatrixMode(GL_MODELVIEW);
}

void gl_output::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_QUADS, 0, 4);
}

void gl_output::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        grabMouse(Qt::ClosedHandCursor);

        rotate_object = true;

        rot_l_x = evt->x();
        rot_l_y = evt->y();
    }
}

void gl_output::mouseReleaseEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        releaseMouse();

        rotate_object = false;
    }
}

void gl_output::mouseMoveEvent(QMouseEvent *evt)
{
    if (!rotate_object)
        return;

    glRotatef(evt->x() - rot_l_x, 0.f, 1.f, 0.f);
    glRotatef(evt->y() - rot_l_y, 1.f, 0.f, 0.f);

    rot_l_x = evt->x();
    rot_l_y = evt->y();
}


bool gl_output::compile_shader(GLuint id, const QString &qsrc)
{
    QByteArray src_arr = qsrc.toUtf8();
    const char *src = src_arr.constData();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status)
        return true;

    GLint illen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &illen);
    if (illen > 1)
    {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(id, illen, NULL, msg);
        msg[illen] = 0;

        QMessageBox::critical(parentWidget(), "Error compiling shader", msg);

        delete msg;
    }

    glDeleteShader(id);

    return false;
}

void gl_output::link_shaders(void)
{
    GLuint nprg = glCreateProgram();

    glAttachShader(nprg, vsh);
    glAttachShader(nprg, fsh);

    glLinkProgram(nprg);

    GLint status;
    glGetProgramiv(nprg, GL_LINK_STATUS, &status);
    if (status)
    {
        prg = nprg;
        glUseProgram(prg);
        return;
    }

    GLint illen;
    glGetProgramiv(nprg, GL_INFO_LOG_LENGTH, &illen);
    if (illen > 1)
    {
        char *msg = new char[illen + 1];

        glGetProgramInfoLog(nprg, illen, NULL, msg);
        msg[illen] = 0;

        QMessageBox::critical(parentWidget(), "Error linking shaders", msg);

        delete msg;
    }

    glDeleteProgram(nprg);
}
