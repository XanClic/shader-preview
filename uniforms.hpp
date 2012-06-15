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
            t_float
        };

        uniform(const QString &t, const QString &n);
        ~uniform(void);

        uniform &operator=(QVariant value);

        QString type, name, value;
        utype proc_type;
        const char *proc_name;
        QVariant proc_value;
        int id;
};

#endif
