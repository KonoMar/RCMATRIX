#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>	/* setw(int) */
using namespace std;

class OutOfMem{};
class IndexOutOfRange{};
class WrongDim{};

class CMatrix
{
    struct rcmatrix;
    rcmatrix *mat;
    
public:
    class Proxy; // umozliwiaja rozroznienie read i write, dzieki
    class Proxy2; // temu podczas odczytywania nie bedzie wykonywana kopia
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

struct CMatrix::rcmatrix
{
    int rows;
    int columns;
    int n;
    double **data;
    
    rcmatrix(int r, int c, double diag, double fill = 0)
    {
        rows = r;
        columns = c;
        data = new double*[rows];
        try
        {
            for(int i = 0; i < rows; i++)
                data[i] = new double[columns];
        }
    
        catch(bad_alloc)
        {
            delete [] data;
            throw;
        }
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                if(i == j)
                    data[i][j] = diag;
                else
                    data[i][j] = fill;
            }
        }   
        n = 1;	  
    };
    
    
    rcmatrix(int r, int c, double** array)
    {
        n = 1;
        rows = r;
        columns = c;
        
        // tworzenie dwuwymiarowej tablicy dynamicznej
        data = new double*[rows];
        for(int i = 0; i < rows; i++)
            data[i] = new double[columns];
        
        // wypelnianie calej tablicy wartosciami
        for(int i = 0; i < rows; i++)
        {
            for(int k = 0; k < columns; k++)
            {
                data[i][k] = array[i][k];
            }
        }
    };
    
    ~rcmatrix()
    {
        for(int i = 0; i < rows; i++)
            delete [] data[i];
        delete [] data;
    };
    
    rcmatrix* detach()
    {
        if(n == 1)
            return this;
        rcmatrix* t = new rcmatrix(rows, columns, data); // glebokie kopiowanie
        n--;
        return t;
    };
    
    void assign(int r, int c, double **array)
    {
        if(rows != r || columns != c)
        {
            // tworzenie dwuwymiarowej tablicy dynamicznej
            double** tmp = new double*[r];
            for(int i = 0; i < r; i++)
                tmp[i] = new double[c];
            
            // wypelnianie calej tablicy wartosciami
            for(int i = 0; i < r; i++)
            {
                for(int k = 0; k < c; k++)
                {
                    tmp[i][k] = array[i][k];
                }
            }
            delete [] data;
            data = tmp;
        }
        else
            for(int i = 0; i < rows; i++){
                for(int k = 0; k < columns; k++){
                    data[i][k] = array[i][k];
                }
            }
    }
    
    private:
    rcmatrix(const rcmatrix&);
    rcmatrix& operator=(const rcmatrix&);
};

CMatrix::CMatrix(fstream& f)
{
    int r;
    int c;
    
    f >> r;
    f >> c;
    
    if(r <= 0 || c <= 0)
        throw IndexOutOfRange();	// Uwaga, rzucam wyjatek!
    
    // tworzenie dwuwymiarowej pomocniczej tablicy dynamicznej
    double **tmp;
    tmp = new double*[r];
    for(int i = 0; i < r; i++){
        try
        {
            tmp[i] = new double[c];
        }
        catch(bad_alloc)
        {
            for(int k = i; k < i; i++)
                delete [] tmp[k];
            delete [] tmp;
        }
    }
    // wypelnienie pomocniczej tablicy wartosciami z pliku
    for(int i = 0; i < r; i++){
        for(int k = 0; k < c; k++){
            f >> tmp[i][k];
        }
    }
    try
    {
        mat = new rcmatrix(r, c, tmp);
    }
    catch(bad_alloc)
    {
        for(int i = 0; i < r; i++)
            delete [] tmp[i];
        delete [] tmp;
    }
    
    for(int i = 0; i < r; i++)
        delete [] tmp[i];
    delete [] tmp;
}

inline CMatrix::CMatrix(int r, int c, double val, double val2)
{
    mat = new rcmatrix(r, c, val, val2);
}
inline CMatrix::CMatrix(const CMatrix& x)
{
    x.mat->n++;
    mat = x.mat;
}
inline CMatrix::~CMatrix(){
    if(--(mat->n) == 0)
        delete mat;
}

CMatrix& CMatrix::operator=(const CMatrix& x)
{
    if(mat->n == 1)
    {
        //mat->assign(x.mat->rows, x.mat->columns, x.mat->data);
        delete mat;
        mat = x.mat;
        mat->n++;
    }
    else
    {
        //rcmatrix *m = new rcmatrix(x.mat->rows, x.mat->columns, x.mat->data);
        mat->n--;
        mat = x.mat;
    }
    return *this;
}

CMatrix CMatrix::operator+(const CMatrix& x) const
{
    CMatrix tmp(*this);
    for(int i = 0; i < mat->rows; i++)
    {
        for(int k = 0; k < mat->columns; k++)
        {
            tmp.mat->data[i][k] = x.mat->data[i][k];
        }
    }
    
    if(--mat->data == 0)
        delete mat;
    
    return tmp;
}

CMatrix CMatrix::operator*(const CMatrix& x) const
{
    if(mat->columns != x.mat->rows)	// rzucam wyjatek!
        throw WrongDim();
    
    //czysta macierz z zerami
    CMatrix tmp(mat->rows, x.mat->columns, 0, 0);
    
    for(int r = 0; r < mat->rows; r++)
    {
        for(int c = 0; c < x.mat->columns; c++)
        {
            for(int i = r; i < mat->columns; i++)
            {
                tmp.mat->data[r][c] += mat->data[r][i]
                * x.mat->data[i][c];
            }
        }
    }
    return tmp;
    cout << "Hej";
}

ostream& operator<<(ostream& o, const CMatrix& m) //operator wypisania
{
    for(int i = 0; i < m.mat->rows; i++)
    {
        for(int k = 0; k < m.mat->columns; k++)
        {
            o << setw(5) << m.mat->data[i][k] << " "; // dlugosc wyswietlania
        }
        o << endl;
    }
    return o;
}

class CMatrix::Proxy2
{
    friend class CMatrix;
    CMatrix& m;
    int i;
    int k;
    Proxy2 (CMatrix& mm, int ii, int kk): m(mm), i(ii), k(kk) {};
    public:
    operator double() const
    {
        cout << "operator double() const" << endl; // konstruktor
        return m.read(i, k);
    }
    CMatrix::Proxy2& operator=(double c)
    {
    cout << "void operator = (double c)" << endl; // przypisanie
    m.write(i, k, c);
    return *this;
    }
    CMatrix::Proxy2& operator=(const Proxy2& ref) // przypisanie
    {
        return operator=((double)ref);
    }
};
  // *********
inline double CMatrix::read(int i, int k) const
{
    return mat->data[i][k];
}
inline void CMatrix::write(int w, int k, double d)
{
mat = mat->detach();
            mat->data[w][k] = d;
}
 //**************
        // Przeladowanie operatora [][]
class CMatrix::Proxy
{
    public:
    Proxy(CMatrix& t, int row_index) : r(row_index), m(t){ }
            
            /*  double& operator[](int index) {
             return _array[index];
             }*/
    Proxy2 operator[](int col_index)
    {
    return Proxy2(m, r, col_index);
    }
        private:
            int r;
            CMatrix &m;
};
        
CMatrix::Proxy CMatrix::operator[](int index)
{
    return Proxy(*this, index); // indeksowanie
}
        
#endif /* __MATRIX_H__ */
