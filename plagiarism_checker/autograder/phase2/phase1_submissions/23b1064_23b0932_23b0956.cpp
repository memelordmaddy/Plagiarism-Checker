#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

// -----------------------------------------------------------------------------

#define MIN_MATCH_LENGTH 10
#define JACCARD_SIMILARITY 0.4
#define MISMATCH_ERROR 0.2
#define MIN_APPROX_MATCH_LENGTH 30

/*
Functions: 
binary_search_last_compatible : O(log i); i = match whose last compatible subset is to be found
max_sum_of_non_overlapping_matches : O(nlogn); n = number of matches whose length is greater than or equal to MIN_MATCH_LENGTH
find_exact_matches : O(m * n); n = submission 1 size, m = submission 2 size
longest_approximate_match : O((m+n) * min(m,n) * log(min(m,n)))
isValidLength : O(min(m,n))
*/

struct Match{
    int startA;
    int startB;
    int length;
    int endA;
    int endB;

    Match() : startA(-1), startB(-1), length(-1), endA(-1), endB(-1){}

    Match(int sA, int sB, int len) : startA(sA), startB(sB), length(len), endA(sA + len - 1), endB(sB + len - 1) {}

    bool operator<(const Match &other) const
    {
        return endA < other.endA;
    }
};

int binary_search_last_compatible(int i, std::vector<Match>& matches){
	int l = 0, r = i-1;
	while (l+1 < r){
		int mid = (l+r)/2;
		if (matches[mid].endA < matches[i].startA){
			l = mid;
		}
		else {
			r = mid - 1;
		}
	}
	if (matches[l].endA >= matches[i].startA){
		return -1;
	}
	return l;
}

int max_sum_of_non_overlapping_matches(std::vector<Match>& matches) {
    // Complexity: O(nlogn); n = number of matches
    if (matches.empty()) {
        return 0;
    }

    std::sort(matches.begin(), matches.end());

    int n = matches.size();
    std::vector<int> dp(n, 0);
    dp[0] = matches[0].length;

    for (int i = 1; i < n; i++) {
		/*
			There are two cases:
			1. In the subset having the maximum length of exact matches, the ith match is also present
			2. The earlier, calculated till i-1 matches is still the maximum

			In the first case, find the last compatible subset's length. If this is greater than without the ith one in the subset, then dp[i] = matches[i].length + dp[last_compatible]
			If it is not greater, then dp[i] = dp[i-1]
		*/
        int include_length = matches[i].length;
		
        int last_compatible = binary_search_last_compatible(i, matches);
        
        if (last_compatible != -1) {
            include_length += dp[last_compatible];
        }
		dp[i] = std::max(include_length, dp[i-1]);
    }
    return dp[n - 1];
}

std::pair<std::vector<Match>, Match> find_exact_matches(std::vector<int>& A, std::vector<int>& B, int min_match_length, std::vector<std::vector<int>>& dp){
    // Complexity: O(m * n); n = submission 1 size, m = submission 2 size
    const int n = A.size();
    const int m = B.size();

    int maxm = 0;
    std::vector<Match> matches;

    Match max_match;
    for (int i = n - 1; i >= 0; i--){
        for (int j = 0; j <= m - 1; j++){
            if (A[i] == B[j]){
                dp[i][j] = dp[i+1][j+1] + 1;
                if (dp[i][j] >= min_match_length){
                    Match match(i, j, dp[i][j]);
                    matches.push_back(match);
                    maxm = std::max(maxm, match.length);
                    if (max_match.length < match.length){
                        max_match = match;
                    }
                }
            }
            else
            {
                dp[i][j] = 0;
            }
        }
    }

    return {matches, max_match};
}

std::pair<bool, int> isValidLength(const std::vector<int>& cumulativeZeros, int len, int maxStart) {
    for (int start = 0; start + len - 1 < cumulativeZeros.size() && start <= maxStart; start++) {
        int end = start + len - 1;
        int zeroCount = (start == 0) ? cumulativeZeros[end] 
                                   : cumulativeZeros[end] - cumulativeZeros[start - 1];
        // Check if the number of zeros is within the allowed threshold for mismatches
        if (zeroCount < MISMATCH_ERROR * len) {
            return {true, start};
        }
    }
    return {false, 0}; // No valid length found
}

