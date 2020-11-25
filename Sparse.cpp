#include "Sparse.h" 

using namespace std ; 

Triplets::Triplets(int num_rows, int num_cols)
{
    m_num_rows = num_rows ; 
    m_num_cols = num_cols ; 
    m_triplets_size = 0 ;
    m_entry_data = (float*) calloc(m_num_rows*m_num_cols,sizeof(float)) ;
    m_IA = (int*) calloc((m_num_rows+1),sizeof(int)) ; 
}

void Triplets::add(int i , int j , float values) 
{
    m_triplets_size++ ; 
    //realloc(m_entry_data , sizeof(m_entry_data)+3*sizeof(float)) ; 
    m_entry_data[3*(m_triplets_size-1)] = (float) i ;
    m_entry_data[3*(m_triplets_size-1)+1] = (float) j ;
    m_entry_data[3*(m_triplets_size-1)+2] = values ;  
    
    
}

void Triplets::show() 
{
    for (int k=0 ; k<m_triplets_size ; k++)
    {
        cout<<m_entry_data[3*k]<<" "<<m_entry_data[3*k+1]<<" "<<m_entry_data[3*k+2]<<endl; 
    }
}

void Triplets::ranging() 
{
    float a1 , a2 , a3 ; 
    float min ; 
    int index ;
    int i ; 
    i=0 ; 
   for  (i = 0 ; i<m_triplets_size ; i++) 
    {
        min = m_entry_data[3*i] ;
	    index = i ;
    
        for (int j=i+1 ; j<m_triplets_size ; j++)
        {
            if (m_entry_data[3*j]<= min)
            {
                min = m_entry_data[3*j] ;
                index =  j ;   
            }
        }
        
       
        a1 = m_entry_data[3*index] ; 
        a2 = m_entry_data[3*index+1] ; 
        a3 = m_entry_data[3*index+2] ; 
        m_entry_data[3*index] = m_entry_data[3*i] ;
        m_entry_data[3*index+1] = m_entry_data[3*i+1] ;
        m_entry_data[3*index+2] = m_entry_data[3*i+2] ; 
        m_entry_data[3*i] = a1 ; 
        m_entry_data[3*i+1] = a2 ;
        m_entry_data[3*i+2] = a3 ;
    }
    /*
    for (int k=0 ; k<m_triplets_size ; k++) 
    {cout<<m_entry_data[3*k]<<" "<<m_entry_data[3*k+1]<<" "<<m_entry_data[3*k+2]<<endl ;}
    */
    cout<<m_entry_data[3*(m_triplets_size-1)+2]<<endl ;  ;
    cout<<"****"<<endl ;
    
}

void Triplets::settriplet() 
{
    ranging() ;
    m_values = (float*) calloc(m_triplets_size,sizeof(float)) ;
    m_IJ = (int*) calloc(m_triplets_size,sizeof(float)) ;
    m_IA[0] = 0 ; 
    int num_elements_per_row;
    int i,j ;
    int index ;  
    i=0 ;
    while (i<m_triplets_size-1)
    {
        j=i+1 ; 
        num_elements_per_row = 1 ; 
        while (m_entry_data[3*j] == m_entry_data[3*i])
        {
            num_elements_per_row++ ;
            j++ ;  
        }
        index = (int) m_entry_data[3*i] ;
        i = j ; 
        m_IA[index+1] = num_elements_per_row  ; 
    }  
    
    if (m_entry_data[3*(m_triplets_size-2)] != m_entry_data[3*(m_triplets_size-1)]) 
    {    
        index = (int) m_entry_data[3*(m_triplets_size-1)] ; 
        m_IA[index+1] = 1 ;  
    }
    for (i=1 ; i<m_num_rows+1 ; i++) 
    {
        m_IA[i] = m_IA[i] + m_IA[i-1] ;
    }
    
    for (int i=0 ; i<m_triplets_size ; i++) 
    {
        m_values[i] =  m_entry_data[3*i+2] ; 
        m_IJ[i] = (int) m_entry_data[3*i+1] ; 

    } 
    cout<<  m_entry_data[3*(m_triplets_size-1)+2]<<endl ;  
    cout<<"****"<<endl ; 
    /*
    for (int i=0 ; i<m_triplets_size ; i++) 
    {
        cout<<m_values[i]<<endl ; 
    }
    */
    
}


SparseMatrix::SparseMatrix(int num_rows , int num_cols)
{
  m_num_rows = num_rows ;
  m_num_cols = num_cols ;
  Triplet = new Triplets(num_rows , num_cols) ;  
}

void SparseMatrix::add_value(int i , int j , float value)
{
  Triplet->add(i,j,value) ; 
}


void SparseMatrix::set_matrix()
{
  Triplet->settriplet() ;
  m_IA = Triplet->get_m_IA() ;
  m_IJ = Triplet->get_m_IJ() ;
  m_values = Triplet->get_m_values() ; 
  /*
  
  for (int i=0 ; i<5 ; i++) 
  {
      cout<<m_IA[i]<<endl ;
  }
  
  for (int i=0 ; i<7 ; i++) 
  {
      cout<<m_IJ[i]<<" "<<m_values[i]<<endl ;
  }
 */

  
}


