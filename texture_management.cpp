#include <QtCore>

#include "opengl.hpp"
#include "texture_management.hpp"

QList<managed_texture *> textures;


managed_texture::managed_texture(unsigned i, const QString &n, bool od):
    name(n), id(i), usage(0), ondemand(od)
{}

managed_texture::~managed_texture(void)
{
    glDeleteTextures(1, &id);
}


managed_texture *add_texture(unsigned id, const QString &name, bool ondemand)
{
    QByteArray n = name.toUtf8();
    managed_texture *mt = new managed_texture(id, name, ondemand);
    textures.push_back(mt);
    return mt;
}

static GLuint qt_load_texture(const QString &filename);

managed_texture *load_texture(const QString &path)
{
    // FIXME: Avoid multiple loading of the same file

    QFileInfo path_fi(path);

    if (!path_fi.isReadable())
        return NULL;

    unsigned nid = qt_load_texture(path);
    return nid ? add_texture(nid, path, true) : NULL;
}

void use_texture(managed_texture *tex)
{
    tex->usage++;
}

void unuse_texture(managed_texture *tex)
{
    if ((--tex->usage <= 0) && tex->ondemand)
        remove_texture(tex);
}

void sweep_textures(void)
{
    for (managed_texture *tex: textures)
        if (tex->ondemand && !tex->usage)
            remove_texture(tex);
}

void remove_texture(managed_texture *tex)
{
    textures.removeOne(tex);
    delete tex;
}


static int tmu = 0;

void reset_texture_bindings(void)
{
    tmu = 0;
}

int bind_texture(managed_texture *tex)
{
    // FIXME: Overflow check
    glActiveTexture(GL_TEXTURE0 + tmu);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    return tmu++;
}



static GLuint qt_load_texture(const QString &filename)
{
    QImage img(filename);

    if (img.isNull())
        return 0;

    img = img.convertToFormat(QImage::Format_ARGB32);


    int w = img.width(), h = img.height();

    uint8_t *buf = new uint8_t[w * h * 4];

    for (int y = 0; y < h; y++)
        memcpy(&buf[(h - y - 1) * w * 4], img.constScanLine(y), w * 4);


    GLuint tex_id;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buf);

    delete buf;


    return tex_id;
}
