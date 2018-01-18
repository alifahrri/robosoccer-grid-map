#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <vector>
#include <tuple>
#include <set>

class GridMap
{
public:
    struct Point
    {
        double x;
        double y;
    };

protected:
    typedef std::tuple<double,double,double> CellData;
    typedef std::tuple<double,double,double,std::vector<int>> Cell;
    typedef std::vector<Cell> Map;

protected:
    inline double &get_x(Cell &c) { return std::get<0>(c); }
    inline double &get_y(Cell &c) { return std::get<1>(c); }
    inline double &get_v(Cell &c) { return std::get<2>(c); }
    inline void update_cell(double x, double y, double value);
    bool in_map_test(double x, double y);
    Cell& map(double x, double y);
    std::vector<int> &get_neighbor(Cell &c);
    std::vector<Cell> neighbor_cell();

public:
    std::set<std::pair<int, int> > circle(int px, int py, int radius);
    std::vector<std::pair<double, double>> circle(double px, double py, double radius);

public:
    GridMap(double width, double height, double cell_size);
    GridMap(std::string map_file, std::string neighbor_file, std::string config_file);
    Cell getCell(double x, double y);
    Map getMap();
    Map getNeighbor(Cell c);
    double width();
    double height();
    double cellSize();
    void resize(double width, double height, double cell_size);
    void save(std::string cell_tuple_path, std::string neighbor_path, std::string config_path);
    void load(std::string cell_tuple_path, std::string neighbor_path, std::string config_path);
    void updateObstacles(std::vector<Point> obs, double radius);
    bool file_exist(std::string filename);
    int index(double px, double py);

    double get_lfree() const;
    void set_lfree(double value);

    double get_loccupied() const;
    void set_loccupied(double value);

    double get_lmin() const;
    void set_lmin(double value);

    double get_lmax() const;
    void set_lmax(double value);

protected:
    double _width;
    double _height;
    double _cell_size;
    double _cell_shift_w;
    double _cell_shift_h;
    double _left_cell_bound;
    double _upper_cell_bound;
    double _right_cell_bound;
    double _bottom_cell_bound;
    int _grid_width;
    int _grid_height;
    double _l_free = (-0.4);
    double _l_occupied = 0.85;
    double _l_min = (-3.0);
    double _l_max = (4.0);
    Map _map;
};

#endif // GRIDMAP_H
