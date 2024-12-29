#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include<unordered_map>
#include <set>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
// Class1 for value1
class TotalLengthMatches {
private:
    void update_any_dangling(int &dangling_left, int &dangling_right, int& left_problem, int& right_problem,
    int &profit, std::vector<std::vector<int>>& match, int &j, int &count, int &n2) {
        int curr_index = j-1;
        // left going
        while(curr_index >= 0 && match[curr_index][0] == match[j][0] && match[curr_index][1] == match[j][1]) 
        {
            left_problem++;
            if(left_problem >= 10) break;
            curr_index--;
        }
        // right going
        curr_index = j+count;
        while(curr_index <= n2-1 && match[curr_index][0] == match[j+count-1][0] && match[curr_index][1] == match[j+count-1][1]) 
        {
            right_problem++;
            if(right_problem >= 10) break;
            curr_index++;
        }
        left_problem = dangling_left ? left_problem : 0;
        right_problem = dangling_right ? right_problem : 0;
        if (left_problem < 10) profit -= left_problem;
        if (right_problem < 10) profit -= right_problem;
    }

    void selection_loop(int &i, int &j, int &count, std::vector<int>& a, std::vector<int>& b, 
    std::vector<std::vector<int>>& match, int n1, int n2, std::vector<int> &ans) {
        while(j <= n2 - 1) {
            if (i < n1 && j < n2 && a[i] == b[j]) {
                while(i+count < n1 && j + count < n2 && a[i+count] == b[j+count]) count++;
                if(count < 10) {
                    j++; if(j > n2-1) break;
                    count = 0;continue;
                }

                bool rkhna_hai = false; // to decide whether we want to keep this match
                int profit = 0;
                int left_problem = 0, right_problem = 0;
                int dangling_left = match[j][0] != -1;
                int dangling_right = match[j+count-1][0] != -1;

                for(int t = j; t < j+count; t++) if(match[t][0] == -1) profit++;
                if (dangling_right || dangling_left) 
                update_any_dangling(dangling_left, dangling_right, left_problem, right_problem, profit, match, j, count, n2);

                if (profit > ans[0]) {
                    std::vector<int> temp {profit, j, count, dangling_left, dangling_right, left_problem, right_problem};
                    ans = temp;
                }
                j = j + count;
                count = 0;
            } 
            else {
                j++;
                if(j > n2-1) break;
            }
        }
    }
    std::vector<int> selection(int i, std::vector<std::vector<int>>& match, std::vector<int>& a, std::vector<int>& b) {
        int count = 0;
        int n1 = a.size();
        int n2 = b.size();
        int j = 0;
        std::vector<int> ans (7, 0);
        selection_loop(i,j,count,a,b,match,n1,n2,ans);
        return ans;
    }
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> calculate_final
    (std::vector<std::vector<int>> &match, int &value1, long long int &operation) {
        std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> result;
        for (int t = 0; t < match.size() - 1; t++) {
            operation++;
            if (match[t][0] != -1 && match[t][0] != match[t + 1][0]) {
                std::pair<std::pair<int, int>, std::pair<int, int>> 
                intermediate2(std::make_pair(t - match[t][1] + match[t][0], t), std::make_pair(match[t][0], match[t][1]));
                result.insert(intermediate2);
            }
            if (match[t][0] != -1) value1++;
        }
        if (match[match.size() - 1][0] != -1) value1++;
        return result;
    }

    void updateOverlap(int &overlap_left, int &overlap_right, int j, int count, 
    int n2, const std::vector<std::vector<int>> &match) {
        int curr_index = j;
        overlap_left = 0;
        overlap_right = 0;

        // Calculate left overlap
        while (curr_index <= j + count - 1 && match[curr_index][0] == match[j][0] && match[curr_index][1] == match[j][1]) {
            curr_index++;
            overlap_left++;
        }

        // Calculate right overlap
        curr_index = j + count - 1;
        while (curr_index >= j && match[curr_index][0] == match[j + count - 1][0] && match[curr_index][1] == match[j + count - 1][1]) {
            curr_index--;
            overlap_right++;
        }
    }

    void updateDanglingLeft(int j, int left_problem, int overlap_left, long long int &operation, 
    std::vector<std::vector<int>> &match) {
        int curr_index = j - 1;

        // Process dangling left segment
        while (curr_index >= 0 && match[curr_index][0] == match[j][0] && match[curr_index][1] == match[j][1]) {
            operation += 3;
            if (left_problem < 10) {
                match[curr_index][0] = -1;
                match[curr_index][1] = -1;
            } else {
                match[curr_index][1] -= overlap_left;
            }
            curr_index--;
        }
    }

