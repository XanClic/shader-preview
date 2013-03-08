#ifndef TEXTURE_MANAGEMENT_HPP
#define TEXTURE_MANAGEMENT_HPP

#include <list>


class managed_texture
{
    public:
        managed_texture(unsigned id, const char *name, bool ondemand = true);
        ~managed_texture(void);

        char *name;
        unsigned id;
        int usage;

        bool ondemand;
};

managed_texture *add_texture(unsigned id, const char *name, bool ondemand = false);
managed_texture *load_texture(const char *name);
void use_texture(managed_texture *tex);
void unuse_texture(managed_texture *tex);
void sweep_textures(void);
void remove_texture(managed_texture *tex);

void reset_texture_bindings(void);
int bind_texture(managed_texture *tex);


extern std::list<managed_texture *> textures;

#endif
