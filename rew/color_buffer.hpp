#ifndef COLOR_BUFFER_HPP
#define COLOR_BUFFER_HPP

#include <QString>

#include "texture_management.hpp"


class color_buffer
{
    public:
        color_buffer(const QString &stage_name, const QString &n);
        ~color_buffer(void);

        QString name;
        unsigned id;

        managed_texture *mt;
};

#endif
