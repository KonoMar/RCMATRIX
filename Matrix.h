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
        
        catch(...)
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
        rcmatrix* t = new rcmatrix(rows, columns, data);
        n--;
        return t;
    };
    
    void assign(int r, int c,double **array)
    {
        if(rows != r || columns != c)
        {
            // tworzenie dwuwymiarowej tablicy dynamicznej
            double** tmp = new double*[r];
            for(int i = 0; i < r; i++)
                tmp[i] = new double[c];
            
            // wypelnianie calej tablicy wartosciami
            for(int i = 0; i < r; i++){
                for(int k = 0; k < c; k++){
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


//****************************************************************


#endif /* __MATRIX_H__ */
