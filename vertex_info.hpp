#ifndef VERTEX_INFO_HPP
#define VERTEX_INFO_HPP

#include <QList>
#include <QString>
#include <QVector>

#include "types.hpp"


class vertex_attrib
{
    public:
        vertex_attrib(const QString &a_name): name(a_name) { epv = 0; }
        virtual ~vertex_attrib(void) {}

        virtual const void *ptr(void) { return NULL; }
        virtual size_t len(void) { return 0; }

        QString name;
        int id;
        int epv; // elements per vertex
};

// TODO: Remove #else when gcc 4.7 works on mingw with qt
#ifndef OLD_CXX11
#define create_type_class(type, e_p_v) \
    class vertex_attrib_##type: public vertex_attrib \
    { \
        public: \
            vertex_attrib_##type(const QString &vaname): vertex_attrib(vaname) { epv = e_p_v; } \
            virtual ~vertex_attrib_##type(void) final override {} \
            virtual const void *ptr(void) final override { return values.constData(); } \
            virtual size_t len(void) final override { return values.size(); } \
            QVector<type> values; \
    }
#else
#define create_type_class(type, e_p_v) \
    class vertex_attrib_##type: public vertex_attrib \
    { \
        public: \
            vertex_attrib_##type(const QString &vaname): vertex_attrib(vaname) { epv = e_p_v; } \
            virtual ~vertex_attrib_##type(void) {} \
            virtual const void *ptr(void) { return values.constData(); } \
            virtual size_t len(void) { return values.size(); } \
            QVector<type> values; \
    }
#endif

create_type_class(float, 1);
create_type_class(vec2,  2);
create_type_class(vec3,  3);
create_type_class(vec4,  4);

#undef create_type_class

class vertex_info
{
    public:
        vertex_info(void);
        ~vertex_info(void);

        void update_buffer(void);

        QList<vertex_attrib *> attribs;
        unsigned buffer;
};

#endif
