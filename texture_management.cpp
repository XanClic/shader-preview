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

static GLuint load_bmp_texture(const char *path);

managed_texture *load_texture(const QString &path)
{
    // FIXME: Avoid multiple loading of the same file

    QFileInfo path_fi(path);

    if (!path_fi.isReadable())
        return NULL;

    QByteArray path_arr = path.toUtf8();

    if (path_fi.suffix().toLower() == "bmp")
    {
        unsigned nid = load_bmp_texture(path_arr.constData());
        return nid ? add_texture(nid, path, true) : NULL;
    }

    return NULL;
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



static GLuint load_bmp_texture(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return 0;

    fseek(fp, 10, SEEK_SET);
    uint32_t ofs;
    if (fread(&ofs, sizeof(ofs), 1, fp) < 1)
    {
        fclose(fp);
        return 0;
    }

    int32_t w, h;
    fseek(fp, 4, SEEK_CUR);
    if ((fread(&w, sizeof(w), 1, fp) < 1) || (fread(&h, sizeof(h), 1, fp) < 1))
    {
        fclose(fp);
        return 0;
    }

    uint16_t bpp;
    fseek(fp, 2, SEEK_CUR);

    if ((fread(&bpp, sizeof(bpp), 1, fp) < 1) || ((bpp != 24) && (bpp != 32)))
    {
        fclose(fp);
        return 0;
    }


    fseek(fp, 0, SEEK_END);
    long lof = ftell(fp);
    fseek(fp, ofs, SEEK_SET);


    unsigned char *content = new unsigned char[lof - ofs];
    if ((long)fread(content, 1, lof - ofs, fp) < lof - ofs)
    {
        delete content;
        fclose(fp);
        return 0;
    }
    fclose(fp);


    bool neg_h = h < 0;

    if (neg_h)
        h = -h;

    unsigned char *buf = new unsigned char[w * h * 4];


    int i = 0, o = 0;

    if (!neg_h)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                buf[o++] = content[i++];
                buf[o++] = content[i++];
                buf[o++] = content[i++];
                buf[o++] = (bpp == 32) ? content[i++] : 0;
            }
        }
    }
    else
    {
        for (int y = h - 1; y >= 0; y--)
        {
            for (int x = 0; x < w; x++)
            {
                // FIXME: I don't know what, but it certainly needs to be fixed
                int op = (y * w + x) * 4;

                buf[op++] = content[i++];
                buf[op++] = content[i++];
                buf[op++] = content[i++];
                buf[op++] = (bpp == 32) ? content[i++] : 0;
            }
        }
    }


    GLuint tex_id;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buf);

    delete content;
    delete buf;


    return tex_id;
}
