#ifndef MACHINE_H
#define MACHINE_H

#include <cstdint>
#include <vector>
#include <bitset>

using namespace std;

class Machine {

    public:

        Machine();

        void SetMachineValues(vector <uint32_t> init_X, vector <uint64_t> init_K, uint32_t C1, uint32_t C2);

        void SetInputData(uint32_t init_N1, uint32_t init_N2);

        void simple_x32_enc();

        void simple_x32_dec();

        void setup_for_gamma(uint64_t init_S, vector <uint64_t> init_T);

        void gamma_enc();

        void gamma_dec(); 

        pair <uint32_t, uint32_t> get_current_result() {
            return make_pair(this->N1, this->N2);
        }

        vector <uint64_t> get_t_in() {
            return this->T_in;
        }

        vector <uint64_t> get_t_out() {
            return this->T_out;
        }

    private:
        
        // Basic components for simple encryption/decryption
        vector <uint32_t> X; 
        uint32_t N1, N2, N3, N4, N5, N6;
        uint32_t CM1, CM2, CM3, CM4, R;
        uint64_t CM5;
        vector <uint64_t> K;
        // Additional components for gamma regime encryption/decryption
        uint64_t S;
        vector <uint64_t> T_in, T_out;

        // Basic helper function of the machine
        void cypher_jth(int j);

};

#include "machine.cpp"

#endif
