#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

std :: vector<int> computeLPS(const std :: vector<int>& pattern) {
    std :: vector<int> lps(10, 0);
    int length = 0;  
    int i = 1;

    while (i < 10) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

int kmpSearch(const std :: vector<int>& vec1, const std :: vector<int>& vec2,
                                                 std :: vector<int>& vec, int x) {
    std :: vector<int> pattern(vec2.begin() + x, vec2.begin() + x + 10);
    int n = vec1.size();
    std :: vector<int> lps = computeLPS(pattern); 
    int i = 0;
    int length = 0;
    int n1 = vec2.size();
    int j = 0;
    // int ans = 0;
    while (i < n) {
        if (vec[i] != 0) {
            i++;
            j = 0;
        }
        if(i>=n || j>=10){break;}
        if (pattern[j] == vec1[i] && vec[i] == 0) {
            i++;
            j++;
        }
        if (j == 10) {
            int k = 0;
            for(int l = 1; l < 11; l++){
                vec[i - l] = 1;
            }
            if((x + 10) < n1 && (i+k) < n){
                while((vec2[x + 10 + k] == vec1[i + k]) && vec[i + k] == 0){
                    vec[i + k] = 1;
                    k++;
                    if((x+10+k)>=n1 || (i+k)>=n){break;}
                    // std::cout<<" "<<std::endl;
                }
            }
            // veca.push_back(x);
            // veca.push_back(x+length);
            length = 10 + k;
            // j = lps[j - 1];
        } 
        else if (i < n && pattern[j] != vec1[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
            // i=i-j+1;
            // j=0;
        }
    }
    // if(length == 0){
        // x += 1;
        // ans += kmpSearch(vec1, vec2, vec, x);
    // }
    // if(length != 0){
        // x += length;
        // ans += kmpSearch(vec1, vec2, vec, x);
    // }
    return length;
}

// Function to store the lengths of Longest Common Subsequence (LCS)
std :: vector<std :: vector<int>> LCS(const std :: vector<int>& a, const std :: vector<int>& b) {
    int n = a.size();
    int m = b.size();
    
    // Creating vector<vector> dp to store the lengths of longest common subsequences
    std :: vector<std :: vector<int>> dp(n + 1, std :: vector<int>(m + 1, 0));

    // Fill the DP table
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std :: max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp;  // returning the LCS vector
}

// Function to find the maximum value
std :: vector<int> findMax(const std :: vector<int>& a, const std :: vector<int>& b) {
    int n2 = b.size();
    int n1 = a.size();
    float x = 0.8;
    int max1_approx = 0, max2_approx = 0;
    int i1 = 0, j1 = 0;
    std :: vector<int> found;
    std :: vector<std :: vector<int>> lcs = LCS(a, b);
    for(int i = 0; i < n2; i++){
        for(int j = 30 + i; j <= n2; j++){
            int length = lcs[j][j] - lcs[j][i];
            int len_sub = j - i;
            if(length >= x * (len_sub)){
                if(max1_approx < len_sub){
                    max1_approx = len_sub;
                    j1 = i;
                }
            }
        }
    }
    for(int i = 0; i < n1; i++){
        for(int j = 30 + i; j <= n1; j++){
            int length = lcs[j][n2] - lcs[i][n2];
            int len_sub = j - i;
            if(length >= x * (len_sub)){
                if(max2_approx < len_sub){
                    max2_approx = len_sub;
                    i1 = i;
                }
            }
        }
    }
    max1_approx = std :: min(max1_approx, max2_approx);
    found.push_back(max1_approx);
    found.push_back(i1);
    found.push_back(j1);
    return found;
}

int match(std::vector<int> &vec1, std::vector<int> &vec2){
    int x = 0;
    int result=0;
    int n1 = vec2.size();
    int n = vec1.size();
    std :: vector<int> vec(n, 0);
    while(x <= n1 - 10){
        int res = kmpSearch(vec1, vec2, vec, x);
        if(res == 0){
            x += 1;
        }
        else{
            x += res;
            result += res;
        }
    }
    return result;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::vector<int> vec1,vec2,veca,vecb;
    if(submission1.size()>submission2.size()){
        vec1 = submission1;
        vec2 = submission2;
    }
    else{
        vec1 = submission2;
        vec2 = submission1;
    }
    int result1 = match(vec1,vec2);
    std :: vector<int> resultbc = findMax(vec1,vec2);
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    result[1] = result1;
    result[2] = resultbc[0];
    result[3] = resultbc[1];
    result[4] = resultbc[2];
    if(std::max(vec1.size(),vec2.size())<=50){
        if(result[2]>=((0.7)*(std::min(vec1.size(),vec2.size())))){
        result[0]=1;
        }
    }
    else if(std::max(vec1.size(),vec2.size())<=200){
        if(result[2]>=((0.45)*(std::min(vec1.size(),vec2.size())))){
        result[0]=1;
        }
    }
    else{
        if(result[2]>=((0.2)*(std::min(vec1.size(),vec2.size())))){
        result[0]=1;
        }
    }
    return result; // dummy return
    // End TODO
}