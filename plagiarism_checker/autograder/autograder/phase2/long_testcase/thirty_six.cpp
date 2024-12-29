#include <array>
#include <iostream>
#include <span>
#include <vector>
// -----------------------------------------------------------------------------
#include <unordered_set>
#include <cmath>
#include <set>
#include <unordered_map>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here


/*
the function exactMatches uses Rabin-Karp-inspired hashing combined with a hash table to efficiently identify matching 
segments between two sequences
It sets up for efficient hash computation by using the hash table to store previously computed hashes for segments of different lengths.
This is in line with rolling hash techniques where you typically want to avoid recalculating the hash from scratch for overlapping segments
*/

/*
Here the main idea to find the longest approximate match is to expand around the exact match in both directions.
while expanding we allow some small mismatches while including as many as possible matching elements, and keep
track of length and exact match length to check and ensure to keep atleast 80% match and we are looking for 
maximum approximate match length
To find longest approximate subsequence match, we are using DP table(2D array) to store  lengths of matching subsequences at each position in
the two submissions, when the minimum exact match is found by using DP table, then we use the function of sliding window to expand the match 
such that we find the approximate match with atleast 80% match.
First we start with initialising DP table to all zeros, and then we fill dp table with lenghts of exact matching subsequences, and when an 
exact match of min length(30) is found, we do the approximate match expansion by allowing small mismatches in both directions, for that while expanding we also
keep track of no.of matches and constantly checks so that approximation does not fall below 80%
*/

// Function to calculate a rolling hash value for a segment of the input data
int hashCalculation(std::vector<int> & data, int start, int length, int base, int modulus){
    long long hashValue = 0;
    for(int i = 0; i < length; i++){        // Calculate the hash value using polynomial rolling hash formula
        hashValue = (hashValue * base + data[start + i]) % modulus;
    }
    return static_cast<int>(hashValue);
}

// Function to check if two vectors are equal in a given range
bool isEqual(std::vector<int> & submission1, std::vector<int> & submission2, int i, int j, int len){
    if (i < 0 || j < 0 || i + len > submission1.size() || j + len > submission2.size()) {
        return false; // Out of bounds
    }
    for (int k = 0; k < len; k++) {
        if (submission1[i + k] != submission2[j + k]) {
            return false; // Found a mismatch
        }
    }
    return true;    // Segments are equal so return true
}

// Function to find matches of segments between two submissions
int exactMatches(std::vector<int> & submission1, std::vector<int> & submission2, int & totalLength){
    int minimumLength = 10;     // Minimum length of segment to consider
    int maximumLength = 20;     // Maximum length of segment to consider
    int matchLength = 0;        // Total length of matching segments

    int base = 256;             
    int modulus = 1000000007;   

    std::unordered_map<int, std::vector<int>> hashTable;    
    std::set<int> matchIndex1;      // To track matched indices in submission1
    std::set<int> matchIndex2;      // To track matched indices in submission2

    for(int i = 0; i <= submission1.size() - maximumLength; i++){   // Creating hash table for segments in submission1
        for(int length = minimumLength; length <= maximumLength; length++){
            int hash = hashCalculation(submission1, i, length, base, modulus);
            hashTable[hash].push_back(i);
        }
    }

    for(int j = 0; j <= submission2.size() - maximumLength; j++){   // Check for matched in submission2 against hash table
        for(int length = minimumLength; length <= maximumLength; length++){
            int hash = hashCalculation(submission2, j, length, base, modulus);  
            if(hashTable.count(hash)){      // Check if this hash exists in hash table
                for(int i : hashTable[hash]){   // Iterate over all starting indices with this hash
                    // Verify if the segments are equal and not already matched
                    if(isEqual(submission1, submission2, i, j, length) && 
                        matchIndex1.find(i) == matchIndex1.end() && 
                        matchIndex2.find(j) == matchIndex2.end()){
                            matchLength += length;  // Updating total match length

                        for(int m = 0; m < length; m++){    // Marking matched indices to avoid double counting
                            matchIndex1.insert(i + m);      // Matched indices from submission1
                            matchIndex2.insert(j + m);      // Matched indices from submission2
                        }

                        break;      // After finding the match for this hash
                    }
                }
            }
        }
    }

    totalLength = matchLength;
    return matchLength;
}

// Fills the DP table and identifies potential matches that are longer than 30 tokens
void updateDPTable(std::vector<std::vector<int>>& dp, const std::vector<int>& A, const std::vector<int>& B, 
                   int minlength, std::vector<std::pair<int, int>>& potentialMatches) {
    int n = A.size();
    int m = B.size();
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            // check if the current elements in vector A and B match
            if (A[i - 1] == B[j - 1]) {
                 //increment the match length from the previous diagonal element
                dp[i][j] = dp[i - 1][j - 1] + 1;
                // If the match length at this position is at least minlength, we add it to the list of potential matches
                if (dp[i][j] >= minlength) {
                     potentialMatches.push_back({i,j});
                }
            } else {
                dp[i][j] = 0;  // Set the match length to 0 if the elements do not match
            }
        }
    }
}

