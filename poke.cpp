// 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
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
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "xcode_redirect.hpp"
#include "poke.h"
using namespace std;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    cout << setprecision(2); // Always show 2 decimal places
    cout << fixed;           // Disable scientific notation for large numbers
    pock game;
    game.setup(argc, argv);
    game.read_graph();
    game.run();
    return 0;
}

double dis(pair<int, int> i, pair<int, int> j)
{
    return ((double)i.first - (double)j.first) * ((double)i.first - (double)j.first) + ((double)i.second - (double)j.second) * ((double)i.second - (double)j.second);
}

void pock::setup(int argc, char *argv[])
{

    // These are used with getopt_long()
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int option_index = 0;
    string cap;
    option long_options[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}};

    //  Fill in the double quotes, to match the mode and help options.
    while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1)
    {
        switch (choice)
        {
        case 'h':
            exit(0);

        case 'm':
            cap = optarg;
            if (cap == "MST")
            {
                mode = 1;
            }
            else if (cap == "FASTTSP")
            {
                mode = 2;
            }
            else if (cap == "OPTTSP")
            {
                mode = 3;
            }
            else
            {
                cerr << "Invalid argument to --show-path\n"
                     << endl;
                cerr << "  Invalid argument is: " << optarg << endl;
                exit(1);
            }

            break;

        default:
            cerr << "Error: invalid option" << endl;
            exit(1);
        } // switch
    }     // while
}

void pock::read_graph()
{
    size_t num;
    int x;
    int y;
    cin >> num;
    if (mode == 1)
    {
        p1_graph.reserve(num);
        for (size_t i = 0; i < num; i++)
        {
            cin >> x >> y;
            p1_graph.emplace_back(x, y);
        }
    }
    else
    {
        graph.reserve(num);
        for (size_t i = 0; i < num; i++)
        {
            cin >> x >> y;
            graph.emplace_back(x, y);
        }
    }
}

double pock::cost_opt()
{
    double cur = 0;
    for (size_t i = 0; i < opt.size() - 1; i++)
    {
        cur += sqrt(dis(graph[opt[i]], graph[opt[i + 1]]));
    }
    cur += sqrt(dis(graph[0], graph[opt.back()]));
    return cur;
}

void pock::run()
{
    if (mode == 1)
    {
        mst();
        cout << cost << '\n';
        for (size_t i = 1; i < temp.size(); i++)
        {
            if (i < temp[i].prev)
            {
                cout << i << " " << temp[i].prev << '\n';
            }
            else
            {
                cout << temp[i].prev << " " << i << '\n';
            }
        }
    }
    else if (mode == 2)
    {
        mst();
        cost = cost_opt();
        cout << cost << '\n';
        for (auto i : opt)
        {
            cout << i << " ";
        }
    }
    else
    {
        mst();
        cost = cost_opt();
        temp.clear();
        tsp();
        cost = cost_opt();
        cout << cost << '\n';
        for (auto i : opt)
        {
            cout << i << " ";
        }
    }
}

void pock::mst()
{
    if (mode == 1)
    {
        temp.resize(p1_graph.size());
    }
    else
    {
        temp.resize(graph.size());
    }
    if (mode == 2 || mode == 3)
    {
        opt.reserve(graph.size());
        opt.push_back(0);
    }
    temp[0].distance = 0;
    temp[0].linked = true;
    update(temp, 0);
    for (size_t i = 1; i < temp.size(); i++)
    {
        mst_helper(temp);
    }
}

void pock::mst_helper(vector<mst_condi> &temp)
{
    double local_min = numeric_limits<double>::infinity();
    size_t local_min_index = 0;
    for (size_t j = 1; j < temp.size(); j++)
    {
        if (temp[j].distance < local_min && !temp[j].linked)
        {
            local_min = temp[j].distance;
            local_min_index = j;
        }
    }
    if (local_min == numeric_limits<double>::infinity())
    {
        exit(1);
        // error;
    }
    temp[local_min_index].linked = true;
    cost += sqrt(temp[local_min_index].distance);
    update(temp, local_min_index);
    if (mode == 2 || mode == 3)
    {
        auto it = find(opt.begin(), opt.end(), temp[local_min_index].prev);
        it = opt.insert(it, local_min_index);
        auto itt = it;
        itt++;
        // size_t i = *itt;
        // *itt = *it;
        // *it = i;
        swap(*it, *itt);
    }
}

