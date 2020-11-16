#include <iostream>
#include "C:\\Program Files\\mingw-w64\\x86_64-8.1.0-win32-seh-rt_v6-rev0\\mingw64\\lib\\gcc\\x86_64-w64-mingw32\\8.1.0\\include\\c++\\dynamic_bitset.hpp"
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

using namespace std;

#define uint unsigned int
#define ASCII_OFFSET 48
#define ERROR 1

int factorial(int number) {
    return (number < 0) ? 0 : ((number < 2) ? 1 : number * factorial(number - 1));
};

double count_combination(int bottom, int top) {  // n, i
    return (double)factorial(bottom) / (double)(factorial(bottom - top) * factorial(top));
};

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
        void run_sim();
        void decode();
        bool is_error();
};

Codec::Codec() {};

void Codec::count_weigth_of_A_words() {
    boost::dynamic_bitset<> A_word(vector_a.size() - r);
    for (int i = 0; i < pow(2, vector_a.size() - r); i++) {
        auto tmp = A_word;
        for (uint i = 0; i < r; i++)
            tmp.push_back(0);
        int weight = encode(tmp).count();

        if (A_weights.find(weight) != A_weights.end())
            A_weights.at(weight)++;
        else
            A_weights.insert(pair<int, int>(weight, 1));

        for (int i = 0; i < A_word.size(); i++) {
            A_word.flip(i);
            if (A_word.test(i))
                break;
        }
    }
}

uint Codec::get_index_of_high_1(boost::dynamic_bitset<> b_s) {
    uint counter = 0;
    for (uint i = 0; i < b_s.size(); i++)
        counter = (b_s.test(i)) ? i : counter;
    return counter;
};

bool Codec::is_error() {
    return (vector_e.any() && dec_remainder.none()) ? true : false;
};

int Codec::init_values(string *g_x, uint l) {
    bool start_null_flag = true;
    for (auto it = g_x->end() - 1; it != g_x->begin(); it--) {
        if (*it != '1' && *it != '0') {
            cout << "Wrong sequence. Please, enter the binary number.\n\n";
            return ERROR;
        }
        else {
            if (start_null_flag && *it == '0')
                continue;
            else
                start_null_flag = 0;
            polynomial.push_back(*it - ASCII_OFFSET);
        }
    }
    polynomial.push_back(*(g_x->begin()) - ASCII_OFFSET);

    r = polynomial.size() - 1;
    vector_a.resize(r + l);
    vector_e.resize(vector_a.size());
    vector_b.resize(vector_a.size());

    for (uint i = 0; i < (vector_a.size() - r - 1); i++)
        polynomial.push_back(0);

    return 0;
};

boost::dynamic_bitset<> Codec::encode(boost::dynamic_bitset<> message) {
    boost::dynamic_bitset<> remainder = message.operator<<(r);
    vector_a = remainder;
    int diff;
    while ((diff = (get_index_of_high_1(remainder) - r)) >= 0) {
        boost::dynamic_bitset<> tmp = polynomial.operator<<(diff);
        remainder.operator^=(tmp);
    }
    vector_a.operator|=(remainder);
    return vector_a;
};

void Codec::decode() {
    dec_remainder = vector_b;
    int diff;
    while ((diff = (get_index_of_high_1(dec_remainder) - r)) >= 0) {
        boost::dynamic_bitset<> tmp = polynomial.operator<<(diff);
        dec_remainder.operator^=(tmp);
    }
};

void Codec::send_through_the_channel() {
    int rand_num = rand() % (int)(pow(2, vector_a.size()));
    vector_e = boost::dynamic_bitset<> (vector_a.size(), rand_num);
    vector_b.operator=(vector_a);
    vector_b.operator^=(vector_e);
};

void Codec::acc_prob_of_dec_err() {
    count_weigth_of_A_words();
    double P_err = 0.0;
    int n = vector_a.size();
    for (int i = d; i <= n; i++)
        P_err += A_weights[i] * pow(p, i) * pow(1 - p, n - i);
    data.push_back(to_string(p));
    data.at(data.size() - 1) += (" " + to_string(P_err));
};

void Codec::up_prob_of_dec_err() {
    double P_err = 0.0;
    uint n = vector_e.size();
    for (uint i = 0; i < d - 1; i++) {
        P_err += count_combination(n, i) * pow(p, i) * pow(1 - p, n - i);
    }
    data.at(data.size() - 1) += (" " + to_string(1 - P_err));
};

void Codec::run_sim() {
    for (p = 0; p <= 1; p += 0.01) {
        acc_prob_of_dec_err();
        up_prob_of_dec_err();
        A_weights.clear();
    }
    for (auto it = data.begin(); it != data.end(); it++) {
        fout << *it << "\n";
    }
};

void Codec::greeting() {
    cout << "Program for calculating the probability of decoding error\n";
    cout << "------------------------------------------------------------------------------------------------------\n\n";
};

void Codec::start() {
    greeting();
    fout.open("data.txt", ios_base::out);
    if (!fout.is_open()) {
        return;
    }
    string g_x;
    uint s_length;
    while (true) {
        cout << "Please, write the generating polynomial [g(x)]: ";
        cin >> g_x;
        cout << "Please, write the length of the sequence [l]: ";
        cin >> s_length;
        cout << "Please, write the distance of the code [d]: ";
        cin >> d;
        if (init_values(&g_x, s_length) != ERROR)
            run_sim();
    question:
        cout << "Do you want to exit? [y/n]: ";
        cin >> g_x;
        if (g_x == "y" || g_x == "Y") {
            cout << "Bye!\n\n";
            break;
        }
        else if (g_x == "n" || g_x == "N") {
        }
        else {
            goto question;
        }
        cout << "\n";   
    }
    fout.close();
};

int main(int argc, char **argv) {
    Codec codec;
    codec.start();
    return 0;
};

// run_sim
/* 
    uint N = ((double)9 / (double)(4 * pow(accuracy, 2))) + 0.5;
    uint N_err = 0;
    for (uint i = 0; i < N; i++) {
        int rand_num = rand() % (int)(pow(2, (vector_a.size() - r)));
        message = boost::dynamic_bitset<> (vector_a.size(), rand_num);
        
        encode(message);
        send_through_the_channel();
        decode();

        if (is_error()) {
            N_err++;
        }
    }
    double P_err = (double)N_err / (double)N;
    cout << "Probability of decoding error when transmitting data over a binary-symmetric channel: " << P_err << "\n"; 
*/