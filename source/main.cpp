#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include "machine.h"

using namespace std;

#define X0_X1_index 0
#define X2_X3_index 1
#define X4_X5_index 2
#define X6_X7_index 3
#define K1_index 4
#define K2_index 5
#define K3_index 6
#define K4_index 7
#define K5_index 8
#define K6_index 9
#define K7_index 10
#define K8_index 11
#define Sign_index 12
#define C1_C2_index 13

#define DEBUG_MOD 0

string standard_help_msg;
string app_name      = "./objects/app";
string input_arg     = " -i [INPUT_FILE]";
string output_arg    = " -o [OUTPUT_FILE]";
string regime_arg    = " -r [REGIME]";
string config_arg    = " -conf [CONFIG_FILE]";
string process_arg   = " (-dec|-enc)\n";

string in_file, out_file, regime, configs, process;

vector <string> NormalizeArguments(int argc, char ** argv);
void ParseAllArguments(vector <string> intake);
bool IsArgumentExist(string ops, vector <string> & intake);
string GetArgumentValue(string ops, vector <string> & intake);
vector <uint64_t> ReadAllContent(ifstream &in);
uint64_t FromHexToInt64(string hh);
string FromInt64ToHex(uint64_t nbr64);
void InsertConfiguration(vector <uint64_t> data, Machine *m, uint64_t &Signature);

