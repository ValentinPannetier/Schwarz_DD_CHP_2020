#include <vector>
#include <cmath> 
#include <algorithm> 
#include <stdlib.h> 
#include <stdio.h> 
#include <iostream> 
 

class Triplets
{
    private:
        int m_num_cols ,m_num_rows ; 
        int m_triplets_size ; 
        float* m_entry_data ; 
        int* m_IA ; 
        int* m_IJ ; 
        float* m_values ;
         
    public: 
        Triplets() {} ;
        Triplets(int m_num_rows, int m_num_cols) ;  
        void add(int i , int j , float value) ; 
        void settriplet() ;
        int* get_m_IA() {return m_IA ; } ;
        int* get_m_IJ() {return m_IJ ; } ;
        float* get_m_values(){return m_values ; } ; 
        void ranging() ;
        void show() ;
};


class SparseMatrix
{
    private: 
        int m_num_rows  , m_num_cols ; 
        Triplets* Triplet ;  
        float* m_values ; 
        int* m_IA ; 
        int* m_IJ ; 
    public: 
        SparseMatrix(int number_rows , int number_columns) ; 
        void add_value(int i , int j , float value) ; 
        void set_matrix() ; 
        float* vector_mult(float* vect) ;
        int get_rows_number() {return m_num_rows ;} ;
        int get_cols_number() {return m_num_cols ;} ; 
};

class CG_solver 
{
private: 
  SparseMatrix* m_A ;
  float* m_X ;
  float* m_b ; 
  float tolerance ; 
  int num_iteration_max ; 
public :
  CG_solver(SparseMatrix* , float* ) ;
  void solve() ;
  float* get_solution(){return m_b ;} ;
  
};
class BICGSTAB_solver 
{
private: 
  SparseMatrix* m_A ;
  float* m_X ;
  float* m_b ; 
  float tolerance ; 
  int num_iteration_max ; 
public:
  BICGSTAB_solver(SparseMatrix* , float* ) ;
  void solve() ;
  float* get_solution(){return m_b ;} ; 
};

float dot_product(int, float* , float*) ; 
float* vect_sum(int, float* , float*) ; 
float* dot_real_vect(int , float*) ; 
float norm(int,float*) ; 


