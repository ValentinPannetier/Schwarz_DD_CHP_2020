  #include "partitions.h"
  #include <string>

  using namespace std ; 

  void charge(int me, int n, int np, int& iBeg, int& iEnd)
  {
      
      int r = n % np;
      if (me < r) {
          iBeg = me*(n/np + 1);
          iEnd = iBeg + (n/np + 1) - 1;
      }
      else {
          iBeg = r + me * (n/np);
          iEnd = iBeg + (n/np) - 1;
      }
      
  }


  //taille de boundary_global_indices = 2*num_proc 
  //taille de column_indexes_to_send = 2*num_proc-2

  void partitions(int num_points ,int num_procs, int recovery_size , int* boundary_global_indices)
  {
      int partitionning1 , partitionning2 ; 
      if (recovery_size%2 ==0) 
      {
          partitionning1 = recovery_size/2 ;
          partitionning2 = recovery_size/2 ;
      }
      else
      {
          partitionning1 = recovery_size/2 + recovery_size%2 ; 
          partitionning2 = recovery_size/2 ; 
      }
      
      
      int break_points[num_procs+1] ; 
      break_points[0] = 0 ;  

      int i_beg ; 
      int i_end ; 
      int i ; 
      
      for (i=0 ; i<num_procs ; i++) 
      {
          charge(i,num_points,num_procs,i_beg,i_end) ;  
          break_points[i+1] = i_end ;                  
      }

    
      boundary_global_indices[0] = 0 ; 
      for (i=1 ; i<num_procs ; i++) 
      {
          boundary_global_indices[2*i-1] = break_points[i] -partitionning1 ;
          boundary_global_indices[2*i] = break_points[i] + partitionning2 ; 
      }
      boundary_global_indices[2*num_procs-1] = num_points-1 ;    
  }

  void build_meshes(string input)
  {


      //les conditions aux bors 
      int boundary_physical , boundary_fictional, inner_point ;  
      inner_point = 0 ;

      ifstream read_input(input.c_str()) ;
      int num_procs ; 
      int num_cols ; 
      int num_lines ; 
      int recovery_size ; 
      string label ;
      
      if(read_input)
      {
          read_input>>label ; 
          read_input>>num_lines ; 
          read_input>>label ;
          read_input>>num_cols ; 
          read_input>>label ; 
          read_input>>num_procs ; 
          read_input>>label ; 
          read_input>>recovery_size ; 
          read_input>>label ; 
          read_input>>boundary_physical;
          read_input>>label ;
          read_input>>boundary_fictional ; 
      } 
      else
      {
          cout<<"ERROR: can't open file"<<endl ;
      }

      float delta_x , delta_y ;
      delta_x = 1./(num_cols-1) ;
      delta_y = 1./(num_lines-1) ; 

      int* boundary_global_indices=new int[2*num_procs] ; 
      
      string proc  ; 
     
          partitions(num_cols, num_procs,recovery_size, boundary_global_indices) ;
          int start , end ; 
          for (int i=0 ; i<num_procs ; i++) 
          {
              if (i==0) 
              {
                  int column_to_send_right ; 
                  start = boundary_global_indices[i] ; 
                  column_to_send_right = boundary_global_indices[i+1] ; 
                  end = boundary_global_indices[i+2] ; 
                  proc= "proc_num_"+to_string(i)+".In" ;
                  ofstream write_mesh(proc.c_str()) ;
                  write_mesh<<num_lines*(end-start+1)<<endl ; 
                  for (int j=start ; j<=end ; j++)
                  {
                    if (j==start) //bord gauche physique
                    {
                      for (int k=0 ; k<num_lines; k++) 
                        {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<0<<endl ;}     
                    }
                    else if (j==end) //bord de droite fictif
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<boundary_physical<<4<<" "<<0<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_fictional<<" "<<0<<endl ;}} 
                    }
                    else if (j==column_to_send_right) 
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<1<<" "<<i+1<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<k+j*num_lines<<boundary_physical<<" "<<1<<" "<<i+1<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<inner_point<<" "<<1<<" "<<i+1<<endl ;}}  
                    }
                    else 
                    {
                        for (int k=0 ; k<num_lines;k++)
                        {
                          {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<inner_point<<" "<<0<<endl ;}} 
                        }
                    }
                   }
              }
              
              else if (i==num_procs-1)
              {
                  int column_to_send_left ;  
                  start = boundary_global_indices[2*(i-1)+1] ; 
                  column_to_send_left = boundary_global_indices[2*(i-1)+2] ;
                  end = boundary_global_indices[2*(i-1)+3] ;
                  proc =  "proc_num_"+ to_string(i) + ".In" ; 
                  ofstream write_mesh(proc.c_str()) ;
                   write_mesh<<num_lines*(end-start+1)<<endl ; 
                  for (int j=start ; j<=end ; j++)
                  {
                    if (j==start) //bord gauche fictif
                    {
                      for (int k=0 ; k<num_lines; k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<boundary_physical<<4<<" "<<0<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_fictional<<" "<<0<<endl ;}}     
                    }
                    else if (j==end) //bord de droite physique
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}   
                    }
                    else if (j==column_to_send_left) 
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<1<<" "<<i-1<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<k+j*num_lines<<boundary_physical<<" "<<1<<" "<<i-1<<endl ;}//bord haut
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<inner_point<<" "<<1<<" "<<i-1<<endl ;}}  
                    }
                    else 
                    {
                        for (int k=0 ; k<num_lines;k++)
                
                          {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<inner_point<<" "<<0<<endl ;} }
            
                    }
                   
              }
              }
              
        else
        {
          int column_to_send_left ;
          int column_to_send_right ;
          start = boundary_global_indices[2*(i-1)+1] ;
          column_to_send_left = boundary_global_indices[2*(i-1)+2] ;
          column_to_send_right = boundary_global_indices[2*(i-1)+3] ;
          end = boundary_global_indices[2*(i-1)+4] ;
          proc= "proc_num_" + to_string(i) + ".In" ;
          ofstream write_mesh(proc.c_str()) ;
          for (int j=start ; j<=end ; j++)
              {
                    if (j==start) //bord gauche fictif
                    {
                      for (int k=0 ; k<num_lines; k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<boundary_physical<<4<<" "<<0<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_fictional<<" "<<0<<endl ;}}      
                    }
                    else if (j==end) //bord de droite fictif
                    {
                      for (int k=0 ; k<num_lines;k++) 
                       {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<boundary_physical<<4<<" "<<0<<endl ;}//bord bas
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<boundary_fictional<<" "<<0<<endl ;}}  
                    }
                    else if (j==column_to_send_left) 
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<1<<" "<<i-1<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<0<<" "<<k*delta_y<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<1<<" "<<i-1<<endl ;}//bord haut
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<inner_point<<" "<<1<<" "<<i-1<<endl ;}}  
                    }
                    else if (j==column_to_send_right) 
                    {
                      for (int k=0 ; k<num_lines;k++) 
                        {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<1<<" "<<i+1<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<1<<" "<<i+1<<endl ;}//bord haut
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<inner_point<<" "<<1<<" "<<i+1<<endl ;}}  
                    }
                    else 
                    {
                        for (int k=0 ; k<num_lines;k++)
                
                          {if (k==0){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else if (k==num_lines-1){write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<boundary_physical<<" "<<0<<endl ;}//bord haut
                          else {write_mesh<<j*delta_x<<" "<<k*delta_y<<" "<<0<<" "<<k+j*num_lines<<" "<<inner_point<<" "<<0<<endl ;} }
            
                    }
                   
              }
          
      } 
          }
  
  }
