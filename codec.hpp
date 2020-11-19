#ifndef __CODEC__
#define __CODEC__

#include <iostream>
#include <cmath>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "functions.hpp"

using namespace std;

class Codec {
    public:
        Codec();
        void start();
        void greeting();

    private:
        uint r;
        uint d;
        float p;
        ofstream fout;
        boost::dynamic_bitset<> dec_remainder;
        boost::dynamic_bitset<> polynomial;
        boost::dynamic_bitset<> message;
        boost::dynamic_bitset<> vector_a;
        boost::dynamic_bitset<> vector_e;
        boost::dynamic_bitset<> vector_b;
        vector<string> data;
        map<int, int> A_weights; // A <- {key = weight, value = volume}

        boost::dynamic_bitset<> encode(boost::dynamic_bitset<>);
        uint get_index_of_high_1(boost::dynamic_bitset<>);
        int init_values(string*, uint);
        void count_weigth_of_A_words();
        void send_through_the_channel();
        void acc_prob_of_dec_err();
        void up_prob_of_dec_err();
        void plot();
        void run_sim();
        void decode();
        bool is_error();
};

#endif