std::tuple<int, int, int> longest_approximate_match(std::vector<std::vector<int>> &dp, Match& max_match){
    /*
    This function calculates the longest approximate matching subsequence between two vectors A and B. Here, we are interpreting an approximate match, if two subsequences of A and B have the same length and more than 80% of the elements at corresponding positions are same, i.e. there are atmost 20% positions, where there is a mismatch.
    
    The length of the longest approximate match cannot be less than the length of longest exact match. Hence, we start from the length of the longest exact match and try to find match with larger length. 

    Here, the (i,j) entry of the dp table represents the length of the longest exact match starting at A[i] and B[j], A and B are given vectoors.

    Idea is the iterate through all the diagonals starting from the top edge and left edge of the dp table. Each diagonal contains all the subsequences with a certain offset between the starting indices of the two vectors. For each diagonal, we build a cumulative zeros vecotr which stores the number of zeros in the diagonal till that point. We use this cumulative zeros vector to find the largest subsequence of the diagonal which passes the threshold using binary search.

    We maintain the largest such subsequence over all diagonals and return it at the end.
    */
    int n = dp.size() - 1;
    int m = dp[0].size() - 1;
    std::tuple<int, int, int> result = {max_match.length, max_match.startA, max_match.startB};

    for (int start_x = 0; start_x < m; start_x++) {
        int i = start_x;
        int j = 0;
        std::vector<int> cumulativeZeros;
        
        while (i < m && j < n) {
            if (dp[j][i] == 0) {
                if (j == 0){
                    cumulativeZeros.push_back(1);
                }
                else{
                    cumulativeZeros.push_back(cumulativeZeros.back() + 1);
                }
            } else {
                if (j == 0){
                    cumulativeZeros.push_back(0);
                }
                else{
                    cumulativeZeros.push_back(cumulativeZeros.back());
                }
            }
            i++;
            j++;
        }

        // Binary search for the largest valid length
        int left = std::max(std::get<0>(result) + 1, MIN_APPROX_MATCH_LENGTH);  // Start from current best + 1
        int right = cumulativeZeros.size();
        int bestLen = 0;
        int bestStart = 0;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            auto [isValid, start] = isValidLength(cumulativeZeros, mid, cumulativeZeros.size() - mid);
            
            if (isValid) {
                if (mid > bestLen) {
                    bestLen = mid;
                    bestStart = start;
                }
                left = mid + 1;  // Try for a larger length
            } else {
                right = mid - 1;  // Try a smaller length
            }
        }

        if (bestLen > std::get<0>(result)) {
            result = {bestLen, start_x + bestStart, bestStart};
        }
    }

    for (int start_y = 0; start_y < n; start_y++) {
        int i = 0;
        int j = start_y;
        std::vector<int> cumulativeZeros;
        
        while (i < m && j < n) {
            if (dp[j][i] == 0) {
                if (i == 0){
                    cumulativeZeros.push_back(1);
                }
                else {
                    cumulativeZeros.push_back(cumulativeZeros.back() + 1);
                }
            } else {
                if (i == 0){
                    cumulativeZeros.push_back(0);
                }
                else {
                    cumulativeZeros.push_back(cumulativeZeros.back());
                }
            }
            i++; j++;
        }

        // Binary search for the largest valid length
        int left = std::max(std::get<0>(result) + 1, MIN_APPROX_MATCH_LENGTH);  // Start from current best + 1
        int right = cumulativeZeros.size();
        int bestLen = 0;
        int bestStart = 0;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            auto [isValid, start] = isValidLength(cumulativeZeros, mid, cumulativeZeros.size() - mid);
            
            if (isValid) {
                if (mid > bestLen) {
                    bestLen = mid;
                    bestStart = start;
                }
                left = mid + 1;  // Try for a larger length
            } else {
                right = mid - 1;  // Try a smaller length
            }
        }

        if (bestLen > std::get<0>(result)) {
            result = {bestLen, start_y + bestStart, bestStart};
        }
    }
    if (std::get<0>(result) < MIN_APPROX_MATCH_LENGTH){
        result = {0, -1, -1};
    }
    return result;
}

std::array<int, 5> find_plag(std::vector<int>& A, std::vector<int>& B)
{
    const int n = A.size();
    const int m = B.size();

    std::vector<std::vector<int>> dp(n+1, std::vector<int>(m+1, 0));
    std::pair<std::vector<Match>, Match> matches_max_match = find_exact_matches(A, B, MIN_MATCH_LENGTH, dp);
    std::vector<Match> matches = matches_max_match.first;
    Match max_match = matches_max_match.second;

    std::tuple<int, int, int> approx_match = longest_approximate_match(dp, max_match);

    std::array<int, 5> result = {0, 0, 0, 0, 0};
    
    result[1] = max_sum_of_non_overlapping_matches(matches);

    if ((double)result[1]/(n+m - result[1]) > JACCARD_SIMILARITY)
        result[0] = 1;
    
    result[2] = std::get<0>(approx_match);
    result[3] = std::get<1>(approx_match);
    result[4] = std::get<2>(approx_match);

    return result;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    std::array<int, 5> result = find_plag(submission1, submission2);

    return result;
}
