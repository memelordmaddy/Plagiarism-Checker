#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>
// function for matching patternd of length 10-20 tokens
int find_exact_matches(const std::vector<int>& s1, const std::vector<int>& s2, int min_len, int max_len, std::set<std::pair<int, int>>& matches) {
    int total_exact_length = 0;

    for (int len = max_len; len >= min_len; --len) {
        for (int i = 0; i + len <= s1.size(); ++i) {
            for (int j = 0; j + len <= s2.size(); ++j) {
                if (std::equal(s1.begin() + i, s1.begin() + i + len, s2.begin() + j)) {
                    bool valid = true;
                    for (int k = 0; k < len; ++k) {
                        std::pair<int, int> target_1 = {i + k, 1};
                        std::pair<int, int> target_2 = {j + k, 2};
                        if (matches.find(target_1) != matches.end() || matches.find(target_2) != matches.end()) {
                            valid = false;
                            break;
                        }
                    }
                    if (valid) {
                        total_exact_length += len;
                        for (int k = 0; k < len; ++k) {
                            matches.insert({i + k, 1});
                            matches.insert({j + k, 2});
                        }
                    }
                }
            }
        }
    }
    return total_exact_length;
}


// function to match 30+ tokens 
std::tuple<int, int, int> find_approximate_matches(const std::vector<int>& s1, const std::vector<int>& s2, int min_len) {
    int longest_match = 0;
    int start_index_s1 = -1;
    int start_index_s2 = -1;

    for (int i = 0; i + min_len <= s1.size(); ++i) {
        for (int j = 0; j + min_len <= s2.size(); ++j) {
            int match_count = 0;
            int current_len = 0;

            // Find longest matching sequence from (i, j) positions in s1 and s2
            while (i + current_len < s1.size() && j + current_len < s2.size() &&
                   (s1[i + current_len] == s2[j + current_len] || 
                    static_cast<double>(match_count) / (current_len + 1) >= 0.8)) {
                if (s1[i + current_len] == s2[j + current_len]) {
                    ++match_count;
                }
                ++current_len;
            }

            // Check if this match meets the 80% threshold and is the longest found
            if (match_count >= 0.8 * current_len && current_len > longest_match) {
                longest_match = current_len;
                start_index_s1 = i;
                start_index_s2 = j;
            }
        }
    }

    return {longest_match, start_index_s1, start_index_s2};
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // total 10-20 matches
    std::set<std::pair<int,int>> matches;
    int exact_match_length = find_exact_matches(submission1, submission2, 10, 20, matches);
    result[1] = exact_match_length;

    // longest approximate match
    auto [longest_match, start_s1, start_s2] = find_approximate_matches(submission1, submission2, 30);
    result[2] = longest_match;
    result[3] = start_s1;
    result[4] = start_s2;

    // result 0
    if (exact_match_length > 100 || longest_match > 0) {
        result[0] = 1;
    }

    return result;
}



