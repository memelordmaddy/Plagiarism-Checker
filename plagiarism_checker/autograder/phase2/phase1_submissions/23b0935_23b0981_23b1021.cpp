#include <array>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <string>
#include <tuple>
#include <algorithm>
#include <utility>

// Finds matches for short patterns (10-20 tokens)
int find_short_pattern_matches(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int total_length = 0;
    std::unordered_map<int, std::vector<int>> positions;
    std::vector<bool> counted1(submission1.size(), false); // Tracks used tokens in submission1
    std::vector<bool> counted2(submission2.size(), false); // Tracks used tokens in submission2

    // Store positions of each token in submission2
    for (size_t i = 0; i < submission2.size(); ++i) {
        positions[submission2[i]].push_back(i);
    }

    // Look for matches in submission1
    for (size_t i = 0; i < submission1.size(); ++i) {
        if (!counted1[i] && positions.count(submission1[i])) {
            for (int pos : positions[submission1[i]]) {
                if (counted2[pos]) continue; // Skip already counted positions in submission2

                int length = 0;
                while (i + length < submission1.size() && pos + length < submission2.size() &&
                       submission1[i + length] == submission2[pos + length] &&
                       !counted1[i + length] && !counted2[pos + length]) {
                    length++;
                    if (length > 20) break;
                }
                if (length >= 10) {
                    // Mark matched tokens as counted
                    for (int k = 0; k < length; ++k) {
                        counted1[i + k] = true;
                        counted2[pos + k] = true;
                    }
                    total_length += length;
                    i += length - 1; // Skip over the matched sequence
                    break;
                }
            }
        }
    }
    return total_length;
}




// Finds the longest sequence of tokens in the same order (not necessarily contiguous) between two submissions
std::tuple<int, int, int> find_longest_approx_match(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int n1 = submission1.size(), n2 = submission2.size();
    int longest_length = 0, start1 = -1, start2 = -1;

    // DP table to store the length of longest sequence ending at each (i, j) position
    std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1, 0));
    std::vector<std::pair<int, int>> matches;

    // Iterate through both submissions to fill the DP table
    for (int i = 1; i <= n1; ++i) {
        for (int j = 1; j <= n2; ++j) {
            if (submission1[i - 1] == submission2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                matches.emplace_back(i-1, j-1);
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    for(int i = 0; i < matches.size(); i++){
           int len = 0;
        int i1 = matches[i].first;
        int i2 = matches[i].second;
        int last_index = matches.size() - 1;
        int end1 = matches[last_index].first;
        int end2 = matches[last_index].second;
        int length=dp[end1][end2]-dp[i1][i2];
        int l=std::max(n1-i1,n2-i2);
        if(length >= l*0.8 && length > longest_length){
            longest_length=length;
            start1=i1;
            start2=i2;
        }
        }
        
    

    return {longest_length, start1, start2};
}


// Main function that combines short and long pattern matching
std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, -1, -1};

    // Detect short pattern matches (10-20 tokens)
    int short_pattern_total_length = find_short_pattern_matches(submission1, submission2);

    // Detect longest approximate pattern match (30 or more tokens)
    auto [longest_approx_match_length, longest_approx_match_start1, longest_approx_match_start2] = find_longest_approx_match(submission1, submission2);

    int n1,n2,n3;
    n1 = submission1.size();
    n2 = submission2.size();
    if(n1>n2) n3 = n2;
    if(n1<=n2) n3 = n1;
    // Determine if the submissions are flagged as plagiarized
    if (short_pattern_total_length > 0.6 * n3 || longest_approx_match_length > 0.3 * n3) {
        result[0] = 1;
    }

    result[1] = short_pattern_total_length;
    result[2] = longest_approx_match_length;
    result[3] = longest_approx_match_start1;
    result[4] = longest_approx_match_start2;

    return result;
}