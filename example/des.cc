// ref: https://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm
#include <iostream>

#include "bitsel.hpp"


using namespace bitsel;
using namespace bitsel::literals;


#define ROUND 16

static const uint32_t IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

static const uint32_t IP_r[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9,  49, 17, 57, 25};

static const uint32_t PC1[56] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43,
    35, 27, 19, 11, 3,  60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,  62, 54,
    46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};
static const uint32_t PC2[48] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,
    26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
static const uint32_t E[48] = {32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
                               8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
                               16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
                               24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};
static const uint32_t S[8][16][4] = {
    {
        {14, 0, 4, 15},
        {4, 15, 1, 12},
        {13, 7, 14, 8},
        {1, 4, 8, 2},
        {2, 14, 13, 4},
        {15, 2, 6, 9},
        {11, 13, 2, 1},
        {8, 1, 11, 7},
        {3, 10, 15, 5},
        {10, 6, 12, 11},
        {6, 12, 9, 3},
        {12, 11, 7, 14},
        {5, 9, 3, 10},
        {9, 5, 10, 0},
        {0, 3, 5, 6},
        {7, 8, 0, 13},
    },
    {
        {15, 3, 0, 13},
        {1, 13, 14, 8},
        {8, 4, 7, 10},
        {14, 7, 11, 1},
        {6, 15, 10, 3},
        {11, 2, 4, 15},
        {3, 8, 13, 4},
        {4, 14, 1, 2},
        {9, 12, 5, 11},
        {7, 0, 8, 6},
        {2, 1, 12, 7},
        {13, 10, 6, 12},
        {12, 6, 9, 0},
        {0, 9, 3, 5},
        {5, 11, 2, 14},
        {10, 5, 15, 9},
    },
    {
        {10, 13, 13, 1},
        {0, 7, 6, 10},
        {9, 0, 4, 13},
        {14, 9, 9, 0},
        {6, 3, 8, 6},
        {3, 4, 15, 9},
        {15, 6, 3, 8},
        {5, 10, 0, 7},
        {1, 2, 11, 4},
        {13, 8, 1, 15},
        {12, 5, 2, 14},
        {7, 14, 12, 3},
        {11, 12, 5, 11},
        {4, 11, 10, 5},
        {2, 15, 14, 2},
        {8, 1, 7, 12},
    },
    {
        {7, 13, 10, 3},
        {13, 8, 6, 15},
        {14, 11, 9, 0},
        {3, 5, 0, 6},
        {0, 6, 12, 10},
        {6, 15, 11, 1},
        {9, 0, 7, 13},
        {10, 3, 13, 8},
        {1, 4, 15, 9},
        {2, 7, 1, 4},
        {8, 2, 3, 5},
        {5, 12, 14, 11},
        {11, 1, 5, 12},
        {12, 10, 2, 7},
        {4, 14, 8, 2},
        {15, 9, 4, 14},
    },
    {
        {2, 14, 4, 11},
        {12, 11, 2, 8},
        {4, 2, 1, 12},
        {1, 12, 11, 7},
        {7, 4, 10, 1},
        {10, 7, 13, 14},
        {11, 13, 7, 2},
        {6, 1, 8, 13},
        {8, 5, 15, 6},
        {5, 0, 9, 15},
        {3, 15, 12, 0},
        {15, 10, 5, 9},
        {13, 3, 6, 10},
        {0, 9, 3, 4},
        {14, 8, 0, 5},
        {9, 6, 14, 3},
    },
    {
        {12, 10, 9, 4},
        {1, 15, 14, 3},
        {10, 4, 15, 2},
        {15, 2, 5, 12},
        {9, 7, 2, 9},
        {2, 12, 8, 5},
        {6, 9, 12, 15},
        {8, 5, 3, 10},
        {0, 6, 7, 11},
        {13, 1, 0, 14},
        {3, 13, 4, 1},
        {4, 14, 10, 7},
        {14, 0, 1, 6},
        {7, 11, 13, 0},
        {5, 3, 11, 8},
        {11, 8, 6, 13},
    },
    {
        {4, 13, 1, 6},
        {11, 0, 4, 11},
        {2, 11, 11, 13},
        {14, 7, 13, 8},
        {15, 4, 12, 1},
        {0, 9, 3, 4},
        {8, 1, 7, 10},
        {13, 10, 14, 7},
        {3, 14, 10, 9},
        {12, 3, 15, 5},
        {9, 5, 6, 0},
        {7, 12, 8, 15},
        {5, 2, 0, 14},
        {10, 15, 5, 2},
        {6, 8, 9, 3},
        {1, 6, 2, 12},
    },
    {
        {13, 1, 7, 2},
        {2, 15, 11, 1},
        {8, 13, 4, 14},
        {4, 8, 1, 7},
        {6, 10, 9, 4},
        {15, 3, 12, 10},
        {11, 7, 14, 8},
        {1, 4, 2, 13},
        {10, 12, 0, 15},
        {9, 5, 6, 12},
        {3, 6, 10, 9},
        {14, 11, 13, 0},
        {5, 0, 15, 3},
        {0, 14, 3, 5},
        {12, 9, 5, 6},
        {7, 2, 8, 11},
    },
};

