//#include "partitions.h"
/*
int main()
{
    /*std::string str ; 
    str = std::to_string(1.0) ; 
    std::cout<<str<<std::endl ;
    
    build_meshes("input.txt") ;
    int* nx_local ; 
    int* indices_to_send_right=new int[10] ; 
    int* indices_to_send_left=new int[10] ;
    read_mesh(3,0,nx_local,indices_to_send_right,indices_to_send_left) ;
    for (int i=0 ; i<10 ; i++) 
    {
        std::cout<<indices_to_send_right[i]<<std::endl ;
    }
    std::cout<<std::endl<<std::endl<<std::endl ; 
    
    read_mesh(3,1,nx_local,indices_to_send_right,indices_to_send_left) ;
    for (int i=0 ; i<10 ; i++) 
    {
    
        std::cout<<indices_to_send_left[i]<<std::endl ;
    }
    std::cout<<"**********"<<std::endl ;
    for (int i=0 ; i<10 ; i++) 
    {
        std::cout<<indices_to_send_right[i]<<std::endl ;
    } 
    std::cout<<std::endl<<std::endl<<std::endl ;

    read_mesh(3,1,nx_local,indices_to_send_right,indices_to_send_left) ; 
    for (int i=0 ; i<10 ; i++) 
    {
        std::cout<<indices_to_send_left[i]<<std::endl ;
    }

}
*/ 
#include "Sparse.h"
int main() 
{/*
    Triplets Triplet(10,10) ; 
    Triplet.add(1,1,0.5) ;
    Triplet.add(3,1,0.75) ;
    Triplet.add(2,1,0.6) ;
    Triplet.add(5,1,0.75) ;
    Triplet.add(2,4,0.) ; 
    Triplet.add(4,1,0.75) ;
    Triplet.add(1,5,0.001) ; 
    Triplet.add(3,7 , 0.1) ;
    Triplet.add(4,7 , 0.1) ;
    Triplet.add(8,7 , 0.1) ;
    Triplet.add(8,7 , 0.1) ;
    Triplet.add(6,7 , 0.1) ;
    Triplet.add(4,7 , 0.1) ;
    Triplet.add(9,7 , 0.1) ;
    //Triplet.ranging() ;
    //Triplet.show() ;
    
    Triplet.settriplet() ;
*/ 
    SparseMatrix Spm(4,4) ;
    
    Spm.add_value(0,0,2) ;
    Spm.add_value(1,1,2) ; 
    Spm.add_value(2,2,2) ;
    Spm.add_value(3,3,2) ; 
    Spm.add_value(0,1,2) ;
    Spm.add_value(1,2,1) ; 
    Spm.add_value(2,3,1) ; 
    Spm.set_matrix() ;
    float* x= new float[4] ; 
    /*
    for(int i=0 ; i< 4 ; i++) 
    {
        x[i] = 1 ;
    //    std::cout<<x[i]<<std::endl ;
    }
    
    
    float* result(0) ;
    result = Spm.vector_mult(x) ; 
    //std::cout<<result<<std::endl; 
    
    for (int i=0 ;i<4 ;i++) 
    {
        std::cout<<result[i]<<std::endl ; 
    } 
*/    
}