    void updateDanglingRight(int j, int count, int right_problem, int overlap_right, 
    long long int &operation, std::vector<std::vector<int>> &match, int n2) {
        int curr_index = j + count;

        // Process dangling right segment
        while (curr_index <= n2 - 1 && match[curr_index][0] == match[j + count - 1][0] && match[curr_index][1] == match[j + count - 1][1]) {
            operation += 3;
            if (right_problem < 10) {
                match[curr_index][0] = -1;
                match[curr_index][1] = -1;
            } else {
                match[curr_index][0] += overlap_right;
            }
            curr_index++;
        }
    }

    void applyMatchUpdates(int i, int j, int count, long long int &operation, std::vector<std::vector<int>> &match) {
        // Apply updates to match vector
        for (int t = j; t < j + count; t++) {
            match[t][0] = i;
            match[t][1] = i + count - 1;
            operation += 3;
        }
    }

    void loop1(std::vector<std::vector<int>> &match, long long int &operation, int &n1, int &n2, 
    std::vector<int> &a, std::vector<int> &b, std::vector<std::vector<int>> &check, int &value1) {
        int j = 0;
        operation += 3;

        for (int i = 0; i < n1; i++) {
            std::vector<int> optimum_match = selection(i, match, a, b);
            if (optimum_match[0] <= 0) continue;

            j = optimum_match[1];
            int count = optimum_match[2];
            bool dangling_left = optimum_match[3];
            bool dangling_right = optimum_match[4];
            int left_problem = optimum_match[5];
            int right_problem = optimum_match[6];
            operation += 5;

            check.push_back({i, j});

            if (dangling_right || dangling_left) {
                int overlap_left = 0, overlap_right = 0;
                updateOverlap(overlap_left, overlap_right, j, count, n2, match);

                if (dangling_left) {
                    updateDanglingLeft(j, left_problem, overlap_left, operation, match);
                }

                if (dangling_right) {
                    updateDanglingRight(j, count, right_problem, overlap_right, operation, match, n2);
                }

                applyMatchUpdates(i, j, count, operation, match);

                i += count;
                j = 0;
                operation += 3;
                if (i > n1 - 1) {
                    operation += 1;
                    break;
                }
            } else if (!dangling_right && !dangling_left) {
                if (update_not_not1(match, operation, count, i, j, n1, n2)) break;
            }

            operation++;
        }
    }
    bool update_not_not1(std::vector<std::vector<int>> &match, long long int &operation, 
    int &count, int &i, int &j, int n1, int n2) {
        bool br = false;
        for(int t = j; t < j+count; t++) 
        {
            match[t][0] = i;
            operation+=2;
            match[t][1] = i+count-1;
        }
        i = i+count; j = 0;
        operation+=3;
        if(i > n1-1){operation++; br = true;}
        return br;
    }
public:
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> exact_matching_length1(std::vector<int> &submission1, 
    std::vector<int> &submission2, int &value1, std::vector<std::vector<int>> &check)
    {
        std::vector<int> a = submission1;
        std::vector<int> b = submission2;
        long long int operation = 0;
        int n1 = submission1.size();
        int n2 = submission2.size();
        operation+=n1;
        operation+=n2;

        std::vector<std::vector<int>> match(n2, std::vector<int>(2, -1));
        operation+=n2*2;
        loop1(match,operation,n1,n2,a,b,check,value1);

        std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> ans = calculate_final(match, value1, operation);
        return ans;
    }
};

// -----------------------------------------------------------------------------

// class for value1 #2
class TotalLengthMatches2 {
private:
    int is_matching_sequence(const std::vector<int>& a, const std::vector<int>& b, int start_a, int start_b) {
        int count = 0;
        while (start_a + count < a.size() && start_b + count < b.size() && a[start_a + count] == b[start_b + count]) {
            count++;
        }
        return count;
    }

    // Helper function to calculate profit of inserting a match
    int calculate_profit(const std::vector<std::vector<int>>& match, int j, 
    int count, bool& dangling_left, bool& dangling_right) {
        
        int profit = 0, left_problem = 0, right_problem = 0;

        // Count unmatched positions
        for (int t = j; t < j + count; t++) {
            if (match[t][0] == -1) profit++;
        }

        if (dangling_right || dangling_left) {
            int curr_index = j - 1;
            
            // Check overlap on the left
            while (curr_index >= 0 && match[curr_index][0] == match[j][0] && match[curr_index][1] == match[j][1]) {
                left_problem++;
                if (left_problem >= 10) break;
                curr_index--;
            }
            
            // Check overlap on the right
            curr_index = j + count;
            while (curr_index < match.size() && match[curr_index][0] == match[j + count - 1][0] && match[curr_index][1] == match[j + count - 1][1]) {
                right_problem++;
                if (right_problem >= 10) break;
                curr_index++;
            }
            
            profit -= (left_problem < 10 ? left_problem : 0) + (right_problem < 10 ? right_problem : 0);
        }
        
        return profit;
    }

