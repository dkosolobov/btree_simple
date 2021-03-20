#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <set>

//#include "Btree.hpp"
//#include "BtreeSimple.hpp"
#include "cartesian_tree.hpp"
#include "btree_simple.hpp"

int main()
{
    std::random_device rand_dev;
    std::default_random_engine generator(rand_dev());
    std::uniform_int_distribution<int> udist(0, INT_MAX);

    std::vector<uint32_t> content;
    std::vector<uint32_t> seq;
    std::vector<int> op;
    std::vector<int> priors;

    const int n = 10000000; //n queries: 3/4 of them insert, 1/4 search

    for (int i = 0; i < n; i++) {
        priors.push_back(udist(generator));
        int choice = udist(generator) % 4;
        if (choice < 3 || content.size() == 0) {
            int val = udist(generator);
            seq.push_back(val);
            op.push_back(0);
            content.push_back(val);
        } else {
            int val;
            if (udist(generator) % 5 != 1)
                val = content[udist(generator) % content.size()];
            else
                val = udist(generator);
            seq.push_back(val);
            op.push_back(1);
        }
    }


    std::cout << "Start!" << std::endl;

    int sum1 = 0;
    btree_simple btree;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) {
        if (op[i] == 0) {
            btree.insert(seq[i]);
        } else if (op[i] == 1) {
            sum1 += btree.search(seq[i]) ? 1 : 0;
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "duration tree16 = " << std::chrono::duration<double, std::milli>(end - start).count()
        << " ms" << std::endl;


    int sum2 = 0;
    std::set<uint32_t> set;
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) {
        if (op[i] == 0) {
            set.insert(seq[i]);
        } else if (op[i] == 1) {
            sum2 += set.find(seq[i]) != set.end() ? 1 : 0;
        }
    }
    end = std::chrono::steady_clock::now();
    std::cout << "duration set    = " << std::chrono::duration<double, std::milli>(end - start).count()
        << " ms" << std::endl;
  

    //int sum3 = 0;
    //btree_simple<int, 8> btree2;
    //start = std::chrono::steady_clock::now();
    //for (int i = 0; i < n; i++) {
    //    if (op[i] == 0) {
    //        btree2.insert(seq[i]);
    //    } else if (op[i] == 1) {
    //        sum3 += btree2.search(seq[i]) ? 1 : 0;
    //    }
    //}
    //end = std::chrono::steady_clock::now();
    //std::cout << "duration tree8  = " << std::chrono::duration<double, std::milli>(end - start).count()
    //    << " ms" << std::endl;


    int sum4 = 0;
    cartesian_tree<uint32_t> ctree;
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) {
        if (op[i] == 0) {
            ctree.insert(seq[i], priors[i]);
        } else if (op[i] == 1) {
            sum4 += ctree.search(seq[i]) ? 1 : 0;
        }
    }
    end = std::chrono::steady_clock::now();
    std::cout << "duration ctree  = " << std::chrono::duration<double, std::milli>(end - start).count()
        << " ms" << std::endl;

    if (sum1 != sum2 || sum1 != sum4) {
        std::cout << "Sums are not equal: " 
            << sum1 << ' ' << sum2 << ' ' << sum4 << std::endl;
    }
}
