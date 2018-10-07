#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

#include <math.h>



class mat4_t
{
    public:

    union
    {
        float m_floats[4][4];
        vec4_t m_rows[4];
    };

    mat4_t();

    mat4_t(const vec4_t &r0, const vec4_t &r1, const vec4_t &r2, const vec4_t &r3) {m_rows[0] = r0; m_rows[1] = r1; m_rows[2] = r2; m_rows[3] = r3;}

};




class mat3_t
{
    public:

    union
    {
        vec3_t m_rows[3];
        float m_floats[3][3];
    };


    mat3_t(){}
    mat3_t(const vec3_t &r0, const vec3_t &r1, const vec3_t &r2) {m_rows[0] = r0; m_rows[1] = r1; m_rows[2] = r2;}


    inline void identity();

    inline void transpose();

    inline vec3_t operator [] (const int i) const;

    inline vec3_t& operator [] (const int i);

    inline void rotate_x(float angle, int set);

    inline void rotate_y(float angle, int set);

    inline void rotate_z(float angle, int set);
};


inline void mat3_t::identity()
{
    m_rows[0] = vec3_t(1.0, 0.0, 0.0);
    m_rows[1] = vec3_t(0.0, 1.0, 0.0);
    m_rows[2] = vec3_t(0.0, 0.0, 1.0);
}


inline void mat3_t::rotate_x(float angle, int set)
{
    float s;
    float c;

    s = sin(angle * 3.14159265);
    c = cos(angle * 3.14159265);


    if(set)
    {
        m_floats[1][1] = c;
        m_floats[1][2] = s;
        m_floats[2][1] = -s;
        m_floats[2][2] = c;
    }
}

inline void mat3_t::rotate_y(float angle, int set)
{
    float s;
    float c;

    s = sin(angle * 3.14159265);
    c = cos(angle * 3.14159265);


    if(set)
    {
        m_floats[0][0] = c;
        m_floats[0][2] = s;
        m_floats[2][0] = -s;
        m_floats[2][2] = c;
    }
}

inline void mat3_t::rotate_z(float angle, int set)
{

}



inline void mat3_t::transpose()
{
   float t;

   t = m_floats[0][1];
   m_floats[0][1] = m_floats[1][0];
   m_floats[1][0] = t;

   t = m_floats[0][2];
   m_floats[0][2] = m_floats[2][0];
   m_floats[2][0] = t;

   t = m_floats[1][2];
   m_floats[1][2] = m_floats[2][1];
   m_floats[2][1] = t;
}

inline vec3_t mat3_t::operator [] (const int i) const
{
    return m_rows[i];
}

inline vec3_t& mat3_t::operator [] (const int i)
{
    return m_rows[i];
}


static mat3_t operator * (const mat3_t &a, const mat3_t &b)
{
    mat3_t r;

    int i;

    for(i = 0; i < 3; i++)
    {
        r.m_floats[i][0] = a.m_floats[i][0] * b.m_floats[0][0] +
                           a.m_floats[i][1] * b.m_floats[1][0] +
                           a.m_floats[i][2] * b.m_floats[2][0];


        r.m_floats[i][1] = a.m_floats[i][0] * b.m_floats[0][1] +
                           a.m_floats[i][1] * b.m_floats[1][1] +
                           a.m_floats[i][2] * b.m_floats[2][1];


        r.m_floats[i][2] = a.m_floats[i][0] * b.m_floats[0][2] +
                           a.m_floats[i][1] * b.m_floats[1][2] +
                           a.m_floats[i][2] * b.m_floats[2][2];
    }

    return r;
}





#endif // MATRIX_H









