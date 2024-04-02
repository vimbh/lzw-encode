#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono> 
#include <unordered_set>
    // auto start = std::chrono::high_resolution_clock::now();
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration2 = std::chrono::duration<double, std::milli>(stop-start);
    // std::cout << "time while file: " << duration2.count() << std::endl;



bool MSB_isOne(char &value) {
    // 1 << 7 -> 10000000
    return value & (1 << 7);
}


int main(int argc, char* argv[]) {

    // Check #no provided args is correct
    if (argc != 3) {
        std::cout << "Please only provide two args: input_file_path & output_file_path" << std::endl;
        return 1; 
    }

    // Use (var & MSB) to flip MSB of value to 0; defined once to avoid redundancy
    uint8_t msb = 0; // 0000
    msb -= 1; // 1111
    msb >>= 1; // 0111


    std::ifstream inFile(argv[1], std::ios::binary | std::ios::app);

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
        std::unordered_map<uint16_t, std::string> codeDict;
        std::unordered_set<std::string> codeSet;
        uint16_t index = 0;

        inFile.get(c);
        outFile.write(reinterpret_cast<const char*>(&c), sizeof(c)); 
        p = c;
        while(inFile.get(c)) {
                    std::cout << c << std::endl;

            // Following 8 bits represent an ASCII char
            if (!MSB_isOne(c)) {

                if (codeSet.find(p+c) == codeSet.end()) {

                    codeDict[index++] = p+c;
                    codeSet.insert(p+c);
                    p = c; 

                } else {        // If the concat. string is in the dict, we compound to get next dict entry
                    p = p+c;
                }
                outFile.write(reinterpret_cast<const char*>(&c), sizeof(c)); 

             // Following 16 bits represent an index
            } else { 

                uint8_t hiByte = c & msb;
                inFile.get(c);
                uint8_t loByte = c;
                uint16_t reqIndex = (hiByte << 8) | loByte;

                if (codeDict.find(reqIndex) != codeDict.end()) {

                    for (const auto& ch : codeDict[reqIndex]) {
                        outFile.write(reinterpret_cast<const char*>(&ch), sizeof(ch)); 
                    }

                    if (codeSet.find(p+codeDict[reqIndex][0]) == codeSet.end()) {
                        codeDict[index++] = p+codeDict[reqIndex][0];
                        codeSet.insert(p+codeDict[reqIndex][0]);
                    }

                // Special case: If the index hasn't been encountered yet, we return p + p[0]
                // Occurs when string concat occurs consecutively when encoding                
                } else {
                    
                    for (const auto& ch : p) {
                        outFile.write(reinterpret_cast<const char*>(&ch), sizeof(ch)); 
                    }
                    outFile.write(reinterpret_cast<const char*>(&p[0]), sizeof(p[0])); 

                    codeDict[index++] = p + p[0];
                    codeSet.insert(p + p[0]);
                }

                p = codeDict[reqIndex];
                
            }

        }  
 
        inFile.close();
        outFile.close();
    }


}
