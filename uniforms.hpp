#ifndef UNIFORMS_HPP
#define UNIFORMS_HPP

#include <QString>
#include <QVariant>


class uniform
{
    public:
        enum utype
        {
            t_unk,

            t_integer,
            t_float,
            t_vec2,
            t_vec3,
            t_vec4
        };

        uniform(const QString &t, const QString &n);
        ~uniform(void);

        uniform &operator=(QVariant value);
        template<typename T> T v(void) { return proc_value.value<T>(); }

        QString type, name, value;
        utype proc_type;
        const char *proc_name;
        QVariant proc_value;
        int id;
};

#endif
