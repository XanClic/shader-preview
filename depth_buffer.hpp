#ifndef DEPTH_BUFFER_HPP
#define DEPTH_BUFFER_HPP

#include <glibmm.h>

#include "texture_management.hpp"


class depth_buffer
{
    public:
        depth_buffer(const Glib::ustring &stage_name);
        ~depth_buffer(void);

        void resize(int w, int h);

        Glib::ustring name;
        unsigned id;

        managed_texture *mt;
};

#endif

