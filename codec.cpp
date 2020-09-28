#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include <cmath>

#define ASCII_OFFSET 47
#define ERROR 1

class Codec {
    public:
        Codec();
        void start();
        void greeting();

    private:
        float accuracy;
        boost::dynamic_bitset<> dec_remainder;
        boost::dynamic_bitset<> polynomial;
        boost::dynamic_bitset<> message;
        boost::dynamic_bitset<> vector_a;
        boost::dynamic_bitset<> vector_e;
        boost::dynamic_bitset<> vector_b;

        uint get_index_of_high_1(boost::dynamic_bitset<>); // TODO
        void run_sim();
        int init_values(std::string*, uint);
        void encode();
        void decode();
        void send_through_the_channel();
        bool is_error(); // TODO
};

Codec::Codec() {};

void Codec::greeting() {

    std::cout << "Program for calculating the upper estimation and accurate estimation of the decoding error probability\n";
    std::cout << "------------------------------------------------------------------------------------------------------\n\n";
};

int Codec::init_values(std::string *g_x, uint l) {
    bool start_null_flag = true;
    for (auto it = g_x->begin(); it != g_x->end(); it++) {
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
    vector_a.resize(polynomial.size() + l);
    vector_e.resize(vector_a.size());
    vector_b.resize(vector_a.size());
};

void Codec::encode() {
    boost::dynamic_bitset<> remainder = message.operator<<(3);
    vector_a = remainder;
    while (remainder.to_ulong() > polynomial.to_ulong()) {
        int mul_digree = get_index_of_high_1(remainder) - get_index_of_high_1(polynomial);
        boost::dynamic_bitset<> tmp = remainder.operator<<(mul_digree);
        remainder.operator&=(tmp);
    }
    vector_a.operator|=(remainder);
};

void Codec::decode() {
    dec_remainder = vector_b.operator<<(3);
    while (dec_remainder.to_ulong() > polynomial.to_ulong()) {
        int mul_digree = get_index_of_high_1(dec_remainder) - get_index_of_high_1(polynomial);
        boost::dynamic_bitset<> tmp = dec_remainder.operator<<(mul_digree);
        dec_remainder.operator&=(tmp);
    }
};

void Codec::send_through_the_channel() {
    vector_b.operator=(vector_a);
    vector_b.operator^=(vector_e);
};

void Codec::run_sim() {
    uint N = 9 / (4 * pow(accuracy, 2));
    uint N_err = 0;
    for (uint i = 0; i < N; i++) {
        int rand_num = rand() % (vector_a.size() - polynomial.size());
        message = boost::dynamic_bitset<> (message.size(), rand_num);
        encode();
        send_through_the_channel();
        decode();
        if (is_error())
            N_err++;
    }
    double P_err = N_err / N;
    std::cout << "Probability of decoding error when transmitting data over a binary-symmetric channel: " << P_err << "\n";
};

void Codec::start() {
    greeting();

    std::string g_x;
    uint s_length;
    while (true) {
        std::cout << "Please, write the generating polynomial: ";
        std::cin >> g_x;
        std::cout << "Please, write the length of the sequence: ";
        std::cin >> s_length;
        std::cout << "Please, write the accuracy: ";
        std::cin >> accuracy;
        if (init_values(&g_x, s_length) == ERROR)
            continue;

        run_sim();

        std::cout << "Do you want to exit? [y/n]: ";
        std::cin >> g_x;
        if (g_x == "y" || g_x == "Y") {
            std::cout << "Bye!\n";
            break;
        }
    }
};

int main(int argc, char **argv) {

    Codec codec;
    codec.start();

    return 0;
};