#ifndef COLOR_BUFFER_HPP
#define COLOR_BUFFER_HPP

#include <QString>

#include "texture_management.hpp"


class color_buffer
{
    public:
        color_buffer(const QString &stage_name, const QString &n);
        ~color_buffer(void);

        void resize(int w, int h);

        QString name;
        unsigned id;

        managed_texture *mt;
};

#endif
