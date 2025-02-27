#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Define a struct to hold the 128-bit hash value
typedef struct {
    uint64_t h1; // First 64 bits of the hash
    uint64_t h2; // Second 64 bits of the hash
} uint128_t;

// by default: little endian
#if !defined(_big_endian_)

// MurmurHash3 32-bit implementation
uint32_t murmurhash3_32(const void *key, size_t len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key; // Cast input data to bytes
    const size_t nblocks = len / 4; // Number of 4-byte blocks

    size_t i = 0;
    uint32_t h1 = seed; // Initialize hash state

    // Constants for mixing
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Process each 4-byte block
    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);
    for (i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i]; // Read a 4-byte block

        // Mix the block
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17); // Rotate left by 15 bits
        k1 *= c2;

        // Mix the hash state
        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19); // Rotate left by 13 bits
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Handle remaining bytes (less than 4 bytes)
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) { // len % 4
        case 3:
            k1 ^= tail[2] << 16; // Process third byte
        case 2:
            k1 ^= tail[1] << 8; // Process second byte
        case 1:
            k1 ^= tail[0]; // Process first byte
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17); // Rotate left by 15 bits
            k1 *= c2;
            h1 ^= k1;
    }

    // Final mixing to ensure well-distributed hash values
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1; // Return the final 32-bit hash value
}

// MurmurHash3 128-bit implementation
void murmurhash3_128(const void *key, size_t len, uint32_t seed, uint128_t *out)
{
    const uint8_t *data = (const uint8_t *)key; // Cast input data to bytes
    const size_t nblocks = len / 16; // Number of 16-byte blocks

    size_t i = 0;
    uint64_t h1 = seed; // Initialize hash state
    uint64_t h2 = seed;

    // Constants for mixing
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    // Process each 16-byte block
    const uint64_t *blocks = (const uint64_t *)(data);
    for (i = 0; i < nblocks; i++) {
        uint64_t k1 = blocks[i * 2]; // Read first 64 bits of the block
        uint64_t k2 = blocks[i * 2 + 1]; // Read next 64 bits of the block

        // Mix k1
        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33); // Rotate left by 31 bits
        k1 *= c2;
        h1 ^= k1;

        // Mix h1
        h1 = (h1 << 27) | (h1 >> 37); // Rotate left by 27 bits
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        // Mix k2
        k2 *= c2;
        k2 = (k2 << 33) | (k2 >> 31); // Rotate left by 33 bits
        k2 *= c1;
        h2 ^= k2;

        // Mix h2
        h2 = (h2 << 31) | (h2 >> 33); // Rotate left by 31 bits
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    // Handle remaining bytes (less than 16 bytes)
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) { // len % 16
        case 15:
            k2 ^= (uint64_t)tail[14] << 48;
        case 14:
            k2 ^= (uint64_t)tail[13] << 40;
        case 13:
            k2 ^= (uint64_t)tail[12] << 32;
        case 12:
            k2 ^= (uint64_t)tail[11] << 24;
        case 11:
            k2 ^= (uint64_t)tail[10] << 16;
        case 10:
            k2 ^= (uint64_t)tail[9] << 8;
        case 9:
            k2 ^= (uint64_t)tail[8] << 0;
            k2 *= c2;
            k2 = (k2 << 33) | (k2 >> 31); // Rotate left by 33 bits
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= (uint64_t)tail[7] << 56;
        case 7:
            k1 ^= (uint64_t)tail[6] << 48;
        case 6:
            k1 ^= (uint64_t)tail[5] << 40;
        case 5:
            k1 ^= (uint64_t)tail[4] << 32;
        case 4:
            k1 ^= (uint64_t)tail[3] << 24;
        case 3:
            k1 ^= (uint64_t)tail[2] << 16;
        case 2:
            k1 ^= (uint64_t)tail[1] << 8;
        case 1:
            k1 ^= (uint64_t)tail[0] << 0;
            k1 *= c1;
            k1 = (k1 << 31) | (k1 >> 33); // Rotate left by 31 bits
            k1 *= c2;
            h1 ^= k1;
    }

    // Final mixing to ensure well-distributed hash values
    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    h2 ^= h2 >> 33;
    h2 *= 0xff51afd7ed558ccdULL;
    h2 ^= h2 >> 33;
    h2 *= 0xc4ceb9fe1a85ec53ULL;
    h2 ^= h2 >> 33;

    h1 += h2;
    h2 += h1;

    // Store the final hash value
    out->h1 = h1;
    out->h2 = h2;
}

