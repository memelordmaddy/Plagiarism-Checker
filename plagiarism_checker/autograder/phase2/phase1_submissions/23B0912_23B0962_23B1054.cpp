#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
            
            std::array<int, 5> result = {0, 0, 0, 0, 0};  // Initialize return array

    // Thresholds for pattern lengths
    const int shortPatternLength = 10;
    const int minLongPatternLength = 30;
    const double approxMatchThreshold = 0.8;

    // Variables to track pattern matches
    int totalShortPatternMatchLength = 0;
    int maxApproximateMatchLength = 0;
    int maxApproximateMatchStartIdx1 = -1;
    int maxApproximateMatchStartIdx2 = -1;
    std::vector<bool> matchedPositions1(submission1.size(), false);
    std::vector<bool> matchedPositions2(submission2.size(), false);

    // Function to find exact matches for shorter patterns
    auto findExactMatches = [&](int start1, int start2, int len) {
        for (int i = 0; i < len; ++i) {
            if (submission1[start1 + i] != submission2[start2 + i]) {
                return false;
            }
        }
        return true;
    };

    // Function to find approximate matches for longer patterns
    auto findApproximateMatches = [&](int start1, int start2, int len) {
        int matchCount = 0;
        for (int i = 0; i < len; ++i) {
            if (submission1[start1 + i] == submission2[start2 + i]) {
                matchCount++;
            }
        }
        return (double(matchCount) / len) >= approxMatchThreshold;
    };

    // Search for exact matches (short patterns)
    for (size_t i = 0; i <= submission1.size() - shortPatternLength; ++i) {
        for (size_t j = 0; j <= submission2.size() - shortPatternLength; ++j) {
            if (findExactMatches(i, j, shortPatternLength)) {
                bool nonOverlapping = true;
                for (int k = 0; k < shortPatternLength; ++k) {
                    if (matchedPositions1[i + k] || matchedPositions2[j + k]) {
                        nonOverlapping = false;
                        break;
                    }
                }
                if (nonOverlapping) {
                    totalShortPatternMatchLength += shortPatternLength;
                    for (int k = 0; k < shortPatternLength; ++k) {
                        matchedPositions1[i + k] = true;
                        matchedPositions2[j + k] = true;
                    }
                }
            }
        }
    }

    // Search for longest approximate match (increasing length gradually)
    for (size_t i = 0; i < submission1.size(); ++i) {
        for (size_t j = 0; j < submission2.size(); ++j) {
            int matchLength = minLongPatternLength;
            while ((i + matchLength <= submission1.size()) &&
                   (j + matchLength <= submission2.size()) &&
                   findApproximateMatches(i, j, matchLength)) {
                if (matchLength > maxApproximateMatchLength) {
                    maxApproximateMatchLength = matchLength;
                    maxApproximateMatchStartIdx1 = i;
                    maxApproximateMatchStartIdx2 = j;
                }
                matchLength++;
            }
        }
    }

    // Set results
    result[0] = (totalShortPatternMatchLength > 0 || maxApproximateMatchLength > 0) ? 1 : 0;  // Flag
    result[1] = totalShortPatternMatchLength;
    result[2] = maxApproximateMatchLength;
    result[3] = maxApproximateMatchStartIdx1;
    result[4] = maxApproximateMatchStartIdx2;

    return result;
}