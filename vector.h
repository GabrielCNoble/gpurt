#ifndef VECTOR_H
#define VECTOR_H


#include <math.h>



/*
===============================================================================================
===============================================================================================
===============================================================================================
*/


class vec3_t
{
    public:

    float m_floats[3];


    vec3_t();
    vec3_t(float x, float y, float z) {m_floats[0] = x; m_floats[1] = y; m_floats[2] = z;}


    inline vec3_t& operator += (const vec3_t &other);

    inline vec3_t& operator -= (const vec3_t &other);

    inline vec3_t& operator *= (const float f);

    inline vec3_t& operator /= (const float f);

    inline float operator [] (const int i) const;

    inline float& operator [] (const int i);

    inline float length() const;
};







inline vec3_t& vec3_t::operator += (const vec3_t &other)
{
    this->m_floats[0] += other.m_floats[0];
    this->m_floats[1] += other.m_floats[1];
    this->m_floats[2] += other.m_floats[2];

    return *this;
}


static inline vec3_t operator + (const vec3_t &a, const vec3_t &b)
{
    return vec3_t(a.m_floats[0] + b.m_floats[0], a.m_floats[1] + b.m_floats[1], a.m_floats[2] + b.m_floats[2]);
}







inline vec3_t& vec3_t::operator -= (const vec3_t &other)
{
    this->m_floats[0] -= other.m_floats[0];
    this->m_floats[1] -= other.m_floats[1];
    this->m_floats[2] -= other.m_floats[2];

    return *this;
}

static inline vec3_t operator - (const vec3_t &a, const vec3_t &b)
{
    return vec3_t(a.m_floats[0] - b.m_floats[0], a.m_floats[1] - b.m_floats[1], a.m_floats[2] - b.m_floats[2]);
}









inline vec3_t& vec3_t::operator *= (const float f)
{
    this->m_floats[0] *= f;
    this->m_floats[1] *= f;
    this->m_floats[2] *= f;

    return *this;
}

static inline vec3_t operator * (const vec3_t &a, const float f)
{
    return vec3_t(a.m_floats[0] * f, a.m_floats[1] * f, a.m_floats[2] * f);
}

static inline vec3_t operator * (const float f, const vec3_t &a)
{
    return vec3_t(a.m_floats[0] * f, a.m_floats[1] * f, a.m_floats[2] * f);
}








inline vec3_t& vec3_t::operator /= (const float f)
{
    this->m_floats[0] /= f;
    this->m_floats[1] /= f;
    this->m_floats[2] /= f;

    return *this;
}

static inline vec3_t operator / (const vec3_t &a, const float f)
{
    return vec3_t(a.m_floats[0] / f, a.m_floats[1] / f, a.m_floats[2] / f);
}

static inline vec3_t operator / (const float f, const vec3_t &a)
{
    return vec3_t(a.m_floats[0] / f, a.m_floats[1] / f, a.m_floats[2] / f);
}







inline float vec3_t::operator [] (const int i) const
{
    return m_floats[i];
}

inline float& vec3_t::operator [] (const int i)
{
    return this->m_floats[i];
}


inline float vec3_t::length() const
{
    return sqrt(m_floats[0] * m_floats[0] + m_floats[1] * m_floats[1] + m_floats[2] * m_floats[2]);
}





static float dot(const vec3_t &a, const vec3_t &b)
{
    return a.m_floats[0] * b.m_floats[0] + a.m_floats[1] * b.m_floats[1] + a.m_floats[2] * b.m_floats[2];
}



static vec3_t normalize(const vec3_t &v)
{
    float len = v.length();
    return v / len;
}


/*
===============================================================================================
===============================================================================================
===============================================================================================
*/



class vec4_t
{
    public:

    float m_floats[4];

    vec4_t();

    vec4_t(float x, float y, float z, float w) {m_floats[0] = x; m_floats[1] = y; m_floats[2] = z; m_floats[3] = w;}
};


#endif // VECTOR_H














