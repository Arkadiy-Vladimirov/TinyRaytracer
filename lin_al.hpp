#ifndef __LIN_AL_HPP__
#define __LIN_AL_HPP__

#include "Image.h" //just for uint8_t type
#include <cmath>

//____________vectors_________________
template<unsigned dim, class type>
struct Vec {
private:
    type data[dim];
public:
    Vec() {for(unsigned i = 0; i < dim; ++i) data[i] = type();}
          type& operator[](unsigned int idx)       {if (idx < dim)  return data[idx]; else throw "error: array index out of bounds";}; 
    const type& operator[](unsigned int idx) const {if (idx < dim)  return data[idx]; else throw "error: array index out of bounds";};
    const Vec<dim,type>& operator=(const Vec<dim,type> rv) {for(unsigned i = 0; i < dim; ++i) data[i] = rv[i]; return *this;};
};

typedef Vec<2,unsigned> Vec2un;
typedef Vec<3,float> Vec3f;
typedef Vec<2,float> Vec2f;

template<class type>    //float/int/etc. 2d coordinates vector
struct Vec<2,type> {
    type x, y;

    Vec() : x(type()), y(type()) {};
    Vec(const type fx, const type fy) : x(fx), y(fy) {}
          type& operator[](unsigned int idx);
    const type& operator[](unsigned int idx) const;
    const Vec<2,type>& operator=(const Vec<2,type> rv) {x = rv.x; y = rv.y; return *this;};
};
template<class type>    //float/int/etc. 3d coordinates vector
struct Vec<3,type> {
    type x, y, z;

    Vec() : x(type()), y(type()), z(type()) {};
    Vec(const type fx, const type fy, const type fz) : x(fx), y(fy), z(fz) {}
    Vec(const type f) : x(f), y(f), z(f) {}
          type& operator[](unsigned int idx);
    const type& operator[](unsigned int idx) const;
    const Vec<3,type>& operator=(const Vec<3,type> rv) {x = rv.x; y = rv.y; z = rv.z; return *this;};
};


template<>              //color vector
struct Vec<3,uint8_t> {
    uint8_t r, g, b;

    Vec(const uint8_t fr, const uint8_t fg, const uint8_t fb) : r(fr), g(fg), b(fb) {}
          uint8_t& operator[](unsigned int idx);
    const uint8_t& operator[](unsigned int idx) const;
    const Vec<3,uint8_t>& operator=(const Vec<3,uint8_t> rv) {r = rv.r; g = rv.g; b = rv.b; return *this;};
};

//_________________other_structures____________________________
struct Repere { //different phi not implemented, has to be private
    Vec<3,float> orig;
    Vec<3,float> e1, e2, e3;

    Repere(); //natural R3 basis
    Repere(const Vec<3,float>& f_origin, const Vec<3,float>& f_forward, const float phi = 0); //normalized right triplet 
    Repere(const Vec<3,float>& f_origin, const Vec<3,float>& f_e1, const Vec<3,float> f_e2);  //right triplet
};

//________________vector_operations____________________________
//________________indexing_overload____________________________
template <class type>
inline       type& Vec<2,type>::operator[](unsigned idx) {
         if (idx == 0) return x;
    else if (idx == 1) return y;
    else               throw "error: array index out of bounds";
}

template <class type>
inline const type& Vec<2,type>::operator[](unsigned idx) const {
         if (idx == 0) return x;
    else if (idx == 1) return y;
    else               throw "error: array index out of bounds";
}

template <class type>
inline       type& Vec<3,type>::operator[](unsigned idx) {
         if (idx == 0) return x;
    else if (idx == 1) return y;
    else if (idx == 2) return z;
    else               throw "error: array index out of bounds";
}

template <class type>
inline const type& Vec<3,type>::operator[](unsigned idx) const {
         if (idx == 0) return x;
    else if (idx == 1) return y;
    else if (idx == 2) return z;
    else               throw "error: array index out of bounds";
}

      inline uint8_t& Vec<3,uint8_t>::operator[](unsigned idx) {
         if (idx == 0) return r;
    else if (idx == 1) return g;
    else if (idx == 2) return b;
    else               throw "error: array index out of bounds";
}

