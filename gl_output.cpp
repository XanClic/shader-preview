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
    refresh_timer.setInterval(1000 / 60);
    connect(&refresh_timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    refresh_timer.start();
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

float mat4_determinant(float *d)
{
    float ret;

    __asm__ __volatile__ (
            "movaps 16(%1),%%xmm1;"
            "movaps 32(%1),%%xmm2;"
            "movaps 48(%1),%%xmm3;"

            "pshufd $0x01,%%xmm1,%%xmm4;"
            "pshufd $0x9E,%%xmm2,%%xmm7;"
            "pshufd $0x7B,%%xmm3,%%xmm8;"
            "mulps  %%xmm4,%%xmm7;"
            "mulps  %%xmm8,%%xmm7;"
            "pshufd $0x5A,%%xmm1,%%xmm5;"
            "pshufd $0x33,%%xmm2,%%xmm8;"
            "pshufd $0x8D,%%xmm3,%%xmm9;"
            "mulps  %%xmm5,%%xmm8;"
            "mulps  %%xmm9,%%xmm8;"
            "pshufd $0xBF,%%xmm1,%%xmm6;"
            "pshufd $0x49,%%xmm2,%%xmm9;"
            "pshufd $0x12,%%xmm3,%%xmm10;"
            "mulps  %%xmm6,%%xmm9;"
            "mulps  %%xmm10,%%xmm9;"
            "addps  %%xmm8,%%xmm7;"
            "addps  %%xmm9,%%xmm7;"

            "pshufd $0x7B,%%xmm2,%%xmm8;"
            "pshufd $0x9E,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm4;"
            "mulps  %%xmm9,%%xmm4;"
            "pshufd $0x8D,%%xmm2,%%xmm8;"
            "pshufd $0x33,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm5;"
            "mulps  %%xmm9,%%xmm5;"
            "pshufd $0x12,%%xmm2,%%xmm8;"
            "pshufd $0x49,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm6;"
            "mulps  %%xmm9,%%xmm6;"
            "addps  %%xmm5,%%xmm4;"
            "addps  %%xmm6,%%xmm4;"

            "subps  %%xmm4,%%xmm7;"
            "mulps   0(%1),%%xmm7;"

            "haddps %%xmm7,%%xmm7;"
            "haddps %%xmm7,%%xmm7;"
            "movss  %%xmm7,%0;"
            : "=x"(ret) : "r"(d) : "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10"
    );

    return ret;
}

#if 0
#define le(a1, a2, a3, a4, a5) d[a1] * (d[a2] * d[a3] - d[a4] * d[a5])

#define ele(x, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF) \
        x * (le(a1, a2, a3, a4, a5) + le(a6, a7, a8, a9, aA) + le(aB, aC, aD, aE, aF))
#endif

// Actually returns a transposed result.
void mat4_invert(float *d)
{
#if 1
    __asm__ __volatile__ (
            "rcpss  %1,%1;"
            "pshufd $0x00,%1,%1;"

            "movaps  0(%0),%%xmm1;"
            "movaps 16(%0),%%xmm2;"
            "movaps 32(%0),%%xmm3;"
            "movaps 48(%0),%%xmm4;"

            "pshufd $0x9E,%%xmm3,%%xmm5;"
            "pshufd $0x7B,%%xmm4,%%xmm6;"
            "pshufd $0x7B,%%xmm3,%%xmm7;"
            "pshufd $0x9E,%%xmm4,%%xmm8;"
            "pshufd $0x01,%%xmm2,%%xmm9;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm9,%%xmm5;"
            "pshufd $0x33,%%xmm3,%%xmm6;"
            "pshufd $0x8D,%%xmm4,%%xmm7;"
            "pshufd $0x8D,%%xmm3,%%xmm8;"
            "pshufd $0x33,%%xmm4,%%xmm9;"
            "pshufd $0x5A,%%xmm2,%%xmm10;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm10,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x49,%%xmm3,%%xmm6;"
            "pshufd $0x12,%%xmm4,%%xmm7;"
            "pshufd $0x12,%%xmm3,%%xmm8;"
            "pshufd $0x49,%%xmm4,%%xmm9;"
            "pshufd $0xBF,%%xmm2,%%xmm10;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm10,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5, 0(%0);"

            "pshufd $0x7B,%%xmm3,%%xmm5;"
            "pshufd $0x9E,%%xmm4,%%xmm6;"
            "pshufd $0x9E,%%xmm3,%%xmm7;"
            "pshufd $0x7B,%%xmm4,%%xmm8;"
            "pshufd $0x01,%%xmm1,%%xmm10;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x8D,%%xmm3,%%xmm6;"
            "pshufd $0x33,%%xmm4,%%xmm7;"
            "pshufd $0x33,%%xmm3,%%xmm8;"
            "pshufd $0x8D,%%xmm4,%%xmm9;"
            "pshufd $0x5A,%%xmm1,%%xmm11;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x12,%%xmm3,%%xmm6;"
            "pshufd $0x49,%%xmm4,%%xmm7;"
            "pshufd $0x49,%%xmm3,%%xmm8;"
            "pshufd $0x12,%%xmm4,%%xmm9;"
            "pshufd $0xBF,%%xmm1,%%xmm12;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,16(%0);"

            "pshufd $0x9E,%%xmm2,%%xmm5;"
            "pshufd $0x7B,%%xmm4,%%xmm6;"
            "pshufd $0x7B,%%xmm2,%%xmm7;"
            "pshufd $0x9E,%%xmm4,%%xmm8;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x33,%%xmm2,%%xmm6;"
            "pshufd $0x8D,%%xmm4,%%xmm7;"
            "pshufd $0x8D,%%xmm2,%%xmm8;"
            "pshufd $0x33,%%xmm4,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x49,%%xmm2,%%xmm6;"
            "pshufd $0x12,%%xmm4,%%xmm7;"
            "pshufd $0x12,%%xmm2,%%xmm8;"
            "pshufd $0x49,%%xmm4,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,32(%0);"

            "pshufd $0x7B,%%xmm2,%%xmm5;"
            "pshufd $0x9E,%%xmm3,%%xmm6;"
            "pshufd $0x9E,%%xmm2,%%xmm7;"
            "pshufd $0x7B,%%xmm3,%%xmm8;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x8D,%%xmm2,%%xmm6;"
            "pshufd $0x33,%%xmm3,%%xmm7;"
            "pshufd $0x33,%%xmm2,%%xmm8;"
            "pshufd $0x8D,%%xmm3,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x12,%%xmm2,%%xmm6;"
            "pshufd $0x49,%%xmm3,%%xmm7;"
            "pshufd $0x49,%%xmm2,%%xmm8;"
            "pshufd $0x12,%%xmm3,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,48(%0);"
            :: "r"(d), "x"(mat4_determinant(d)) : "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12"
    );
#else
    float dt = 1.f / mat4_determinant(d);

    float nd[16] = {
        ele(dt,   5, 10, 15, 11, 14,    6, 11, 13,  9, 15,    7,  9, 14, 10, 13),0
        ele(dt,   1, 11, 14, 10, 15,    2,  9, 15, 11, 13,    3, 10, 13,  9, 14),1
        ele(dt,   1,  6, 15,  7, 14,    2,  7, 13,  5, 15,    3,  5, 14,  6, 13),2
        ele(dt,   1,  7, 10,  6, 11,    2,  5, 11,  7,  9,    3,  6,  9,  5, 10),3
        ele(dt,   4, 11, 14, 10, 15,    6,  8, 15, 11, 12,    7, 10, 12,  8, 14),4
        ele(dt,   0, 10, 15, 11, 14,    2, 11, 12,  8, 15,    3,  8, 14, 10, 12),5
        ele(dt,   0,  7, 14,  6, 15,    2,  4, 15,  7, 12,    3,  6, 12,  4, 14),6
        ele(dt,   0,  6, 11,  7, 10,    2,  7,  8,  4, 11,    3,  4, 10,  6,  8),7
        ele(dt,   4,  9, 15, 11, 13,    5, 11, 12,  8, 15,    7,  8, 13,  9, 12),8
        ele(dt,   0, 11, 13,  9, 15,    1,  8, 15, 11, 12,    3,  9, 12,  8, 13),9
        ele(dt,   0,  5, 15,  7, 13,    1,  7, 12,  4, 15,    3,  4, 13,  5, 12),a
        ele(dt,   0,  7,  9,  5, 11,    1,  4, 11,  7,  8,    3,  5,  8,  4,  9),b
        ele(dt,   4, 10, 13,  9, 14,    5,  8, 14, 10, 12,    6,  9, 12,  8, 13),c
        ele(dt,   0,  9, 14, 10, 13,    1, 10, 12,  8, 14,    2,  8, 13,  9, 12),d
        ele(dt,   0,  6, 13,  5, 14,    1,  4, 14,  6, 12,    2,  5, 12,  4, 13),e
        ele(dt,   0,  5, 10,  6,  9,    1,  6,  8,  4, 10,    2,  4,  9,  5,  8) f
    };

    memcpy(d, nd, sizeof(nd));
#endif
}

void gl_output::mouseMoveEvent(QMouseEvent *evt)
{
    if (!rotate_object)
        return;

    glRotatef(evt->x() - rot_l_x, 0.f, 1.f, 0.f);
    glRotatef(evt->y() - rot_l_y, 1.f, 0.f, 0.f);

    float mvm[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mvm);
    mat4_invert(mvm);
    glUniformMatrix4fv(inv_mat_uniform, 1, true, mvm);

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

        inv_mat_uniform = glGetUniformLocation(prg, "inverse_modelview");

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
