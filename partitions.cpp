  #include "partitions.h"


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
      ifstream read_input(input.c_str()) ;
      int num_procs ; 
      int num_cols ; 
      int num_lines ; 
      int recovery_size ; 
      string cut_off_plan ;
      
      if(read_input)
      {
          read_input>>cut_off_plan ; 
          read_input>>num_lines ; 
          read_input>>cut_off_plan ;
          read_input>>num_cols ; 
          read_input>>cut_off_plan ; 
          read_input>>num_procs ; 
          read_input>>cut_off_plan ; 
          read_input>>recovery_size ; 
          read_input>>cut_off_plan ; 
          read_input>>cut_off_plan ;
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
      if (cut_off_plan=="v") 
      {
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
                  write_mesh<<"nlines"<<endl ; 
                  write_mesh<<num_lines<<endl ; 
                  write_mesh<<"start"<<endl ;
                  write_mesh<<start<<endl ;
                  write_mesh<<"end"<<endl ;
                  write_mesh<<end<<endl ; 
                  for (int j=start ; j<=end ; j++)
                  {
                    for (int k=0 ; k<num_lines ; k++)
                    {
                      write_mesh<<j*delta_x<<" "<<k*delta_y<<endl ; 
                    }
                  }
                  write_mesh<<"**************************"<<endl ; 
                  write_mesh<<"R"<<endl ;
                for (int j=0 ; j<num_lines ; j++)
                {
                write_mesh<<(column_to_send_right-start)*num_lines+j<<endl ; 
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
                  write_mesh<<"nlines"<<endl ; 
                  write_mesh<<num_lines<<endl ; 
                  write_mesh<<"start"<<endl ;
                  write_mesh<<start<<endl ;
                  write_mesh<<"end"<<endl ;
                  write_mesh<<end<<endl ; 
                  for (int j=start ; j<=end ; j++)
                  {
                    for (int k=0 ; k<num_lines ; k++)
                    {
                      write_mesh<<k*delta_y<<" "<<j*delta_x<<endl ; 
                    }
                  }
                  write_mesh<<"************************************************"<<endl ;
                  write_mesh<<"L"<<endl ; 
                  for (int j=0 ; j<num_lines ; j++)
                  {
                    write_mesh <<(column_to_send_left-start)*num_lines+j<<endl ; 
                  }	
              }
              
        else
        {
          int column_to_send_left ;
          int column_to_send_right ;
          start = boundary_global_indices[2*(i-1)+1] ;
          column_to_send_left = boundary_global_indices[2*(i-1)+2] ;
          column_to_send_right = boundary_global_indices[2*(i-1)+3] ;
          end = boundary_global_indices[2*(i-1)+3] ;
          proc= "proc_num_" + to_string(i) + ".In" ;
          ofstream write_mesh(proc.c_str()) ;
          write_mesh<<"nlines"<<endl ; 
          write_mesh<<num_lines<<endl ; 
          write_mesh<<"start"<<endl ; 
          write_mesh<<start<<endl ;
          write_mesh<<"end"<<endl ;
          write_mesh<<end<<endl ;
          for (int j=start ; j<=end ; j++)
          {
            for (int k=0 ; k<num_lines ; k++)
            {
              write_mesh<<k*delta_y<<" "<<j*delta_x<<endl ; 
            }
          }
          write_mesh<<"************************************************************"<<endl ;
          write_mesh<<"L"<<endl ;
          for (int j=0 ; j< num_lines ; j++)
          {
            write_mesh<<(column_to_send_left-start)*num_lines+j<<endl ; 
          }
          write_mesh<<"************************************************************"<<endl ;
          write_mesh<<"R"<<endl ;
          for (int j=0 ; j<num_lines ; j++)
          {
            write_mesh<<(column_to_send_right-start)*num_lines+j<<endl ; 
          }
                              
        }
      } 
    }
  }

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

  //taille indices_to_send_right ny ou 0 si rank = num_procs -1  
  //taille indices_to_send_left ny  ou 0 si rank = 0 
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
  }