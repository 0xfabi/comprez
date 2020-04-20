#include <stdio.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>

//https://www.cplusplus.com/reference/climits/

using namespace std;

//TODO: try different block sizes ...
vector<unsigned char> blocks;
const auto BUFFERSIZE = (unsigned int) (pow(2.0, (double)(sizeof(decltype(blocks)::value_type) * 8)) / 8);

ifstream::pos_type get_filesize(const string& filename)
{
    ifstream in(filename.c_str(), ifstream::ate | ifstream::binary);
    return in.tellg(); 
}

void compress(FILE *ptr) 
{
    unsigned char buffer[BUFFERSIZE];
    unsigned char result[BUFFERSIZE];
    unsigned int last_pos[BUFFERSIZE];

    auto bytesGet = fread(buffer, 1, sizeof(buffer), ptr); // read BUFFERSIZE bytes to our buffer
    auto bytesWorked = bytesGet;
    unsigned int maxNull = 0;
    unsigned int maxEins = 0;
    while(bytesGet > 0) {
        //printf("Reading %ld\n", bytesGet);
        for (unsigned int i = 0; i < bytesGet; i++) {
            unsigned char actual_byte = buffer[i];
            bool last = false;
            last_pos[i] = 0;

            // first bit
            result[i] = 0; // this is really important! Do not forget or start crying...
            result[i] |= actual_byte & 1;

            // 0 part
            for (unsigned int j = 0; j < (sizeof(char) * 8) - 1; j++) {
                if ((actual_byte & 1) == 0) {
                    actual_byte >>= 1;
                    result[i] <<= 1;
                    result[i] |= actual_byte & 1;
                    ++last_pos[i];
                } else {
                    actual_byte >>= 1;
                }
            }

            // cover last bit of 0 part
            if ((actual_byte & 1) == 0) {
                last = true;
            }

            // 1 part
            actual_byte = buffer[i];
            for (unsigned int j = 0; j < (sizeof(char) * 8) - 1; j++) {
                if ((actual_byte & 1) == 1) {
                    actual_byte >>= 1;
                    result[i] <<= 1;
                    result[i] |= actual_byte & 1;
                } else {
                    actual_byte >>= 1;
                }
            }
            // cover last bit of 1 part
            if (!last) {
                last_pos[i] = (sizeof(char) * 8) - 1;
            }

            // printf("%x ", buffer[i]);
            // printf("%x ", result[i]);
            // printf("%d\n", last_pos[i]);
        }
        
        unsigned int counter = 0;
        bool bit_zero = true;

        for (unsigned int i = 0; i < BUFFERSIZE; i++) {
            unsigned char actual_byte = result[i];
            for (unsigned int j = 0; j < (sizeof(char) * 8); j++) {

                if ((actual_byte & 1) == 0) {
                    if (bit_zero) {
                        ++counter;
                    } else {
                        blocks.emplace_back(counter);
                        if(maxNull < counter){
                            maxNull = counter;
                        }
                        counter = 1;
                    }
                    bit_zero = true;

                } else if ((actual_byte & 1) == 1) {
                    if (!bit_zero) {
                        ++counter;
                    } else {
                        blocks.emplace_back(counter);
                        if(maxEins < counter){
                            maxEins = counter;
                        }
                        counter = 1;
                    }
                    bit_zero = false;
                }
                actual_byte >>= 1;
            }
        }

        if(bytesGet < BUFFERSIZE) {
            break;
        }
        bytesGet = fread(buffer, 1, sizeof(buffer), ptr);
        bytesWorked += bytesGet;
   }

   printf("Reading %ld\n", bytesWorked);
   printf("maxEins %d\n", maxEins);
   printf("maxNull %d\n", maxNull);
   
}


int main()
{
    printf("Hello, World! This is a native C program compiled on the command line.\n");
    //../data/test.JPG
    string filename = "/home/fherrmann/Dokumente/Projekte/comprez/data/Unbenannt.png";
    cout << "Please enter filename with path." << endl;
    cin >> filename;

    const string outputFile = "/home/fherrmann/Dokumente/Projekte/comprez/data/test.cpz";

    printf("Buffer size: %d\n", BUFFERSIZE);

    FILE *ptr;
    ptr = fopen(filename.c_str(), "rb");  // r for read, b for binary
    unsigned int size = get_filesize(filename.c_str());
    printf("Input file size: %d\n", size);
    if(size <= 0) {
        cout << "Datei konnte nicht gelese werden oder ist leer" << endl;
        return -1;
    }
    
    FILE *write_ptr;
    write_ptr = fopen(outputFile.c_str(),"wb");  // w for write, b for binary
    compress(ptr);

    auto resultSize = fwrite(blocks.data(), sizeof(decltype(blocks)::value_type), blocks.size(), write_ptr);
    printf("Output file size: %ld\n", resultSize);

    fclose(ptr);
    fclose(write_ptr);

    return 0;
}