void pock::update(vector<mst_condi> &temp, size_t cur)
{
    for (size_t i = 1; i < temp.size(); i++)
    {
        if (!temp[i].linked)
        {
            if (mode == 1)
            {
                if (!(p1_graph[i].condi == 0 && p1_graph[cur].condi == 2) && !(p1_graph[cur].condi == 0 && p1_graph[i].condi == 2))
                {
                    double cur_dis = dis(p1_graph[i].coordinate, p1_graph[cur].coordinate);
                    if (cur_dis < temp[i].distance)
                    {
                        temp[i].distance = cur_dis;
                        temp[i].prev = cur;
                    }
                }
            }
            else
            {
                double cur_dis = dis(graph[i], graph[cur]);
                if (cur_dis < temp[i].distance)
                {
                    temp[i].distance = cur_dis;
                    temp[i].prev = cur;
                }
            }
        }
    }
}

void pock::tsp()
{
    genperm(1);
    opt = true_opt;
}

void pock::genperm(size_t len)
{
    if (len == opt.size())
    {
        double cur_cost = cost_opt();
        if (cur_cost < cost)
        {
            cost = cur_cost;
            true_opt = opt;
        }
        return;
    }
    if (!check(len))
    {
        return;
    }
    for (size_t i = len; i < opt.size(); i++)
    {
        swap(opt[len], opt[i]);
        genperm(len + 1);
        swap(opt[len], opt[i]);
    }
}

void pock::update_3(vector<mst_condi>& temp,size_t len,size_t local_min_index){
    for (size_t i = len+1; i < temp.size(); i++)
        {
            if (!temp[opt[i]].linked)
            {
                double cur_dis = dis(graph[opt[i]], graph[local_min_index]);
                if (cur_dis < temp[opt[i]].distance)
                {
                    temp[opt[i]].distance = cur_dis;
                    temp[opt[i]].prev = local_min_index;
                }
            }
        }
}

bool pock::check(size_t len)
{
    double cur_cost = 0;
    //turn the first len line to true;
    for (size_t i = 0; i < len; i++)
    {
        temp[opt[i]].linked = true;
        cur_cost += sqrt(dis(graph[opt[i]], graph[opt[i + 1]]));
    }
    //set for opt[len]
    temp[opt[len]].linked = true;
    update_3(temp,len,len);

    //mst the rest part
    for (size_t i = len+1; i < opt.size(); i++)
    {
        double local_min = numeric_limits<double>::infinity();
        size_t local_min_index = 0;
        for (size_t j = 1; j < temp.size(); j++)
        {
            if (temp[opt[j]].distance < local_min && !temp[opt[j]].linked)
            {
                local_min = temp[opt[j]].distance;
                local_min_index = opt[j];
            }
        }
        temp[local_min_index].linked = true;
        cur_cost += sqrt(temp[local_min_index].distance);
        update_3(temp,len,local_min_index);
    }

    double cur_min =numeric_limits<double>::infinity();
    double cur_min2 = numeric_limits<double>::infinity();
    for(size_t i = 0; i<len; i++){
        for(size_t j = len; j<opt.size(); j++){
            double cur_num = dis(graph[opt[i]],graph[opt[j]]);
            if(cur_num < cur_min){
                swap(cur_min,cur_num);
            }
            if(cur_num<cur_min2){
                swap(cur_min2,cur_num);
            }
        }
    }
    cur_cost+= sqrt(cur_min) + sqrt(cur_min2);
    
    for (size_t i = 1; i < temp.size(); i++)
    {
        temp[i].linked = false;
    }
    return cur_cost < cost;
}


