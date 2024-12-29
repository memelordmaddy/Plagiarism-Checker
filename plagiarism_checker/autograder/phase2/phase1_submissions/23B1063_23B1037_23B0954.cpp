#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

#include <unordered_map>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

#define SHORTMINLEN 10
#define SHORTMAXLEN 20
#define LONGMINLEN 30
#define MODULO 1000000007
#define BASE 257
const double LONGTHRESHOLD = 0.8;

// function to compute hash
size_t compute_hash(const std::vector<int> &array, int start, int len) {
    size_t HashVal = 0;
    for(int i = 0; i < len;i++) {
        HashVal = (HashVal * BASE + array[start + i]) % MODULO;
    }
    return HashVal;
}

// Function to match submissions
std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    std::unordered_map<int, std::pair<int, int>> short_Hashes1;
    std::unordered_map<int, std::pair<int, int>> short_hashes2;
    // Variables to track matching details
    int TotalShortLength = 0;
    int LongestMatchLength = 0;
    int LongestMatchStart1 = -1;
    int LongestMatchStart2 = -1;
    std::vector<bool> Counted1(submission1.size(), false);
    std::vector<bool> Counted2(submission2.size(), false);

    // for loop for finding short matches
    for (int len = SHORTMINLEN; len <= SHORTMAXLEN;len++) {
        std::unordered_map<int, std::vector<int>> Hashes1;
        for (int i = 0; i + len <= submission1.size();i++) {
            int HashVal = compute_hash(submission1, i, len);
            Hashes1[HashVal].push_back(i);
        }

        for (int j = 0; j + len <= submission2.size();j++) {
            int HashVal = compute_hash(submission2, j, len);
            if (Hashes1.count(HashVal)) {
                for (int start1 : Hashes1[HashVal]) {
                    bool overlap = false;
                    for (int k = start1; k < start1 + len;k++) {
                        if (Counted1[k]) {
                            overlap = true;
                            break;
                        }
                    }
                    for (int k = j; k < j + len;k++) {
                        if (Counted2[k]) {
                            overlap = true;
                            break;
                        }
                    }
                    if (!overlap) {
                        TotalShortLength += len;
                        for (int k = start1; k < start1 + len;k++) Counted1[k] = true;
                        for (int k = j; k < j + len;k++) Counted2[k] = true;
                        if (len > LongestMatchLength) {
                            LongestMatchLength = len;
                            LongestMatchStart1 = start1;
                            LongestMatchStart2 = j;
                        }
                    }
                }
            }
        }
    }

    // for loop for finding long matches
    for (int len = LONGMINLEN; len <= submission1.size();len++) {
        for (int i = 0; i + len <= submission1.size();i++) {
            int HashVal = compute_hash(submission1, i, len);
            short_Hashes1[HashVal] = {i, len};
        }
        for (int j = 0; j + len <= submission2.size();j++) {
            int HashVal = compute_hash(submission2, j, len);
            if (short_Hashes1.count(HashVal)) {
                int start1 = short_Hashes1[HashVal].first;
                int common_len = 0;
                for (int k = 0; k < len;k++) {
                    if (submission1[start1 + k] == submission2[j + k]) ++common_len;
                }
                if (common_len >= LONGTHRESHOLD * len) {
                    if (common_len > LongestMatchLength) {
                        LongestMatchLength = common_len;
                        LongestMatchStart1 = start1;
                        LongestMatchStart2 = j;
                    }
                }
            }
        }
    }
    double maximum = std::max(submission1.size(),submission2.size());
    const double PLAGTHRESHOLD = 0.3;
    // initializing the result array
    result[0] = ((static_cast<double>(TotalShortLength)/maximum) > PLAGTHRESHOLD) ? 1 : 0;
    result[1] = TotalShortLength;
    result[2] = LongestMatchLength;
    result[3] = LongestMatchStart1;
    result[4] = LongestMatchStart2;
    return result;
    // End TODO
}