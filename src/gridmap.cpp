#include "gridmap.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <ios>
#include <fstream>
#include <sstream>
#include <set>
#include <sys/stat.h>

#define DEFAULT_WIDTH       (9.0)
#define DEFAULT_HEIGHT      (6.0)
#define DEFAULT_CELL_SIZE   (0.15)
#define DEFAULT_VX_RANGE    (3.5)
#define DEFAULT_VY_RANGE    (3.5)
#define DEFAULT_VEL_SIZE    (0.1)
//#define l_occupied          (0.85)  //log odds
//#define l_free              (-0.4)  //log odds
//#define l_min               (-3.0)
//#define l_max               (4.0)
#define OBSTACLE_RADIUS     (0.75)

GridMap::GridMap(double width, double height, double cell_size)
{
    resize(width,height,cell_size);
}

GridMap::GridMap(std::__cxx11::string map_file, std::__cxx11::string neighbor_file, std::__cxx11::string config_file)
{
    if(file_exist(map_file) && file_exist(neighbor_file) && file_exist(config_file))
        load(map_file,neighbor_file,config_file);
    else
    {
        resize(DEFAULT_WIDTH,DEFAULT_HEIGHT,DEFAULT_CELL_SIZE);
        save(map_file,neighbor_file,config_file);
    }
}

void GridMap::resize(double width, double height, double cell_size)
{
    _width = width; _height = height; _cell_size = cell_size;
    _grid_width = (int)(width/cell_size);
    _grid_height = (int)(height/cell_size);
    if(!(_grid_height%2))
        _grid_height++;
    if(!(_grid_width%2))
        _grid_width++;
    _map.clear();
    _map.resize(_grid_width*_grid_height);

    _cell_shift_w = (((_grid_width)/2)*_cell_size)-(_width/2.0);
    _cell_shift_h = (((_grid_height)/2)*_cell_size)-(_height/2.0);

    _left_cell_bound = -(_width/2.0)-_cell_shift_w-(_cell_size/2.0);
    _upper_cell_bound = -(_height/2.0)-_cell_shift_h-(_cell_size/2.0);
    _right_cell_bound = fabs(_left_cell_bound);
    _bottom_cell_bound = fabs(_upper_cell_bound);

    auto neighbor_cell_test = neighbor_cell();

    for(int h=0; h<_grid_height; h++)
        for(int w=0; w<_grid_width; w++)
        {
            auto& c = _map.at(h*_grid_width+w);
            get_x(c) = (w*_cell_size)-(_width/2.0)-_cell_shift_w;
            get_y(c) = (h*_cell_size)-(_height/2.0)-_cell_shift_h;
            auto& neighbor = get_neighbor(c);
            auto index = (int)(h*_grid_width+w);
            for(auto& n : neighbor_cell_test)
            {
                auto x = get_x(c) + get_x(n);
                auto y = get_y(c) + get_y(n);
                auto i = get_neighbor(n).at(0);
                if(in_map_test(x,y))
                    neighbor.push_back(index+i);
            }
        }
}

GridMap::Map GridMap::getMap()
{
    return _map;
}

GridMap::Cell GridMap::getCell(double x, double y)
{
    if(in_map_test(x,y))
        return map(x,y);
    return Cell();
}

GridMap::Map GridMap::getNeighbor(GridMap::Cell c)
{
    Map neighbor;
    for(auto n : get_neighbor(c))
        neighbor.push_back(_map[n]);
    return neighbor;
}

void GridMap::save(std::__cxx11::string cell_tuple_path, std::__cxx11::string neighbor_path, std::__cxx11::string config_path)
{
    std::ofstream config_file;
    std::ofstream map_file;
    std::ofstream neighbor_file;
    std::vector<int> neighbor_map;
    std::vector<int> neighbor_map_idx;
    std::vector<CellData> cell_map;

    config_file.open(config_path, std::ios_base::out);
    std::stringstream cs_ss;
    cs_ss << _cell_size << '\n';
    cs_ss << _width << '\n';
    cs_ss << _height << '\n';
    cs_ss << _cell_shift_w << '\n';
    cs_ss << _cell_shift_h << '\n';
    cs_ss << _left_cell_bound << '\n';
    cs_ss << _upper_cell_bound << '\n';
    cs_ss << _right_cell_bound << '\n';
    cs_ss << _bottom_cell_bound << '\n';
    cs_ss << _grid_width << '\n';
    cs_ss << _grid_height << '\n';
    config_file.write((char*)cs_ss.str().c_str(),sizeof(char)*cs_ss.str().size());
    config_file.close();

    auto i(1);
    neighbor_map_idx.resize(_map.size()+1);
    neighbor_map_idx.at(0) = _map.size()+1;
    for(auto& m : _map)
    {
        auto& neighbor = get_neighbor(m);
        cell_map.push_back(std::make_tuple(get_x(m),get_y(m),get_v(m)));
        neighbor_map_idx.at(i++) = neighbor.size();
        for(const auto& n : neighbor)
            neighbor_map.push_back(n);
    }

    neighbor_map.insert(neighbor_map.begin(),neighbor_map_idx.begin(),neighbor_map_idx.end());

    std::cout << "_map size : " << _map.size() << std::endl;
    std::cout << "sizeof double : " << sizeof(double) << std::endl;
    std::cout << "sizeof cellmap : " << cell_map.size() << "*" << sizeof(CellData) << std::endl;
    std::cout << "sizeof neighbor : " << neighbor_map.size() << "*" << sizeof(int) << std::endl;

    map_file.open(cell_tuple_path,std::ios_base::out | std::ios_base::binary);
    map_file.write((char*)cell_map.data(),sizeof(double)*3*cell_map.size());
    map_file.close();

    neighbor_file.open(neighbor_path, std::ios_base::out | std::ios_base::binary);
    neighbor_file.write((char*)neighbor_map.data(),sizeof(int)*neighbor_map.size());
    //    std::copy(neighbor_map.begin(),neighbor_map.end(),std::ostreambuf_iterator<char>(neighbor_file));
    neighbor_file.close();
}

