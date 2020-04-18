#include <stdio.h>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;

#define BUFFERSIZE 512

vector<unsigned int> blocks;


ifstream::pos_type get_filesize(const char* filename)
{
    ifstream in(filename, ifstream::ate | ifstream::binary);
    return in.tellg(); 
}

void compress(FILE *ptr) 
{
    unsigned char buffer[BUFFERSIZE];
    unsigned char result[BUFFERSIZE];
    unsigned int last_pos[BUFFERSIZE];

    fread(buffer, sizeof(buffer), 1, ptr); // read BUFFERSIZE bytes to our buffer

    for (unsigned int i = 0; i < BUFFERSIZE; i++) {
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
                    counter = 1;
                }
                bit_zero = true;

            } else if ((actual_byte & 1) == 1) {
                if (!bit_zero) {
                    ++counter;
                } else {
                    blocks.emplace_back(counter);
                    counter = 1;
                }
                bit_zero = false;
            }
            actual_byte >>= 1;
        }
    }
}


int main()
{
    printf("Hello, World! This is a native C program compiled on the command line.\n");

    FILE *ptr;
    ptr = fopen("../data/test.JPG", "rb");  // r for read, b for binary
    unsigned int size = get_filesize("../data/test.JPG");
    printf("%d", size);

    FILE *write_ptr;
    write_ptr = fopen("../data/test.cpz","wb");  // w for write, b for binary
    compress(ptr);
    fwrite(blocks.data(), sizeof(decltype(blocks)::value_type), blocks.size(), write_ptr);

    // unsigned int number_iterations = (size / BUFFERSIZE);
    // for (unsigned int iter = 0; iter < number_iterations; iter ++) {
    //     compress(ptr);
    //     fwrite(blocks.data(), sizeof(decltype(blocks)::value_type), blocks.size(), write_ptr);
    //     ptr += BUFFERSIZE;
    //     write_ptr += BUFFERSIZE;
    // }

    return 0;
}