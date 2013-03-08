#include "opengl.hpp"

#include <cmath>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <gdk/gdk.h>

#include "dialogs.hpp"
#include "renderer.hpp"


extern GtkWidget *main_wnd;


static unsigned compile_shader(GLenum type, const char *src)
{
    unsigned sh = glCreateShader(type);

    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);

    int status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return sh;

    int illen;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1)
        message_dialogs::error("Error compiling shader", "[error reason unknown]");
    else
    {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(sh, illen, NULL, msg);
        msg[illen] = 0;

        if (type == GL_VERTEX_SHADER)
            message_dialogs::error("Error compiling vertex shader", msg);
        else if (type == GL_GEOMETRY_SHADER)
            message_dialogs::error("Error compiling geometry shader", msg);
        else if (type == GL_FRAGMENT_SHADER)
            message_dialogs::error("Error compiling fragment shader", msg);
        else
            message_dialogs::error("Error compiling shader", msg);

        delete msg;
    }

    glDeleteShader(sh);
    return 0;
}


static void resize_wrapper(GtkWidget *output, GdkEvent *evt, renderer *rnd)
{
    (void)output;

    rnd->resize(evt->configure.width, evt->configure.height);
}

static void redraw_wrapper(GtkWidget *output, GdkEvent *evt, renderer *rnd)
{
    (void)output;
    (void)evt;

    rnd->redraw();
}


renderer::renderer(void)
{
    output = gtk_drawing_area_new();

    glconf = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
    gtk_widget_set_gl_capability(output, glconf, NULL, TRUE, GDK_GL_RGBA_TYPE);


    mat_mem = new char[3 * sizeof(mat4) + sizeof(mat3)];
    modelview    = new (reinterpret_cast<mat4 *>(mat_mem)    ) mat4;
    projection   = new (reinterpret_cast<mat4 *>(mat_mem) + 1) mat4;
    it_modelview = new (reinterpret_cast<mat4 *>(mat_mem) + 2) mat4;
    normal_mat   = new (reinterpret_cast<mat4 *>(mat_mem) + 3) mat3;

    modelview->translate(vec3(0.f, 0.f, -3.f));


    g_signal_connect(output, "configure_event", G_CALLBACK(&resize_wrapper), this);
    g_signal_connect(output, "expose_event",    G_CALLBACK(&redraw_wrapper), this);
}


void renderer::initialize_gl(void)
{
    drawable = gtk_widget_get_gl_drawable(output);
    context  = gtk_widget_get_gl_context (output);

    begin_gl();


#ifdef _WIN32
    glewInit();
#endif

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmus);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &vattrs);

    ogl_maj = 0;
    ogl_min = 0;

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


    if (ogl_maj >= 3)
    {
        for (int i = 0; i < vattrs; i++)
            glEnableVertexAttribArray(i); // FIXME


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


    end_gl();
}


void renderer::resize(int w, int h)
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


void renderer::redraw(void)
{
    begin_gl();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    end_gl();

    swap_buffers();
}


void renderer::fbo_display_setting_changed(bool new_state)
{
    (void)new_state;
}
