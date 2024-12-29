#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------  
#include <unordered_set>
#include <unordered_map>
#include <utility>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here


// Computes blockwise hash values for a given vector for winnowing
std::vector<int> hash_function(const std::vector<int>& s1, int k, int base = 31, int mod = 2147483647) {            
    int n = s1.size();
    std::vector<int> powers(n + 1, 1); 

 
    for (int i = 1; i <= n; i++) {
        powers[i] = (1LL * powers[i - 1] * base) % mod;
    }

    std::vector<int> hash_vector((n + k - 1) / k); 
    int index = 0;

    // Compute hash for each k-length block
    for (int i = 0; i < n; i += k) {
        int hash_ = 0;
        for (int j = i; j < i + k && j < n; j++) {
            hash_ = (1LL * hash_ * base + s1[j]) % mod;
        }
        hash_vector[index++] = hash_;
    }

    return hash_vector;
}

// Winnowing function to identify fingerprints in the hash vector 
std::vector<std::pair<int, int>> winnowing(const std::vector<int>& hash_vector, int window) {
    int n = hash_vector.size();     
    std::vector<std::pair<int, int>> fingerprints;

    if (n < window) return fingerprints;

    int min_index = 0;
    for (int i = 1; i < window; i++) {
        if (hash_vector[i] < hash_vector[min_index]) {
            min_index = i;
        }
    }
    fingerprints.push_back({hash_vector[min_index], min_index});

    // Sliding window for selecting fingerprints that selects at least one fingerprint window distance apart
    for (int i = 1; i <= n - window; i++) {
        int new_element_index = i + window - 1;

        if (min_index < i) {
            min_index = i;
            for (int j = i; j <= new_element_index; j++) {
                if (hash_vector[j] < hash_vector[min_index]) {
                    min_index = j;
                }
            }
            fingerprints.push_back({hash_vector[min_index], min_index});
        } else {
            if (hash_vector[new_element_index] <= hash_vector[min_index]) {
                min_index = new_element_index;
                fingerprints.push_back({hash_vector[min_index], min_index});
            }
        }
    }

    return fingerprints;
}

// Calculates the length of the matching sequence between two fingerprint indices in the vectors `s1` and `s2`
int match_length(const std::vector<int>& s1, const std::vector<int>& s2, int k, std::pair<int, int> f1, std::pair<int, int> f2) {
    int length = 0;
    int idx1 = f1.second * k;
    int idx2 = f2.second * k;
    int i1 = idx1;
    int i2 = idx2;
    int n1 = s1.size();
    int n2 = s2.size();

    // Match forward
    while (i1 < n1 && i2 < n2 && s1[i1] == s2[i2]) {
        i1++;
        i2++;
        length++;
    }

    // Match backward
    i1 = idx1 - 1;
    i2 = idx2 - 1;
    while (i1 >= 0 && i2 >= 0 && s1[i1] == s2[i2]) {
        i1--;
        i2--;
        length++;
    }

    return length;
}

// Finds matching pairs of hashes between two fingerprints by using a hash map
std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> getMatchingPairsHashing(
    const std::vector<std::pair<int, int>>& vec1, 
    const std::vector<std::pair<int, int>>& vec2) {

    //Create a hash map to store elements from vec1
    std::unordered_map<int, std::pair<int, int>> hashMap;
    for (const auto& p : vec1) {
        hashMap[p.first] = p;
    }

    //Finding matching pairs by checking if first elements of vec2 exist in the hash map
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> matchingPairs;
    for (const auto& p : vec2) {
        auto it = hashMap.find(p.first);
        if (it != hashMap.end()) {
            matchingPairs.push_back({it->second, p});
        }
    }

    return matchingPairs;
}

