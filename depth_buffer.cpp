#include <QString>

#include "depth_buffer.hpp"
#include "opengl.hpp"
#include "texture_management.hpp"


depth_buffer::depth_buffer(const QString &stage_name)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    mt = add_texture(id, stage_name + "/depth");
}

depth_buffer::~depth_buffer(void)
{
    glDeleteTextures(1, &id);

    remove_texture(mt);
}


void depth_buffer::resize(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, id);
    if (width  < 0) width  = 1024;
    if (height < 0) height = 1024;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}
