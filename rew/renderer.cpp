#define GL_GLEXT_PROTOTYPES

#include <cmath>
#include <cstdlib>

#include <QtCore>
#include <QtGui>

#include "color_buffer.hpp"
#include "dialogs.hpp"
#include "main_window.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "stage_tab.hpp"
#include "texture_management.hpp"
#include "trackable.hpp"
#include "types.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


int ogl_maj, ogl_min;

static unsigned common_depth;
static main_window *main_wnd;

volatile bool gl_initialized = false;

static unsigned compile_shader(GLenum type, const QString &str)
{
    unsigned sh = glCreateShader(type);

    QByteArray ba = str.toUtf8();
    const char *src = ba.constData();

    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);

    int status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return sh;

    int illen;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1)
        QMessageBox::critical(main_wnd, "Error compiling shader", "[error reason unknown]");
    else
    {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(sh, illen, NULL, msg);
        msg[illen] = 0;

        if (type == GL_VERTEX_SHADER)
            QMessageBox::critical(main_wnd, "Error compiling vertex shader", msg);
        if (type == GL_GEOMETRY_SHADER)
            QMessageBox::critical(main_wnd, "Error compiling geometry shader", msg);
        if (type == GL_FRAGMENT_SHADER)
            QMessageBox::critical(main_wnd, "Error compiling fragment shader", msg);
        else
            QMessageBox::critical(main_wnd, "Error compiling shader", msg);

        delete msg;
    }

    glDeleteShader(sh);
    return 0;
}


renderer::renderer(main_window *parent):
    QGLWidget(parent),
    rotate_object(false),
    tex_bound(NULL)
{
    main_wnd = parent;

    refresh_timer.setInterval(1000 / 60);
    connect(&refresh_timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    refresh_timer.start();

    mat_mem = malloc(3 * sizeof(mat4) + sizeof(mat3));
    modelview    = new (reinterpret_cast<mat4 *>(mat_mem)    ) mat4;
    projection   = new (reinterpret_cast<mat4 *>(mat_mem) + 1) mat4;
    it_modelview = new (reinterpret_cast<mat4 *>(mat_mem) + 2) mat4;
    normal_mat   = new (reinterpret_cast<mat4 *>(mat_mem) + 3) mat3;

    modelview->translate(vec3(0.f, 0.f, -3.f));


    add_trackable(modelview, "Modelview matrix");
    add_trackable(projection, "Projection matrix");
    add_trackable(it_modelview, "Inversed modelview matrix");
    add_trackable(normal_mat, "Normal matrix");


    connect(popup_menu.addAction("Set displayed te&xture"), SIGNAL(triggered()), this, SLOT(set_bound_texture()));
}

renderer::~renderer(void)
{
    free(mat_mem);

    glDeleteRenderbuffers(1, &common_depth);
    glDeleteBuffers(1, &tex_draw_buf);


    // TODO: Remove trackables
}


void renderer::initializeGL(void)
{
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmus);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &vattrs);

    glGetIntegerv(GL_MAJOR_VERSION, &ogl_maj);
    glGetIntegerv(GL_MINOR_VERSION, &ogl_min);


    for (int i = 0; i < tmus; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glEnable(GL_TEXTURE_2D);
    }

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearDepth(1.f);


    glGenRenderbuffers(1, &common_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, common_depth);
    // TODO: Variable resolution
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

    if (ogl_maj >= 3)
    {
        glGenBuffers(1, &tex_draw_buf);
        glBindBuffer(GL_ARRAY_BUFFER, tex_draw_buf);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec4), NULL, GL_STATIC_DRAW);

        vec4 *dst = static_cast<vec4 *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        dst[0] = vec4(-1.f,  1.f, 0.f, 1.f);
        dst[1] = vec4(-1.f, -1.f, 0.f, 1.f);
        dst[2] = vec4( 1.f, -1.f, 0.f, 1.f);
        dst[3] = vec4( 1.f,  1.f, 0.f, 1.f);

        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    else
    {
        tex_draw_data[0] = vec4(-1.f,  1.f, 0.f, 1.f);
        tex_draw_data[1] = vec4(-1.f, -1.f, 0.f, 1.f);
        tex_draw_data[2] = vec4( 1.f, -1.f, 0.f, 1.f);
        tex_draw_data[3] = vec4( 1.f,  1.f, 0.f, 1.f);
    }

    unsigned ptvs, ptfs;

    if (ogl_maj >= 3)
    {
        ptvs = compile_shader(GL_VERTEX_SHADER, "#version 130\nin vec4 vertex;\nout vec2 tex_coord;\nvoid main(void){gl_Position = vertex; tex_coord = vertex.xy * vec2(.5, .5) + vec2(.5, .5);}");
        if (!ptvs) throw 42; // FIXME

        ptfs = compile_shader(GL_FRAGMENT_SHADER, "#version 130\nin vec2 tex_coord\n;out vec4 color;\nuniform sampler2D tex;\nvoid main(void){color = texture2D(tex, tex_coord);}");
        if (!ptfs) throw 42; // FIXME
    }
    else
    {
        ptvs = compile_shader(GL_VERTEX_SHADER, "#version 110\nvoid main(void){gl_Position = gl_Vertex; gl_TexCoord[0] = vec4(gl_Vertex.xy * vec2(.5, .5) + vec2(.5, .5), 0., 0.);}");
        if (!ptvs) throw 42; // FIXME

        ptfs = compile_shader(GL_FRAGMENT_SHADER, "#version 110\nuniform sampler2D tex;\nvoid main(void){gl_FragColor = texture2D(tex, gl_TexCoord[0].st);}");
        if (!ptfs) throw 42; // FIXME
    }

    tex_draw_prg = glCreateProgram();
    glAttachShader(tex_draw_prg, ptvs);
    glAttachShader(tex_draw_prg, ptfs);
    if (ogl_maj >= 3)
        glBindFragDataLocation(tex_draw_prg, 0, "color");
    glLinkProgram(tex_draw_prg);

    int status;
    glGetProgramiv(tex_draw_prg, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) throw 42; // FIXME

    glDeleteShader(ptvs);
    glDeleteShader(ptfs);

    glUseProgram(tex_draw_prg);
    tex_draw_tex_uniform = glGetUniformLocation(tex_draw_prg, "tex");
    if (ogl_maj >= 3)
        tex_draw_vtx_attrib = glGetAttribLocation(tex_draw_prg, "vertex");


    gl_initialized = true;
}

