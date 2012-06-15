#include <cstdio>

#include <QString>

#include <GL/gl.h>

#include "textures.hpp"


static GLuint load_bmp_texture(const char *path);

unsigned load_texture(const QString &path, const QString &suffix)
{
    QByteArray path_arr = path.toUtf8();

    if (suffix == "bmp")
        return load_bmp_texture(path_arr.constData());

    return 0;
}

static GLuint load_bmp_texture(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return 0;

    fseek(fp, 10, SEEK_SET);
    uint32_t ofs;
    fread(&ofs, sizeof(ofs), 1, fp);

    int32_t w, h;
    fseek(fp, 4, SEEK_CUR);
    fread(&w, sizeof(w), 1, fp);
    fread(&h, sizeof(h), 1, fp);

    uint16_t bpp;
    fseek(fp, 2, SEEK_CUR);
    fread(&bpp, sizeof(bpp), 1, fp);

    if ((bpp != 24) && (bpp != 32))
        return 0;


    fseek(fp, 0, SEEK_END);
    long lof = ftell(fp);
    fseek(fp, ofs, SEEK_SET);


    unsigned char *content = new unsigned char[lof - ofs];
    fread(content, lof - ofs, 1, fp);
    fclose(fp);


    bool neg_h = h < 0;

    if (neg_h)
        h = -h;

    unsigned char *buf = new unsigned char[w * h * 4];


    int i = 0, o = 0;

    // I have absolutely no clue, why this is the wrong way round
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
                int o = (y * w + x) * 4;

                buf[o++] = content[i++];
                buf[o++] = content[i++];
                buf[o++] = content[i++];
                buf[o++] = (bpp == 32) ? content[i++] : 0;
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
