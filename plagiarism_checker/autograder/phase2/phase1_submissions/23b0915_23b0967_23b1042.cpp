#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <unordered_map>
#include <string>
#include <algorithm>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

const int BASE = 31;
const int MOD = 1e9 + 7;

int initial_hash(const std::vector<int>& tokens, int start, int length) {   // Computes a hash value for a subsequence of tokens using polynomial hashing
    int hash_value = 0;
    for (int i = 0; i < length; ++i) {
        hash_value = (1LL * hash_value * BASE + tokens[start + i]) % MOD;
    }
    return hash_value;
}

int mod_exp(int base, int exp, int mod) {   // Computes (base^exp)% mod using modular exponentiation by squaring

    int result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) { 
            result = (1LL * result * base) % mod;
        }
        base = (1LL * base * base) % mod;
        exp /= 2;
    }
    return result;
}

int roll_hash(int old_hash, int old_value, int new_value, int length) {  // Updates the hash value for a rolling hash by removing the old token
    
    int high_order_term = (1LL * old_value * mod_exp(BASE, length - 1, MOD)) % MOD;
    old_hash = (old_hash - high_order_term + MOD) % MOD;  
    old_hash = (1LL * old_hash * BASE + new_value) % MOD;  
    return old_hash;
}

// This below function is used to find the longest approximate match position and length
void longest_approximate_match(const std::vector<int>& submission1, const std::vector<int>& submission2, int& longest_length, int& start_index1, int& start_index2) {
   
    const int threshold_percent = 80;
    const int min_match_length = 30;
    longest_length = 0;
    start_index1 = -1;
    start_index2 = -1;

    for (int i = 0; i <= submission1.size()-min_match_length; ++i) {
        for (int j = 0; j <= submission2.size()-min_match_length; ++j) {
            int match_count = 0;
            int window_length = std::min((int)submission1.size()-i, (int)submission2.size()-j);
            
            // Comparing tokens in the window
            for (int k = 0; k < window_length; ++k) {
                if (submission1[i + k] == submission2[j + k]) {
                    match_count++;
                }

                int required_match_count = (threshold_percent*(k + 1))/ 100;
                if (match_count >= required_match_count && (k + 1) >= min_match_length) {
                    if (k + 1 > longest_length) {
                        longest_length = k + 1;
                        start_index1 = i;
                        start_index2 = j;
                    }
                }
            }
        }
    }
}


std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    int min_exact_length = 10;
    int max_exact_length = 20;
    int total_exact_matchlength = 0;

    std::array<int, 5> result = {0, 0, 0, -1, -1};
    

    std::vector<bool> matched1(submission1.size(), false);
    std::vector<bool> matched2(submission2.size(), false);

    for (int len = max_exact_length; len >= min_exact_length; len--) {
       
        std::unordered_map<int, std::vector<int>> subseq_hash;  // Map to store hash values for subsequences
        
        int hash1 = initial_hash(submission1, 0, len);
        subseq_hash[hash1].push_back(0);

        for (int i = 1; i <= submission1.size() - len; ++i) {
            hash1 = roll_hash(hash1, submission1[i - 1], submission1[i + len - 1], len);   // Using Rabin-Karp rolling hash for submission1 
            subseq_hash[hash1].push_back(i);
        }
        
        int hash2 = initial_hash(submission2, 0, len);  // Using Rabin-Karp rolling hash for submission2 
        if (subseq_hash.count(hash2)) {
            for (int start1 : subseq_hash[hash2]) {
                if (std::none_of(matched1.begin() + start1, matched1.begin() + start1 + len, [](bool matched) { return matched; })) {
                    std::fill(matched1.begin() + start1, matched1.begin() + start1 + len, true);
                    std::fill(matched2.begin(), matched2.begin() + len, true);
                    total_exact_matchlength += len;
                    break; // We break for avoiding counting multiple matches for the same hash
                }
            }
        }

        for (int j = 1; j <= submission2.size() - len; ++j) {
            hash2 = roll_hash(hash2, submission2[j - 1], submission2[j + len - 1], len);

            if (subseq_hash.count(hash2)) {
                for (int start1 : subseq_hash[hash2]) {
                    if (std::none_of(matched1.begin() + start1, matched1.begin() + start1 + len, [](bool matched) { return matched; })) {
                        std::fill(matched1.begin() + start1, matched1.begin() + start1 + len, true);
                        std::fill(matched2.begin() + j, matched2.begin() + j + len, true);
                        total_exact_matchlength += len;
                        break;
                    }
                }
            }
        }
    }

    longest_approximate_match(submission1, submission2, 
                                   result[2], result[3], result[4]);

    result[1] = total_exact_matchlength;

    const int exactMatchThreshold = 0.5*(std::min(submission1.size(),submission2.size()));
    const int approxMatchThreshold = 30;

    if (total_exact_matchlength > exactMatchThreshold || 
        (result[2] >= approxMatchThreshold && 
         result[2] >= 0.3 * std::min(submission1.size(), submission2.size()))) {
        result[0] = 1;
    }

    return result;
}
