#define GL_GLEXT_PROTOTYPES

#include "opengl.hpp"
#include "texture_management.hpp"
#include "types.hpp"
#include "uniform.hpp"

#include <cstdint>

#include <QString>


uniform::uniform(void):
    type(uniform::t_unknown), valstr(""), track(NULL), transposed(false), id(0)
{}

uniform::uniform(utype t, const QString &vs, void *trk):
    type(t), valstr(vs), track(trk), transposed(false), id(0)
{}

int_uniform::int_uniform(void):
    uniform(uniform::t_int, "0", NULL), val(0)
{}

float_uniform::float_uniform(void):
    uniform(uniform::t_float, "0", NULL), val(0.f)
{}

vec2_uniform::vec2_uniform(void):
    uniform(uniform::t_vec2, "(0, 0)", NULL), val(0.f, 0.f)
{}

vec3_uniform::vec3_uniform(void):
    uniform(uniform::t_vec3, "(0, 0, 0)", NULL), val(0.f, 0.f, 0.f)
{}

vec4_uniform::vec4_uniform(void):
    uniform(uniform::t_vec4, "(0, 0, 0, 0)", NULL), val(0.f, 0.f, 0.f, 0.f)
{}

mat2_uniform::mat2_uniform(void):
    uniform(uniform::t_mat2, "((1, 0), (0, 1))", NULL)
{}

mat3_uniform::mat3_uniform(void):
    uniform(uniform::t_mat3, "((1, 0, 0), (0, 1, 0), (0, 0, 1))", NULL)
{}

mat4_uniform::mat4_uniform(void):
    uniform(uniform::t_mat4, "((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1))", NULL)
{}

sampler2d_uniform::sampler2d_uniform(void):
    uniform(uniform::t_sampler2d, "(nil)", NULL), val(NULL)
{}

void int_uniform::set(const int &v)
{
    val = v;
    valstr = QString::number(v);

    track = NULL;
}

void float_uniform::set(const float &v)
{
    val = v;
    valstr = QString::number(v);

    track = NULL;
}

void vec2_uniform::set(const vec2 &v)
{
    val = v;
    valstr = QString("(%1, %2)").arg(v.x).arg(v.y);

    track = NULL;
}

void vec3_uniform::set(const vec3 &v)
{
    val = v;
    valstr = QString("(%1, %2, %3)").arg(v.x).arg(v.y).arg(v.z);

    track = NULL;
}

void vec4_uniform::set(const vec4 &v)
{
    val = v;
    valstr = QString("(%1, %2, %3, %4)").arg(v.x).arg(v.y).arg(v.z).arg(v.w);

    track = NULL;
}

void mat2_uniform::set(const mat2 &v)
{
    val = v;
    valstr = QString("((%1, %2), (%3, %4))")
            .arg(v[0][0]).arg(v[1][0])
            .arg(v[0][1]).arg(v[1][1]);

    transposed = false;
    track = NULL;
}

void mat3_uniform::set(const mat3 &v)
{
    val = v;
    valstr = QString("((%1, %2, %3), (%4, %5, %6), (%7, %8, %9))")
            .arg(v[0][0]).arg(v[1][0]).arg(v[2][0])
            .arg(v[0][1]).arg(v[1][1]).arg(v[2][1])
            .arg(v[0][2]).arg(v[1][2]).arg(v[2][2]);

    transposed = false;
    track = NULL;
}

void mat4_uniform::set(const mat4 &v)
{
    val = v;
    valstr = QString("((%1, %2, %3, %4), (%5, %6, %7, %8), (%9, %10, %11, %12), (%13, %14, %15, %16))")
            .arg(v[0][0]).arg(v[1][0]).arg(v[2][0]).arg(v[3][0])
            .arg(v[0][1]).arg(v[1][1]).arg(v[2][1]).arg(v[3][1])
            .arg(v[0][2]).arg(v[1][2]).arg(v[2][2]).arg(v[3][2])
            .arg(v[0][3]).arg(v[1][3]).arg(v[2][3]).arg(v[3][3]);

    transposed = false;
    track = NULL;
}

void sampler2d_uniform::set(managed_texture *v)
{
    val = v;
    valstr = (v != NULL) ? v->name : "(nil)";
}


void int_uniform::assign(void)
{
    glUniform1i(id, (track == NULL) ? val : *reinterpret_cast<const int *>(track));
}

void float_uniform::assign(void)
{
    glUniform1f(id, (track == NULL) ? val : *reinterpret_cast<const float *>(track));
}

void vec2_uniform::assign(void)
{
    glUniform2fv(id, 1, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void vec3_uniform::assign(void)
{
    glUniform3fv(id, 1, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void vec4_uniform::assign(void)
{
    glUniform4fv(id, 1, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void mat2_uniform::assign(void)
{
    glUniformMatrix2fv(id, 1, transposed, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void mat3_uniform::assign(void)
{
    glUniformMatrix3fv(id, 1, transposed, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void mat4_uniform::assign(void)
{
    glUniformMatrix4fv(id, 1, transposed, (track == NULL) ? val.d : static_cast<const float *>(track));
}

void sampler2d_uniform::assign(void)
{
    if (val != NULL)
        glUniform1i(id, bind_texture(val));
}