/*
  bool compare_string(string str1 , string str2)
  {
    if (str1[0]==str2[0])
      {
        return true ; 
      }
    else
      {
        return false;
      }
  }
*/
  //taille indices_to_send_right ny ou 0 si rank = num_procs -1  
  //taille indices_to_send_left ny  ou 0 si rank = 0 
  /*
  void read_mesh(int num_procs ,int num_proc , int* nx_local , int* indices_to_send_right , int* indices_to_send_left) 
  {
    string file_name ; 
    file_name = "proc_num_" + to_string(num_proc) + ".In" ; 
    ifstream read_file(file_name.c_str()) ; 
    int start , end ;
    string parser ; 
    int ny ; 
    read_file>>parser ; //ny 
    read_file>>ny ;
    read_file>>parser ; // start 
    read_file>>start ; 
    read_file>>parser ; //end; 
    read_file>>end ; 
    read_file>>parser ; 
    int val ; 
    val = end -start ;
    nx_local = &val ;
   
    
    while (!compare_string(parser,"*"))
    {
      read_file>>parser ; 
    }

    if (num_proc != 0) 
    {
      read_file>>parser ;  
      for (int i=0 ; i<ny ; i++) 
      {
        read_file>>indices_to_send_left[i] ;
      }
      if (!read_file.eof())
        {
          read_file>>parser ; 
        }    
    }

    if (num_proc != num_procs-1) 
    {
      read_file>>parser ; 
      for (int i=0 ; i<ny ; i++) 
      {
        read_file>>indices_to_send_right[i] ;
      }
    }
  }*/