#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <cmath>
#include <fstream>


class CPU {
public:
    CPU() : TC(0), IR("NONE"), PS("0"), PC(0), A("0000000000000000000000"), registers(10, "0000000000000000000000") {}

    std::string decimalToBinary22(int decimal) {
        std::string binary = "";

        bool isNegative = (decimal < 0);
        decimal = std::abs(decimal);

        for (int i = 21; i >= 0; i--) {
            int bit = (decimal >> i) & 1;
            binary += std::to_string(bit);
        }

        if (isNegative) {
            for (char &bit : binary) {
                bit = (bit == '0') ? '1' : '0';
            }

            bool carry = 1;
            for (int i = 21; i >= 0; i--) {
                char &bit = binary[i];
                if (bit == '0') {
                    bit = '1';
                    carry = 0;
                    break;
                } else {
                    bit = '0';
                }
            }

            if (carry) {
                binary = '1' + binary;
            }
        }

        return binary;
    }




    std::pair<std::string, std::string> getCommandAndValue(const std::string& input) {
        std::regex regexPattern(R"(\s+)");

        std::sregex_token_iterator iter(input.begin(), input.end(), regexPattern, -1);
        std::sregex_token_iterator end;

        std::pair<std::string, std::string> result;

        if (iter != end) {
            result.first = *iter;
            ++iter;

            if (iter != end) {
                result.second = *iter;
            }
        }

        return result;
    }

    void mainProc(const std::vector<std::string>& cmdlist) {
        for (const std::string& curr_cmd : cmdlist) {
            std::pair<std::string, std::string> res = getCommandAndValue(curr_cmd);
            cmd = res.first;
            val = res.second;
            TC = 0;

            if (cmd == "move" || cmd == "add" || cmd == "save" || cmd == "add_mode2") {
                waitForKeyPress();
                tact1();
                waitForKeyPress();
                tact2();
            } else {
                std::cout << "Invalid command: " << cmd << std::endl;
            }
        }
    }

    void waitForKeyPress() {
           std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
       }

    void tact1() {
        TC = 1;
        IR = cmd + " "+ "|" + " " + val;
        PC += 1;
        printState();
    }

    void tact2() {
        TC = 2;
        doCommand();

        if (cmd != "save") {
            PS = A[0];
        }

        printState();
//
    }

    void doCommand() {
        if (cmd == "move") {
            move();
        } else if (cmd == "add") {
            add();
        } else if (cmd == "save") {
            save();
        } else if (cmd == "add_mode2"){
            add_mode2();
        }
    }

    void move() {
        if (val[0] != 'R') {
            A = decimalToBinary22(std::stoi(val));
        } else{
            int n = std::stoi(val.substr(1)) - 1;
            A = registers[n];
        }
    }

    void add() {
        if (val[0] != 'R') {
            std::string binary_val = decimalToBinary22(std::stoi(val));
            int carry = 0;

            for (int i = 21; i >= 0; i--) {
                int bit_A = A[i] - '0';
                int bit_val = binary_val[i] - '0';

                int sum = bit_A + bit_val + carry;
                carry = sum / 2;

                A[i] = '0' + (sum % 2);
            }
        }else{
            int n = std::stoi(val.substr(1)) - 1;
            std::string binary_b = registers[n];
            int carry = 0;

            for (int i = 21; i >= 0; i--) {
                int bit_A = A[i] - '0';
                int bit_val = binary_b[i] - '0';

                int sum = bit_A + bit_val + carry;
                carry = sum / 2;

                A[i] = '0' + (sum % 2);

    }}}

    void add_mode2() {
        std::string result = "";

        if (val[0] != 'R') {
            std::string binary_b = decimalToBinary22(std::stoi(val));

            for (int i = 0; i <= 21; i++) {
                    if (A[i] == '0' && binary_b[i] == '0' || A[i] == '1' && binary_b[i] == '1') {
                        result += '0';
                    } else {
                        result += '1';
                    }
                }

            A = result;
        } else {
            int n = std::stoi(val.substr(1)) - 1;
            if (n >= 1 && n <= 10) {
                std::string binary_b = registers[n];

                for (int i = 0; i <= 21; i++) {
                        if (A[i] == '0' && binary_b[i] == '0' || A[i] == '1' && binary_b[i] == '1') {
                            result += '0';
                        } else {
                            result += '1';
                        }
                    }

                A = result;
            }
        }
    }



    void save() {
        if (val.size() >= 2 && val[0] == 'R') {
            int registerIndex = std::stoi(val.substr(1)) - 1;
            if (registerIndex >= 0 && registerIndex < registers.size()) {
                registers[registerIndex] = A;
                std::cout << registers[registerIndex] << std::endl;
            } else {
                std::cout << "Invalid register index: " << val << std::endl;
            }
        } else {
            std::cout << "Invalid register format: " << val << std::endl;
        }
    }

    void printState() {
           std::cout << "Current State:\n";
           std::cout << "IR: " << IR << "\n";
           std::cout << "A: " << A << "\n";
           std::cout << "Registers:\n";
           for (int i = 0; i < registers.size(); i++) {
               std::cout << "R" << i + 1 << ": " << registers[i] << "\n";
           }
           std::cout << "PS: " << PS << "\n";
           std::cout << "PC: " << PC << "\n";
           std::cout << "TC: " << TC << "\n";
       }

private:
    int TC;
    std::string IR;
    std::string PS = "0";
    int PC;
    std::string cmd;
    std::string val;
    std::string A;
    std::vector<std::string> registers;
};


int main() {
    CPU cpu;

    std::ifstream file("/Users/Admin/Desktop/CLionProjects/aes_lab2/comands.txt");

    if (!file.is_open()) {
        std::cerr << "Помилка відкриття файлу comands.txt" << std::endl;
        return 1;
    }

    std::vector<std::string> cmdlist;
    std::string line;

    std::cout << "Команди з файлу comands.txt:" << std::endl;
    while (std::getline(file, line)) {
        cmdlist.push_back(line);
        std::cout << line << std::endl;
    }

    file.close();

    std::cout << "\nВиконання команд:" << std::endl;

    cpu.mainProc(cmdlist);

    return 0;
}


