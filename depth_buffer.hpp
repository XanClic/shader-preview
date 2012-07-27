#ifndef DEPTH_BUFFER_HPP
#define DEPTH_BUFFER_HPP

#include <QString>

#include "texture_management.hpp"


class depth_buffer
{
    public:
        depth_buffer(const QString &stage_name);
        ~depth_buffer(void);

        void resize(int w, int h);

        QString name;
        unsigned id;

        managed_texture *mt;
};

#endif

