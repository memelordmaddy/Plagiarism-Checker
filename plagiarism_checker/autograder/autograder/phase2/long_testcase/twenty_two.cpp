#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>

// -----------------------------------------------------------------------------
#include <set>
#include <unordered_map>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Helper function to check for an approximate match with a threshold of 20%
bool hasApproximateMatch(const std::vector<int>& sub1, const std::vector<int>& sub2,
                         int start1, int start2, int length) {
    int mismatches = 0;
    for (int i = 0; i < length; ++i) {
        if (sub1[start1 + i] != sub2[start2 + i]) {
            ++mismatches;
            if (mismatches >= length / 5) return false;
        }
    }
    return true;
}

// Function to find the longest approximate match using binary search
// Function to find the longest approximate match using binary search
int findLongestApproximateMatch(const std::vector<int>& sub1, const std::vector<int>& sub2,
                                 int start1, int start2) {
    int left = 30; 
    int right = std::min(static_cast<int>(sub1.size() - start1),
                         static_cast<int>(sub2.size() - start2));
    int best_length = 0;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (hasApproximateMatch(sub1, sub2, start1, start2, mid)) {
            best_length = mid;
            left = mid + 1; 
        } else {
            right = mid - 1; 
        }
    }
    return best_length;
}

// Helper function to check for exact matches
void checkExactMatches(const std::vector<int>& submission1,
                       const std::vector<int>& submission2,
                       std::unordered_map<int, std::vector<int>>& token_map,
                       std::set<int>& used_indices,
                       int& total_length_exact, int& start_index1,
                       int& start_index2) {
    for (size_t i = 0; i < submission1.size(); ++i) {
        int longest_match_length = 0;
        int matching_j = -1;
// Check if the token_map contains any matches for submission1[i]
        if (token_map.find(submission1[i]) == token_map.end()) {
            continue; // No match found, skip to the next iteration
        }
        for (int j : token_map[submission1[i]]) {
            if (used_indices.count(j) > 0) continue;

            int length = 10;
            bool match = true; //check for match till 10 positions

            for (size_t k = 0; k < 10; ++k) {
                if (i + k >= submission1.size() || j + k >= submission2.size() ||
                    submission1[i + k] != submission2[j + k] ||
                    used_indices.count(j + k) > 0) {
                    match = false;
                    break;
                }
            }

            if (match) {
                while (length < 20 && i + length < submission1.size() &&
                       j + length < submission2.size() &&
                       submission1[i + length] == submission2[j + length] &&
                       used_indices.count(j + length) == 0) {
                    ++length;
                }

                if (length > longest_match_length) {
                    longest_match_length = length;
                    matching_j = static_cast<int>(j);
                }
            }
        }

        if (longest_match_length >= 10) {
            total_length_exact += longest_match_length;

            for (size_t k = 0; k < longest_match_length; ++k) {
                used_indices.insert(matching_j + k);
            }

            if (start_index1 == -1 || start_index2 == -1) {
                start_index1 = static_cast<int>(i);
                start_index2 = matching_j;
            }

            i += longest_match_length - 1; // Move to end of matched segment
        }
    }
}

// Helper function to find approximate matches
void findApproximateMatches(const std::vector<int>& submission1,
                             const std::vector<int>& submission2,
                             int& longest_approximate_match_length,
                             std::array<int, 5>& result) {
    for (size_t i = 0; i < submission1.size(); ++i) {
        for (size_t j = 0; j < submission2.size(); ++j) {
            int length = findLongestApproximateMatch(submission1, submission2, i, j);
            if (length >= 30 && length > longest_approximate_match_length) {
                longest_approximate_match_length = length;
                result[3] = static_cast<int>(i);
                result[4] = static_cast<int>(j);
            }
        }
    }
}

// Main function to match submissions
std::array<int, 5> match_submissions(std::vector<int>& submission1,
                                       std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int total_length_exact = 0;
    int start_index1 = -1;
    int start_index2 = -1;
    int longest_approximate_match_length = 0;

    std::set<int> used_indices; 
    std::unordered_map<int, std::vector<int>> token_map;

    // Build a map for quick lookup of tokens in submission2
    for (size_t j = 0; j < submission2.size(); ++j) {
        token_map[submission2[j]].push_back(j);
    }

    // Check for exact matches
    checkExactMatches(submission1, submission2, token_map, used_indices,
                      total_length_exact, start_index1, start_index2);

    // Find approximate matches
    findApproximateMatches(submission1, submission2, longest_approximate_match_length, result);

    // Set the plagiarism flag based on a threshold
    int min_tokens = std::min(submission1.size(), submission2.size());
    if ((static_cast<double>(total_length_exact) / min_tokens >= 0.4) ||
        (static_cast<double>(longest_approximate_match_length) / min_tokens >= 0.2)) {
        result[0] = 1; 
    }

    result[1] = total_length_exact;
    result[2] = longest_approximate_match_length;
    return result;
}