#include <cmath>
#include <cstdint>

#include "types.hpp"


static inline float inv_sqrtf(float val) __attribute__((pure));

static inline float inv_sqrtf(float val)
{
    // Some measurement showed that rsqrtss takes 67 % less time than the
    // infamous integer approximation below (without the Newton part, it's
    // still about 25 % faster).
#ifdef X86_ASSEMBLY
    float ret;
    __asm__ __volatile__ ("rsqrtss %1,%0" : "=x"(ret) : "x"(val));
    return ret;
#else
    // hell yeah
    float half = .5f * val;
    uint32_t i = 0x5F375A86 - (*(uint32_t *)&val >> 1);
    val = *(float *)&i;
    val *= 1.5f - half * val * val;
    return val;
#endif
}


mat4 &mat4::operator*=(const mat4 &m)
{
#ifdef X64_ASSEMBLY
    __asm__ __volatile__ (
            "movaps  0(%0),%%xmm0;"
            "movaps 16(%0),%%xmm1;"
            "movaps 32(%0),%%xmm2;"
            "movaps 48(%0),%%xmm3;"

            "movaps  0(%1),%%xmm4;"
            "movaps 16(%1),%%xmm5;"
            "movaps 32(%1),%%xmm6;"
            "movaps 48(%1),%%xmm7;"

            "pshufd $0x00,%%xmm4,%%xmm8;"
            "pshufd $0x00,%%xmm5,%%xmm9;"
            "pshufd $0x00,%%xmm6,%%xmm10;"
            "pshufd $0x00,%%xmm7,%%xmm11;"
            "mulps  %%xmm0,%%xmm8;"
            "mulps  %%xmm1,%%xmm9;"
            "mulps  %%xmm2,%%xmm10;"
            "mulps  %%xmm3,%%xmm11;"
            "addps  %%xmm9,%%xmm8;"
            "addps  %%xmm11,%%xmm10;"
            "addps  %%xmm10,%%xmm8;"
            "movaps %%xmm8, 0(%0);"

            "pshufd $0x55,%%xmm4,%%xmm8;"
            "pshufd $0x55,%%xmm5,%%xmm9;"
            "pshufd $0x55,%%xmm6,%%xmm10;"
            "pshufd $0x55,%%xmm7,%%xmm11;"
            "mulps  %%xmm0,%%xmm8;"
            "mulps  %%xmm1,%%xmm9;"
            "mulps  %%xmm2,%%xmm10;"
            "mulps  %%xmm3,%%xmm11;"
            "addps  %%xmm9,%%xmm8;"
            "addps  %%xmm11,%%xmm10;"
            "addps  %%xmm10,%%xmm8;"
            "movaps %%xmm8,16(%0);"

            "pshufd $0xAA,%%xmm4,%%xmm8;"
            "pshufd $0xAA,%%xmm5,%%xmm9;"
            "pshufd $0xAA,%%xmm6,%%xmm10;"
            "pshufd $0xAA,%%xmm7,%%xmm11;"
            "mulps  %%xmm0,%%xmm8;"
            "mulps  %%xmm1,%%xmm9;"
            "mulps  %%xmm2,%%xmm10;"
            "mulps  %%xmm3,%%xmm11;"
            "addps  %%xmm9,%%xmm8;"
            "addps  %%xmm11,%%xmm10;"
            "addps  %%xmm10,%%xmm8;"
            "movaps %%xmm8,32(%0);"

            "pshufd $0xFF,%%xmm4,%%xmm8;"
            "pshufd $0xFF,%%xmm5,%%xmm9;"
            "pshufd $0xFF,%%xmm6,%%xmm10;"
            "pshufd $0xFF,%%xmm7,%%xmm11;"
            "mulps  %%xmm0,%%xmm8;"
            "mulps  %%xmm1,%%xmm9;"
            "mulps  %%xmm2,%%xmm10;"
            "mulps  %%xmm3,%%xmm11;"
            "addps  %%xmm9,%%xmm8;"
            "addps  %%xmm11,%%xmm10;"
            "addps  %%xmm10,%%xmm8;"
            "movaps %%xmm8,48(%0)"
            :: "r"(d), "r"(m.d)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm10", "xmm11", "memory"
    );
#else
    float nd[16] = {
        d[ 0] * m.d[ 0] + d[ 4] * m.d[ 1] + d[ 8] * m.d[ 2] + d[12] * m.d[ 3],
        d[ 1] * m.d[ 0] + d[ 5] * m.d[ 1] + d[ 9] * m.d[ 2] + d[13] * m.d[ 3],
        d[ 2] * m.d[ 0] + d[ 6] * m.d[ 1] + d[10] * m.d[ 2] + d[14] * m.d[ 3],
        d[ 3] * m.d[ 0] + d[ 7] * m.d[ 1] + d[11] * m.d[ 2] + d[15] * m.d[ 3],
        d[ 0] * m.d[ 4] + d[ 4] * m.d[ 5] + d[ 8] * m.d[ 6] + d[12] * m.d[ 7],
        d[ 1] * m.d[ 4] + d[ 5] * m.d[ 5] + d[ 9] * m.d[ 6] + d[13] * m.d[ 7],
        d[ 2] * m.d[ 4] + d[ 6] * m.d[ 5] + d[10] * m.d[ 6] + d[14] * m.d[ 7],
        d[ 3] * m.d[ 4] + d[ 7] * m.d[ 5] + d[11] * m.d[ 6] + d[15] * m.d[ 7],
        d[ 0] * m.d[ 8] + d[ 4] * m.d[ 9] + d[ 8] * m.d[10] + d[12] * m.d[11],
        d[ 1] * m.d[ 8] + d[ 5] * m.d[ 9] + d[ 9] * m.d[10] + d[13] * m.d[11],
        d[ 2] * m.d[ 8] + d[ 6] * m.d[ 9] + d[10] * m.d[10] + d[14] * m.d[11],
        d[ 3] * m.d[ 8] + d[ 7] * m.d[ 9] + d[11] * m.d[10] + d[15] * m.d[11],
        d[ 0] * m.d[12] + d[ 4] * m.d[13] + d[ 8] * m.d[14] + d[12] * m.d[15],
        d[ 1] * m.d[12] + d[ 5] * m.d[13] + d[ 9] * m.d[14] + d[13] * m.d[15],
        d[ 2] * m.d[12] + d[ 6] * m.d[13] + d[10] * m.d[14] + d[14] * m.d[15],
        d[ 3] * m.d[12] + d[ 7] * m.d[13] + d[11] * m.d[14] + d[15] * m.d[15]
    };

    memcpy(d, nd, sizeof(nd));
#endif

    return *this;
}