    // Helper function to update overlapping matches
    void update_overlap(std::vector<std::vector<int>>& match, int j, int count, int i, 
    bool dangling_left, bool dangling_right, int overlap_left, int overlap_right) {
        if (dangling_left) {
            int curr_index = j - 1;
            while (curr_index >= 0 && match[curr_index][0] == match[j][0] && match[curr_index][1] == match[j][1]) {
                if (overlap_left < 10) {
                    match[curr_index][0] = -1;
                    match[curr_index][1] = -1;
                } else {
                    match[curr_index][1] -= overlap_left;
                }
                curr_index--;
            }
        }
        if (dangling_right) {
            int curr_index = j + count;
            while (curr_index < match.size() && match[curr_index][0] == match[j + count - 1][0] && match[curr_index][1] == match[j + count - 1][1]) {
                if (overlap_right < 10) {
                    match[curr_index][0] = -1;
                    match[curr_index][1] = -1;
                } else {
                    match[curr_index][0] += overlap_right;
                }
                curr_index++;
            }
        }
    }

    // Helper function to insert a match into the match vector
    void insert_match(std::vector<std::vector<int>>& match, int i, int j, int count) {
        for (int t = j; t < j + count; t++) {
            match[t][0] = i;
            match[t][1] = i + count - 1;
        }
    }

    // Helper function to build the output set
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> build_output_set(const std::vector<std::vector<int>>& match, int& value1) {
        std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> result;
        for (int t = 0; t < match.size() - 1; t++) {
            if (match[t][0] != -1 && match[t][0] != match[t + 1][0]) {
                std::pair<int, int> outer_pair(t - match[t][1] + match[t][0], t);
                std::pair<int, int> inner_pair(match[t][0], match[t][1]);
                result.insert(std::make_pair(outer_pair, inner_pair));
            }
            if (match[t][0] != -1) value1++;
        }
        if (match[match.size() - 1][0] != -1) value1++;
        return result;
    }

public:
    // Main function with modularized calls
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> exact_matches2(std::vector<int>& submission1, 
    std::vector<int>& submission2, int& value1, std::vector<std::vector<int>>& check) {
        std::vector<int> a = submission1;
        std::vector<int> b = submission2;
        int n1 = a.size();
        int n2 = b.size();
        std::vector<std::vector<int>> match(n2, std::vector<int>(2, -1));
        int i = 0, j = 0, count = 0;

        while (true) {
            if (i < n1 && j < n2 && a[i] == b[j]) {
                count = is_matching_sequence(a, b, i, j);
                if (count >= 10) {
                    bool dangling_left = match[j][0] != -1;
                    bool dangling_right = match[j + count - 1][0] != -1;
                    int profit = calculate_profit(match, j, count, dangling_left, dangling_right);

                    if (profit > 0) {
                        check.push_back({i, j});
                        insert_match(match, i, j, count);
                        i += count;
                        j = 0;
                        if (i >= n1) break;
                    } else {
                        j += count;
                    }
                } else {
                    j++;
                }
                count = 0;
            } else {
                j++;
                if (j >= n2) {
                    i++;
                    j = 0;
                    if (i >= n1) break;
                }
            }
        }
        return build_output_set(match, value1);
    }
};

// -----------------------------------------------------------------------------

// class for LCS
class LongestCommonSubsequence {
private:
    // Function to compute the LCS table
    std::vector<std::vector<int>> compute_lcs_table(const std::vector<int>& v1, const std::vector<int>& v2) {
        int n = v1.size();
        int m = v2.size();
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

        // Fill the DP table
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j) {
                if (v1[i - 1] == v2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } 
                else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }
        return dp;
    }

    // Function to trace back and get the LCS and its index pairs
    std::pair<std::vector<int>, std::vector<std::pair<int, int>>> trace_lcs(const std::vector<std::vector<int>>& dp, 
    const std::vector<int>& v1, const std::vector<int>& v2) {
        std::vector<int> lcs_sequence;
        std::vector<std::pair<int, int>> index_pairs;
        int i = v1.size();
        int j = v2.size();
        // Traceback to find the LCS sequence
        while (i > 0 && j > 0) {
            if (v1[i - 1] == v2[j - 1]) {
                lcs_sequence.push_back(v1[i - 1]);
                index_pairs.emplace_back(i - 1, j - 1);
                --i;
                --j;
            } 
            else if (dp[i - 1][j] > dp[i][j - 1]) {
                --i;
            } 
            else {
                --j;
            }
        }
        // Reverse to get the correct order
        std::reverse(lcs_sequence.begin(), lcs_sequence.end());
        std::reverse(index_pairs.begin(), index_pairs.end());

        return {lcs_sequence, index_pairs};
    }

    // Function to return the maximum length of the subarray
    std::vector<int> mx_len_indices_return(std::vector<std::vector<int>> dp, std::vector<int> lcs_sequence, 
    std::vector<std::pair<int, int>> index_pairs) {
        int max_length = 0;  
        // To keep track of the maximum length over accepted subarrays
        int n = lcs_sequence.size();
        int final_start1 = -1;
        int final_start2 = -1;
        for (int start = 0; start < n; ++start) {
            for (int end = start + 1; end <= n; ++end) {
                // Find corresponding subarrays in v1 and v2
                int start1 = index_pairs[start].first;
                int start2 = index_pairs[start].second;
                int end1 = index_pairs[end - 1].first + 1;
                int end2 = index_pairs[end - 1].second + 1;
                
                // Check 80% match criterion
                if (end-start >= 0.8*std::max(end1-start1, end2-start2)) {
                    int longer_length = std::min(end1-start1, end2-start2);
                    // Update max length if this subarray is accepted
                    // max_length = std::max(max_length, longer_length);
                    if(longer_length > max_length) {
                        max_length = longer_length;
                        final_start1 = start1;
                        final_start2 = start2;
                    }  
                }
            }
        }
        std::vector<int> info = {max_length, final_start1, final_start2};
        return info;
    }
