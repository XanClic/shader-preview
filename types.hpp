#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstring>


class vec4
{
    public:
        vec4(float xv, float yv, float zv, float wv)
        { x = xv; y = yv; z = zv; w = wv; }

        vec4(const float *v)
        { memcpy(d, v, sizeof(d)); }

        vec4(const vec4 &v)
        { memcpy(d, v.d, sizeof(d)); }

        vec4(void)
        { x = 0.f; y = 0.f; z = 0.f; w = 0.f; }


        const float &operator[](int i) const
        { return d[i]; }

        float &operator[](int i)
        { return d[i]; }


        vec4 operator*(float scale) const
        { return vec4(x * scale, y * scale, z * scale, w * scale); }

        vec4 operator+(const vec4 &ov) const
        { return vec4(x + ov.x, y + ov.y, z + ov.z, w + ov.w); }


        union
        {
            struct { float x, y, z, w; };
            struct { float s, t, p, q; };
            struct { float r, g, b, a; };
            float d[4];
        };
};

class vec3
{
    public:
        vec3(float xv, float yv, float zv)
        { x = xv; y = yv; z = zv; }

        vec3(const float *v)
        { memcpy(d, v, sizeof(d)); }

        vec3(const vec3 &v)
        { memcpy(d, v.d, sizeof(d)); }

        vec3(const vec4 &v)
        { memcpy(d, v.d, sizeof(d)); }

        vec3(void)
        { x = 0.f; y = 0.f; z = 0.f; }


        const float &operator[](int i) const
        { return d[i]; }

        float &operator[](int i)
        { return d[i]; }


        vec3 operator*(float scale) const
        { return vec3(x * scale, y * scale, z * scale); }

        vec3 operator+(const vec3 &ov) const
        { return vec3(x + ov.x, y + ov.y, z + ov.z); }

        vec3 &operator+=(const vec3 &ov)
        { x += ov.x; y += ov.y; z += ov.z; return *this; }

        vec3 operator-(void) const
        { return vec3(-x, -y, -z); }

        vec3 operator^(const vec3 &ov) const
        { return vec3(y * ov.z - z * ov.y, z * ov.x - x * ov.z, x * ov.y - y * ov.x); }


        union
        {
            struct { float x, y, z; };
            struct { float s, t, p; };
            struct { float r, g, b; };
            float d[3];
        };
};

class vec2
{
    public:
        vec2(float xv, float yv)
        { x = xv; y = yv; }

        vec2(const float *v)
        { memcpy(d, v, sizeof(d)); }

        vec2(const vec2 &v)
        { memcpy(d, v.d, sizeof(d)); }

        vec2(void)
        { x = 0.f; y = 0.f; }


        const float &operator[](int i) const
        { return d[i]; }

        float &operator[](int i)
        { return d[i]; }


        union
        {
            struct { float x, y; };
            struct { float s, t; };
            struct { float r, g; };
            float d[2];
        };
};


class mat4
{
    public:
        mat4(const vec4 &c1, const vec4 &c2, const vec4 &c3, const vec4 &c4)
        { memcpy(&d[ 0], c1.d, sizeof(c1.d)); memcpy(&d[ 4], c2.d, sizeof(c2.d)); memcpy(&d[ 8], c3.d, sizeof(c3.d)); memcpy(&d[12], c4.d, sizeof(c4.d)); }

        mat4(const float *v)
        { memcpy(d, v, sizeof(d)); }

        mat4(const mat4 &m)
        { memcpy(d, m.d, sizeof(d)); }

        mat4(void)
        { memset(d, 0, sizeof(d)); d[0] = d[5] = d[10] = d[15] = 1.f; }


        const vec4 &operator[](int i) const
        { return *reinterpret_cast<const vec4 *>(&d[i * 4]); }

        vec4 &operator[](int i)
        { return *reinterpret_cast<vec4 *>(&d[i * 4]); }


        mat4 &operator=(const mat4 &m)
        { memcpy(d, m.d, sizeof(d)); return *this; }

        mat4 &operator*=(const mat4 &m);

        mat4 operator*(const mat4 &m) const;
        vec4 operator*(const vec4 &v) const;


        mat4 &translate(const vec3 &vec);
        mat4 &rotate(float angle, const vec3 &axis);
        mat4 &scale(const vec3 &fac);

        mat4 translated(const vec3 &vec) const;
        mat4 rotated(float angle, const vec3 &axis) const;
        mat4 scaled(const vec3 &fac) const;

        float det(void);
        void transposed_invert(void);


        static mat4 translation(const vec3 &vec)
        { return mat4(vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f), vec4(vec.x, vec.y, vec.z, 1.f)); }


        float d[16];
};

class mat3
{
    public:
        mat3(const vec3 &c1, const vec3 &c2, const vec3 &c3)
        { memcpy(&d[0], c1.d, sizeof(c1.d)); memcpy(&d[3], c2.d, sizeof(c2.d)); memcpy(&d[6], c3.d, sizeof(c3.d)); }

        mat3(const float *v)
        { memcpy(d, v, sizeof(d)); }

        mat3(const mat3 &m)
        { memcpy(d, m.d, sizeof(d)); }

        mat3(const mat4 &m)
        { memcpy(&d[0], &m.d[0], sizeof(d[0]) * 3); memcpy(&d[3], &m.d[4], sizeof(d[3]) * 3); memcpy(&d[6], &m.d[8], sizeof(d[6]) * 3); }

        mat3(void)
        { memset(d, 0, sizeof(d)); d[0] = d[4] = d[8] = 1.f; }


        const vec3 &operator[](int i) const
        { return *reinterpret_cast<const vec3 *>(&d[i * 3]); }

        vec3 &operator[](int i)
        { return *reinterpret_cast<vec3 *>(&d[i * 3]); }


        mat3 &operator=(const mat3 &m)
        { memcpy(d, m.d, sizeof(d)); return *this; }

        mat3 &operator=(const mat4 &m)
        { memcpy(&d[0], &m.d[0], sizeof(d[0]) * 3); memcpy(&d[3], &m.d[4], sizeof(d[3]) * 3); memcpy(&d[6], &m.d[8], sizeof(d[6]) * 3); return *this; }


        vec3 operator*(const vec3 &v) const;


        float det(void);
        void transposed_invert(void);


        float d[9];
};

class mat2
{
    public:
        mat2(const vec2 &c1, const vec2 &c2)
        { memcpy(&d[0], c1.d, sizeof(c1.d)); memcpy(&d[2], c2.d, sizeof(c2.d)); }

        mat2(const float *v)
        { memcpy(d, v, sizeof(d)); }

        mat2(const mat2 &m)
        { memcpy(d, m.d, sizeof(d)); }

        mat2(void)
        { d[1] = d[2] = 0.f; d[0] = d[3] = 1.f; }


        const vec2 &operator[](int i) const
        { return *reinterpret_cast<const vec2 *>(&d[i * 2]); }

        vec2 &operator[](int i)
        { return *reinterpret_cast<vec2 *>(&d[i * 2]); }


        float d[4];
};

#endif
