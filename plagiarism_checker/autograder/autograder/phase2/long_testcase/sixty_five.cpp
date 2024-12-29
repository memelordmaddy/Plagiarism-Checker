#include <array>
#include <cmath>
#include <iostream>
#include <span>
#include <vector>
// -----------------------------------------------------------------------------
#include <chrono>
#include <string>
#include <fstream> // ---- ADDED By TA: Tejas Sharma to fix compilation errors.
// -----------------------------------------------------------------------------
#include <unordered_map>

#define TEST_NUM_MATCHES 0

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// color codes for good printing
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string BOLD = "\033[1m";

template <typename T>
void printTimeTaken(T duration) {
    std::cout << CYAN << "Time taken in (s): " << duration << RESET << std::endl;
}

template <typename T>
void printContainer(T& v, const std::string& color = GREEN, std::string sep = " ") {
    for (auto& i : v) {
        std::cout << color << i << RESET << sep;
    }
    std::cout << "\n";
}

template <typename T>
void printContainerToFile(T& v, std::string filename) {
    std::ofstream fileStream(filename);
    if (!fileStream) {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
    for (auto& i : v) {
        fileStream << i << "\n";
    }
    fileStream << "\n";
    fileStream.close();
}

// @brief: Get the hash of a given substring from a vector
// @param `v`: vector of integers
// @param `i`: starting index of the substring
// @param `L`: length of the substring
std::string getHash(const std::vector<int>& v, int i, int L) {
    std::string key;
    key.reserve(L * sizeof(int));
    for (int j = 0; j < L; ++j) {
        key.append(reinterpret_cast<const char*>(&v[i + j]), sizeof(int));
    }
    return key;
}

// @brief: Calculates the all hashes of substrings of a given length from a
// vector and also maps these hashes to their corresponding substring's
// starting index
// @param `v`: vector of integers
// @param `L`: length of the substrings we want
// @return: map mapping each hash of a substring to its starting index
typedef std::unordered_multimap<std::string, int> SubstrMap;
SubstrMap allSubstrHashes(const std::vector<int>& v, int L) {
    SubstrMap substr_map;

    int n = v.size();
    for (int i = 0; i <= n - L; ++i) {
        std::string key = getHash(v, i, L);
        substr_map.emplace(key, i);
    }

    return substr_map;
}

// @brief: Checks if a substring of a vector has already been checked
// @param `visited`: vector of booleans to keep track of checked positions
// @param `i`: starting index of the substring
// @param `L`: length of the substring
// @return: true if some portion substring has already been checked, false
// otherwise
bool isAlreadyChecked(const std::vector<bool>& visited, int i, int L) {
    for (int k = i; k < i + L; ++k) {
        if (visited[k]) return true;
    }
    return false;
}

// @brief: Calculates the number of exact matches between two vectors
// @param `v1`: first vector of integers
// @param `v2`: second vector of integers
int numExactMatches(const std::vector<int>& v1, const std::vector<int>& v2) {
    if (v1.empty() || v2.empty()) return 0;

    const int n = v1.size();
    const int m = v2.size();
    int num = 0;

    std::vector<bool> used(m, false);

    for (int len = 20; len >= 10; --len) {
        if (len > n || len > m) continue;

        // get all hashes of substrings of length `len` from v1
        SubstrMap substr_map = allSubstrHashes(v1, len);

        // iterating through all non-checked substrings of length `len` in v2
        for (int i = 0; i <= m - len; ++i) {
            if (isAlreadyChecked(used, i, len)) continue;

            std::string key = getHash(v2, i, len);

            // returns start and end of a range of elements matching the key
            auto range = substr_map.equal_range(key);
            if (range.first != range.second) {
                for (int k = i; k < i + len; ++k)
                    used[k] = true;
                num += len;
            }
        }
    }
    return num;
}

// @brief: Calculates the longest approximate match between two vectors
// A longest approximate match is the longest sequence of elements from both
// vectors such that the length of longest common subsequences is at least 80%
// of the longer of the two subsequences
// @param `v1`: first vector of integers
// @param `v2`: second vector of integers
std::array<int, 3> longestApproxMatch(std::vector<int>& v1,
                                      std::vector<int>& v2) {
    int n = v1.size(), m = v2.size();
    int maxLen = 0, maxI = 0, maxJ = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int len = 0, match = 0;
            while (i + len < n && j + len < m) {
                if (v1[i + len] == v2[j + len]) {
                    match++;
                }
                len++;
                if (match >= 0.8 * len) {
                    if (len > maxLen) {
                        maxLen = len;
                        maxI = i;
                        maxJ = j;
                    }
                } else {
                    break;
                }
            }
        }
    }
    return {maxLen, maxI, maxJ};
}

// @brief: Checks if the two vectors are plagarized
bool isPlagged(const int exactMatches, const int approxMatches,
               const int v1Size, const int v2Size) {
    double exactMatchRatio = (double)exactMatches / v1Size;
    double approxMatchRatio = (double)approxMatches / v1Size;

    double threshold = 0.4;
    double weightExact = 0.8;
    double weightApprox = 1 - weightExact;
    double score = weightExact * exactMatchRatio + weightApprox * approxMatchRatio;

    if (score > threshold) return true;
    return false;
}

std::array<int, 5> match_submissions(std::vector<int>& submission1,
                                     std::vector<int>& submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    result[1] = numExactMatches(submission2, submission1);
    std::array<int, 3> approxMatch = longestApproxMatch(submission1, submission2);
    result[2] = approxMatch[0];
    result[3] = approxMatch[1];
    result[4] = approxMatch[2];
    result[0] = isPlagged(result[1],
                          result[2],
                          submission1.size(),
                          submission2.size());
    return result;
    // End TODO
}
