#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <cstdint> // for int64_t


// For the rolling hash function we have used the below reference
// https://www.geeksforgeeks.org/introduction-to-rolling-hash-data-structures-and-algorithms/
std::vector<int64_t> rolling_hash(std::vector<int> s, int window_size, int64_t base = 10, int64_t mod = 1000000007)
{
    int n = s.size();
    std::vector<int64_t> power(n + 1, 1);
    std::vector<int64_t> hash_values(n - window_size + 1);
 
    // Precompute the powers of the base modulo the mod
    for (int i = 1; i <= n; i++) {
        power[i] = (power[i - 1] * base) % mod;
    }
 
    // Compute the hash value of the first window
    int64_t current_hash = 0;
    for (int i = 0; i < window_size; i++) {
        current_hash = (current_hash * base + s[i]) % mod;
    }
    hash_values[0] = current_hash;
 
    // Compute the hash values of the rest of the substrings
    for (int i = 1; i <= n - window_size; i++) {
        // Remove the contribution of the first character in the window
        current_hash = (current_hash - (power[window_size - 1] * s[i - 1]) % mod + mod) % mod;
 
        // Shift the window by one character and add the new character to the hash
        current_hash = (current_hash * base + s[i + window_size - 1]) % mod;
 
        hash_values[i] = current_hash;
    }
    return hash_values;
}

int helper1(std::vector<int> &v1, std::vector<int> &v2, int window_size){
    std::vector<int64_t> hash1 = rolling_hash(v1, window_size);
    std::vector<int64_t> hash2 = rolling_hash(v2, window_size);
    int result = 0;
    if(hash1.size() < hash2.size()){
        for (const auto& h1 : hash1) {
            for (const auto& h2 : hash2) {
                if (h1 == h2) {
                    result++;
                    break;
                }
            }
        }
    }
    if(hash1.size() >= hash2.size()){
        for (const auto& h2 : hash2) {
            for (const auto& h1 : hash1) {
                if (h1 == h2) {
                    result++;
                    break;
                }
            }
        }
    }
    return result;
}


std::tuple<int, int, int> findMaxWindowSize(const std::vector<int>& vec1, const std::vector<int>& vec2, double targetMatch = 0.8) {
    int n1 = vec1.size();
    int n2 = vec2.size();
    int maxWindowSize = 0;
    int startIdx1 = -1;
    int startIdx2 = -1;

    // DP table to store lengths of matching subarrays
    std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1, 0));
    // DP table to store the number of mismatches in each subarray
    std::vector<std::vector<int>> mismatches(n1 + 1, std::vector<int>(n2 + 1, 0));

    // Fill DP table and keep track of mismatches
    for (int i = 1; i <= n1; ++i) {
        for (int j = 1; j <= n2; ++j) {
            if (vec1[i - 1] == vec2[j - 1]) {
                    mismatches[i][j] = mismatches[i - 1][j - 1]; 
                    dp[i][j] = dp[i - 1][j - 1] + 1; // Increment based on the previous match
                    int temp = (1 -targetMatch)* (dp[i][j] + 1);
                    if(mismatches[i][j] + 1 <= temp && i - 1 - dp[i-1][j-1] > 0 && j -1 - dp[i-1][j-1] > 0) dp[i][j]++;
            } else {
                // If elements don't match, consider it a mismatch
                int potentialWindowSize = dp[i - 1][j - 1] + 1;
                int potentialMismatches = mismatches[i - 1][j - 1] + 1;

                // Calculate maximum allowed mismatches for this potential window size
                int maxMismatches = ((1 - targetMatch) * potentialWindowSize);

                // Check if the potential mismatches stay within allowed limits
                if (potentialMismatches <= maxMismatches) {
                    dp[i][j] = potentialWindowSize;
                    mismatches[i][j] = potentialMismatches;
                } else {
                    dp[i][j] = 0; // Reset dp since we exceeded mismatches
                    mismatches[i][j] = 0; // Reset mismatches
                }
            }

            // Update maxWindowSize and starting indices if a larger valid window is found
            if (dp[i][j] > maxWindowSize) {
                maxWindowSize = dp[i][j];
                startIdx1 = i - maxWindowSize;
                startIdx2 = j - maxWindowSize;
            }
        }
    }
    if(maxWindowSize < 30){
        maxWindowSize = 0;
        startIdx1 = -1;
        startIdx2 = -1;
    }

    return std::make_tuple(maxWindowSize, startIdx1, startIdx2);
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int m = std::min(submission1.size(), submission2.size());
    std::vector<int> matches;
    auto [maxWindow, startIdx1, startIdx2] = findMaxWindowSize(submission1, submission2, 0.8);
    result[2] = maxWindow;
    result[3] = startIdx1;
    result[4] = startIdx2;
    for (int i = 10; i <= 20; i++) {
        matches.push_back(helper1(submission1, submission2, i));
    }
    int p = 10*matches[0];
    p -= 9*matches[1];
    result[1] = p;
    result[0] = p > 0.4 * m ? 1 : 0;
    return result;
}