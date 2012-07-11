#ifndef UNIFORM_HPP
#define UNIFORM_HPP

#include <QString>

#include "texture_management.hpp"
#include "types.hpp"


class uniform
{
    public:
        enum utype
        {
            t_unknown,

            t_int,
            t_sampler2d,
            t_float,
            t_vec2,
            t_vec3,
            t_vec4,
            t_mat2,
            t_mat3,
            t_mat4
        };

        uniform(void);
        uniform(utype t, const QString &vs, void *trk);
        virtual ~uniform(void) {}

        virtual void assign(void) {}

        utype type;
        QString name, valstr;
        const void *track;
        bool transposed;
        unsigned id;
};

#define create_itype_uniform_class(type_name, set_type, save_type) \
    class type_name##_uniform: public uniform \
    { \
        public: \
            type_name##_uniform(void); \
            virtual ~type_name##_uniform(void) {} \
            void set(set_type v); \
            virtual void assign(void); \
            save_type val; \
    }

#define create_type_uniform_class(type) create_itype_uniform_class(type, const type &, type)

create_type_uniform_class(int);
create_type_uniform_class(float);
create_type_uniform_class(vec2);
create_type_uniform_class(vec3);
create_type_uniform_class(vec4);
create_type_uniform_class(mat2);
create_type_uniform_class(mat3);
create_type_uniform_class(mat4);

create_itype_uniform_class(sampler2d, managed_texture *, managed_texture *);

#undef create_type_uniform_class
#undef create_itype_uniform_class

#endif
