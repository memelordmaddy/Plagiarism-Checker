#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// Function to find matching exact substrings of length minLength or more
int SumOfExactMatches(const std::vector<int>& vec1, const std::vector<int>& vec2, const int& minLength) {
    int len1 = vec1.size();
    int len2 = vec2.size();
    int sum_exact_matches = 0;

    // iterate over all possible starting positions in vec1 and vec2
    for(int pos1=0; pos1<len1; pos1++){
        int match_pos1 = 0;
        for(int pos2=0; pos2<len2; pos2++){
            int curr_match = 0;
            for(; pos1+curr_match<len1 && pos2+curr_match<len2; curr_match++){
                if(vec1[pos1+curr_match]!=vec2[pos2+curr_match]) break;
            }
            if(curr_match>match_pos1){
                match_pos1 = curr_match;
            }
        }

        // if a match of length minLength or more is found, add it to the sum and skip to the end of the match
        if(match_pos1>=minLength) {
            sum_exact_matches += match_pos1;
            pos1+=match_pos1-1;
        }
    }
    return sum_exact_matches;
}

int approxMatchFromStart(const std::vector<int>& doc1, const std::vector<int>& doc2, const int& start1, const int& start2, const double& mismatch_threshold) {
    int m = doc1.size() - start1;
    int n = doc2.size() - start2;

    // dp[i][j] = length of edit distance from doc1[start1..start1+i-1] to doc2[start2..start2+j-1]
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, -1));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int i = 0; i <= n; ++i) dp[0][i] = i;

    int longest_len = 0;

    for (int t = 0; t < m*n; ++t) {
        int i = t/n + 1, j = t%n + 1;

        // considers 3 operations: insert, delete, replace
        if (doc1[start1 + i - 1] == doc2[start2 + j - 1]) {
            dp[i][j] = dp[i - 1][j - 1];
        } else {
            dp[i][j] = 1 + std::min(dp[i - 1][j - 1], std::min(dp[i][j - 1], dp[i - 1][j]));
        }

        if (dp[i][j] <= mismatch_threshold * std::min(i,j) ) {
            longest_len = std::max(longest_len, std::max(i,j));
        }
    }
    return longest_len;
}

std::tuple<int, int, int> longestApproximateMatch(const std::vector<int>& doc1, const std::vector<int>& doc2, double mismatch_threshold) {
    int len1 = doc1.size();
    int len2 = doc2.size();

    int longest_len = 0;
    int best_pos1 = -1, best_pos2 = -1;

    int add_index = std::min(20, std::max(10, std::min(len1, len2)/80));

    for (int i = 0; i < len1; i+=add_index) {
        for (int j = 0; j < len2; j+=add_index) {
            if (std::max(len1-i,len2-j) < longest_len + add_index) continue; // early stopping

            int match_len = approxMatchFromStart(doc1, doc2, i, j, mismatch_threshold);
            if (match_len > longest_len) {
                longest_len = match_len;
                best_pos1 = i;
                best_pos2 = j;
            }
        }
    }

    if (longest_len < 30) return {0, -1, -1};
    return {longest_len, best_pos1, best_pos2};
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    int len1 = submission1.size(), len2 = submission2.size();

    std::array<int, 5> result = {0, 0, 0, 0, 0};

    result[1] = SumOfExactMatches(submission1, submission2, 10);

    std::tuple<int, int, int> fuzzyMatch = longestApproximateMatch(submission1, submission2, 0.20); // 20% mismatch threshold
    result[2] = std::get<0>(fuzzyMatch);
    result[3] = std::get<1>(fuzzyMatch);
    result[4] = std::get<2>(fuzzyMatch);

    if (result[1] > 0.5*std::min(len1,len2) && (result[2] > 0.3*std::min(len1,len2) || result[2] > 150 )) result[0]=1;
    // plagged if 30% or lenth 150 tokens or more of continuous code approx matches, and almost 50% of code is exact copied

    return result;
}