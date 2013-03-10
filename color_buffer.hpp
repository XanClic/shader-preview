#ifndef COLOR_BUFFER_HPP
#define COLOR_BUFFER_HPP

#include <glibmm.h>

#include "texture_management.hpp"


class color_buffer
{
    public:
        color_buffer(const Glib::ustring &stage_name, const Glib::ustring &n);
        ~color_buffer(void);

        void resize(int w, int h);

        Glib::ustring name;
        unsigned id;

        managed_texture *mt;
};

#endif