static const uint32_t P[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23,
                               26, 5, 18, 31, 10, 2,  8,  24, 14, 32, 27,
                               3,  9, 19, 13, 30, 6,  22, 11, 4,  25};

static const uint32_t Shift[ROUND] = {1, 1, 2, 2, 2, 2, 2, 2,
                                      1, 2, 2, 2, 2, 2, 2, 1};



bits get_new_r(const bits &l, const bits &r, const bits &k)
{
    // Expand R from 32-bits to 48-bits
    bits e = bits::zeros(48);
    for (uint32_t i = 0; i < 48; i++) {
        e.set(i, r[E[i] - 1]);
    }
    std::cout << "e = " << e.reverse().to_string() << std::endl;

    bits ke = k ^ e;
    std::cout << "ke = " << ke.reverse().to_string() << std::endl;

    // TODO: Be more elegant
    // Subtitution
    bits sb;
    for (uint32_t i = 0; i < 8; i++) {
        bits b = ke(6 * (i + 1) - 1, 6 * i);

        /* Interpret as integers instead of bits */
        std::size_t row = cat(b(5, 5), b(0, 0)).reverse().to_uint64();
        std::size_t col = b(4, 1).reverse().to_uint64();

        bits a = bits{4, S[i][col][row]};
        a = a.reverse();

        sb = cat(a, sb);
    }
    std::cout << "sb = " << sb.reverse().to_string() << std::endl;

    // Permutation
    bits p = bits::zeros(32);
    for (int32_t i = 0; i < 32; i++) {
        p.set(i, sb[P[i] - 1]);
    }

    return p ^ l;
}

bits DES(const bits &data, const bits &key)
{
    bits p_key = bits::zeros(56);

    // Key permutation
    for (uint32_t i = 0; i < 56; i++) {
        p_key.set(i, key[PC1[i] - 1]);
    }
    std::cout << p_key.reverse().to_string() << std::endl;

    // Split the key into left and right havles
    bits c = p_key(28 - 1, 0);
    bits d = p_key(56 - 1, 28);

    // Initial permutation of message
    bits ip = bits::zeros(64);
    for (uint32_t i = 0; i < 64; i++) {
        ip.set(i, data[IP[i] - 1]);
    }
    std::cout << ip.reverse().to_string() << std::endl;

    // Split the IP into left and right havles
    bits l = ip(32 - 1, 0);
    bits r = ip(64 - 1, 32);

    for (uint32_t rnd = 0; rnd < ROUND; rnd++) {
        std::cout << "c = " << c.reverse().to_string() << std::endl;
        std::cout << "d = " << d.reverse().to_string() << std::endl;
        std::cout << "l = " << l.reverse().to_string() << std::endl;
        std::cout << "r = " << r.reverse().to_string() << std::endl;

        std::cout << "Round " << rnd + 1 << ":\n";

        // Right rotate
        uint32_t shift = Shift[rnd];
        c = cat(c(shift - 1, 0), c(28 - 1, shift));
        d = cat(d(shift - 1, 0), d(28 - 1, shift));

        // cd means c is in the lower bit position
        bits cd = cat(d, c);
        std::cout << "cd = " << cd.reverse().to_string() << std::endl;

        bits k = bits::zeros(48);
        for (uint32_t i = 0; i < 48; i++) {
            k.set(i, cd[PC2[i] - 1]);
        }
        std::cout << "k = " << k.reverse().to_string() << std::endl;

        bits ol = l;
        l = r;
        r = get_new_r(ol, r, k);
    }
    std::cout << "c = " << c.reverse().to_string() << std::endl;
    std::cout << "d = " << d.reverse().to_string() << std::endl;
    std::cout << "l = " << l.reverse().to_string() << std::endl;
    std::cout << "r = " << r.reverse().to_string() << std::endl;

    bits res = cat(l, r);


    // Final Permutation
    bits enc_data = bits::zeros(64);
    for (int32_t i = 0; i < 64; i++) {
        enc_data.set(i, res[IP_r[i] - 1]);
    }
    return enc_data;
}

int main()
{
    // The first bit of data is "0". The last bit is "0". We read from left to
    // right. So we need to reverse the bitstring at initialization and when
    // printing
    bits data = "0x0123456789ABCDEF"_u().reverse();
    bits key = "0x133457799BBCDFF1"_u().reverse();

    std::cout << "Data = " << data.reverse().to_string() << std::endl;
    std::cout << "Key = " << key.reverse().to_string() << std::endl;

    bits enc_data = DES(data, key);

    // Should be 85E813540F0AB405
    std::cout << "Encrypted Data = " << enc_data.reverse().to_string()
              << std::endl;
}
