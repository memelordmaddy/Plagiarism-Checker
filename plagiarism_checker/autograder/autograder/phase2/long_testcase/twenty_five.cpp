#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <unordered_set>
#include <set>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Compute KMP prefix table
std::vector<int> computeKMPTable(const std::vector<int>& pattern) {
    int m = pattern.size();
    std::vector<int> kmpTable(m + 1, 0);
    int j = 0;
    int i = 1;
    kmpTable[0] = -1;

    while (i < m) {
        if (pattern[j] != pattern[i]) {
            kmpTable[i] = j;
            while (j >= 0 && pattern[j] != pattern[i]) {
                j = kmpTable[j];
            }
        } else {
            kmpTable[i] = kmpTable[j];
        }
        i++;
        j++;
    }
    kmpTable[m] = j;
    return kmpTable;
}

// KMP search for a pattern in the text
std::set<int> kmpSearch(const std::vector<int>& pattern, const std::vector<int>& text) {
    std::vector<int> kmpTable = computeKMPTable(pattern);
    int m = pattern.size();
    int n = text.size();
    int j = 0;  // index for pattern[]
    int i = 0;
    std::set<int> found;

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
            if (j == m) {
                found.insert(i - j); // Add starting index of the match
                j = kmpTable[j];     // Continue searching for next match
            }
        } else {
            j = kmpTable[j];
            if (j < 0) {
                i++;
                j++;
            }
        }
    }

    return found;  // Return the set of match starting indices
}

int findShortPatterns(const std::vector<int>& base, const std::vector<int>& compare) {
    std::unordered_set<int> visitedIndices;  // Track visited starting indices
    for (int length = 20; length >= 10; --length) {
        for (size_t start = 0; start <= base.size() - length; ++start) {
            std::vector<int> pattern(base.begin() + start, base.begin() + start + length);
            std::set<int> matchIndices = kmpSearch(pattern, compare);
            // Check if there are any valid matches
            if (!matchIndices.empty()) {
                bool isGoodMatch = true;  // Assume it's good until proven otherwise
                // Process all found match indices
                for (int matchIndex : matchIndices) {
                 //Check if matching indices from matchIndex to matchIndex+length-1 are in visited
                    for (int i = matchIndex; i < matchIndex + length; ++i) {
                        if (visitedIndices.find(i) != visitedIndices.end()) {
                            isGoodMatch = false;  // Found a previously visited index
                            break;
                        }
                    }
                    // If it's a good match, accumulate total length and mark indices as visited
                    if (isGoodMatch) {
                        // Mark all indices from matchIndex to matchIndex + length - 1 as visited
                        for (int i = matchIndex; i < matchIndex + length; ++i) {
                            visitedIndices.insert(i);
                        }
                        break;  // Break to go to the next pattern
                    }
                }
            }
        }
    }             
    return visitedIndices.size();
}

//LCS : Longest Common Subsequence
// Function to calculate the LCS table for the entire sequences
//dp[i][j] represents the LCS length for the subarrays base[0:i] and compare[0:j]
void computeLCSTable(const std::vector<int>& base, const std::vector<int>& compare,
                     std::vector<std::vector<int>>& dp) {
    int m = base.size();
    int n = compare.size();
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (base[i - 1] == compare[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1; // Characters match
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]); // Take the maximum
            }
        }
    }
}

// Function to find the LCS length for a given sub-pattern using the LCS table
int getLCSLength(const std::vector<std::vector<int>>& dp, 
                    int baseStart, int baseEnd, int compareStart, int compareEnd) {
    return dp[baseEnd][compareEnd] - dp[baseStart][compareStart];
}

// Function to find the largest approximate pattern using the same LCS table
int findApproxPattern(const std::vector<int>& base, const std::vector<int>& compare, 
                        int& start1, int& start2) {
    const double threshold = 0.8;
    int m = base.size();
    int n = compare.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    computeLCSTable(base, compare, dp);
    for (int baseLength = m; baseLength >= 30; --baseLength) {
        for (int baseStart = 0; baseStart <= m - baseLength; ++baseStart) {
            int baseEnd = baseStart + baseLength;

            for (int compareLength = baseLength+10; compareLength >= baseLength-10; --compareLength){
                for (int compareStart = 0; compareStart <= n - compareLength; ++compareStart) {
                    int compareEnd = compareStart + compareLength;

                    // Use the LCS table to get the LCS length for the current sub-pattern
                    int lcsLength = getLCSLength(dp, baseStart, baseEnd, compareStart, compareEnd);
                
                    // Check if this match meets the 80% threshold
                    int maxLength = std::max(baseLength,compareLength);
                    double matchPercentage = static_cast<double>(lcsLength) / maxLength;
                    if (matchPercentage >= threshold) {
                        start1 = baseStart;
                        start2 = compareStart;
                        return maxLength;
                    }
                }
            }  
        }
    }
    return 0;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    result[1] = findShortPatterns(submission1, submission2);

    int baseIndex = -1;
    int compareIndex = -1;
    result[2] = findApproxPattern(submission1, submission2, baseIndex, compareIndex);
    result[3] = baseIndex;
    result[4] = compareIndex;

    // Set Flag for plag check
    if (submission1.size() > 0 && 
        (static_cast<double>(result[1]) / submission1.size() >= 0.5 && result[2]>100)) {
        result[0] = 1;
    }

    return result; 
}