void GridMap::load(std::__cxx11::string cell_tuple_path, std::__cxx11::string neighbor_path, std::__cxx11::string config_path)
{
    std::ifstream config_file;
    std::ifstream map_file;
    std::ifstream neighbor_file;
    std::vector<int> neighbor_map;
    std::vector<CellData> cell_map;

    std::cout << "loading map..\n";

    config_file.open(config_path, std::ios_base::in);

    std::cout << "grid map config : \n";
    std::string config_str;
    std::getline(config_file,config_str);
    _cell_size = std::stod(config_str);
    std::cout << config_str << " " << _cell_size << '\n';
    std::getline(config_file,config_str);
    _width = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _height = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _cell_shift_w = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _cell_shift_h = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _left_cell_bound = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _upper_cell_bound = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _right_cell_bound = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _bottom_cell_bound = std::atof(config_str.c_str());
    std::getline(config_file,config_str);
    _grid_width = std::atoi(config_str.c_str());
    std::getline(config_file,config_str);
    _grid_height = std::atoi(config_str.c_str());

    config_file.close();

    neighbor_file.open(neighbor_path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    auto neighbor_size = neighbor_file.tellg();
    neighbor_file.seekg(0);
    neighbor_map.resize(neighbor_size/(sizeof(int)));
    neighbor_file.read((char*)(neighbor_map.data()),neighbor_size);
    neighbor_file.close();

    map_file.open(cell_tuple_path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    auto map_size = map_file.tellg();
    map_file.seekg(0);
    cell_map.resize(map_size/(sizeof(double)*3));
    map_file.read((char*)(cell_map.data()),map_size);
    map_file.close();

    std::cout << "neighbor size : " << neighbor_size << std::endl;
    std::cout << "map size : " << map_size << std::endl;
    std::cout << "neighbor map size : " << neighbor_map.size() << std::endl;
    std::cout << "cell map size : " << cell_map.size() << std::endl;

    _map.clear();
    auto i(1);
    auto neighbor_offset = neighbor_map.at(0);
    for(auto& m : cell_map)
    {
        Cell c;
        get_x(c) = std::get<0>(m);
        get_y(c) = std::get<1>(m);
        get_v(c) = std::get<2>(m);
        auto neighbor_size = neighbor_map.at(i++);
        for(int k = 0; k < neighbor_size; k++)
            get_neighbor(c).push_back((int)neighbor_map.at(k+neighbor_offset));
        neighbor_offset += neighbor_size;
        _map.push_back(c);
    }
    std::cout << "_map size : " << _map.size() << std::endl;
}

void GridMap::updateObstacles(std::vector<GridMap::Point> obs, double radius)
{
    std::set<int> obs_idx;
    for(const auto& o : obs) {
        auto obs_circle = circle(o.x,o.y,radius);
        for(const auto& c : obs_circle)
            obs_idx.insert(index(c.first,c.second));
    }
    for(int i = 0; i<_map.size(); i++) {
        auto& v = get_v(_map[i]);
        if(obs_idx.find(i) != obs_idx.end()) {
            v += _l_occupied;
        }
        else {
            v += _l_free;
        }
        v = std::min(_l_max,std::max(_l_min,v));
//        v = (1.0-(1.0/(1.0+std::exp(v))));
//        v = (1.0-(1.0/(1.0+std::pow(2.718281828,v))));
    }
}

double GridMap::width()
{
    return _width;
}

double GridMap::height()
{
    return _height;
}

double GridMap::cellSize()
{
    return _cell_size;
}

bool GridMap::file_exist(std::__cxx11::string filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

int GridMap::index(double px, double py)
{
    auto x = px;
    auto y = py;
    if(fabs(x)>(fabs(_width/2.0)-_cell_size/2.0))
        x = (x<0) ? -(_width/2.0-_cell_size/2.0) : _width/2.0-_cell_size/2.0;
    if(fabs(y)>(fabs(_height/2.0)-_cell_size/2.0))
        y = (y<0) ? -(_height/2.0-_cell_size/2.0) : _height/2.0-_cell_size/2.0;
    auto ix = (int)(round((x+_width/2.0)/_cell_size));
    auto iy = (int)(round((y+_height/2.0)/_cell_size));
//    ix = (ix<0) ? 0 : (ix>=_grid_width) ? _grid_width-1 : ix;
//    iy = (iy<0) ? 0 : (iy>=_grid_height) ? _grid_height-1 : iy;
    return iy*_grid_width+ix;
}

double GridMap::get_lfree() const
{
    return _l_free;
}

void GridMap::set_lfree(double value)
{
    _l_free = value;
}

double GridMap::get_loccupied() const
{
    return _l_occupied;
}

void GridMap::set_loccupied(double value)
{
    _l_occupied = value;
}

double GridMap::get_lmin() const
{
    return _l_min;
}

void GridMap::set_lmin(double value)
{
    _l_min = value;
}

double GridMap::get_lmax() const
{
    return _l_max;
}

void GridMap::set_lmax(double value)
{
    _l_max = value;
}

inline
GridMap::Cell &GridMap::map(double x, double y)
{
    return _map.at(index(x,y));
}

inline
std::vector<int> &GridMap::get_neighbor(GridMap::Cell &c)
{
    return std::get<3>(c);
}

inline
std::vector<GridMap::Cell> GridMap::neighbor_cell()
{
    std::vector<Cell> neighbor_cell_test;
    for(int i=0; i<8; i++)
    {
        Cell c;
        if(i<2)
        {
            //diagonal cell
            //right side
            get_x(c) = -_cell_size;
            if(i%2)
            {
                get_y(c) = _cell_size;
                get_neighbor(c).push_back(_grid_width-1);
            }
            else
            {
                get_y(c) = -_cell_size;
                get_neighbor(c).push_back(-_grid_width-1);
            }
        }
        else if(i<4)
        {
            //diagonal cell
            //left side
            get_x(c) = _cell_size;
            if(i%2)
            {
                get_y(c) = _cell_size;
                get_neighbor(c).push_back(_grid_width+1);
            }
            else
            {
                get_y(c) = -_cell_size;
                get_neighbor(c).push_back(-_grid_width+1);
            }
        }
        else if(i<6)
        {
            //horizontal cell
            get_y(c) = 0.0;
            if(i%2)
            {
                get_x(c) = -_cell_size;
                get_neighbor(c).push_back(-1);
            }
            else
            {
                get_x(c) = _cell_size;
                get_neighbor(c).push_back(1);
            }
        }
        else
        {
            //vertical cell
            get_x(c) = 0.0;
            if(i%2)
            {
                get_y(c) = -_cell_size;
                get_neighbor(c).push_back(-_grid_width);
            }
            else
            {
                get_y(c) = _cell_size;
                get_neighbor(c).push_back(_grid_width);
            }
        }
        neighbor_cell_test.push_back(c);
    }
    return neighbor_cell_test;
}

std::set<std::pair<int, int> > GridMap::circle(int px, int py, int radius)
{
    std::set<std::pair<int,int>> ret;
    int x = radius;
    int y = 0;
    int err = 0;
    while (x>=y) {
        ret.insert(std::make_pair(px+x, py+y));
        ret.insert(std::make_pair(px+y, py+x));
        ret.insert(std::make_pair(px-y, py+x));
        ret.insert(std::make_pair(px-x, py+y));
        ret.insert(std::make_pair(px-x, py-y));
        ret.insert(std::make_pair(px-y, py-x));
        ret.insert(std::make_pair(px+y, py-x));
        ret.insert(std::make_pair(px+x, py-y));
        y+=1;
        if(err<=0)
          err += 2*y+1;
        else
        {
          x -= 1;
          err += 2*(y-x) + 1;
        }
    }
    //fill
    for(int i=0; i<radius; i++)
    {
      for(int j=0; j<radius; j++)
      {
        if(ret.find(std::make_pair(px+i,py+j))!=ret.end())
          break;
        ret.insert(std::make_pair(px+i,py+j));
        ret.insert(std::make_pair(px-i,py-j));
        ret.insert(std::make_pair(px+i,py-j));
        ret.insert(std::make_pair(px-i,py+j));
      }
    }
    return ret;
}

std::vector<std::pair<double, double> > GridMap::circle(double px, double py, double radius)
{
    std::vector<std::pair<double, double>> ret;
    int ix = std::round(px/_cell_size);
    int iy = std::round(py/_cell_size);
    int rad = std::round(radius/_cell_size);
    auto grid_circle = circle(ix,iy,rad);
    for(const auto& g : grid_circle)
    {
        auto dx = g.first*_cell_size;
        auto dy = g.second*_cell_size;
        ret.push_back(std::make_pair(dx,dy));
    }
    return ret;
}

inline
bool GridMap::in_map_test(double x, double y)
{
    if(x>_left_cell_bound)
        if(x<_right_cell_bound)
            if(y>_upper_cell_bound)
                if(y<_bottom_cell_bound)
                    return true;
    return false;
}

inline
void GridMap::update_cell(double x, double y, double value)
{
    auto& c = map(x,y);
    get_v(c) = value;
}
