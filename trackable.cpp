#include <vector>

#include <glibmm.h>

#include "trackable.hpp"


using namespace Glib;
using namespace std;


vector<trackable *> trackables;


trackable *add_trackable(float *v, const ustring &name)
{
    trackable *t = new trackable(uniform::t_float, name, v);
    trackables.push_back(t);
    return t;
}

trackable *add_trackable(int *v, const ustring &name)
{
    trackable *t = new trackable(uniform::t_int, name, v);
    trackables.push_back(t);
    return t;
}

trackable *add_trackable(mat3 *v, const ustring &name)
{
    trackable *t = new trackable(uniform::t_mat3, name, v->d);
    trackables.push_back(t);
    return t;
}

trackable *add_trackable(mat4 *v, const ustring &name)
{
    trackable *t = new trackable(uniform::t_mat4, name, v->d);
    trackables.push_back(t);
    return t;
}

void remove_trackable(trackable *t)
{
    trackables.erase(std::remove(trackables.begin(), trackables.end(), t), trackables.end());
    delete t;
}