// Expands the window backward from the initial exact match to increase approximate match length
void expandBackward( std::vector<int>& A,  std::vector<int>& B, int initial_start1, int initial_start2,
                    int exactMatchLength, int& longestMatch, int& startindex1, int& startindex2) {
    int match_count = exactMatchLength; //to track the count of matching elements in the current window
    int window_length = exactMatchLength; //initial window length is set as the match length

    while (initial_start1 - (window_length - exactMatchLength) >= 0 &&
           initial_start2 - (window_length - exactMatchLength) >= 0) {
        float match_ratio = static_cast<float>(match_count) / window_length;

        // Update the longest match if match ratio>=0.8 and current window is longer than longestMatch
        if (match_ratio >= 0.8 && window_length > longestMatch) {
            longestMatch = window_length;
            startindex1 = initial_start1 - (window_length - exactMatchLength) ;
            startindex2 = initial_start2 - (window_length - exactMatchLength) ;
        }

        window_length++;

        // Check the next element in the backward expansion to see if it matches
        if (initial_start1 - (window_length - exactMatchLength) >= 0 &&
            initial_start2 - (window_length - exactMatchLength) >= 0) {
            if (A[initial_start1 - (window_length - exactMatchLength)] ==
                B[initial_start2 - (window_length - exactMatchLength)]) {
                match_count++; // Increase match count if elements match
            }
        }

        if (static_cast<float>(match_count) / window_length < 0.8) break;
    }
}

// Expands the window forward from the initial exact match to increase approximate match length
void expandForward( std::vector<int>& A, std::vector<int>& B, int i, int j, 
                   int exactMatchLength, int& longestMatch, int& startindex1, int& startindex2) {
    int match_count = exactMatchLength;
    int window_length = exactMatchLength;
    int m = A.size();
    int n = B.size();

    // make sure the indices i, j are within the vector boundaries
    while (i + (window_length - exactMatchLength) < m && 
           j + (window_length - exactMatchLength) < n) {
        float match_ratio = static_cast<float>(match_count) / window_length;

        // Update the longest match if match ratio>=0.8 and current window is longer than longestMatch
        if (match_ratio >= 0.8 && window_length > longestMatch) {
            longestMatch = window_length;
            startindex1 = i - exactMatchLength;
            startindex2 = j - exactMatchLength;
        }

        window_length++; //increment the window_length
        // Check the next element in the forward expansion to see if it matches
        if (i + (window_length - exactMatchLength) < m && 
            j + (window_length - exactMatchLength) < n) {
            if (A[i + (window_length - exactMatchLength)] == 
                B[j + (window_length - exactMatchLength)]) {
                match_count++;  // Increase match count if elements match
            }
        }

        // Stop expanding if match ratio falls below 0.8
        if (static_cast<float>(match_count) / window_length < 0.8) break;
    }
}

// we try both forward and backward expansion around each exact match to get longest aproximate match
void expandWindow(std::vector<int>& A,  std::vector<int>& B, int i, int j, 
                  int exactMatchLength, int& longestMatch, int& startindex1, int& startindex2) {
    int initial_start1 = i - exactMatchLength; // Starting index in A for backward expansion
    int initial_start2 = j - exactMatchLength; // Starting index in B for backward expansion

    expandBackward(A, B, initial_start1, initial_start2, exactMatchLength, longestMatch, startindex1, startindex2);
    expandForward(A, B, i, j, exactMatchLength, longestMatch, startindex1, startindex2);
}

// Main function that coordinates the longest approximate match finding
int longestApproximateMatch(std::vector<int>& A, std::vector<int>& B, int& startindex1, int& startindex2) {
    int n = A.size(), m = B.size();
    int minlength = 30;
    int longestmatch = 0;

    // Initialize DP table and vector to store potential matches
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    std::vector<std::pair<int, int>> potentialMatches;

    // Fill DP table and get potential matches
    updateDPTable(dp, A, B, minlength, potentialMatches);

    // Process each potential match and expand around it
    for (const auto& match : potentialMatches) {
        int i = match.first;
        int j = match.second;
        expandWindow(A, B, i, j, dp[i][j], longestmatch, startindex1, startindex2);
    }

    return longestmatch;
}

//main function
std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    int flag = 0;
    int totalexactmatchlength = 0;
    int longestapproxmatchlength = 0;
    int startindex1 = -1, startindex2 = -1;
    // setting the indices to -1 just in case if there is no match at all

    //calculating the totalexactmatchlength(array[1]) using exactMatches function
    exactMatches(submission1, submission2, totalexactmatchlength);

    int nooftoken=std::min(submission1.size(),submission2.size());

    longestapproxmatchlength = longestApproximateMatch(submission1, submission2, startindex1, startindex2);

    // creating two kind of thresholds based on number of tokens
    if(nooftoken <= 200) flag = (totalexactmatchlength > 0.3* nooftoken || longestapproxmatchlength >= 0.5 * nooftoken) ? 1 : 0;
    else  flag = (totalexactmatchlength > 0.4* nooftoken || longestapproxmatchlength >= 0.7 * nooftoken) ? 1 : 0;

    std::array<int, 5> result = {flag, totalexactmatchlength, longestapproxmatchlength, startindex1, startindex2};
    
    return result;
}