inline const uint8_t& Vec<3,uint8_t>::operator[](unsigned idx) const {
         if (idx == 0) return r;
    else if (idx == 1) return g;
    else if (idx == 2) return b;
    else               throw "error: array index out of bounds";
}
//______________________________________________________________


//________________arithmetic_operators_overload_________________
//                        unary:
template <unsigned dim, class type>
Vec<dim,type> operator+(const Vec<dim,type>& a) {
    Vec<dim,type> res;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = a[i];
    return res;
}

template <unsigned dim, class type>
Vec<dim,type> operator-(const Vec<dim,type>& a) {
    Vec<dim,type> res; 
    for (unsigned i = 0; i < dim; ++i)
        res[i] = -a[i];
    return res;
}
//                       binary:
template <unsigned dim, class type>
Vec<dim,type> operator+(const Vec<dim,type>& a, const Vec<dim,type>& b) {
    Vec<dim,type> res;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = a[i] + b[i];
    return res;
}

template <unsigned dim, class type>
Vec<dim,type> operator-(const Vec<dim,type>& a, const Vec<dim,type>& b) {
    Vec<dim,type> res;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = a[i] - b[i];
    return res;
}

template <unsigned dim, class type> //componentwise product
Vec<dim,type> operator*(const Vec<dim,type>& a, const Vec<dim,type>& b) {
    Vec<dim,type> res;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = a[i] * b[i];
    return res;
}

//______________________________________________________________


//_________________special_vector_operations____________________

template <unsigned dim, class type>
Vec<dim,type> operator*(const Vec<dim,type>& a, float alpha);

template <unsigned dim, class type>
Vec<dim,type> operator*(float alpha, const Vec<dim,type>& a);

template <class type>
Vec<3,type> cross(const Vec<3,type>& a, const Vec<3,type>& b);

template <unsigned dim, class type>
type scalar(const Vec<dim,type>& a, const Vec<dim,type>& b); 

template <unsigned dim, class type>
type norm(const Vec<dim,type>& a);

template <unsigned dim, class type>
Vec<3,type> normalize(const Vec<dim,type>& a);

//______________________________________________________________


Repere::Repere() {
    orig = 0;
    e1 = Vec<3,float>(1,0,0); e2 = Vec<3,float>(0,1,0); e3 = Vec<3,float>(0,0,1);
}

Repere::Repere(const Vec<3,float>& f_origin, const Vec<3,float>& f_forward, const float phi) {
    orig = f_origin;

    const Repere basic;

    e1 = normalize(f_forward);

    Vec<3,float> e1_proj = scalar(e1,basic.e1) * basic.e1 + scalar(e1,basic.e2) * basic.e2; //projection on x: <e3,x> = 0 plane
    e2.x = -e1_proj.y; e2.y = e1_proj.x; e3.z = 0; //+90 degrees rotation
    e2 =  (1/(norm(e2)))*e2;

    e3 = cross(e1,e2);
}

Repere::Repere(const Vec<3,float>& f_origin, const Vec<3,float>& f_e1, const Vec<3,float> f_e2){
    orig = f_origin; e1 = f_e1; e2 = f_e2;
    e3 = cross(e1,e2);
}


template <unsigned dim, class type>
Vec<dim,type> operator*(const Vec<dim,type>& a, float alpha) {
    Vec<dim,type> res = a;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = res[i]*alpha;
    return res;
}

template <unsigned dim, class type>
Vec<dim,type> operator*(float alpha, const Vec<dim,type>& a) {
    Vec<dim,type> res = a;
    for (unsigned i = 0; i < dim; ++i)
        res[i] = res[i]*alpha;
    return res;
}

template <class type>
Vec<3,type> cross(const Vec<3,type>& a, const Vec<3,type>& b) {
    return Vec<3,type>(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

template <unsigned dim, class type>
type scalar(const Vec<dim,type>& a, const Vec<dim,type>& b) {
    type res(0);
    for (unsigned i = 0; i < dim; ++i) {
        res = res + a[i]*b[i];
    }
    return res;
}

template <unsigned dim, class type>
type norm(const Vec<dim,type>& a) {
    return sqrt(scalar(a,a));
}

template <unsigned dim, class type>
Vec<3,type> normalize(const Vec<dim,type>& a) {
    if (norm(a) != 0) 
        return (1/(norm(a)))*a;
    else throw "error: unable to normalize vector (zero norm)";
}

#endif //__LIN_AL_HPP__