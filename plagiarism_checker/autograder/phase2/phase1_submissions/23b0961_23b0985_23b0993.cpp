#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <set>
#include <map>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Rolling Hash function for computing hashes for a given window size (k)
// Source : https://www.geeksforgeeks.org/introduction-to-rolling-hash-data-structures-and-algorithms/
// Formula : hash = sum(i = 0 to k-1, (A[i] * base^(k-1-i))) % mod
// hash = (A[0] * base^(k-1) + A[1] * base^(k-2) + ... + A[k-2] * base + A[k-1]) % mod
std::vector<unsigned long long> rolling_hash(std::vector<int> &A, int window_size) {
    unsigned long long base = 37;
    unsigned long long mod = 10000000007ULL;
    int length = A.size();
    std::vector<unsigned long long> powers(window_size, 1);
    std::vector<unsigned long long> hash_values(length - window_size + 1);

    // Pre-computing the powers of base and take remainder wrt mod
    for (int i = 1; i < window_size; i++) {
        powers[i] = (powers[i-1] * base) % mod;
    }

    // Compute hash of first window
    unsigned long long current_hash = 0;
    for (int i = 0; i < window_size; i++) {
        current_hash += ((A[i] * powers[window_size - i - 1]) % mod);
        current_hash = current_hash % mod;
    }
    hash_values[0] = current_hash;

    // Calculating remaining hashes using the previous hash
    for (int i = 1; i <= length - window_size; i++) {

        // Remove contribution of A[i - 1] (just before the updated window)
        // Add mod before taking remainder for bound checking (safety)
        current_hash = (current_hash + mod - A[i - 1] * powers[window_size - 1] % mod) % mod;

        // Shift the window by one, i.e., multiply current_hash by base
        // Add contribution of A[i + window_size - 1] (end of updated window)
        current_hash = (current_hash * base + A[i + window_size - 1]) % mod;
        hash_values[i] = current_hash;
    }
    
    return hash_values;
}

// Levenshtein distance for checking difference between two vectors of tokens in terms of similarity
// Source : https://www.geeksforgeeks.org/introduction-to-levenshtein-distance/
int levenshtein_distance(std::vector<int> &sequence1, std::vector<int> &sequence2) {
    int m = sequence1.size();
    int n = sequence2.size();

    // Matrix to store and compute distances
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    
    // First column
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }

    // First row
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }

    // Interior rows and columns
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            
            // If match, same as previous
            if (sequence1[i - 1] == sequence2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {

                // If no match, add 1 to smallest distance among neighbours
                dp[i][j] = 1
                        + std::min(
 
                            // Obtained via insert
                            dp[i][j - 1],
                            std::min(
 
                                // Obtained via remove
                                dp[i - 1][j],
 
                                // Obtained via replace
                                dp[i - 1][j - 1]));
            }
        }
    }
    return dp[m][n];
}

// ALMOST WORKS
// Function to count total number of tokens matched in both submissions via hashes
int count_matching_tokens(std::vector<int> &submission1, std::vector<int> &submission2) {
    int count = 0;

    // Hash window for our case is 10 (minimum length for a valid match)
    std::vector<unsigned long long> hash_values1 = rolling_hash(submission1, 10);
    std::vector<unsigned long long> hash_values2 = rolling_hash(submission2, 10);

    // Store hashes of first submission as a set and matches found as another set
    std::set<unsigned long long> hashes(hash_values1.begin(), hash_values1.end());
    std::set<unsigned long long> matched_hashes;

    // Iterate on the hashes of second submission
    int i = 0;
    while (i < hash_values2.size()) {

        // If a unique match is found, increment count by 10
        if (hashes.find(hash_values2[i]) != hashes.end() &&
            matched_hashes.find(hash_values2[i]) == matched_hashes.end()) {
            matched_hashes.insert(hash_values2[i]);
            count += 10;

            // Check subsequent hashes until there is a mismatch/it is already present
            int j = i + 1;
            while (j < hash_values2.size() &&
                   hashes.find(hash_values2[j]) != hashes.end() &&
                   matched_hashes.find(hash_values2[j]) == matched_hashes.end()) {
                matched_hashes.insert(hash_values2[j]);
                
                // Increment count and jump value by 1 for each match
                count++;
                j++;
            }

            // If no more matches, i jumps to the next unchecked hash (j + 1)
            i = j + 1;

        } else {
            
            // No match at all? Just increment i
            i++;

        }
    }
    return count;
}