//Calculates match_length, counting each position only once
int match_length_with_tracking(const std::vector<int>& s1, const std::vector<int>& s2, int k, 
                               std::pair<int, int> f1, std::pair<int, int> f2, 
                               std::vector<bool>& matched1, std::vector<bool>& matched2) {
    int length = 0;

    // Calculate starting indices based on hash positions
    int idx1 = f1.second * k;
    int idx2 = f2.second * k;
    int n1 = s1.size();
    int n2 = s2.size();

    // Match forward and mark positions as matched
    int i1 = idx1, i2 = idx2;
    while (i1 < n1 && i2 < n2 && s1[i1] == s2[i2]) {
        if (!matched1[i1] && !matched2[i2]) { // Only count if not previously matched
            matched1[i1] = true;
            matched2[i2] = true;
            length++;
        }
        i1++;
        i2++;
    }

    // Match backward and mark positions as matched
    i1 = idx1 - 1;
    i2 = idx2 - 1;
    while (i1 >= 0 && i2 >= 0 && s1[i1] == s2[i2]) {
        if (!matched1[i1] && !matched2[i2]) { // Only count if not previously matched
            matched1[i1] = true;
            matched2[i2] = true;
            length++;
        }
        i1--;
        i2--;
    }

    return length;
}

// Computes the total length of all matched sequences between the given two vectors
int total_length(std::vector<int>& s1, std::vector<int>& s2, 
                 std::vector<std::pair<int, int>>& f1, 
                 std::vector<std::pair<int, int>>& f2, int k) {
    int total_length = 0; 
    auto pairs = getMatchingPairsHashing(f1, f2);
    std::vector<bool> counted_s1(s1.size(), false);
    std::vector<bool> counted_s2(s2.size(), false);

    for (auto& p : pairs) {
        int match_len = match_length(s1, s2, k, p.first, p.second);
        
        int idx1 = p.first.second * k;
        int idx2 = p.second.second * k;

        //Only add to total length if indices have not been counted
        int actual_match_len = 0;
        for (int i = 0; i < match_len; ++i) {
            if (idx1 + i < s1.size() && idx2 + i < s2.size() &&
                !counted_s1[idx1 + i] && !counted_s2[idx2 + i]) {
                counted_s1[idx1 + i] = true;
                counted_s2[idx2 + i] = true;
                actual_match_len++;
            }
        }

        total_length += actual_match_len;
    }

    return total_length;
}

// Main function to calculate similarity score based on winnowing and matching length
int final_result1(std::vector<int>& s1, std::vector<int>& s2) {
    int n1 = s1.size();
    int n2 = s2.size();
    
    int k = 2;
    int t = 2;

    std::vector<int> hash1 = hash_function(s1, k);
    std::vector<int> hash2 = hash_function(s2, k);

    int window = t - k + 1;
    std::vector<std::pair<int, int>> f1 = winnowing(hash1, window);
    std::vector<std::pair<int, int>> f2 = winnowing(hash2, window);

    return total_length(s1, s2, f1, f2, k);
}

// Finds the longest approximate sequence and start indices in both vectors
std::tuple<int, int, int> findLongestVectorMatch(
    const std::vector<int>& s1,
    const std::vector<int>& s2,
    double threshold = 0.8) 
{
    size_t n = s1.size();
    size_t m = s2.size();
    int longestMatchLength = 0;
    int startIndexs1 = -1;
    int startIndexs2 = -1;

    // Compare each token in submission1 with each token in submission2
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            if (s1[i] == s2[j]) {
                int length = 1; 
                int k = 1;

                int matched_length = 1;
                while (i + k < n && j + k < m && (double(matched_length / length) > threshold)) {
                    if (s1[i + k] == s2[j + k]) {
                        length++;
                        matched_length++;
                        k++;
                    } else {
                        length++;
                        k++;
                    }
                }

                // Update longest match if the current is longer
                if (length > longestMatchLength) {
                    longestMatchLength = length;
                    startIndexs1 = i;
                    startIndexs2 = j;
                }
            }
        }
    }

    return std::make_tuple(startIndexs1, startIndexs2, longestMatchLength);
}

// Main function to compare two submissions and return match data
std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int n1 = submission1.size();
    int n2 = submission2.size();

    result[1] = final_result1(submission1, submission2);
    double threshold = 0.0;

    auto result_ = findLongestVectorMatch(submission1, submission2, threshold);
    result[2] = std::get<2>(result_);
    result[3] = std::get<0>(result_);
    result[4] = std::get<1>(result_);
    
    double combinedScore = (result[1] * 100.0) / (n1 + n2 - result[1]);
    combinedScore += (result[2] * 100.0) / (n1 + n2 - result[2]);

    result[0] = combinedScore > 40.0 ? 1 : 0;
    return result; 
}
