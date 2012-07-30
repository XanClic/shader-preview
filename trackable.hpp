#ifndef TRACKABLE_HPP
#define TRACKABLE_HPP

#include <QList>
#include <QString>

#include "types.hpp"
#include "uniform.hpp"


class trackable
{
    public:
        trackable(uniform::utype t, const QString &n, void *p): type(t), name(n), ptr(p) {}

        uniform::utype type;
        QString name;
        void *ptr;
};


trackable *add_trackable(float *v, const QString &name);
trackable *add_trackable(int *v, const QString &name);
trackable *add_trackable(mat3 *v, const QString &name);
trackable *add_trackable(mat4 *v, const QString &name);
void remove_trackable(trackable *t);


extern QList<trackable *> trackables;

#endif
