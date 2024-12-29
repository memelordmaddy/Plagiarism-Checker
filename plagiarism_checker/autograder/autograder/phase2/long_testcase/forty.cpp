#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
//-----------------------------------------------------------------------------
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Function to compute a rolling hash for a sub-vector from start of given length
std::size_t rolling_hash(const std::vector<int>& v, int start, int length, std::size_t base = 257, std::size_t mod = 1e9 + 7) {
    std::size_t hash = 0;
    for (int i = 0; i < length; i++) {
        hash = (hash * base + v[start + i]) % mod;
    }
    return hash;
}

// Finds exact matching subsequences of a given length in two submissions
std::pair<std::vector<std::pair<int, int>>, int> find_exact_matches(const std::vector<int>& submission1,
    const std::vector<int>& submission2, int length) {
    std::vector<std::pair<int, int>> matches; // Storing exact match indices
    std::unordered_map<size_t, std::vector<int>> subseq_map; // Maps hash to starting indices in submission1
    std::size_t base = 257; // Base for hashing
    std::size_t mod = 1e9 + 7; // Modulus for hashing

    // Computing hashes for all subsequences of given length in submission1
    for (int i = 0; i <= submission1.size() - length; i++) {
        std::size_t hash = rolling_hash(submission1, i, length, base, mod);
        subseq_map[hash].push_back(i);
    }

    std::set<int> matched_indices1; // For Tracking matched indices in submission1
    std::set<int> matched_indices2; // For Tracking matched indices in submission2
    int total_length = 0;

    // Find matching subsequences in submission2 by comparing hashes
    for (int i = 0; i <= submission2.size() - length; ++i) {
        std::size_t hash = rolling_hash(submission2, i, length, base, mod);
        if (subseq_map.find(hash) != subseq_map.end()) { // Hash match found
            for (int index1 : subseq_map[hash]) { // Check all starting positions in submission1 with the same hash
                int index2 = i;
                bool overlap = false;

                // Check for overlap with already matched indices
                for (int k = 0; k < length; k++) {
                    if (matched_indices1.count(index1 + k) || matched_indices2.count(index2 + k)) {
                        overlap = true;
                        break;
                    }
                }

                // If no overlap, add the match
                if (!overlap) {
                    matches.emplace_back(index1, index2);
                    total_length += length;

                    // Mark indices as matched
                    for (int k = 0; k < length; k++) {
                        matched_indices1.insert(index1 + k);
                        matched_indices2.insert(index2 + k);
                    }

                    i += length - 1; // Move the index to avoid overlapping matches
                    break; // Move to the next subsequence in submission2
                }
            }
        }
    }

    return {matches, total_length};
}

// Calculate total matching length of non-overlapping subsequences for lengths from 10 to 20
int calculate_total_length(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int total_length = 0;
    std::set<int> matched_indices1;
    std::set<int> matched_indices2;

    for (int len = 10; len <= 20; len++) {
        auto [matches, length] = find_exact_matches(submission1, submission2, len);
        for (const auto& match : matches) {
            int index1 = match.first;
            int index2 = match.second;
            bool overlap = false;

            // Check for overlap before counting match
            for (int k = 0; k < len; k++) {
                if (matched_indices1.count(index1 + k) || matched_indices2.count(index2 + k)) {
                    overlap = true;
                    break;
                }
            }

            // If no overlap, add match to total length
            if (!overlap) {
                total_length += len;

                // Mark indices as matched
                for (int k = 0; k < len; k++) {
                    matched_indices1.insert(index1 + k);
                    matched_indices2.insert(index2 + k);
                }
            }
        }
    }

    return total_length;
}

// Find the longest approximate match with >=80% similarity in both submissions
std::tuple<int, int, int> find_longest_approximate_match(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int max_length = 0;
    int start1 = -1, start2 = -1;
    double max_match_percentage = 0.0;

    int m = submission1.size();
    int n = submission2.size();

    if (m < 30 || n < 30) {
        return {0, -1, -1};
    }

    // Dynamic programming table to store lengths of longest common suffixes
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // Fill the DP table
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (submission1[i - 1] == submission2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = 0;
            }
        }
    }

    // Iterate over all possible lengths
    for (int len = 30; len <= std::min(m, n); ++len) {
        // Check for matches of the current length
        for (int i = len; i <= m; ++i) {
            for (int j = len; j <= n; ++j) {
                int match_length = dp[i][j];
                if (match_length >= len) {
                    double match_percentage = static_cast<double>(match_length) / len;
                    if (match_percentage >= 0.8 && match_percentage > max_match_percentage) {
                        max_length = match_length;
                        max_match_percentage = match_percentage;
                        start1 = i - match_length;
                        start2 = j - match_length;
                    }
                }
            }
        }
    }

    return {max_length, start1, start2};
}


// Main function to match submissions and return the result array
std::array<int, 5> match_submissions(std::vector<int>& submission1,
    std::vector<int>& submission2) {
    std::array<int, 5> result = { 0, 0, 0, 0, 0 }; // initialize result array

    // Compute total length of exact matches
    result[1] = calculate_total_length(submission1, submission2);
    bool short_submission = false;
      // Handling small sized vectors which could lead to wrong results
    if(std::max(submission1.size(),submission2.size()) <= 40){
        short_submission = true;
        if(result[1] >= 20){
            result[0] = 1;
        }
    }

    // Finding longest approximate match and store results
    auto [max_length, start1, start2] = find_longest_approximate_match(submission1, submission2);
    result[2] = max_length;
    result[3] = start1;
    result[4] = start2;
    if(!short_submission){
        // Determine if match is significant
        if (result[1] > 0.3*((submission1.size() + submission2.size())/2) || max_length > 0.1*((submission1.size() + submission2.size())/2)) {
            result[0] = 1;
        }
    }
    return result;
}