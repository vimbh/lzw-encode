#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono> 
    // auto start = std::chrono::high_resolution_clock::now();
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration2 = std::chrono::duration<double, std::milli>(stop-start);
    // std::cout << "time while file: " << duration2.count() << std::endl;

#define MAX_INDEX 32768

// Pass in a copy of index so we don't modify the dict index in case it's used in a future iteration
// Function flips MSB of index to 1 -> isolate high/low bytes -> swap and combine
uint16_t byteManipulate(uint16_t value, uint16_t &msb) {
    value |= msb;
    uint16_t hi = (value & 0xff00) >> 8;
    uint16_t lo = value & 0xff;
    value = (lo << 8) | hi;
    
    return value;
}

int main(int argc, char* argv[]) {

    // Check #no provided args is correct
    if (argc != 3) {
        std::cout << "Please only provide two args: input_file_path & output_file_path" << std::endl;
        return 1; 
    }

    // Use (var | MSB) to flip MSB of value to 1; defined once to avoid redundancy
    uint16_t msb = 0; // 0000
    msb -= 1; // 1111
    msb >>= 1; // 0111
    msb += 1; // 1000

    std::ifstream inFile(argv[1]);

    // Clear this later
    std::ofstream out;
    out.open(argv[2], std::ofstream::out | std::ofstream::trunc);
    out.close();
    //

    std::ofstream outFile(argv[2], std::ios::binary | std::ios::app);

    // Check files opens correctly
    if (!inFile || !outFile )
    {
        std::cout << "Unable to open or write to File." << std::endl;
    }


    if(inFile.is_open() && outFile.is_open()) {

        char c;
        std::string p = "";
        std::unordered_map<std::string, uint16_t> codeDict;
        uint16_t index = 0;

        while(inFile.get(c)) {

            if (index < MAX_INDEX) {

                std::string pc = p + c;

                if (codeDict.find(pc) != codeDict.end() || pc.length() == 1) {

                    p = pc;

                } else {

                    codeDict[pc] = index++;

                    // Output chars if pc is 2 chars or less, else output code
                    if (p.length() == 1) {
                        // std::cout << static_cast<char>(p[0]) << std::endl;
                        outFile.write(reinterpret_cast<const char*>(&p[0]), sizeof(p[0]));                 
                    } else if (p.length() == 2) {
                        // std::cout << static_cast<char>(p[0]) << static_cast<char>(p[1]) << std::endl; 
                        outFile.write(reinterpret_cast<const char*>(&p[0]), sizeof(p[0]));                 
                        outFile.write(reinterpret_cast<const char*>(&p[1]), sizeof(p[1]));
                    } else {
                        // std::cout << static_cast<int>(codeDict[p]) << std::endl;
                        auto trans_index = byteManipulate(codeDict[p],msb);
                        outFile.write(reinterpret_cast<const char*>(&trans_index), sizeof(trans_index));
                    }

                    p = c;
                }

            } else {
                // If dict is full, ignore LZW algorithm
                outFile.write(reinterpret_cast<const char*>(&c), sizeof(c));  

            }

        }
        outFile.write(reinterpret_cast<const char*>(&c), sizeof(c));
        // think about if c works everytime for last iter

        inFile.close();
        outFile.close();
    }


}
