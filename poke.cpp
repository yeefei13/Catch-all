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

double dis(const pair<int, int> &i, const pair<int, int> &j)
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
        // mst();
        ni();
        cost = cost_opt();
        cout << cost << '\n';
        for (auto i : opt)
        {
            cout << i << " ";
        }
    }
    else
    {
        fill_table();
        ni_3();
        cost = 0;
        for (size_t i = 0; i < opt.size() - 1; i++)
        {
            cost += (adtable[opt[i]][opt[i+1]]);
        }
        cost += (adtable[opt[0]][opt.back()]);
        // cout << cost << '\n';
        tsp();
        // cost = cost_opt();
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
    true_opt = opt;
    partial_len.resize(opt.size(),0);
    // for(size_t i =0; i<opt.size();i++){
    //     opt[i]=i;
    // }
    genperm(1);
    opt = true_opt;
}

void pock::genperm(size_t len)
{
    if (len == opt.size())
    {
        double cur_cost = partial_len[len - 1] + (adtable[opt[0]][opt.back()]);
        // cout << cur_cost << '\n';
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
        partial_len[len] = partial_len[len - 1] + (adtable[opt[len]][opt[len-1]]);
        genperm(len + 1);
        swap(opt[len], opt[i]);
    }
}

void pock::update_3(vector<mst_condi> &temp3, size_t len, size_t local_min_index)
{
    for (size_t i = len; i < opt.size(); i++)
    {
        if (!temp3[opt[i]].linked)
        {
            double cur_dis = adtable[opt[i]][local_min_index];
            if (cur_dis < temp3[opt[i]].distance)
            {
                temp3[opt[i]].distance = cur_dis;
                // cout<< "current x: "<<graph[opt[i]].first << " "<< graph[local_min_index].first<< " current y: "<<graph[opt[i]].second 
                // << " "<< graph[local_min_index].second<<" "<< local_min_index<<'\n';
                // cout << "currrent best "<< sqrt(cur_dis)<< "  "<<i<<'\n'; 
                // temp[opt[i]].prev = local_min_index;
            }
        }
    }
}

bool pock::check(size_t len)
{
    if(opt.size() - len <= 4){
        return true;
    }
    vector<mst_condi> temp3;
    temp3.resize(opt.size());
    double cur_cost = 0;
    cur_cost += partial_len[len - 1];
    // set for opt[len]
    temp3[opt[len]].linked = true;
    update_3(temp3, len, opt[len]);

    // mst the rest part
    for (size_t i = len + 1; i < opt.size(); i++)
    {
        double local_min = numeric_limits<double>::infinity();
        size_t local_min_index = 10;
        for (size_t j = len + 1; j < opt.size(); j++)
        {
            if (temp3[opt[j]].distance < local_min && !temp3[opt[j]].linked)
            {
                local_min = temp3[opt[j]].distance;
                local_min_index = opt[j];
            }
        }
        temp3[local_min_index].linked = true;
        cur_cost += (temp3[local_min_index].distance);
        update_3(temp3, len, local_min_index);
    }
    // add two line
    double cur_min = adtable[opt[0]][opt.back()];
    double cur_min2 = adtable[opt[len-1]][opt.back()];

    for (size_t j = len; j < opt.size(); j++)
    {
        double cur_num = adtable[opt[0]][opt[j]];
        double cur_num2 = adtable[opt[len-1]][opt[j]];
        if (cur_num < cur_min)
        {
            swap(cur_min, cur_num);
        }
        if (cur_num2 < cur_min2)
        {
            swap(cur_min2, cur_num2);
        }
    }
    cur_cost += (cur_min) + (cur_min2);
    // cout<< cur_cost<< " "<< cost<<'\n';
    return cur_cost < cost;
}

