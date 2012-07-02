#include <QList>
#include <QString>

#include "trackable.hpp"


QList<trackable *> trackables;


trackable *add_trackable(mat3 *v, const QString &name)
{
    trackable *t = new trackable(uniform::t_mat3, name, v->d);
    trackables.push_back(t);
    return t;
}

trackable *add_trackable(mat4 *v, const QString &name)
{
    trackable *t = new trackable(uniform::t_mat4, name, v->d);
    trackables.push_back(t);
    return t;
}

void remove_trackable(trackable *t)
{
    trackables.removeOne(t);
    delete t;
}
