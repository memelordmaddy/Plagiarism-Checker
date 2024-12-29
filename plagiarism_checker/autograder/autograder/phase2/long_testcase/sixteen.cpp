#include <array>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>

//Constants for hashing
const int64_t base = 257;
const int64_t mod = 1000000007;

//auxiliary function to build the KMP table
std::vector<int> buildKMPTable(const std::vector<int>& pattern) {
    std::vector<int> lps(pattern.size(), 0);
    int len = 0;
    for (int i = 1; i < pattern.size();) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else {
            if (len) {
                len = lps[len - 1];
            } else {
                lps[i++] = 0;
            }
        }
    }
    return lps;
}

//KMP search function to find all occurrences of the pattern/subsequences in the text
std::vector<int> KMPsearch(const std::vector<int>& text, const std::vector<int>& pattern) {
    std::vector<int> lps = buildKMPTable(pattern);
    std::vector<int> matchIndices;
    int i = 0, j = 0;

    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }
        if (j == pattern.size()) {
            matchIndices.push_back(i - j); //match found at index (i - j)
            j = lps[j - 1]; //prepare for the next match
        } else if (i < text.size() && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return matchIndices;
}

//calculate the hash for the first subarray of the specified length
int64_t computeHash(const std::vector<int>& sequence, int len) {
    int64_t hash = 0;
    int64_t power = 1;
    for (int i = 0; i < len; ++i) {
        hash = (hash + sequence[i] * power) % mod;
        power = (power * base) % mod;
    }
    return hash;
}

std::array<int, 5> match_submissions(std::vector<int>& submission1,
                                      std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    //for keeping track of short and long matches
    int shortMatchesLength = 0;
    int longestApproxMatchLength = 0;
    int startIndex1 = -1, startIndex2 = -1;

    //count exact matches (length 10-20)
    for (size_t len = 10; len <= 20; ++len) {
        for (size_t i = 0; i <= submission1.size() - len; ++i) {
            std::vector<int> pattern(submission1.begin() + i, submission1.begin() + i + len);
            auto matchIndices = KMPsearch(submission2, pattern);
            if (!matchIndices.empty()) {
                shortMatchesLength += len;

                //track the first occurrence of a match
                if (startIndex1 == -1) {
                    startIndex1 = i; //store the starting index of the first match in submission1
                    startIndex2 = matchIndices[0]; //store the starting index of the first match in submission2
                }

                //remove matched segments from submission2 to avoid overlapping
                for (int index : matchIndices) {
                    submission2.erase(submission2.begin() + index, submission2.begin() + index + len);
                }
            }
        }
    }

    //update longestApproxMatchLength if shortMatchesLength > 0
    if (shortMatchesLength > 0) {
        longestApproxMatchLength = shortMatchesLength;
    }

    // Count approximate matches (length 30+)
    for (size_t len = 30; len <= std::min(submission1.size(), submission2.size()); ++len) {
        std::unordered_map<int64_t, std::vector<int>> submission1Hashes;
        int64_t hash = computeHash(submission1, len);

        submission1Hashes[hash].push_back(0);

        int64_t power = 1;
        for (int k = 1; k < len; ++k) power = (power * base) % mod;

        for (size_t i = 1; i <= submission1.size() - len; ++i) {
            hash = (hash - submission1[i - 1] * power % mod + mod) % mod;
            hash = (hash * base + submission1[i + len - 1]) % mod;
            submission1Hashes[hash].push_back(i);
        }

        hash = computeHash(submission2, len);
        if (submission1Hashes.find(hash) != submission1Hashes.end()) {
            for (int idx1 : submission1Hashes[hash]) {
                int matchLength = 0;
                for (size_t k = 0; k < len; ++k) {
                    if (submission1[idx1 + k] == submission2[k]) {
                        matchLength++;
                    }
                }
                if (matchLength >= static_cast<int>(0.8 * len)) {
                    if (matchLength > longestApproxMatchLength) {
                        longestApproxMatchLength = matchLength;
                        startIndex1 = idx1;
                        startIndex2 = 0;
                    }
                }
            }
        }
    }

    //determine if the submissions are plagiarized based on the threshold
    const int plagiarismThreshold = 80;
    if (shortMatchesLength >= plagiarismThreshold || longestApproxMatchLength >= plagiarismThreshold) {
        result[0] = 1; //plagiarized
    } else {
        result[0] = 0; //not plagiarized
    }

    result[1] = shortMatchesLength;
    result[2] = longestApproxMatchLength;
    result[3] = startIndex1;
    result[4] = startIndex2;

    //output values for debugging
    //std::cout << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
    return result;
}

