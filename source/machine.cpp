#include <bitset>
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "machine.h"

using namespace std;

#define MACHINE_LOG 0

Machine :: Machine() {
    this->X.resize(8);
    fill(this->X.begin(), this->X.end(), 0);
    this->N1 = this->N2 = this->N3 = this->N4 = this->N5 = this->N6 = 0;
    this->CM1 = this->CM2 = this->CM3 = this->CM4 = this->CM5 = this->R = 0;
    this->K.resize(10);
    fill(this->K.begin(), this->K.end(), 0);
}

void Machine :: SetMachineValues(vector <uint32_t> init_X, vector <uint64_t> init_K, uint32_t C1, uint32_t C2) {
    for (int i = 0; i < 8; i++) {
        this->X[i] = init_X[i];
    }
    for (int i = 1; i <= 8; i++) {
        this->K[i] = init_K[i];
    }
    #if MACHINE_LOG
        cout << "Insert values of the machine: " << "\n";
        for (int i = 0; i < 8; i += 2) {
            if (i < 2) {
                cout << this->X[i] << " " << this->X[i + 1] << "\n";
            }
            cout << hex << setw(8) << setfill('0') << this->X[i];
            cout << hex << setw(8) << setfill('0') << this->X[i + 1];
            cout << "\n";
        }
        for (int i = 1; i <= 8; i++) {
            cout << hex << setw(16) << setfill('0') << this->K[i];
            cout << "\n";
        }
        cout << "The value of C1 = ";
        cout << hex << setw(8) << setfill('0') << C1;
        cout << "\n";
        cout << "The value of C2 = ";
        cout << hex << setw(8) << setfill('0') << C2;
        cout << "\n";
    #endif
    this->N6 = C1;
    this->N5 = C2;
}

void Machine :: SetInputData(uint32_t init_N1, uint32_t init_N2) {
    this->N1 = init_N1;
    this->N2 = init_N2;
}

void Machine :: cypher_jth(int j) {
    uint64_t buff_sum = N1 * 1ull + 1ull * (this->X[j]);
    if (buff_sum >= (1ull << 32ull)) {
        this->CM1 = buff_sum - (1ull << 32ull);
    } else {
        this->CM1 = buff_sum;
    }
    uint16_t sub_CM1[10] = {0};
    uint16_t sub_R[10] = {0};
    for (int i = 1; i <= 8; i++) {
        sub_CM1[i] = ((CM1 >> (4 * (i - 1))) & 15);
        sub_R[i] = ((R >> (4 * (i - 1))) & 15);
    }
    for (int i = 1; i <= 8; i++) {
        sub_R[i] = ((this->K[i] >> (4 * sub_CM1[i])) & 15);
    }
    #if MACHINE_LOG
        cout << "Substituted R -> " << "\n";
        for (int i = 1; i <= 8; i++) {
            cout << "[" << i << "]: ";
            cout << hex << sub_R[i];
            cout << "\n";
        }
    #endif
    this->R = 0;
    for (int i = 1; i <= 8; i++) {
        this->R += (sub_R[i] << (4 * (i - 1)));
    }
    uint32_t upper_part = ((this->R) >> 21);
    this->R = (((this->R) << 11) + upper_part);
    this->CM2 = ((this->R) ^ (this->N2));
    #if MACHINE_LOG
        cout << "Cycle " << j << "\n";
        cout << "Register R = ";
        cout << hex << setw(8) << setfill('0') << this->R;
        cout << "\n";
        cout << hex << setw(8) << setfill('0') << this->CM2;
        cout << "\n";
    #endif
} 

void Machine :: simple_x32_enc() {
    for (int i = 1; i <= 3; i++) {
        for (int j = 0; j < 8; j++) {
            this->cypher_jth(j);
            this->N2 = this->N1;
            this->N1 = this->CM2;
        }
    }
    for (int j = 7; j >= 1; j--) {
        this->cypher_jth(j);
        this->N2 = this->N1;
        this->N1 = this->CM2;
    }
    this->cypher_jth(0);
    this->N2 = this->CM2;
}

