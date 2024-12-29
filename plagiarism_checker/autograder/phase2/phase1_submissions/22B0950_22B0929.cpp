#include <bits/stdc++.h>
using namespace std;


// a struct to store details of approximate LCS
struct LCSResult {
    int length;
    pair<int, int> start_a, end_a; // start and end in vector a
    pair<int, int> start_b, end_b; // start and end in vector b
};

// Helper function to compute the Longest Common Subsequence (LCS)
int lcs_length(const vector<int>& seq1, const vector<int>& seq2) {
    int n = seq1.size();
    int m = seq2.size();
    
    vector<vector<int>> dp(n+1, vector<int>(m+1, 0));
    
    // Fiint the DP table
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (seq1[i - 1] == seq2[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }
    return dp[n][m];  
}

// Modified DP function to get LCS with aintowed changes and track positions // binary search on DP
LCSResult LCS_with_changes(vector<int> &a, vector<int> &b, int n, int m, int allowed_changes) {
    vector<vector<int>> dp(n+1, vector<int>(m+1, 0));
    vector<vector<int>> changes(n+1, vector<int>(m+1, 0)); // track number of changes
    vector<vector<pair<int, int>>> trace_a(n+1, vector<pair<int, int>>(m+1, {-1, -1}));  // to trace start position in a
    vector<vector<pair<int, int>>> trace_b(n+1, vector<pair<int, int>>(m+1, {-1, -1}));  // to trace start position in b

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i-1] == b[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
                changes[i][j] = changes[i-1][j-1];
                trace_a[i][j] = trace_a[i-1][j-1] == make_pair(-1, -1) ? make_pair(i, i) : trace_a[i-1][j-1];
                trace_b[i][j] = trace_b[i-1][j-1] == make_pair(-1, -1) ? make_pair(j, j) : trace_b[i-1][j-1];
            } else if (changes[i-1][j-1] < allowed_changes) {  // allow change if under limit
                dp[i][j] = dp[i-1][j-1] + 1;
                changes[i][j] = changes[i-1][j-1] + 1;  // increase change count
                trace_a[i][j] = trace_a[i-1][j-1] == make_pair(-1, -1) ? make_pair(i, i) : trace_a[i-1][j-1];
                trace_b[i][j] = trace_b[i-1][j-1] == make_pair(-1, -1) ? make_pair(j, j) : trace_b[i-1][j-1];
            }
            if (dp[i-1][j] > dp[i][j]) {
                dp[i][j] = dp[i-1][j];
                changes[i][j] = changes[i-1][j];
                trace_a[i][j] = trace_a[i-1][j];
                trace_b[i][j] = trace_b[i-1][j];
            }
            if (dp[i][j-1] > dp[i][j]) {
                dp[i][j] = dp[i][j-1];
                changes[i][j] = changes[i][j-1];
                trace_a[i][j] = trace_a[i][j-1];
                trace_b[i][j] = trace_b[i][j-1];
            }
        }
    }

    // Backtrack to find positions
    int max_length = dp[n][m];
    pair<int, int> start_a = {-1, -1}, start_b = {-1, -1};
    pair<int, int> end_a = {-1, -1}, end_b = {-1, -1};

    for (int i = n; i > 0; i--) {
        for (int j = m; j > 0; j--) {
            if (dp[i][j] == max_length) {
                end_a = {i-1, i};  // Convert to 0-based indexing
                end_b = {j-1, j};
                start_a = trace_a[i][j];
                start_b = trace_b[i][j];
                break;
            }
        }
    }

    return {max_length, start_a, end_a, start_b, end_b};
}

// Check if we can achieve an LCS of given length with aintowed changes
bool check(vector<int> &a, vector<int> &b, int n, int m, int len, int allowed_changes) {
    LCSResult result = LCS_with_changes(a, b, n, m, allowed_changes);
    return result.length >= len;
}

// Find maximum LCS length with allowed changes
LCSResult max_approximate_lcs(vector<int> &a, vector<int> &b) {
    int n = a.size(), m = b.size();

    // Step 1: Compute original LCS without any changes
    int original_lcs_length = lcs_length(a, b);

    // Step 2: Calculate the number of changes aintowed (20% of original LCS length)
    int allowed_changes = original_lcs_length / 9999 ;

    // Step 3: Perform binary search on the LCS length
    int low = 0, high = min(n, m), ans_length = 0;
    LCSResult final_result;

    while (low <= high) {
        int mid = (low + high) / 2;
        if (check(a, b, n, m, mid, allowed_changes)) {
            ans_length = mid;
            low = mid + 1;  // try for larger LCS length
            final_result = LCS_with_changes(a, b, n, m, allowed_changes);  // capture the result
        } else {
            high = mid - 1; // reduce the search space
        }
    }
    return final_result;
}

// changed int to long long to avoid overflow
array<int, 5> match_submissions(vector<int> &submission1, vector<int> &submission2) {

    array<int, 5> result = {0, 0, 0, 0, 0};
    // 1. Exact matches (10-20 tokens) - use LCS
    int total_exact_match_len = lcs_length(submission1, submission2);

    // 2. Longest Approximate Match (30+ tokens) - use Binary Search on DP
    LCSResult max_approximate_len = max_approximate_lcs(submission1,submission2);

    // Setting result
    result[1] = (total_exact_match_len>=10)?total_exact_match_len:0;
    result[2] =(max_approximate_len.length>=30)?max_approximate_len.length:0;
    result[3] = (total_exact_match_len == 0) ? -1 : max_approximate_len.start_a.first;
    result[4] = (max_approximate_len.length == 0) ? -1 : max_approximate_len.start_b.first;
    

    if (total_exact_match_len >=10 || max_approximate_len.length > total_exact_match_len) {
        result[0] = 1; // Plagiarism found
    }
    
    return result;
}