void pock::ni_3()
{
    opt.reserve(adtable.size());
    opt.push_back(0);
    opt.push_back(1);
    for (size_t i = 2; i < adtable.size(); i++)
    {
        size_t x;
        double min = numeric_limits<double>::infinity();
        double temp = 0;
        for (size_t j = 0; j < i - 1; j++)
        {
            temp = (adtable[i][opt[j]]) + (adtable[i][opt[j+1]]) - (adtable[opt[j]][opt[j+1]]);
            if (temp < min)
            {
                min = temp;
                x = opt[j + 1];
            }
        }
        temp = (adtable[i][opt[0]]) + (adtable[i][opt.back()]) - (adtable[opt[0]][opt.back()]);
        if (temp < min)
        {
            opt.push_back(i);
        }
        else
        {
            opt.insert(find(opt.begin(), opt.end(), x), i);
        }
    }
    for (auto i = opt.begin(),p=i+1; p<opt.end(); i++,p++){
        for(auto  k=i+2,l=i+3; l<opt.end(); k++,l++){
            double a = adtable[*i][*p] + adtable[*l][*k];
            double b = adtable[*i][*k] + adtable[*p][*l];
            if(b<a){
                reverse(p,k+1);
            }
        }
    }
}

void pock::ni()
{
    opt.reserve(graph.size());
    opt.push_back(0);
    opt.push_back(1);
    for (size_t i = 2; i < graph.size(); i++)
    {
        size_t x;
        // size_t k;
        // double min_c = numeric_limits<double>::infinity();
        double min = numeric_limits<double>::infinity();
        double temp = 0;
        // for(size_t j = 0; j<i;j++){
        //     for (size_t p = i; p < graph.size(); p++){
        //         if(dis(graph[opt[j]],graph[p])<min_c){
        //             min_c = dis(graph[opt[j]],graph[p]);
        //             k = p;
        //         }
        //     }
        // }
        for (size_t j = 0; j < i - 1; j++)
        {
            temp = sqrt(dis(graph[i], graph[opt[j]])) + sqrt(dis(graph[i], graph[opt[j + 1]])) - sqrt(dis(graph[opt[j]], graph[opt[j + 1]]));
            if (temp < min)
            {
                min = temp;
                x = opt[j + 1];
            }
        }
        temp = sqrt(dis(graph[i], graph[opt[0]])) + sqrt(dis(graph[i], graph[opt.back()])) - sqrt(dis(graph[opt[0]], graph[opt.back()]));
        // cout << temp <<"  "<< min << '\n';
        if (temp < min)
        {
            opt.push_back(i);
        }
        else
        {
            opt.insert(find(opt.begin(), opt.end(), x), i);
        }
    }
}

// void pock::ni_3()
// {
//     opt.reserve(graph.size());
//     opt.push_back(0);
//     opt.push_back(1);
//     for (size_t i = 2; i < graph.size(); i++)
//     {
//         size_t x;;
//         double min = numeric_limits<double>::infinity();
//         double temp = 0;
//         for (size_t j = 0; j < i - 1; j++)
//         {
//             temp = sqrt(dis(graph[i], graph[opt[j]])) + sqrt(dis(graph[i], graph[opt[j + 1]])) - sqrt(dis(graph[opt[j]], graph[opt[j + 1]]));
//             if (temp < min)
//             {
//                 min = temp;
//                 x = opt[j + 1];
//             }
//         }
//         temp = sqrt(dis(graph[i], graph[opt[0]])) + sqrt(dis(graph[i], graph[opt.back()])) - sqrt(dis(graph[opt[0]], graph[opt.back()]));
//         // cout << temp <<"  "<< min << '\n';
//         if (temp < min)
//         {
//             opt.push_back(i);
//         }
//         else
//         {
//             opt.insert(find(opt.begin(), opt.end(), x), i);
//         }
//     }
//     for (auto i = opt.begin(); i!=opt.end(); i++){
//         auto k =i;
//         k++;
//         for(; k!=opt.end(); k++){
//             auto j = i;
//             j++;
//             auto ii = i;
//             auto jj = j;
//             ii++;
//             jj++;
//             double a = dis(graph[(*i)],graph[*ii]) + dis(graph[(*j)],graph[(*jj)]);
//             double b = dis(graph[(*j)],graph[*ii]) + dis(graph[(*i)],graph[(*jj)]);
//             if(b<a){
//                 reverse(ii,jj);
//             }
//         }
//     }
// }

void pock::fill_table(){
    vector<double> temp;
    temp.resize(graph.size());
    adtable.resize(graph.size(),temp);
    for(size_t i = 0; i< graph.size(); i++){
        for(size_t j = 0; j<graph.size(); j++){
            adtable[i][j] = sqrt(dis(graph[i],graph[j]));
        }
    }
    graph.clear();
}