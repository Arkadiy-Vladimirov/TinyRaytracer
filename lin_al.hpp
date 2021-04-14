#ifndef __LIN_AL_HPP__
#define __LIN_AL_HPP__

#include "Image.h"

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

#endif //__LIN_AL_HPP__