#include <cstdio>
#include <cstring>

#include <QtCore>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include "uniforms.hpp"


static uniform::utype name2type(const QString &t)
{
    if (t == "sampler2D")
        return uniform::t_integer;
    else if (t == "float")
        return uniform::t_float;
    else if (t == "vec2")
        return uniform::t_vec2;
    else if (t == "vec3")
        return uniform::t_vec3;
    else if (t == "vec4")
        return uniform::t_vec4;
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
            value = "0";
            proc_value.setValue(0.f);
            break;
        case t_vec2:
            value = "(0, 0)";
            proc_value.setValue(QVector2D(0.f, 0.f));
            break;
        case t_vec3:
            value = "(0, 0, 0)";
            proc_value.setValue(QVector3D(0.f, 0.f, 0.f));
            break;
        case t_vec4:
            value = "(0, 0, 0, 0)";
            proc_value.setValue(QVector4D(0.f, 0.f, 0.f, 0.f));
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

    if (proc_type == t_vec2)
    {
        QVector2D vec = val.value<QVector2D>();
        value = "(" + QString::number(vec.x()) + ", " + QString::number(vec.y()) + ")";
    }
    else if (proc_type == t_vec3)
    {
        QVector3D vec = val.value<QVector3D>();
        value = "(" + QString::number(vec.x()) + ", " + QString::number(vec.y()) + ", " + QString::number(vec.z()) + ")";
    }
    else if (proc_type == t_vec4)
    {
        QVector4D vec = val.value<QVector4D>();
        value = "(" + QString::number(vec.x()) + ", " + QString::number(vec.y()) + ", " + QString::number(vec.z()) + ", " + QString::number(vec.w()) + ")";
    }
    else
        value = val.toString();

    return *this;
}
