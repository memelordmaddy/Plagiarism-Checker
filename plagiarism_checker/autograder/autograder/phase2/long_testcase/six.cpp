#include <array>
#include <vector>
#include <queue>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <unordered_set>
using namespace std;

struct ComparePairs {
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) {
        if (a.second != b.second) return a.second < b.second;
        else return a.first > b.first;
    }
};

class Matcher {
private:
    vector<int> submission1;
    vector<int> submission2;
    map<int, pair<int, int>> matchedSubstrings;
    unordered_set<int> usedIndices;
    std::vector<bool> matched_submission1;
    std::vector<bool> matched_submission2;
    std::map<int, std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, ComparePairs>> match_queues;


    std::pair<std::vector<int>, std::vector<int>> compute_lcs_indices() {
        int n = submission1.size();
        int m = submission2.size();
        
        // DP table to store LCS lengths
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

        // Fill DP table
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (submission1[i - 1] == submission2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        // Backtrack to find the indices of LCS in both submission1 and submission2
        std::vector<int> lcs_indices_submission1;
        std::vector<int> lcs_indices_submission2;
        
        int i = n, j = m;
        while (i > 0 && j > 0) {
            if (submission1[i - 1] == submission2[j - 1]) {
                // Store index of submission1 and submission 2 if they are equal
                lcs_indices_submission1.push_back(i - 1);  
                lcs_indices_submission2.push_back(j - 1);  
                i--;
                j--;
            } else if (dp[i - 1][j] > dp[i][j - 1]) {
                i--;
            } else {
                j--;
            }
        }
        
        // Reverse the collected indices to maintain order
        std::reverse(lcs_indices_submission1.begin(), lcs_indices_submission1.end());
        std::reverse(lcs_indices_submission2.begin(), lcs_indices_submission2.end());

        return {lcs_indices_submission1, lcs_indices_submission2};
    }

    int longest_common_subsequence() {
        
        int n = submission1.size();
        int m = submission2.size();
        
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (submission1[i - 1] == submission2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        return dp[submission1.size()][submission2.size()];
    }

    void build_queue() {
        
        // Build match priority queue by finding matches of length >= 10
        
        for (int i = 0; i < submission1.size(); i++) {
            for (int j = 0; j < submission2.size(); j++) {
                int index1 = i;
                int index2 = j;
                int match_count = 0;
                while (index1 < submission1.size() && index2 < submission2.size() && submission1[index1] == submission2[index2]) {
                    index1++;
                    index2++;
                    match_count++;
                }
                if (match_count >= 10) {
                    matched_submission1[i] = true;
                    match_queues[i].push(std::make_pair(j, match_count));
                    j += match_count - 1;
                }
            }
        }
    }

    pair<bool, int> check_matched(std::pair<int, int> match_pair) { 
        int partial_match_length;
        for (int j = match_pair.first; j < match_pair.first + match_pair.second; j++) {
            // Check if the matched region in submission2 is already matched, if yes calculate partial_match_length
            if (matched_submission2[j]) { 
                partial_match_length = j - match_pair.first;
                return {true, partial_match_length};
            }
        }
        return {false, 0};
    }

    void partial_update(std::pair<int, int> &match_pair, vector<int> &match_lengths, int partial_match_length) { 
        for (int j = match_pair.first; j < match_pair.first + partial_match_length; j++) {
            matched_submission2[j] = true;
            match_lengths[j] = partial_match_length;
        }
    }

    void full_update(std::pair<int, int> &match_pair, vector<int> &match_lengths) { 
        for (int j = match_pair.first; j < match_pair.first + match_pair.second; j++) {
            matched_submission2[j] = true;
            match_lengths[j] = match_pair.second;
        }
    }

    int calculate_total_length() {
        
        // Calculate the total length of matched subsequences between two submissions.
        // Marks matched regions in `matched_submission1` and `matched_submission2` to avoid double-counting.

        int total_matched_length = 0;
        std::vector<int> match_lengths(submission2.size(), 0);
        
        // Builds priority queues for all matches of length >= 19
        build_queue();

        for (int i = 0; i < submission1.size(); i++) {
            if (!matched_submission1[i]) continue;

            while (!match_queues[i].empty()) {
                std::pair<int, int> match_pair = match_queues[i].top();
                match_queues[i].pop();

                auto [matched, partial_match_length] = check_matched(match_pair);

                if (matched && partial_match_length >= 10) {
                    // If partial match length >= 10, mark the matched region in submission2 and add to match length
                    partial_update(match_pair, match_lengths, partial_match_length);
                    total_matched_length += partial_match_length;
                    i += partial_match_length - 1;
                    break;

                } 
                
                else if(!matched) {
                    total_matched_length +=  match_pair.second;
                    full_update(match_pair, match_lengths);
                    i += match_pair.second - 1;
                    break;
                }
            }
        }
        return total_matched_length;
    }

    void calculate_approximate_length(int &max_subsequence_length, int &start_index_submission1, int &start_index_submission2) { 
        
        auto [lcs_indices_submission1, lcs_indices_submission2] = compute_lcs_indices();
        int lcs_length = longest_common_subsequence();


        for (int i = 0; i < lcs_length; i++) {
            for (int j = i + 1; j < lcs_length; j++) {
                int current_length = std::max(lcs_indices_submission1[j] - lcs_indices_submission1[i], lcs_indices_submission2[j] - lcs_indices_submission2[i]) + 1;
                
                // Check if 80% criteria is satisfied and the current length is greater than the max subsequence length
                if (j - i + 1 >= 0.8 * current_length) {
                    if (current_length > max_subsequence_length) {
                        max_subsequence_length = current_length;
                        start_index_submission1 = lcs_indices_submission1[i];
                        start_index_submission2 = lcs_indices_submission2[i];
                    }
                }
            }
        }  
    }
    
    bool plag_check(int totalShortLength, int maxLongLength){
        int minlen = submission1.size() > submission2.size() ? submission2.size() : submission1.size();
        float shortpercent = (totalShortLength*1.0)/minlen;
        float longpercent = (maxLongLength*1.0)/minlen;
        
        if(shortpercent > 0.4){
            return 1;
        }
        else if(longpercent > 0.3) { 
            return 1;
        }
        else if(shortpercent+longpercent > 0.6) {
            return 1;
        }
        return 0;
    }

public:
    Matcher(const vector<int>& sub1, const vector<int>& sub2)
        : submission1(sub1), submission2(sub2) {
            matched_submission1 = vector<bool>(submission1.size(), false);
            matched_submission2 = vector<bool>(submission2.size(), false);
        }

    std::array<int, 5> matchSubmissions() {
        std::array<int, 5> results = {0, 0, 0, 0, 0};        
        
        results[1] = calculate_total_length();
        calculate_approximate_length(results[2], results[3], results[4]); 
        results[0] = plag_check(results[1], results[2]);

        return results;
    }

};

class Matcher2 {
private:
    vector<int> submission1;
    vector<int> submission2;
    map<int, pair<int, int>> matchedSubstrings;
    unordered_set<int> usedIndices;
    int totalMatchLength = 0;

    int match_token(int start_1, int start_2) { 
        int length = 0;
        // match three charcters at a time
        while(start_1 < submission1.size() && start_2 < submission2.size()) {
            if(usedIndices.count(start_2)) { 
                if(length > 10) { 
                    return length;
                }
                return 0;
            }
            if(submission1[start_1] == submission2[start_2]) {
                start_1++;
                start_2++;
                length++;
            } else {
                break;
            }
        }
        if(length > 10) {
            return length;
        }

        return 0;

    }

    void update_match(int start_1, int start_2, int length) {
        
        bool overlap = false;
        for (int k = start_2; k < start_2 + length; k++) {
            if (usedIndices.count(k)) {
                overlap = true;
                break;
            }
        }

        // If no overlap, proceed to update totalMatchLength and mark indices as used
        if (!overlap) {
            // Update totalMatchLength by adding the new match length
            totalMatchLength += length;

            // Mark new indices as used for the current match
            for (int k = start_2; k < start_2 + length; k++) {
                usedIndices.insert(k);
                matchedSubstrings[k] = {length, start_2};
            }
        }
    }

    // Check for matching substrings and handle overlaps
    void checkMatching() {
        
        int len1 = submission1.size();
        int len2 = submission2.size();
        
        for(int start=0; start<len1; ++start) {
            for(int start_2=0; start_2<len2; ++start_2) { 
                int length = match_token(start, start_2);
                if(length > 10) {
                    update_match(start, start_2, length);
                }
            }
        }
    }


public:
    Matcher2(const vector<int>& sub1, const vector<int>& sub2)
        : submission1(sub1), submission2(sub2) {}

    int matchSubmissions() {

        checkMatching();

        return totalMatchLength; // Return a placeholder result array
    }
};

// Wrapper function
std::array<int, 5> match_submissions(vector<int> &submission1, vector<int> &submission2) {
    Matcher matcher(submission1, submission2);

    array<int, 5> result = matcher.matchSubmissions();

    if(result[0] == 0) { 
        // If method 1 does not give plagiarism, check with method 2
        Matcher2 matcher2(submission2, submission1);
        int total_length =  matcher2.matchSubmissions();

        if(total_length > result[1]) { 
            result[1] = total_length;
            int min_len = submission1.size() > submission2.size() ? submission2.size() : submission1.size();
            result[0] = total_length > 0.4 * min_len ? 1 : 0;
            if(result[0] == 0) { 
                result[0] = (result[1]+result[2]) > 0.6 * min_len ? 1 : 0;
            }
        }
        
    }
    return result;
}   