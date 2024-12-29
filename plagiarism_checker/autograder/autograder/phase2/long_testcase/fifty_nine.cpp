#include <array>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <algorithm>

// Constants for match lengths and thresholds
const int SHORT_MIN_LEN = 10;
const int SHORT_MAX_LEN = 20;
const int LONG_MIN_LEN = 30;
const double LONG_MATCH_THRESHOLD = 0.8;
const int MOD = 1e9 + 7;
const int BASE = 257;

// Helper function to build the KMP table
std::vector<int> buildKMPTable(const std::vector<int>& pattern) {
    int m = pattern.size();
    std::vector<int> kmpTable(m, 0);
    int j = 0;

    // Build the KMP table
    for (int i = 1; i < m; ++i) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = kmpTable[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            j++;
        }
        kmpTable[i] = j;
    }
    return kmpTable;
}

// Helper function to find all matches using the KMP algorithm
std::vector<int> kmpSearch(const std::vector<int>& text, const std::vector<int>& pattern) {
    std::vector<int> matches;
    std::vector<int> kmpTable = buildKMPTable(pattern);
    int n = text.size();
    int m = pattern.size();
    int j = 0;

    // Search for pattern matches in the text
    for (int i = 0; i < n; ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            j = kmpTable[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == m) {
            matches.push_back(i - m + 1); // Store the starting index of the match
            j = kmpTable[j - 1]; // Look for the next match
        }
    }
    return matches;
}

// Helper function to calculate rolling hash of a window
int compute_hash(const std::vector<int>& arr, int start, int len) {
    long long hash_value = 0;
    for (int i = 0; i < len; ++i) {
        hash_value = (hash_value * BASE + arr[start + i]) % MOD;
    }
    return static_cast<int>(hash_value);
}

// Helper function to check for short matches (length 10-20)
int checkShortMatches(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int totalMatchLength = 0;
    std::unordered_set<int> counted_indices1;
    std::unordered_set<int> counted_indices2;
    std::unordered_map<int, std::vector<int>> hash_map;

    // Iterate over possible lengths for short matches
    for (int length = SHORT_MIN_LEN; length <= SHORT_MAX_LEN; ++length) {
        // Compute hashes for all substrings of the current length in submission1
        for (size_t start1 = 0; start1 <= submission1.size() - length; ++start1) {
            int hash_value = compute_hash(submission1, start1, length);
            hash_map[hash_value].push_back(start1);
        }
        // Compute hashes for all substrings of the current length in submission2
        for (size_t start2 = 0; start2 <= submission2.size() - length; ++start2) {
            int hash_value = compute_hash(submission2, start2, length);
            if (hash_map.count(hash_value)) {
                // Check for matches and ensure no overlap
                for (int start1 : hash_map[hash_value]) {
                    bool overlap = false;
                    for (int k = 0; k < length; ++k) {
                        if (counted_indices1.count(start1 + k) || counted_indices2.count(start2 + k)) {
                            overlap = true;
                            break;
                        }
                    }
                    if (!overlap) {
                        std::vector<int> pattern(submission1.begin() + start1, submission1.begin() + start1 + length);
                        auto matches = kmpSearch(submission2, pattern);
                        for (int match_start : matches) {
                            if (match_start == start2) {
                                for (int k = 0; k < length; ++k) {
                                    counted_indices1.insert(start1 + k);
                                    counted_indices2.insert(start2 + k);
                                }
                                totalMatchLength += length;
                                break;
                            }
                        }
                    }
                }
            }
        }
        hash_map.clear();
    }
    return totalMatchLength;
}

// Helper function to check for long matches (length > 30)
std::tuple<int, int, int> checkLongMatches(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int longestMatchLength = 0;
    int longestStart1 = -1;
    int longestStart2 = -1;
    int n = submission1.size();
    int m = submission2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    // Use dynamic programming to find the longest common substring
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (submission1[i - 1] == submission2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] >= LONG_MIN_LEN && dp[i][j] > longestMatchLength) {
                    longestMatchLength = dp[i][j];
                    longestStart1 = i - dp[i][j];
                    longestStart2 = j - dp[i][j];
                }
            }
        }
    }

    return {longestMatchLength, longestStart1, longestStart2};
}

std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // Check for short matches
    result[1] = checkShortMatches(submission1, submission2);

    // Check for long matches
    auto [longestMatchLength, longestStart1, longestStart2] = checkLongMatches(submission1, submission2);
    result[2] = longestMatchLength >= LONG_MIN_LEN ? longestMatchLength : 0;
    result[3] = longestMatchLength >= LONG_MIN_LEN ? longestStart1 : -1;
    result[4] = longestMatchLength >= LONG_MIN_LEN ? longestStart2 : -1;

    // Flag as plagiarized if there are significant short or long matches
    int shortMatchThreshold = 50;  // Threshold for flagging based on short matches
    int longMatchThreshold = 60;   // Threshold for flagging based on long match length

    if (result[1] > shortMatchThreshold || result[2] > longMatchThreshold) {
        result[0] = 1;  // Set the plagiarism flag
    }

    return result;
}