void renderer::resizeGL(int w, int h)
{
    width = w; height = h;

    float f = 1.f / tanf(1.f / 6.f * (float)M_PI);
    projection->d[ 0] = f * (float)h / (float)w;
    projection->d[ 5] = f;
    projection->d[10] = (      .1f + 100.f) / (.1f - 100.f);
    projection->d[11] = -1.f;
    projection->d[14] = (2.f * .1f * 100.f) / (.1f - 100.f);
    projection->d[15] = 0.f;
}

void renderer::paintGL(void)
{
    bool enable_array[vattrs];

    glViewport(0, 0, 1024, 1024);

    for (stage_tab *st: main_wnd->stage_tabs)
    {
        if (!st->ready_to_roll)
            continue;


        glActiveTexture(GL_TEXTURE0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, st->rpd.fbo);
        GLenum *bufs = new GLenum[st->rpd.fbo_cb_bindings];
        for (int i = 0; i < st->rpd.fbo_cb_bindings; i++)
            bufs[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(st->rpd.fbo_cb_bindings, bufs);
        delete bufs;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(st->rpd.prg);


        reset_texture_bindings();


        for (uniform *u: *st->uniforms)
            u->assign();


        if (ogl_maj >= 3)
        {
            memset(enable_array, 0, sizeof(enable_array));

            for (vertex_attrib *va: st->vertices->attribs)
                enable_array[va->id] = true;

            for (int i = 0; i < vattrs; i++)
                (enable_array[i] ? glEnableVertexAttribArray : glDisableVertexAttribArray)(i);


            glBindBuffer(GL_ARRAY_BUFFER, st->vertices->buffer);
        }
        else
        {
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
        }


        uintptr_t ofs = 0;
        for (vertex_attrib *va: st->vertices->attribs)
        {
            if (ogl_maj >= 3)
                glVertexAttribPointer(va->id, va->epv, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void *>(ofs));
            else
            {
                if (va->name == "gl_Vertex")
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(va->epv, GL_FLOAT, 0, va->ptr());
                }
                else if (va->name == "gl_MultiTexCoord0")
                {
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer(va->epv, GL_FLOAT, 0, va->ptr());
                }
                else if (va->name == "gl_Color")
                {
                    glEnableClientState(GL_COLOR_ARRAY);
                    glColorPointer(va->epv, GL_FLOAT, 0, va->ptr());
                }
                else if (va->name == "gl_Normal")
                {
                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT, 0, va->ptr());
                }
            }

            ofs += va->len() * va->epv * sizeof(float);
        }

        glDrawArrays(st->vertex_rendering_method, 0, st->vertices->attribs[0]->len());
    }

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (tex_bound == NULL)
        return;

    glUseProgram(tex_draw_prg);

    if (ogl_maj >= 3)
    {
        for (int i = 0; i < vattrs; i++)
            ((i == static_cast<int>(tex_draw_vtx_attrib)) ? glEnableVertexAttribArray : glDisableVertexAttribArray)(i);

        glBindBuffer(GL_ARRAY_BUFFER, tex_draw_buf);
        glVertexAttribPointer(tex_draw_vtx_attrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    else
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(4, GL_FLOAT, 0, tex_draw_data);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_bound->id);
    glUniform1i(tex_draw_tex_uniform, 0);

    glDrawArrays(GL_QUADS, 0, 4);
}