mat4 &mat4::translate(const vec3 &vec)
{
#ifdef X64_ASSEMBLY
    __asm__ __volatile__ (
            "pshufd $0x00,%0,%0;"
            "pshufd $0x00,%1,%1;"
            "pshufd $0x00,%2,%2;"
            "mulps   0(%3),%0;"
            "mulps  16(%3),%1;"
            "mulps  32(%3),%2;"
            "addps  %1,%0;"
            "addps  %2,%0;"
            "addps  48(%3),%0;"
            "movaps %0,48(%3)"
            :: "x"(vec.x), "x"(vec.y), "x"(vec.z), "r"(&d[0])
            : "memory"
    );
#else
    d[12] += vec.x * d[ 0] + vec.y * d[ 4] + vec.z * d[ 8];
    d[13] += vec.x * d[ 1] + vec.y * d[ 5] + vec.z * d[ 9];
    d[14] += vec.x * d[ 2] + vec.y * d[ 6] + vec.z * d[10];
    d[15] += vec.x * d[ 3] + vec.y * d[ 7] + vec.z * d[11];
#endif

    return *this;
}

mat4 &mat4::rotate(float angle, const vec3 &axis)
{
    float x = axis.x, y = axis.y, z = axis.z;
    // this should actually be exact
    float rlen = 1.f / sqrtf(x * x + y * y + z * z);

    x *= rlen;
    y *= rlen;
    z *= rlen;

    float s = sinf(angle);
    float c = cosf(angle);
    float omc = 1.f - c;

#ifdef X64_ASSEMBLY
    float rm[16] = {
        x * x * omc +     c, x * y * omc - z * s, x * z * omc + y * s, 0.f,
        y * x * omc + z * s, y * y * omc +     c, y * z * omc - x * s, 0.f,
        z * x * omc - y * s, z * y * omc + x * s, z * z * omc +     c, 0.f,
                        0.f,                 0.f,                 0.f, 1.f
    };

    *this *= *reinterpret_cast<mat4 *>(rm);
#else
    float _00 = x * x * omc +     c;
    float _01 = x * y * omc - z * s;
    float _02 = x * z * omc + y * s;

    float _10 = y * x * omc + z * s;
    float _11 = y * y * omc +     c;
    float _12 = y * z * omc - x * s;

    float _20 = z * x * omc - y * s;
    float _21 = z * y * omc + x * s;
    float _22 = z * z * omc +     c;

    float n00 = d[ 0] * _00 + d[ 4] * _10 + d[ 8] * _20;
    float n01 = d[ 1] * _00 + d[ 5] * _10 + d[ 9] * _20;
    float n02 = d[ 2] * _00 + d[ 6] * _10 + d[10] * _20;

    float n04 = d[ 0] * _01 + d[ 4] * _11 + d[ 8] * _21;
    float n05 = d[ 1] * _01 + d[ 5] * _11 + d[ 9] * _21;
    float n06 = d[ 2] * _01 + d[ 6] * _11 + d[10] * _21;

    float n08 = d[ 0] * _02 + d[ 4] * _12 + d[ 8] * _22;
    float n09 = d[ 1] * _02 + d[ 5] * _12 + d[ 9] * _22;
    float n10 = d[ 2] * _02 + d[ 6] * _12 + d[10] * _22;

    d[ 0] = n00; d[ 1] = n01; d[ 2] = n02;
    d[ 4] = n04; d[ 5] = n05; d[ 6] = n06;
    d[ 8] = n08; d[ 9] = n09; d[10] = n10;
#endif

    return *this;
}

