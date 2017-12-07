#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>	/* setw(int) */
using namespace std;

class IndexOutOfRange{};
class WrongDim{};

class CMatrix
{
    struct rcmatrix;
    rcmatrix *mat;
    
public:
    class Proxy;
    class Proxy2;
    CMatrix(fstream& f);
    CMatrix(int r, int c, double val, double val2 = 0);
    CMatrix(const CMatrix& x);
    ~CMatrix();
    CMatrix& operator=(const CMatrix& x);
    CMatrix operator+(const CMatrix& x) const;
    CMatrix operator*(const CMatrix& x) const;
    friend ostream& operator<<(ostream&, const CMatrix&);
    Proxy operator[](int index);
    // operator[](int index);
    inline double read(int i, int k) const;
    inline void write(int w, int k, double d);
};




//****************************************************************


#endif /* __MATRIX_H__ */
