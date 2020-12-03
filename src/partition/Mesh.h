#include <vector> 
#include <iostream> 
#include <string> 
#include <cmath> 
#include <fstream> 

class Point
{
  private: 
      std::vector<double> m_coords; 
      int m_global_num; 
      int m_label; 
      int m_num_tags; 
      std::vector<int> m_send_2_procs; 
   public:
      Point(){};  
      void set_point(std::vector<double> coords, std::vector<int> procs_2_send, int global_num, int label, int num_tags);
      std::vector<double> get_coords(){return m_coords;}; 
      int get_label(){return m_label;}; 
      int get_num_tags(){return m_num_tags;}; 
      std::vector<int> get_procs_2_send(){return m_send_2_procs;};   
};

class Mesh
{
    private:
        std::vector<Point*> list_of_points;
        std::string m_input_file;
        int m_num_of_elements;
    public: 
        Mesh(std::string input_file); 
        void read_mesh(); 
};