mat4 &mat4::scale(const vec3 &fac)
{
#ifdef X64_ASSEMBLY
    __asm__ __volatile__ (
            "pshufd $0x00,%0,%0;"
            "pshufd $0x00,%1,%1;"
            "pshufd $0x00,%2,%2;"
            "mulps   0(%3),%0;"
            "mulps  16(%3),%1;"
            "mulps  32(%3),%2;"
            "movaps %0, 0(%3);"
            "movaps %1,16(%3);"
            "movaps %2,32(%3)"
            :: "x"(fac.x), "x"(fac.y), "x"(fac.z), "r"(&d[0])
            : "memory"
    );
#else
    d[0] *= fac.x; d[4] *= fac.y; d[ 8] *= fac.z;
    d[1] *= fac.x; d[5] *= fac.y; d[ 9] *= fac.z;
    d[2] *= fac.x; d[6] *= fac.y; d[10] *= fac.z;
    d[3] *= fac.x; d[7] *= fac.y; d[11] *= fac.z;
#endif

    return *this;
}


float mat3::det(void)
{
    return d[0] * (d[4] * d[8] - d[5] * d[7]) -
           d[3] * (d[1] * d[8] - d[2] * d[7]) +
           d[6] * (d[1] * d[5] - d[2] * d[4]);
}