public:
    // Main function to process LCS and check subarrays
    std::vector<int> process_lcs(const std::vector<int>& v1, const std::vector<int>& v2) {
        // Step 1: Compute LCS table
        std::vector<std::vector<int>> dp = compute_lcs_table(v1, v2);
        
        // Step 2: Trace LCS sequence and index pairs
        auto [lcs_sequence, index_pairs] = trace_lcs(dp, v1, v2);

        // Step 3: Check 80% match for all subarrays of LCS
        std::vector<int> info = mx_len_indices_return(dp,lcs_sequence,index_pairs); // Function to return the maximum length of the subarray
        // return the maximum length of the subarray
        return info; 
    }
};
 

// The main function to match the submissions
std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    // TODO: Write your code here

    // Initialising the TotalLengthMatches class
    TotalLengthMatches tlm;
    TotalLengthMatches2 tlm2;
    // value1 is the result[1] in the return array
    int value1 = 0;
    int value2 = 0;
    int value3 = 0;
    int value4 = 0;
    // check vector for testing purposes (ignore!)
    std::vector<std::vector<int>> check;
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> helo1 = tlm.exact_matching_length1(submission1, submission2, value1, check);
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> helo2 = tlm.exact_matching_length1(submission2, submission1, value2, check);

    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> helo3 = tlm2.exact_matches2(submission1, submission2, value3, check);
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> helo4 = tlm2.exact_matches2(submission2, submission1, value4, check);

    int valuef = std::max(std::max(value1,value2),std::max(value3,value4));
    // for(std::set<std::pair<std::pair<int, int>, std::pair<int, int>>>::iterator it = helo1.begin(); it != helo1.end(); it++) {
    //     std::cout << it->first.first << " " << it->first.second << " " << it->second.first << " " << it->second.second << std::endl;
    // }

    // Initialising the LCS class
    LongestCommonSubsequence lcs;
    // max_length_cs is the result[2] in the return array
    std::vector<int> info = lcs.process_lcs(submission1, submission2);

    // std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> helo = exact_matching(submission1,submission2,value1,check);
    // std::cout << info[0]<<" " << info[1] << " " << info[2] << " " << std::endl;
    // std::cout  << submission1.size() << " " << submission2.size() << " "<< " 1. sub1,sub2 " << value1 << " " << std::endl;
    // std::cout  << submission1.size() << " " << submission2.size() << " "<< " 1. sub2,sub1 " << value2 << " " << std::endl;
    // std::cout  << submission1.size() << " " << submission2.size() << " "<< " 2. sub1,sub2 " << value3 << " " << std::endl;
    // std::cout  << submission1.size() << " " << submission2.size() << " "<< " 2. sub2,sub1 " << value4 << " " << std::endl;
    if(info[0] < 30) {
        info[0] = 0; 
        info[1] = -1; 
        info[2] = -1;
    }
    int criterion = 0;
    if (valuef >= 0.35*(std::min(submission1.size(),submission2.size())))
    {
        criterion = 1;
    }
    else 
    {
        if (info[0]>=0.3*(std::min(submission1.size(),submission2.size())))
        {
            criterion = 1;
        }
    }
    // std::cout<<criterion<<" "<<valuef<<" "<<info[0]<<" "<<info[1]<<" "<<info[2]<<"\n";
    std::array<int, 5> result = {criterion, valuef, info[0], info[1], info[2]};
    return result; 
    // End TODO
}