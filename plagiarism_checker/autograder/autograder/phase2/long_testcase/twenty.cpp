#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <ostream>
#include <tuple>
#include <algorithm>
#include <cstring>


// Levenshtein distance with dp approach (Minimum edit distance)
int levenshtein_distance(std::vector<int> &submission1, std::vector<int> &submission2) {
    int n = submission1.size();
    int m = submission2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 0; i <= n; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= m; j++) {
        dp[0][j] = j;
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int cost = submission1[i - 1] == submission2[j - 1] ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
        }
    }
    return dp[n][m];
}

// Longest Common Subsequence with dp approach returns tuple of (length of lcs, 
// starting index of lcs in submission1, starting index of lcs in submission2)
std::tuple<int, int, int> longest_common_subsequence(std::vector<int> &submission1,
                std::vector<int> &submission2) {

    int n = submission1.size();
    int m = submission2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (submission1[i - 1] == submission2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } 
            else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    int len = dp[n][m];
    std::vector<int> lcs(len);
    int index = len - 1;
    int i = n;
    int j = m;
    
    // while (i > 0 && j > 0) {
    //     if (submission1[i - 1] == submission2[j - 1]) {
    //         lcs[index] = submission1[i - 1];
    //         index--;
    //         i--;
    //         j--;
    //     } 
    //     else {
    //         if (dp[i - 1][j] > dp[i][j - 1]) {
    //             i--;
    //         } 
    //         else {
    //             j--;
    //         }
    //     }
    // }
    // std::cout << "Longest Common Subsequence : " << lcs << std::endl;

    int startSub1 = 0;
    i = n;
    j = m;
    while (i > 0 && j > 0) {
        if (submission1[i - 1] == submission2[j - 1]) {
            startSub1 = i - 1;
            i--;
            j--;
        } 
        else {
            if (dp[i - 1][j] > dp[i][j - 1]) {
                i--;
            } 
            else {
                j--;
            }
        }
    }
    int startSub2 = 0;
    i = n;
    j = m;
    while (i > 0 && j > 0) {
        if (submission1[i - 1] == submission2[j - 1]) {
            startSub2 = j - 1;
            i--;
            j--;
        } 
        else {
            if (dp[i - 1][j] > dp[i][j - 1]) {
                i--;
            } 
            else {
                j--;
            }
        }
    }
    return {len, startSub1, startSub2};
}

// sum of total length of common subsequences in submission1 and submission2
// using dp
int cnt = 0;
int sum = 0;
int lcs_sum(const std::vector<int> &s1, const std::vector<int> &s2, int i, int j, std::vector<std::vector<int>> &memo) {
    if (i == s1.size() || j == s2.size()) {
        sum += cnt;
        return 0;
    }
    if (memo[i][j] != -1) {
        return memo[i][j];
    }
    if (s1[i] == s2[j]) {
        cnt++;
        memo[i][j] = 1 + lcs_sum(s1, s2, i + 1, j + 1, memo);
        cnt--;
    } else {
        memo[i][j] = std::max(lcs_sum(s1, s2, i + 1, j, memo), lcs_sum(s1, s2, i, j + 1, memo));
    }
    return memo[i][j];
}

int lcs_recursive_sum(const std::vector<int> &s1, const std::vector<int> &s2) {
    std::vector<std::vector<int>> memo(s1.size(), std::vector<int>(s2.size(), -1));
    return lcs_sum(s1, s2, 0, 0, memo);
}
std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // case 0 : when both submissions are empty
    if (submission1.empty() && submission2.empty()) {
        return result;
    }

    // case 1 : when one of the submissions is empty
    if (submission1.empty() || submission2.empty()) {
        return result;
    }
    
    // case 2 : when both submissions.size() <= 20
    if (submission1.size() <= 25 and submission2.size() <= 25) {
        
        auto [max, startSub1, startSub2] = (longest_common_subsequence(submission1, submission2));
        // if lcs of submission1 and submission2 is exactly equal to min(submission1.size(), submission2.size()) result[0] = 1
        if (max == std::min(submission1.size(), submission2.size())) {
            result[0] = 1;
        }
        // let `sum` be sum of total length of common substrings in submission1 and submission2 result[1] = sum
        result[1] = 0;

        // let max be the length of the longest common subsequences in submission1 and submission2 result[2] = max
        result[2] = max;

        // starting indx of the longest common subsequences (submission1 and submission2) in submission1 result[3] = startSub1
        result[3] = startSub1;

        // starting indx of the longest common subsequences (submission1 and submission2) in submission2 result[4] = startSub2
        result[4] = startSub2;
    }

    // case 3 : when both submissions.size() > 20
    else if (submission1.size() > 25 and submission2.size() > 25) {
        std::vector<int> sub1 = submission1;
        std::vector<int> sub2 = submission2;

        // levenshtein distance between sub1 and sub2
        int leven = 0;                   
        leven = levenshtein_distance(sub1, sub2);

        std::cerr << "leven = " << leven << std::endl;
        auto [max, startSub1, startSub2] = (longest_common_subsequence(submission1, submission2));
        // leven < 20% of the length of the larger submission fill result[0] with 1
        if (leven <= 0.3 * std::max(sub1.size(), sub2.size()) || max >= 0.6 * std::max(sub1.size(), sub2.size())) {
            result[0] = 1;
        }

        // let `sum` be sum of total length of common substrings in sub1 and sub2 result[1] = sum 
        result[1] = max;

        // let max be the length of the longest common subsequences in sub1 and sub2 result[2] = max
        result[2] = max;

        // starting indx of the longest common subsequences (sub1 and sub2) in sub1 result[3] = startSub1
        result[3] = startSub1;

        // starting indx of the longest common subsequences (sub1 and sub2) in sub2 result[4] = startSub2
        result[4] = startSub2;
    }

    // case 4 : rest all cases
    else {
        auto [max, startSub1, startSub2] = (longest_common_subsequence(submission1, submission2));
        // if lcs of submission1 and submission2 is exactly equal to min(submission1.size(), submission2.size()) result[0] = 1
        if (max == std::min(submission1.size(), submission2.size())) {
            result[0] = 1;
        }
        // let `sum` be sum of total length of common substrings in submission1 and submission2 result[1] = sum
        result[1] = 0;

        // let max be the length of the longest common subsequences in submission1 and submission2 result[2] = max
        result[2] = max;

        // starting indx of the longest common subsequences (submission1 and submission2) in submission1 result[3] = startSub1
        result[3] = startSub1;

        // starting indx of the longest common subsequences (submission1 and submission2) in submission2 result[4] = startSub2
        result[4] = startSub2;
    }

    // printing result

    sum = 0;
    cnt = 0;
    return result;
}

