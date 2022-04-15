//5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct pock{
    //1 = MST, 2 = AOPT, 3 = OPT
    uint8_t mode = 0;
    double cost=0;
    void setup(int argc, char *argv[]);
    struct complex{
        pair<int,int> coordinate;
        //0 = land, 1 = coast, 2 = sea
        uint8_t condi;
        complex(int x, int y): coordinate(x,y)
        {
            if(x<0 && y<0){
                condi = 2;
            }
            else if(x>0 || y>0){
                condi = 0;
            }
            else{
                condi = 1;
            }
        }
    };

    struct mst_condi{
        bool linked = false;
        double distance =numeric_limits<double>::infinity();
        size_t prev;
    };

    struct ni_condi{

    };


    vector<complex> p1_graph;
    vector<pair<int,int> > graph;
    vector<size_t> opt;
    vector<mst_condi> temp;
    vector<size_t> true_opt;
    vector<double> partial_len;
    vector<vector<double> > adtable; 
    void read_graph();
    void mst();
    void update(vector<mst_condi>& temp,size_t i);
    void mst_helper(vector<mst_condi>& temp);
    void run();
    void tsp();
    bool check(size_t len);
    void genperm(size_t len);
    double cost_opt();
    void update_3(vector<mst_condi>& temp,size_t len,size_t local_min_index);
    void ni();
    void ni_3();
    void fill_table();
};