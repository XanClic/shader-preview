#ifndef TRACKABLE_HPP
#define TRACKABLE_HPP

#include <vector>

#include <glibmm.h>

#include "types.hpp"
#include "uniform.hpp"


class trackable
{
    public:
        trackable(uniform::utype t, const Glib::ustring &n, void *p): type(t), name(n), ptr(p) {}

        uniform::utype type;
        Glib::ustring name;
        void *ptr;
};


trackable *add_trackable(float *v, const Glib::ustring &name);
trackable *add_trackable(int *v, const Glib::ustring &name);
trackable *add_trackable(mat3 *v, const Glib::ustring &name);
trackable *add_trackable(mat4 *v, const Glib::ustring &name);
void remove_trackable(trackable *t);


extern std::vector<trackable *> trackables;

#endif