// Function to guess size of longest match based on hashes
int longest_match_guess(std::vector<int> &submission1, 
        std::vector<int> &submission2, int initial_guess) {
    int window_size = initial_guess;
    int last_guess = initial_guess;

    // Iterate through window_size by doubling until we find no matches
    while (window_size <= std::min(submission1.size(), submission2.size())) {
        
        // Compute hashes
        std::vector<unsigned long long> hash_values1 = rolling_hash(submission1, window_size);
        std::vector<unsigned long long> hash_values2 = rolling_hash(submission2, window_size);

        // Store hash values and indices having those hashes in a map for first submission
        std::map<unsigned long long, std::vector<int>> hashes;
        for (int i = 0; i < hash_values1.size(); i++) {
            hashes[hash_values1[i]].push_back(i);
        }

        // Search hashes of second submission in the map, if found, update guess
        bool found = false;
        for (unsigned long long current_hash : hash_values2) {
            if (hashes.count(current_hash) > 0) {
                found = true;
                last_guess = window_size;
                break;
            }
        }

        // If not found, stop increasing window size, we have a good guess
        if(!found) break;
        
        // NOTE : Doubling messes up because you'd have to check over a large gap
        // Also your number of matches drops very fast on doubling, hence linear increment
        window_size += initial_guess;
    }
    return last_guess;
}

// THIS IS HARD
// Function to calculate longest approximate match between the two submissions
std::array<int, 3> longest_approximate_match(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    int max_approx_match = 0;
    int start_index1 = -1;
    int start_index2 = -1;

    // Get a guess value using initial guess of 30
    int window_size = longest_match_guess(submission1, submission2, 30);
    
    // Compute hashes for this new window size
    std::vector<unsigned long long> hash_values1 = rolling_hash(submission1, window_size);
    std::vector<unsigned long long> hash_values2 = rolling_hash(submission2, window_size);

    // Store hash values and indices having those hashes in a map for first submission
    std::map<unsigned long long, std::vector<int>> hashes;
    for (int i = 0; i < hash_values1.size(); i++) {
        hashes[hash_values1[i]].push_back(i);
    }

    // Search for hashes from second submission in the map
    for (int i = 0; i < hash_values2.size(); i++) {
        unsigned long long current_hash = hash_values2[i];
        if(hashes.count(current_hash) > 0) {
            for (int j : hashes[current_hash]) {
                int current_match = window_size;

                // We have indices and a lower bound for match length, now determine the exact length
                // Extend subsequences by 1, until levenshtein distance (mismatch) becomes 5%
                while(j + current_match < submission1.size() && 
                    i + current_match < submission2.size()) {

                    // Take subsequences for levenshtein distance
                    std::vector<int> subseq1(submission1.begin() + j, 
                        submission1.begin() + j + current_match);
                    std::vector<int> subseq2(submission2.begin() + i, 
                        submission2.begin() + i + current_match);
                    
                    // Take distance and verify, if mismatches > 5%, break
                    int distance = levenshtein_distance(subseq1, subseq2);
                    if(distance > 0.05 * current_match) break;
                    current_match++;
                }

                // If current match length is more than the maximum, update
                if(current_match > max_approx_match) {
                    max_approx_match = current_match;
                    start_index1 = j;
                    start_index2 = i;
                }
            }
        }
    }

    std::array<int, 3> result = {max_approx_match, start_index1, start_index2};
    return result;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // Get results of first to fourth values
    result[1] = count_matching_tokens(submission1, submission2);
    std::array<int, 3> max_match = longest_approximate_match(submission1, submission2);
    result[2] = max_match[0];
    result[3] = max_match[1];
    result[4] = max_match[2];

    // Main flag : 1 if either 50% tokens match or longest match is over 120 tokens
    int length = std::min(submission1.size(), submission2.size());
    if(result[1] >= 0.5 * length || result[2] >= 120) result[0] = 1;
    return result;
    // End TODO
}