void Machine :: simple_x32_dec() {
    for (int j = 0; j < 8; j++) {
        this->cypher_jth(j);
        this->N2 = this->N1;
        this->N1 = this->CM2;
    }
    for (int i = 1; i <= 2; i++) {
        for (int j = 7; j >= 0; j--) {
            this->cypher_jth(j);
            this->N2 = this->N1;
            this->N1 = this->CM2;
        }
    }
    for (int j = 7; j >= 1; j--) {
        this->cypher_jth(j);
        this->N2 = this->N1;
        this->N1 = this->CM2;
    }
    this->cypher_jth(0);
    this->N2 = this->CM2;
}

void Machine :: setup_for_gamma(uint64_t init_S, vector<uint64_t> init_T) {
    this->S = init_S;
    for (uint64_t current_data : init_T) {
        this->T_in.push_back(current_data);
    }
}

void Machine :: gamma_enc() {
    this->N1 = ((this->S) & ((1ll << 32ll) - 1ll));
    this->N2 = ((this->S) >> 32ll);
    this->T_out.resize(this->T_in.size());
    this->simple_x32_enc();
    #if MACHINE_LOG
        cout << "First iteration of gamma encryption ended\n";
    #endif
    this->N3 = this->N1;
    this->N4 = this->N2;
    for (int i = 0; i < (int)(this->T_in.size()); i++) {
        #if MACHINE_LOG
            cout << "Starting to encrypt the #" << (i + 1) << " block of data\n";
        #endif
        uint64_t buff_sum1 = this->N3 + this->N5;
        uint64_t buff_sum2 = this->N4 + this->N6;
        if (buff_sum1 >= ((1ll << 32ll) - 1ll)) {
            buff_sum1 -= ((1ll << 32ll) - 1ll);
        }
        if (buff_sum2 >= ((1ll << 32ll) - 1ll)) {
            buff_sum2 -= ((1ll << 32ll) - 1ll);
        }
        this->N3 = static_cast<uint32_t>(buff_sum1);
        this->N4 = static_cast<uint32_t>(buff_sum2);
        this->N1 = this->N3;
        this->N2 = this->N4;
        this->simple_x32_enc();
        uint64_t current_mask = static_cast<uint64_t>(this->N2);
        current_mask = (current_mask << 32ll);
        current_mask += static_cast<uint64_t>(this->N1);
        this->T_out[i] = (this->T_in[i] ^ current_mask);
    }
}

void Machine :: gamma_dec() {
    for (uint64_t x : this->T_in) {
        this->T_out.push_back(x);
    }
    this->T_in.clear();
    this->T_in.resize(this->T_out.size());
    this->N1 = ((this->S) & ((1ll << 32ll) - 1ll));
    this->N2 = ((this->S) >> 32ll);
    this->simple_x32_enc();
    this->N3 = this->N1;
    this->N4 = this->N2;
    for (int i = 0; i < (int)(this->T_out.size()); i++) {
        uint64_t buff_sum1 = this->N3 + this->N5;
        uint64_t buff_sum2 = this->N4 + this->N6;
        if (buff_sum1 >= ((1ll << 32ll) - 1ll)) {
            buff_sum1 -= ((1ll << 32ll) - 1ll);
        }
        if (buff_sum2 >= ((1ll << 32ll) - 1ll)) {
            buff_sum2 -= ((1ll << 32ll) - 1ll);
        }
        this->N3 = static_cast<uint32_t>(buff_sum1);
        this->N4 = static_cast<uint32_t>(buff_sum2);
        this->N1 = this->N3;
        this->N2 = this->N4;
        this->simple_x32_enc();
        uint64_t current_mask = static_cast<uint64_t>(this->N2);
        current_mask = (current_mask << 32ll);
        current_mask += static_cast<uint64_t>(this->N1);
        this->T_in[i] = (this->T_out[i] ^ current_mask);
    }
}
