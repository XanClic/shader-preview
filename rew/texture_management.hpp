#ifndef TEXTURE_MANAGEMENT_HPP
#define TEXTURE_MANAGEMENT_HPP

#include <QString>


class managed_texture
{
    public:
        managed_texture(unsigned id, const QString &name, bool ondemand = true);
        ~managed_texture(void);

        QString name;
        unsigned id;
        int usage;

        bool ondemand;
};

managed_texture *add_texture(unsigned id, const QString &name, bool ondemand = false);
managed_texture *load_texture(const QString &name);
void use_texture(managed_texture *tex);
void unuse_texture(managed_texture *tex);
void sweep_textures(void);
void remove_texture(managed_texture *tex);

void reset_texture_bindings(void);
int bind_texture(managed_texture *tex);


extern QList<managed_texture *> textures;

#endif