void mat3::transposed_invert(void)
{
#ifdef X64_ASSEMBLY
    __asm__ __volatile__ (
            "rcpss  %1,%1;"
            "pshufd $0x00,%1,%1;"
            "movaps  0(%0),%%xmm0;"
            "movups 12(%0),%%xmm1;"
            "movups 24(%0),%%xmm2;"

            "pshufd $0x09,%%xmm0,%%xmm3;"
            "pshufd $0x09,%%xmm1,%%xmm4;"
            "pshufd $0x09,%%xmm2,%%xmm5;"
            "pshufd $0x12,%%xmm0,%%xmm6;"
            "pshufd $0x12,%%xmm1,%%xmm7;"
            "pshufd $0x12,%%xmm2,%%xmm8;"

            "movaps %%xmm4,%%xmm9;"
            "movaps %%xmm7,%%xmm10;"
            "mulps  %%xmm8,%%xmm9;"
            "mulps  %%xmm5,%%xmm10;"

            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm3,%%xmm8;"

            "mulps  %%xmm7,%%xmm3;"
            "mulps  %%xmm4,%%xmm6;"

            "subps  %%xmm10,%%xmm9;"
            "subps  %%xmm8,%%xmm5;"
            "subps  %%xmm6,%%xmm3;"

            "mulps  %1,%%xmm9;"
            "mulps  %1,%%xmm5;"
            "mulps  %1,%%xmm3;"

            "movaps %%xmm9, 0(%0);"
            "movups %%xmm5,12(%0);"
            "movups %%xmm3,24(%0)"
            :: "r"(d), "x"(det())
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "memory"
    );
#else
    float nd[9], rcp_det = 1.f / det();

    nd[0] = rcp_det * (d[4] * d[8] - d[5] * d[7]);
    nd[1] = rcp_det * (d[5] * d[6] - d[3] * d[8]);
    nd[2] = rcp_det * (d[3] * d[7] - d[4] * d[6]);

    nd[3] = rcp_det * (d[2] * d[7] - d[1] * d[8]);
    nd[4] = rcp_det * (d[0] * d[8] - d[2] * d[6]);
    nd[5] = rcp_det * (d[1] * d[6] - d[0] * d[7]);

    nd[6] = rcp_det * (d[1] * d[5] - d[2] * d[4]);
    nd[7] = rcp_det * (d[2] * d[3] - d[0] * d[5]);
    nd[8] = rcp_det * (d[0] * d[4] - d[1] * d[3]);

    memcpy(d, nd, sizeof(d));
#endif
}


#ifndef X64_ASSEMBLY
#define te(a, b, c) d[a] * d[b] * d[c]
#define row(x, a1, a2, a3, a4, a5, a6, a7, a8, a9) d[x] * (te(a1, a2, a3) + te(a4, a5, a6) + te(a7, a8, a9))
#endif

float mat4::det(void)
{
#ifdef X64_ASSEMBLY
    float ret;

    __asm__ __volatile__ (
            "movaps 16(%1),%%xmm1;"
            "movaps 32(%1),%%xmm2;"
            "movaps 48(%1),%%xmm3;"

            "pshufd $0x01,%%xmm1,%%xmm4;"
            "pshufd $0x9E,%%xmm2,%%xmm7;"
            "pshufd $0x7B,%%xmm3,%%xmm8;"
            "mulps  %%xmm4,%%xmm7;"
            "mulps  %%xmm8,%%xmm7;"
            "pshufd $0x5A,%%xmm1,%%xmm5;"
            "pshufd $0x33,%%xmm2,%%xmm8;"
            "pshufd $0x8D,%%xmm3,%%xmm9;"
            "mulps  %%xmm5,%%xmm8;"
            "mulps  %%xmm9,%%xmm8;"
            "pshufd $0xBF,%%xmm1,%%xmm6;"
            "pshufd $0x49,%%xmm2,%%xmm9;"
            "pshufd $0x12,%%xmm3,%%xmm10;"
            "mulps  %%xmm6,%%xmm9;"
            "mulps  %%xmm10,%%xmm9;"
            "addps  %%xmm8,%%xmm7;"
            "addps  %%xmm9,%%xmm7;"

            "pshufd $0x7B,%%xmm2,%%xmm8;"
            "pshufd $0x9E,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm4;"
            "mulps  %%xmm9,%%xmm4;"
            "pshufd $0x8D,%%xmm2,%%xmm8;"
            "pshufd $0x33,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm5;"
            "mulps  %%xmm9,%%xmm5;"
            "pshufd $0x12,%%xmm2,%%xmm8;"
            "pshufd $0x49,%%xmm3,%%xmm9;"
            "mulps  %%xmm8,%%xmm6;"
            "mulps  %%xmm9,%%xmm6;"
            "addps  %%xmm5,%%xmm4;"
            "addps  %%xmm6,%%xmm4;"

            "subps  %%xmm4,%%xmm7;"
            "mulps   0(%1),%%xmm7;"

            "haddps %%xmm7,%%xmm7;"
            "haddps %%xmm7,%%xmm7;"
            "movss  %%xmm7,%0;"
            : "=x"(ret) : "r"(d) : "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10"
    );

    return ret;
#else
    return row( 0,   5, 10, 15,   9, 14,  7,  13,  6, 11) +
           row( 4,   1, 14, 11,   9,  2, 15,  13, 10,  3) +
           row( 8,   1,  6, 15,   5, 14,  3,  13,  2,  7) +
           row(12,   1, 10,  7,   5,  2, 11,   9,  6,  3) -
           row( 0,   5, 14, 11,   9,  6, 15,  13, 10,  7) -
           row( 4,   1, 10, 15,   9, 14,  3,  13,  2, 11) -
           row( 8,   1, 14,  7,   5,  2, 15,  13,  6,  3) -
           row(12,   1,  6, 11,   5, 10,  3,   9,  2,  7);
#endif
}

