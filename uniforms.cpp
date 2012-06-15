#include <cstdio>
#include <cstring>

#include <QtCore>

#include "uniforms.hpp"


static uniform::utype name2type(const QString &t)
{
    if (t == "sampler2D")
        return uniform::t_integer;
    else if (t == "float")
        return uniform::t_float;
    else
        return uniform::t_unk;
}

uniform::uniform(const QString &t, const QString &n):
    type(t), name(n), id(0)
{
    proc_type = name2type(t);

    switch (proc_type)
    {
        case t_integer:
            value = "0";
            proc_value.setValue(0);
            break;
        case t_float:
            value = "0.0";
            proc_value.setValue(0.f);
            break;
        case t_unk:
            throw 42; // FIXME
    }

    QByteArray arr = n.toUtf8();
    proc_name = strdup(arr.constData());
}

uniform::~uniform(void)
{
    delete proc_name;
}

uniform &uniform::operator=(QVariant val)
{
    proc_value = val;
    value = val.toString();

    return *this;
}
