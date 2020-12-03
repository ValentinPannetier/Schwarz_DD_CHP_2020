#include "Mesh.h"


void Point::set_point(std::vector<double> coords, std::vector<int> send_2_procs, int global_num, int label, int num_tags)
{
    m_coords = coords; 
    m_send_2_procs = send_2_procs; 
    m_global_num = global_num; 
    m_label = label; 
    m_num_tags = num_tags;    
}

Mesh::Mesh(std::string input_file) 
{
    m_input_file = input_file; 
}

void Mesh::read_mesh() 
{
    std::ifstream read(m_input_file.c_str()); 
    Point* point = new Point ; 
    read>>m_num_of_elements;
    std::vector<int> send_2_procs; 
    std::vector<double> coords;
    coords.resize(3);
    int global_num, label, num_tags;
    for (int i=0; i<m_num_of_elements; i++) 
    {
      read>>coords[0]>>coords[1]>>coords[2]>>global_num>>label>>num_tags ; 
      if (num_tags==1)
      {
        send_2_procs.resize(1); 
        read>>send_2_procs[0];    
      } 
      point->set_point(coords, send_2_procs, global_num, label, num_tags); 
      list_of_points.push_back(point); 
    } 
}
