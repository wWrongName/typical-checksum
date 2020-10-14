#include <iostream>
#include "C:\\Program Files\\mingw-w64\\x86_64-8.1.0-win32-seh-rt_v6-rev0\\mingw64\\lib\\gcc\\x86_64-w64-mingw32\\8.1.0\\include\\c++\\dynamic_bitset.hpp"
#include <cmath>
#include <map>

#define uint unsigned int
#define ASCII_OFFSET 48
#define ERROR 1

class Codec {
    public:
        Codec();
        void start();
        void greeting();

    private:
        float accuracy;
        uint r;
        uint d;
        float p;
        boost::dynamic_bitset<> dec_remainder;
        boost::dynamic_bitset<> polynomial;
        boost::dynamic_bitset<> message;
        boost::dynamic_bitset<> vector_a;
        boost::dynamic_bitset<> vector_e;
        boost::dynamic_bitset<> vector_b;
        std::map<int, int> A_weights; // A <- {key = weight, value = volume}

        uint get_index_of_high_1(boost::dynamic_bitset<>);
        int init_values(std::string*, uint);
        void count_weigth_of_A_words();
        bool is_error();

        void run_sim();
        boost::dynamic_bitset<> encode(boost::dynamic_bitset<>);
        void decode();
        void send_through_the_channel();
        void up_prob_of_dec_err();
        void acc_prob_of_dec_err();
};

Codec::Codec() {};

void Codec::count_weigth_of_A_words() {
    boost::dynamic_bitset<> A_word(vector_a.size() - r);
    for (int i = 0; i < pow(2, vector_a.size() - r); i++) {
        // std::cout << A_word << "\n";
        auto tmp = A_word;
        for (uint i = 0; i < r; i++)
            tmp.push_back(0);
        int weight = encode(tmp).count();
        // std::cout << A_weights[weight] << "\n";
        if (A_weights.find(weight) != A_weights.end())
            A_weights.at(weight)++;
        else
            A_weights.insert(std::pair<int, int>(weight, 1));

        for (int i = 0; i < A_word.size(); i++) {
            A_word.flip(i);
            if (A_word.test(i))
                break;
        }
    }
    // for (auto it = A_weights.begin(); it != A_weights.end(); ++it) {
    //     std::cout << (*it).first << " : " << (*it).second << "\n";
    // }
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

int Codec::init_values(std::string *g_x, uint l) {
    if (accuracy <= 0 || accuracy > 1) {
        std::cout << "Wrong accuracy. Please, enter some value in the range 0 < accuracy <= 1.\n\n";
        return ERROR;
    }
    if (p <= 0 || p > 1) {
        std::cout << "Wrong accuracy. Please, enter some value in the range 0 < accuracy <= 1.\n\n";
        return ERROR;
    }
    bool start_null_flag = true;
    for (auto it = g_x->end() - 1; it != g_x->begin(); it--) {
        if (*it != '1' && *it != '0') {
            std::cout << "Wrong sequence. Please, enter the binary number.\n\n";
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

int factorial(int number) {
    return (number < 0) ? 0 : ((number < 2) ? 1 : number * factorial(number - 1));
};

double count_combination(int bottom, int top) {  // n, i
    return (double)factorial(bottom) / (double)(factorial(bottom - top) * factorial(top));
};

void Codec::acc_prob_of_dec_err() {
    count_weigth_of_A_words();
    double P_err = 0.0;
    int n = vector_a.size();
    for (int i = d; i <= n; i++)
        P_err += A_weights[i] * pow(p, i) * pow(1 - p, n - i);
    std::cout << "Accurate probability of decoding error: " << P_err << "\n";
};

void Codec::up_prob_of_dec_err() {
    double P_err = 0.0;
    uint n = vector_e.size();
    for (uint i = 0; i < d - 1; i++) {
        P_err += count_combination(n, i) * pow(p, i) * pow(1 - p, n - i);
    }
    std::cout << "Upper probability of decoding error: " << 1 - P_err << "\n";
};

void Codec::run_sim() {
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
    std::cout << "Probability of decoding error when transmitting data over a binary-symmetric channel: " << P_err << "\n";
};

void Codec::greeting() {
    std::cout << "Program for calculating the probability of decoding error\n";
    std::cout << "------------------------------------------------------------------------------------------------------\n\n";
};

void Codec::start() {
    greeting();

    std::string g_x;
    uint s_length;
    while (true) {
        std::cout << "Please, write the generating polynomial [g(x)]: ";
        std::cin >> g_x;
        std::cout << "Please, write the length of the sequence [l]: ";
        std::cin >> s_length;
        std::cout << "Please, write the accuracy [epsilon]: ";
        std::cin >> accuracy;
        std::cout << "Please, write the pobabity of the error [p]: ";
        std::cin >> p;
        std::cout << "Please, write the distance of the code [d]: ";
        std::cin >> d;
        if (init_values(&g_x, s_length) != ERROR)
            run_sim();
        up_prob_of_dec_err();
        acc_prob_of_dec_err();
question:
        std::cout << "Do you want to exit? [y/n]: ";
        std::cin >> g_x;
        if (g_x == "y" || g_x == "Y") {
            std::cout << "Bye!\n\n";
            break;
        }
        else if (g_x == "n" || g_x == "N") {
        }
        else {
            goto question;
        }
        std::cout << "\n";
    }
};

int main(int argc, char **argv) {

    Codec codec;
    codec.start();

    return 0;
};