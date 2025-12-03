#pragma once

#include <string>
#include <array>
#include <cstddef> // For std::size_t
#include <utility> // For std::index_sequence

// Compile-time random number generator for obfuscation keys
// Adapted from: https://www.foonathan.net/2017/01/constexpr-random/
namespace detail {
    // Generate a pseudo-random number based on a seed
    constexpr unsigned int linear_congruent_generator(unsigned int n) {
        return n * 1103515245 + 12345;
    }

    // Get a compile-time seed that varies per file and line
    constexpr unsigned int get_seed(const char* file, int line) {
        // Combine file and line information for a reasonably unique seed
        return linear_congruent_generator(
            static_cast<unsigned int>(file[0]) +
            static_cast<unsigned int>(file[1]) +
            static_cast<unsigned int>(file[2]) +
            static_cast<unsigned int>(line)
        );
    }

    // Encrypt a single character at compile time
    constexpr char encrypt_char(char c, unsigned int key) {
        return c ^ static_cast<char>(key & 0xFF);
    }

    // A type to hold the obfuscated string data
    template <std::size_t N>
    struct ObfuscatedData {
        std::array<char, N> data;
        unsigned int key;

        constexpr ObfuscatedData(const char (&s)[N], unsigned int k) : key(k) {
            for (std::size_t i = 0; i < N; ++i) {
                data[i] = encrypt_char(s[i], linear_congruent_generator(k + i));
            }
        }

        // Decrypt the string into a std::string at runtime
        std::string decrypt() const {
            std::string decrypted_str;
            decrypted_str.reserve(N - 1); // Exclude null terminator
            for (std::size_t i = 0; i < N - 1; ++i) { // Exclude null terminator from decryption
                decrypted_str += encrypt_char(data[i], linear_congruent_generator(key + i));
            }
            return decrypted_str;
        }
    };
} // namespace detail

// Macro to obfuscate a string literal
#define OBFUSCATE_STRING(s) \
    ([] { \
        constexpr const char* str = s; \
        constexpr std::size_t N = sizeof(s) / sizeof(s[0]); \
        constexpr unsigned int seed = detail::get_seed(__FILE__, __LINE__); \
        constexpr detail::ObfuscatedData<N> obfuscated_data(s, seed); \
        return obfuscated_data; \
    }())