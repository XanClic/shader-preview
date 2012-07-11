#define GL_GLEXT_PROTOTYPES

#include <cstdlib>

#include "opengl.hpp"
#include "renderer.hpp"
#include "vertex_info.hpp"

vertex_info::vertex_info(void)
{
    if (ogl_maj > 3)
        glGenBuffers(1, &buffer);
}

vertex_info::~vertex_info(void)
{
    if (ogl_maj > 3)
        glDeleteBuffers(1, &buffer);
}


void vertex_info::update_buffer(void)
{
    if (ogl_maj <= 3)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    size_t data_sz = 0;
    for (vertex_attrib *va: attribs)
        data_sz += va->len() * va->epv * sizeof(float);

    glBufferData(GL_ARRAY_BUFFER, data_sz, NULL, GL_STATIC_DRAW);

    uint8_t *dst = static_cast<uint8_t *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    for (vertex_attrib *va: attribs)
    {
        size_t va_sz = va->len() * va->epv * sizeof(float);
        memcpy(dst, va->ptr(), va_sz);
        dst += va_sz;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}
