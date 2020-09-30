#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include <cmath>

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

        uint get_index_of_high_1(boost::dynamic_bitset<>);
        void run_sim();
        int init_values(std::string*, uint);
        void encode();
        void decode();
        void send_through_the_channel();
        bool is_error();
        void up_prob_of_dec_err();
};

Codec::Codec() {};

void Codec::greeting() {

    std::cout << "Program for calculating the probability of decoding error\n";
    std::cout << "------------------------------------------------------------------------------------------------------\n\n";
};

uint Codec::get_index_of_high_1(boost::dynamic_bitset<> b_s) {
    uint counter = 0;
    for (uint i = 0; i < b_s.size(); i++)
        counter = (b_s.test(i)) ? i + 1 : counter;
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
            polynomial.push_back(ASCII_OFFSET - *it);
        }
    }
    polynomial.push_back(ASCII_OFFSET - *(g_x->begin()));

    r = polynomial.size();
    vector_a.resize(polynomial.size() + l);
    vector_e.resize(vector_a.size());
    vector_b.resize(vector_a.size());

    for (uint i = 0; i < (vector_a.size() - r); i++)
        polynomial.push_back(0);

    return 0;
};

void Codec::encode() {
    boost::dynamic_bitset<> remainder = message.operator<<(r);
    vector_a = remainder;
    int diff;
    while ((diff = (get_index_of_high_1(remainder) - r)) >= 0) {
        boost::dynamic_bitset<> tmp = polynomial.operator<<(diff);
        remainder.operator^=(tmp);
    }
    vector_a.operator|=(remainder);
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

void Codec::up_prob_of_dec_err() {
    double prob = 0;
    uint n = vector_e.size();
    for (uint i = 0; i < d - 1; i++) {
        prob += count_combination(n, i) * pow(p, i) * pow(1 - p, n - i);
    }
    std::cout << "Upper probability of decoding error: " << prob << "\n";
};

void Codec::run_sim() {
    uint N = (9 / (4 * pow(accuracy, 2))) + 0.5;
    uint N_err = 0;
    for (uint i = 0; i < N; i++) {
        int rand_num = rand() % (int)(pow(2, (vector_a.size() - r)) + 1);
        message = boost::dynamic_bitset<> (vector_a.size(), rand_num);
        
        encode();
        send_through_the_channel();
        decode();
        if (is_error()) {
            N_err++;
        }
    }
    double P_err = (double)N_err / (double)N;
    std::cout << "Probability of decoding error when transmitting data over a binary-symmetric channel: " << P_err << "\n";
    
    up_prob_of_dec_err();
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
        std::cout << "Please, write the accuracy: ";
        std::cin >> accuracy;
        std::cout << "Please, write the pobabity of the error [p]: ";
        std::cin >> p;
        std::cout << "Please, write the distance of the code [d]: ";
        std::cin >> d;
        if (init_values(&g_x, s_length) != ERROR)
            run_sim();

        std::cout << "Do you want to exit? [y/n]: ";
        std::cin >> g_x;
        if (g_x == "y" || g_x == "Y") {
            std::cout << "Bye!\n\n";
            break;
        }
        std::cout << "\n";
    }
};

int main(int argc, char **argv) {

    Codec codec;
    codec.start();

    return 0;
};