float* SparseMatrix::vector_mult(float* x)
{
  float* result ;
  result = (float*) calloc(m_num_rows,sizeof(float)) ;

  int num_element_per_row ; 
  int index = 0 ; 
  for (int i=0 ; i<m_num_rows ; i++)
    {
      num_element_per_row = m_IA[i+1] - m_IA[i] ;
      result[i] = 0 ; 
      for (int j=0 ; j<num_element_per_row ; j++)
	{
	  index = index + j ;
	  result[i] += m_values[index]*x[m_IJ[index]] ;
	}
      index++ ; 
    }
    return result ; 
  
}
/* 
float dot_product(int n_size, float* vect1, float* vect2)
{
    float result = 0 ;  
    for (int i=0 ; i<n_size ; i++) 
    {
        result += vect1[i]*vect2[i] ;
    }
    return result ; 
}

float* vect_sum(int n_size, float* vect1, float* vect2) 
{
    float* result = new float[n_size] ; 
    for (int i=0 ; i<n_size ; i++ ) 
    {
        result[i] = vect1[i] + vect2[2] ; 
    }
    return result ; 
}

float* dot_real_vect(int n_size , float alpha , float* vect)
{
    float* result = (float*) calloc(n_size,sizeof(float)) ; 
    for (int i=0 ; i<n_size ; i++) 
    {
        result[i] = alpha* vect[i] ; 
    }
    return result ; 
}

float norm(int n_size , float* vect) 
{
    float result ; 
    result = dot_product(n_size, vect , vect) ; 
    return sqrt(result) ;
}

CG_solver::CG_solver(SparseMatrix* A , float* b)
{
    m_A = A ; 
    m_b = b ; 
    m_X = (float*) calloc(m_A->get_cols_number(),sizeof(float)) ;
    tolerance = 1e-6 ;
    num_iteration_max = 10000 ;   
}

void CG_solver::solve() 
{
    int cols = m_A->get_cols_number() ; 
    int rows = m_A->get_rows_number() ; 
    float* r0 = new float[rows]; 
    float* r1(0) ; 
    float* X0 = new float[cols] ;
    float* X1(0) ; 
    float* p0 = new float[rows] ; 
    float* p1(0) ;  

    for (int i=0 ; i<rows ; i++) 
    {
        r0[i] = m_b[i] ; 
        p0[i] = m_b[i] ; 
    } 
    
    int k = 0 ; 
    float alpha , beta ;   
    while (k<num_iteration_max && norm(m_A->get_rows_number(),r0)) 
    {
        alpha = dot_product(rows,r0,r0)/dot_product(rows,p0,m_A->vector_mult(p0)) ; 
        X1 = vect_sum(rows,X0,dot_real_vect(alpha,p0))  ;
        r1 = vect_sum(rows,r0,dot_real_vect(-alpha,p0)) ; 
        beta = dot_product(rows,r1,r1)/dot_product(rows,r0,r0) ;
        p1 = vect_sum(rows,r1,dot_real_vect(beta,p0)) ; 
        k++ ; 
        for (int i=0 ; i<rows ; i++) 
        {
            r0[i] = r1[i] ; 
            p0[i] = p1[i] ; 
            X0[i] = X1[i] ;  
        }
        delete p1 ; 
        delete r1 ; 
        delete X1 ; 
    }
}

BICGSTAB_solver::BICGSTAB_solver(SparseMatrix* A , float* b)
{
    m_A = A ; 
    m_b = b ; 
    m_X = (float*) calloc(m_A->get_cols_number(),sizeof(float)) ;
    tolerance = 1e-6 ;
    num_iteration_max = 10000 ;    
} 

void BICGSTAB_solver::solve() 
{
    int cols = m_A->get_cols_number() ; 
    float* X0 = new float[cols] ; 
    float* X1(0) ; 
    float* r0 = new float[cols] ; 
    float* r1(0) ; 
    float* r0_bar = m_b ; 
    float* v0 = new float[cols] ; 
    float* v1(0) ; 
    float* p0 = new float[cols] ; 
    float* p1(0) ; 
    float* s ; 
    float* t ; 
    float* h ; 
    float* vect ;  
    float alpha(1), beta , rho0(1) , rho1 , w0(1), w1  ; 
    int k =0 ; 
    for (int i=0 ; i<cols ; i++) 
    {
        r0[i] = m_b[i] ; 
    }
    while (k<num_iteration_delete p1 ;(alpha/w0) ; 
        vect = vect_sum(cols,p0,dot_real_vect(-w0,v0)) ;
        p1 = vect_sum(cols,r0,dot_real_vect(beta,vect)) ;  
        v1  = m_A->vector_mult(p1) ; 
        alpha = rho1/dot_product(cols,r0_bar,v1) ;  
        h = vect_sum(cols,X0,dot_real_vect(alpha,p1)) ; 
        s = vect_sum(cols,r0,dot_real_vect(-alpha,v1)) ; 
        if (norm(cols,s)<= tolerance) 
        {
            break ; 
        }
        t = m_A->vector_mult(s) ; 
        w1 = dot_product(cols,t,s)/dot_product(cols,t,t) ; 
        X1 = vect_sum(cols,h,dot_real_vect(w1,s)) ; 
        r1 = vect_sum(cols,s,dot_real_vect(-w1,t)) ; 
        k++ ; 
        for (int i=0 ; i<cols ;i++) 
        {
            p0[i] = p1[i] ; 
            X0[i] = X1[i] ; 
            v0[i] = v1[i] ; 
            r0[i] = r1[i] ;  
        }
        rho0 = rho1 ; 
        w0 = w1 ; 
        delete p1 ;
        delete X1 ;
        delete v1 ;
        delete r1 ; 
        delete h ; 
        delete s ; 
        delete t ; 
    }

}*/