void renderer::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        grabMouse(Qt::ClosedHandCursor);

        rotate_object = true;

        rot_l_x = evt->x();
        rot_l_y = evt->y();
    }
    else if (evt->button() == Qt::RightButton)
        popup_menu.popup(evt->globalPos());
}

void renderer::mouseReleaseEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        releaseMouse();

        rotate_object = false;
    }
}

void renderer::mouseMoveEvent(QMouseEvent *evt)
{
    if (!rotate_object)
        return;

    modelview->rotate((evt->x() - rot_l_x) * (float)M_PI / 180.f, vec3(0.f, 1.f, 0.f));
    modelview->rotate((evt->y() - rot_l_y) * (float)M_PI / 180.f, vec3(1.f, 0.f, 0.f));

    *it_modelview = *modelview;
    it_modelview->transposed_invert();

    *normal_mat = *modelview;
    normal_mat->transposed_invert();

    rot_l_x = evt->x();
    rot_l_y = evt->y();
}


void renderer::set_bound_texture(void)
{
    bool ok;
    managed_texture *mt = texture_dialog::get_tex(this, "Choose displayed texture", "Choose a texture to be displayed:", &ok);
    if (!ok || (mt == tex_bound))
        return;

    if (tex_bound != NULL)
        unuse_texture(tex_bound);
    use_texture(mt);

    tex_bound = mt;
}


render_stage::render_stage(void)
{
    glGenFramebuffers(1, &fbo);
    fbo_cb_bindings = 0;
}

render_stage::~render_stage(void)
{
    glDeleteFramebuffers(1, &fbo);
}

bool render_stage::update_shaders(stage_tab *st)
{
    unsigned vsh, fsh, gsh = 0;

    vsh = compile_shader(GL_VERTEX_SHADER, st->vsh_edit.toPlainText());
    if (!vsh)
        return false;

    fsh = compile_shader(GL_FRAGMENT_SHADER, st->fsh_edit.toPlainText());
    if (!fsh)
    {
        glDeleteShader(vsh);
        return false;
    }

    if (st->gsh_edit.isEnabled())
    {
        gsh = compile_shader(GL_GEOMETRY_SHADER, st->gsh_edit.toPlainText());
        if (!gsh)
        {
            glDeleteShader(vsh);
            glDeleteShader(fsh);
            return false;
        }
    }


    unsigned nprg = glCreateProgram();

    glAttachShader(nprg, vsh);
    glAttachShader(nprg, fsh);
    if (gsh)
        glAttachShader(nprg, gsh);

    int i = 0;
    for (color_buffer *cb: *st->outputs)
    {
        QByteArray ba = cb->name.toUtf8();
        glBindFragDataLocation(nprg, i++, ba.constData());
    }

    glLinkProgram(nprg);

    int status;
    glGetProgramiv(nprg, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        int illen;
        glGetProgramiv(nprg, GL_INFO_LOG_LENGTH, &illen);

        if (illen > 1)
        {
            char *msg = new char[illen + 1];

            glGetProgramInfoLog(nprg, illen, NULL, msg);
            msg[illen] = 0;

            QMessageBox::critical(main_wnd, "Error linking program", msg);

            delete msg;
        }

        glDeleteProgram(nprg);
    }

    glDeleteShader(vsh);
    glDeleteShader(fsh);
    if (gsh)
        glDeleteShader(fsh);

    if (status == GL_TRUE)
        prg = nprg;

    return status;
}

void render_stage::update_fbo(stage_tab *st)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, common_depth);


    int bi = 0;

    for (color_buffer *cb: *st->outputs)
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bi++, GL_TEXTURE_2D, cb->id, 0);

    for (int i = bi; i < fbo_cb_bindings; i++)
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);

    fbo_cb_bindings = bi;
}

void render_stage::update_uniforms(stage_tab *st)
{
    for (uniform *u: *st->uniforms)
    {
        QByteArray ba = u->name.toUtf8();
        u->id = glGetUniformLocation(prg, ba.constData());
    }
}

void render_stage::update_vertex_buffers(stage_tab *st)
{
    for (vertex_attrib *va: st->vertices->attribs)
    {
        QByteArray ba = va->name.toUtf8();
        va->id = glGetAttribLocation(prg, ba.constData());
    }
}