int main(int argc, char **argv) {
    // Initializing the standard help message
    standard_help_msg += app_name;
    standard_help_msg += input_arg;
    standard_help_msg += output_arg;
    standard_help_msg += regime_arg;
    standard_help_msg += config_arg;
    standard_help_msg += process_arg;
    standard_help_msg += "INPUT_FILE -> name/directory of the file with original data\n";
    standard_help_msg += "OUTPUT_FILE -> name/directory of the file where resultant data should be\n";
    standard_help_msg += "REGIME -> regime of the encryption/decryption, either simple or gamma\n";
    standard_help_msg += "CONFIG_FILE -> name/directory of the file were configuration values are stored\n";
    standard_help_msg += "(-dec|-enc) choose one of the processes: -dec for decrpytion, -enc for encryption\n";

    // Collecting all the arguments in vector as string
    // Parse the needed arguments to decide the action flow
    vector <string> arg_collect = NormalizeArguments(argc, argv);
    ParseAllArguments(arg_collect);
    Machine *m = new Machine();


    ifstream fin;
    fin.open(in_file);
    if (!fin.is_open()) {
        cerr << "Cannot open file " << in_file << "\n";
        return -1;
    }
    vector <uint64_t> input_data = ReadAllContent(fin);
    fin.close();
    cout << "The file " << in_file << " was processed succesfully\n";

    ifstream conf_in;
    conf_in.open(configs);
    if (!conf_in.is_open()) {
        cerr << "Cannot open file " << configs << "\n";
        return -1;
    }
    vector <uint64_t> conf_data = ReadAllContent(conf_in);
    conf_in.close();
    cout << "The file " << configs << " was processed succesfully\n";

    uint64_t Signature = 0ll;
    InsertConfiguration(conf_data, m, Signature);

    vector <uint64_t> resultant_data;

    // Choosing the process for the data processing
    // enc -> encryption;  dec -> decryption;
    if (process == "enc") {
        // simple - (режим простой замены) / regime of simple change
        // gamma - (режим гаммирование) / gamma regime
        if (regime == "simple") {
            for (uint64_t open_data : input_data) {
                uint32_t fi_part = static_cast<uint32_t>((open_data & ((1ll << 32) - 1ll)));
                uint32_t se_part = static_cast<uint32_t>((open_data >> 32ll));
                m->SetInputData(fi_part, se_part);
                m->simple_x32_enc();
                pair <uint32_t, uint32_t> enc_data = m->get_current_result();
                uint64_t encrypt64 = static_cast<uint64_t>(enc_data.second);
                encrypt64 = (encrypt64 << static_cast<uint64_t>(32));
                encrypt64 += static_cast<uint64_t>(enc_data.first);
                resultant_data.push_back(encrypt64);
            }
        } else if (regime == "gamma") {
            m->setup_for_gamma(Signature, input_data);
            #if DEBUG_MOD
                cout << "Set up for gamma regime encrpytion ended\n";
                cout << "Signature = ";
                cout << hex << setw(16) << setfill('0') << Signature;
                cout << "\n";
                cout << "Size of the input data = " << (int)(input_data.size()) << "\n";
            #endif
            m->gamma_enc();
            resultant_data = m->get_t_out();
            #if DEBUG_MOD
                cout << "Resultant data length = " << (int)(resultant_data.size()) << "\n";
            #endif
        } else {
            // It seems that user typed in incorrect name of the regime
            cerr << "\nUnknown name of the regime...\n";
            cerr << standard_help_msg << "\n";
        }
    } else if (process == "dec") {
        // here words simple/gamma means the same as above
        if (regime == "simple") {
            for (uint64_t close_data : input_data) {
                uint32_t fi_part = static_cast<uint32_t>((close_data & ((1ll << 32) - 1ll)));
                uint32_t se_part = static_cast<uint32_t>((close_data >> 32ll));
                m->SetInputData(fi_part, se_part);
                m->simple_x32_dec();
                pair <uint32_t, uint32_t> dec_data = m->get_current_result();
                uint64_t decrypt64 = static_cast<uint64_t>(dec_data.second);
                decrypt64 = (decrypt64 << static_cast<uint64_t>(32));
                decrypt64 += static_cast<uint64_t>(dec_data.first);
                resultant_data.push_back(decrypt64);
            }
        } else if (regime == "gamma") {
            m->setup_for_gamma(Signature, input_data);
            #if DEBUG_MOD
                cout << "Set up for gamma regime decrpytion ended\n";
                cout << "Signature = ";
                cout << hex << setw(16) << setfill('0') << Signature;
                cout << "\n";
                cout << "Size of the input data = " << (int)(input_data.size()) << "\n";
            #endif
            m->gamma_dec();
            resultant_data = m->get_t_in();
            #if DEBUG_MOD
                cout << "Resultant data length = " << (int)(resultant_data.size()) << "\n";
            #endif
        } else {
            // It seems that user typed in incorrect name of the regime
            cerr << "\nUnknown name of the regime...\n";
            cerr << standard_help_msg << "\n";
        }
    } else {
        // All the errors should've been catched in before this point
        // Something went terribly wrong
        cerr << "\n\nSomething went wrong with parsing the process...\n\n";
    }

    // Outstreaming the resultant data to the output file
    // Name/directory of the output file given in out_file string
    ofstream fout;
    fout.open(out_file, ostream :: trunc | ofstream :: out);
    if (!fout.is_open()) {
        cerr << "Cannot open file " << out_file << "\n";
        return -1;
    }
    for (uint64_t current_data : resultant_data) {
        fout << FromInt64ToHex(current_data) << "\n";
    }
    fout.close();

    delete m;
    return 0;
}

vector <string> NormalizeArguments(int argc, char **argv) {
    vector <string> res;
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        string current;
        for (int pos = 0; pos < len; pos++) {
            current.push_back(argv[i][pos]);
        }
        res.push_back(current);
    }
    return res;
}

