#include <QString>

#include "color_buffer.hpp"
#include "opengl.hpp"
#include "texture_management.hpp"


color_buffer::color_buffer(const QString &stage_name, const QString &n):
    name(n)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);

    mt = add_texture(id, stage_name + "/" + n);
}

color_buffer::~color_buffer(void)
{
    glDeleteTextures(1, &id);

    remove_texture(mt);
}


void color_buffer::resize(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, id);
    if (width  < 0) width  = 1024;
    if (height < 0) height = 1024;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
}