#else // big endian
// Convert big-endian to host byte order (for 32-bit values)
uint32_t big_endian_to_host32(uint32_t value) {
    return ((value & 0xFF) << 24) |
           ((value & 0xFF00) << 8) |
           ((value & 0xFF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

// Convert big-endian to host byte order (for 64-bit values)
uint64_t big_endian_to_host64(uint64_t value) {
    return ((value & 0xFF) << 56) |
           ((value & 0xFF00) << 40) |
           ((value & 0xFF0000) << 24) |
           ((value & 0xFF000000) << 8) |
           ((value & 0xFF00000000) >> 8) |
           ((value & 0xFF0000000000) >> 24) |
           ((value & 0xFF000000000000) >> 40) |
           ((value & 0xFF00000000000000) >> 56);
}

// MurmurHash3 32-bit implementation (supports big-endian)
uint32_t murmurhash3_32(const void *key, size_t len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key; // Cast input data to bytes
    const size_t nblocks = len / 4; // Number of 4-byte blocks

    size_t i = 0;
    uint32_t h1 = seed; // Initialize hash state

    // Constants for mixing
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Process each 4-byte block
    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);
    for (i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i]; // Read a 4-byte block

        // Convert big-endian to host byte order
        k1 = big_endian_to_host32(k1);

        // Mix the block
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17); // Rotate left by 15 bits
        k1 *= c2;

        // Mix the hash state
        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19); // Rotate left by 13 bits
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Handle remaining bytes (less than 4 bytes)
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) { // len % 4
        case 3:
            k1 ^= tail[2] << 16; // Process third byte
        case 2:
            k1 ^= tail[1] << 8; // Process second byte
        case 1:
            k1 ^= tail[0]; // Process first byte
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17); // Rotate left by 15 bits
            k1 *= c2;
            h1 ^= k1;
    }

    // Final mixing to ensure well-distributed hash values
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1; // Return the final 32-bit hash value
}

// MurmurHash3 128-bit implementation (supports big-endian)
void murmurhash3_128(const void *key, size_t len, uint32_t seed, uint128_t *out)
{
    const uint8_t *data = (const uint8_t *)key; // Cast input data to bytes
    const size_t nblocks = len / 16; // Number of 16-byte blocks

    size_t i = 0;
    uint64_t h1 = seed; // Initialize hash state
    uint64_t h2 = seed;

    // Constants for mixing
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    // Process each 16-byte block
    const uint64_t *blocks = (const uint64_t *)(data);
    for (i = 0; i < nblocks; i++) {
        uint64_t k1 = blocks[i * 2]; // Read first 64 bits of the block
        uint64_t k2 = blocks[i * 2 + 1]; // Read next 64 bits of the block

        // Convert big-endian to host byte order
        k1 = big_endian_to_host64(k1);
        k2 = big_endian_to_host64(k2);

        // Mix k1
        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33); // Rotate left by 31 bits
        k1 *= c2;
        h1 ^= k1;

        // Mix h1
        h1 = (h1 << 27) | (h1 >> 37); // Rotate left by 27 bits
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        // Mix k2
        k2 *= c2;
        k2 = (k2 << 33) | (k2 >> 31); // Rotate left by 33 bits
        k2 *= c1;
        h2 ^= k2;

        // Mix h2
        h2 = (h2 << 31) | (h2 >> 33); // Rotate left by 31 bits
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    // Handle remaining bytes (less than 16 bytes)
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) { // len % 16
        case 15:
            k2 ^= (uint64_t)tail[14] << 48;
        case 14:
            k2 ^= (uint64_t)tail[13] << 40;
        case 13:
            k2 ^= (uint64_t)tail[12] << 32;
        case 12:
            k2 ^= (uint64_t)tail[11] << 24;
        case 11:
            k2 ^= (uint64_t)tail[10] << 16;
        case 10:
            k2 ^= (uint64_t)tail[9] << 8;
        case 9:
            k2 ^= (uint64_t)tail[8] << 0;
            k2 *= c2;
            k2 = (k2 << 33) | (k2 >> 31); // Rotate left by 33 bits
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= (uint64_t)tail[7] << 56;
        case 7:
            k1 ^= (uint64_t)tail[6] << 48;
        case 6:
            k1 ^= (uint64_t)tail[5] << 40;
        case 5:
            k1 ^= (uint64_t)tail[4] << 32;
        case 4:
            k1 ^= (uint64_t)tail[3] << 24;
        case 3:
            k1 ^= (uint64_t)tail[2] << 16;
        case 2:
            k1 ^= (uint64_t)tail[1] << 8;
        case 1:
            k1 ^= (uint64_t)tail[0] << 0;
            k1 *= c1;
            k1 = (k1 << 31) | (k1 >> 33); // Rotate left by 31 bits
            k1 *= c2;
            h1 ^= k1;
    }

    // Final mixing to ensure well-distributed hash values
    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    h2 ^= h2 >> 33;
    h2 *= 0xff51afd7ed558ccdULL;
    h2 ^= h2 >> 33;
    h2 *= 0xc4ceb9fe1a85ec53ULL;
    h2 ^= h2 >> 33;

    h1 += h2;
    h2 += h1;

    // Store the final hash value
    out->h1 = h1;
    out->h2 = h2;
}
#endif

int _test_main(int argc, char *argv[])
{
    const char *str = "Hello, World!";
    uint32_t seed = 42; // Seed value

    int choice;
    printf("Choose MurmurHash3 version:\n");
    printf("1. 32-bit\n");
    printf("2. 128-bit\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            // Compute the 32-bit hash
            uint32_t hash = murmurhash3_32(str, strlen(str), seed);
            printf("MurmurHash3 32-bit: 0x%08x\n", hash);
            break;
        }
        case 2: {
            // Compute the 128-bit hash
            uint128_t hash;
            murmurhash3_128(str, strlen(str), seed, &hash);
            printf("MurmurHash3 128-bit: 0x%016lx%016lx\n", hash.h1, hash.h2);
            break;
        }
        default:
            printf("Invalid choice!\n");
            break;
    }

    return 0;
}