void ParseAllArguments(vector <string> intake) {
    if ((int)(intake.size()) < 9) {
        cerr << "Too few arguments...\n";
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    if ((int)(intake.size()) > 9) {
        cerr << "Too many arguments...\n";
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    if (!IsArgumentExist("i", intake)) {
        cerr << "No input file argument found" << endl;
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    in_file = GetArgumentValue("i", intake);
    if (!IsArgumentExist("o", intake)) {
        cerr << "No output file argument found" << endl;
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    out_file = GetArgumentValue("o", intake);
    if (!IsArgumentExist("r", intake)) {
        cerr << "No regime argument found" << endl;
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    regime = GetArgumentValue("r", intake);
    if (!IsArgumentExist("conf", intake)) {
        cerr << "No config file found" << endl;
        cerr << standard_help_msg << endl;
        exit(-1);
    }
    configs = GetArgumentValue("conf", intake);
    bool is_dec = IsArgumentExist("dec", intake);
    bool is_enc = IsArgumentExist("enc", intake);
    if (is_dec && is_enc) {
        cerr << "Only one process is available at one time" << endl;
        cerr << "Choose either -enc or -dec" << endl;
        exit(-1);
    }
    if (!is_dec && !is_enc) {
        cerr << "No process of data is presented" << endl;
        cerr << "Give -enc for encryption or -dec for decryption" << endl;
        exit(-1);
    }
    process = (is_dec ? "dec" : "enc");
}

bool IsArgumentExist(string ops, vector <string> & intake) {
    for (string x : intake) {
        if ("-" + ops == x) {
            return true;
        }
    }
    return false;
}

string GetArgumentValue(string ops, vector <string> & intake) {
    for (size_t it = 0; it + 1 < intake.size(); it++) {
        if (intake[it] == ("-" + ops)) {
            return intake[it + 1];
        }
    }
    return "#";
}

vector <uint64_t> ReadAllContent(ifstream &in) {
    // a lambda function that checks if the given file 
    // contains the data in correct format
    auto IsHexInt64 = [&](string line) {
        const char hex_digits[16] = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
        };
        // Check the length and characters of line
        if ((int)(line.size()) != 16) return false;
        for (char curr : line) {
            bool bad = true;
            for (int d = 0; d < 16; d++) {
                if (curr == hex_digits[d]) {
                    bad = false;
                    break;
                }
            }    
            if (bad) return false;
        }
        return true;
    };
    // Sanitize for printables
    auto Sanitizer = [&](string token) {
        string result;
        for (int it = 0; it < (int)(token.size()); it++) {
            int ascii_code = (int)(token.at(it));
            if (32 <= ascii_code && ascii_code <= 127) {
                result.push_back((char)(ascii_code));
            }
        }
        #if DEBUG_MOD
            cout << "Sanitizer process: " << token << " " << result << "\n";
            cout << "Token start code: " << (int)(token.front()) << "\n"; 
            cout << "Token size: " << token.size() << "\n";
            cout << "Resultant size: " << result.size() << "\n";
        #endif
        return result;
    };
    vector <uint64_t> result;
    string current_token;
    while (in >> current_token) {
        if (in.eof()) break;
        current_token = Sanitizer(current_token);
        if (current_token.empty()) continue;
        if (!IsHexInt64(current_token)) {
            cerr << "Invalid token " << current_token << "\n";
            cerr << "File contents have invalid format\n";
            cerr << "There should be 64 bit number in hexodecimal\n";
            cerr << "format written in each line seperately.\n";
            exit(-1);
        }
        result.push_back(FromHexToInt64(current_token));
    }
    return result;
}

uint64_t FromHexToInt64(string hh) {
    const char hex_digits[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };
    uint64_t shift_base = 4ull;
    uint64_t result = 0ull;
    for (size_t it = 0; it < hh.size(); it++) {
        uint64_t current_digit = 0;
        for (int pos = 0; pos < 16; pos++) {
            if (hex_digits[pos] == hh[it]) {
                current_digit = pos;
                break;
            }
        }
        result = (result << shift_base) + current_digit;
    }
    #if DEBUG_MOD
        cout << "from hex to int64 funciton:\n";
        cout << hh << "\n";
        cout << result << "\n";
        cout << hex << setw(16) << setfill('0') << result;
        cout << "\n";
    #endif
    return result;
}

string FromInt64ToHex(uint64_t nbr64) {
    const char hex_digits[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };
    string result;
    for (int pos = 15; pos >= 0; pos--) {
        int current_digit = static_cast<int>( ( ( nbr64 >> (pos * 4ull) ) & 15ull ) );
        result.push_back(hex_digits[current_digit]);
    }
    #if DEBUG_MOD
        cout << "from int64 to hex: \n";
        cout << nbr64 << "\n";
        cout << hex << setw(16) << setfill('0') << nbr64;
        cout << "\n";
        cout << result << "\n";
    #endif
    return result;
}

void InsertConfiguration(vector <uint64_t> data, Machine *m, uint64_t &Signature) {
    // First check the number of tokens for configs
    // If there is invalid number of tokens
    // Then notify and stop
    if ((int)(data.size()) != 14) {
        cerr << "The config file must contain 14 lines with\n";
        cerr << "64 bit number written in each line in hexodecimal format\n";
        cerr << ".X0.X1  ->  1st line,   concatination of two 32 bit numbers\n";
        cerr << ".X2.X3  ->  2nd line,   concatination of two 32 bit numebrs\n";
        cerr << ".X4.X5  ->  3rd line,   concatination of two 32 bit numebrs\n";
        cerr << ".X6.X7  ->  4th line,   concatination of two 32 bit numbers\n";
        cerr << ".K1     ->  5th line,   one whole 64 bit number\n";
        cerr << ".K2     ->  6th line,   one whole 64 bit number\n";
        cerr << ".K3     ->  7th line,   one whole 64 bit number\n";
        cerr << ".K4     ->  8th line,   one whole 64 bit number\n";
        cerr << ".K5     ->  9th line,   one whole 64 bit number\n";
        cerr << ".K6     ->  10th line,  one whole 64 bit number\n";
        cerr << ".K7     ->  11th line,  one whole 64 bit number\n";
        cerr << ".K8     ->  12th line,  one whole 64 bit number\n";
        cout << ".S      ->  13th line,  one whole 64 bit number\n";
        cerr << ".C1.C2  ->  14th line,  concatination of two 32 bit numbers\n";
        exit(-1);
    }
    vector <uint32_t> init_X;
    uint64_t x01 = data[X0_X1_index];
    init_X.push_back(static_cast<uint32_t>((x01 >> 32ull)));
    init_X.push_back(static_cast<uint32_t>((x01 & ((1ull << 32ull) - 1ull))));
    uint64_t x23 = data[X2_X3_index];
    init_X.push_back(static_cast<uint32_t>((x23 >> 32ull)));
    init_X.push_back(static_cast<uint32_t>((x23 & ((1ull << 32ull) - 1ull))));
    uint64_t x45 = data[X4_X5_index];
    init_X.push_back(static_cast<uint32_t>((x45 >> 32ull)));
    init_X.push_back(static_cast<uint32_t>((x45 & ((1ull << 32ull) - 1ull))));
    uint64_t x67 = data[X6_X7_index];
    init_X.push_back(static_cast<uint32_t>((x67 >> 32ull)));
    init_X.push_back(static_cast<uint32_t>((x67 & ((1ull << 32ull) - 1ull))));
    vector <uint64_t> init_K;
    init_K.resize(10);
    init_K[1] = data[K1_index];
    init_K[2] = data[K2_index];
    init_K[3] = data[K3_index];
    init_K[4] = data[K4_index];
    init_K[5] = data[K5_index];
    init_K[6] = data[K6_index];
    init_K[7] = data[K7_index];
    init_K[8] = data[K8_index];
    // Initializing Signature
    Signature = data[Sign_index];
    // Getting C1, C2;
    uint32_t init_C1 = static_cast<uint32_t>((data[C1_C2_index] >> 32ull));
    uint32_t init_C2 = static_cast<uint32_t>((data[C1_C2_index] & ((1ull << 32ull) - 1ull)));
    // Passing the arguments
    m->SetMachineValues(init_X, init_K, init_C1, init_C2);
}