#ifndef X64_ASSEMBLY
#define le(a1, a2, a3, a4, a5) d[a1] * (d[a2] * d[a3] - d[a4] * d[a5])
#define ele(x, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF) \
        x * (le(a1, a2, a3, a4, a5) + le(a6, a7, a8, a9, aA) + le(aB, aC, aD, aE, aF))
#endif

void mat4::transposed_invert(void)
{
#ifdef X64_ASSEMBLY
    __asm__ __volatile__ (
            "rcpss  %1,%1;"
            "pshufd $0x00,%1,%1;"

            "movaps  0(%0),%%xmm1;"
            "movaps 16(%0),%%xmm2;"
            "movaps 32(%0),%%xmm3;"
            "movaps 48(%0),%%xmm4;"

            "pshufd $0x9E,%%xmm3,%%xmm5;"
            "pshufd $0x7B,%%xmm4,%%xmm6;"
            "pshufd $0x7B,%%xmm3,%%xmm7;"
            "pshufd $0x9E,%%xmm4,%%xmm8;"
            "pshufd $0x01,%%xmm2,%%xmm9;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm9,%%xmm5;"
            "pshufd $0x33,%%xmm3,%%xmm6;"
            "pshufd $0x8D,%%xmm4,%%xmm7;"
            "pshufd $0x8D,%%xmm3,%%xmm8;"
            "pshufd $0x33,%%xmm4,%%xmm9;"
            "pshufd $0x5A,%%xmm2,%%xmm10;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm10,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x49,%%xmm3,%%xmm6;"
            "pshufd $0x12,%%xmm4,%%xmm7;"
            "pshufd $0x12,%%xmm3,%%xmm8;"
            "pshufd $0x49,%%xmm4,%%xmm9;"
            "pshufd $0xBF,%%xmm2,%%xmm10;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm10,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5, 0(%0);"

            "pshufd $0x7B,%%xmm3,%%xmm5;"
            "pshufd $0x9E,%%xmm4,%%xmm6;"
            "pshufd $0x9E,%%xmm3,%%xmm7;"
            "pshufd $0x7B,%%xmm4,%%xmm8;"
            "pshufd $0x01,%%xmm1,%%xmm10;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x8D,%%xmm3,%%xmm6;"
            "pshufd $0x33,%%xmm4,%%xmm7;"
            "pshufd $0x33,%%xmm3,%%xmm8;"
            "pshufd $0x8D,%%xmm4,%%xmm9;"
            "pshufd $0x5A,%%xmm1,%%xmm11;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x12,%%xmm3,%%xmm6;"
            "pshufd $0x49,%%xmm4,%%xmm7;"
            "pshufd $0x49,%%xmm3,%%xmm8;"
            "pshufd $0x12,%%xmm4,%%xmm9;"
            "pshufd $0xBF,%%xmm1,%%xmm12;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,16(%0);"

            "pshufd $0x9E,%%xmm2,%%xmm5;"
            "pshufd $0x7B,%%xmm4,%%xmm6;"
            "pshufd $0x7B,%%xmm2,%%xmm7;"
            "pshufd $0x9E,%%xmm4,%%xmm8;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x33,%%xmm2,%%xmm6;"
            "pshufd $0x8D,%%xmm4,%%xmm7;"
            "pshufd $0x8D,%%xmm2,%%xmm8;"
            "pshufd $0x33,%%xmm4,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x49,%%xmm2,%%xmm6;"
            "pshufd $0x12,%%xmm4,%%xmm7;"
            "pshufd $0x12,%%xmm2,%%xmm8;"
            "pshufd $0x49,%%xmm4,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,32(%0);"

            "pshufd $0x7B,%%xmm2,%%xmm5;"
            "pshufd $0x9E,%%xmm3,%%xmm6;"
            "pshufd $0x9E,%%xmm2,%%xmm7;"
            "pshufd $0x7B,%%xmm3,%%xmm8;"
            "mulps  %%xmm6,%%xmm5;"
            "mulps  %%xmm8,%%xmm7;"
            "subps  %%xmm7,%%xmm5;"
            "mulps  %%xmm10,%%xmm5;"
            "pshufd $0x8D,%%xmm2,%%xmm6;"
            "pshufd $0x33,%%xmm3,%%xmm7;"
            "pshufd $0x33,%%xmm2,%%xmm8;"
            "pshufd $0x8D,%%xmm3,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm11,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"
            "pshufd $0x12,%%xmm2,%%xmm6;"
            "pshufd $0x49,%%xmm3,%%xmm7;"
            "pshufd $0x49,%%xmm2,%%xmm8;"
            "pshufd $0x12,%%xmm3,%%xmm9;"
            "mulps  %%xmm7,%%xmm6;"
            "mulps  %%xmm9,%%xmm8;"
            "subps  %%xmm8,%%xmm6;"
            "mulps  %%xmm12,%%xmm6;"
            "addps  %%xmm6,%%xmm5;"

            "mulps  %1,%%xmm5;"
            "movaps %%xmm5,48(%0);"
            :: "r"(d), "x"(det()) : "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "memory"
    );
#else
    float dt = 1.f / det();

    float nd[16] = {
        ele(dt,   5, 10, 15, 11, 14,    6, 11, 13,  9, 15,    7,  9, 14, 10, 13),
        ele(dt,   1, 11, 14, 10, 15,    2,  9, 15, 11, 13,    3, 10, 13,  9, 14),
        ele(dt,   1,  6, 15,  7, 14,    2,  7, 13,  5, 15,    3,  5, 14,  6, 13),
        ele(dt,   1,  7, 10,  6, 11,    2,  5, 11,  7,  9,    3,  6,  9,  5, 10),
        ele(dt,   4, 11, 14, 10, 15,    6,  8, 15, 11, 12,    7, 10, 12,  8, 14),
        ele(dt,   0, 10, 15, 11, 14,    2, 11, 12,  8, 15,    3,  8, 14, 10, 12),
        ele(dt,   0,  7, 14,  6, 15,    2,  4, 15,  7, 12,    3,  6, 12,  4, 14),
        ele(dt,   0,  6, 11,  7, 10,    2,  7,  8,  4, 11,    3,  4, 10,  6,  8),
        ele(dt,   4,  9, 15, 11, 13,    5, 11, 12,  8, 15,    7,  8, 13,  9, 12),
        ele(dt,   0, 11, 13,  9, 15,    1,  8, 15, 11, 12,    3,  9, 12,  8, 13),
        ele(dt,   0,  5, 15,  7, 13,    1,  7, 12,  4, 15,    3,  4, 13,  5, 12),
        ele(dt,   0,  7,  9,  5, 11,    1,  4, 11,  7,  8,    3,  5,  8,  4,  9),
        ele(dt,   4, 10, 13,  9, 14,    5,  8, 14, 10, 12,    6,  9, 12,  8, 13),
        ele(dt,   0,  9, 14, 10, 13,    1, 10, 12,  8, 14,    2,  8, 13,  9, 12),
        ele(dt,   0,  6, 13,  5, 14,    1,  4, 14,  6, 12,    2,  5, 12,  4, 13),
        ele(dt,   0,  5, 10,  6,  9,    1,  6,  8,  4, 10,    2,  4,  9,  5,  8)
    };

    memcpy(d, nd, sizeof(nd));
#endif
}
