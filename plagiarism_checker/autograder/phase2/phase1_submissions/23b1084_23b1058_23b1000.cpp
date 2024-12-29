#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include<algorithm>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

//Function to find the length of exact matches in pattern
int exactMatch(std::vector<int> pattern1, int a, std::vector<int> pattern2, int b){
    int M = pattern1.size();
    int N = pattern2.size();
    int length=0;

    for(int i=0; i<std::min(M-a,N-b);i++){
        if(pattern1[i+a]==pattern2[i+b]){
            length++;
        }
        else return length;
    }

    return length;
}

//Function to find longest common sequence
std::vector<std::pair<int, int>> longestApproxMatchPositions(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int m=submission1.size();
    int n=submission2.size();

    //making dynamic programming array to store longest matching sequences
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i=1;i<=m;i++){
        for (int j=1;j<=n;j++){
            if(submission1[i-1]==submission2[j-1]){ 
                dp[i][j]=dp[i-1][j-1] + 1;
            } 
            else{
                dp[i][j]=std::max(dp[i-1][j],dp[i][j-1]);
            }
        }
    }

    // Positions in the respective vectors that are matched as subsequence(longest) in the form (a,b) and Backtracking to find the positions of elements in the LCS
    std::vector<std::pair<int, int>> lcs_positions; // a is the position in vector 1 and b is the position in vector2 in pair(a,b)
    int i=m;
    int j=n;

    while(i>0 && j>0){
        if(submission1[i-1]==submission2[j-1]){
            lcs_positions.push_back({i-1,j-1});
            i=i-1;
            j=j-1;
        }
        else if(dp[i-1][j]>=dp[i][j-1]){
            i=i-1;
        }
        else{
            j=j-1;
        }
    }

    std::reverse(lcs_positions.begin(), lcs_positions.end());

    return lcs_positions;
}



std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    int total_length=0;
    std::vector<int> dummy=submission2;

    // Loops for getting exact pattern match length without any overlaps in matches
    for(int i=0; i<submission1.size();i++){
        int maxlength=0;
        int index=0;
        for(int j=0; j<submission2.size();j++){
            int length1 = exactMatch(submission1, i, dummy, j);
            if(length1 > maxlength) {
                maxlength=length1;
                index=j;
            }
        }
        if(maxlength>10) {
            total_length+=maxlength;
            i+=maxlength;
            for(int j=0;j<maxlength;j++){
                dummy[index+j]=-1;
            }
        }
    }

    // Finding longest approximate subsequence and starting indices
    auto positions=longestApproxMatchPositions(submission1,submission2);
    int longest=0;
    int a=0;
    int b=0;

    for(int i=0;i<positions.size();i++){
        for(int j=i+23;j<positions.size();j++){
            if(j-i+1>(double)(0.8*(positions[j].first-positions[i].first+1))){
                if(j-i+1>(double)(0.8*(positions[j].second-positions[i].second+1))){
                    if(std::max(positions[j].first-positions[i].first+1,positions[j].second-positions[i].second+1)>longest){
                        a=positions[i].first;
                        b=positions[i].second;
                        longest=std::max({positions[j].first-positions[i].first+1,positions[j].second-positions[i].second+1});
                    }
                }
            }
        }
    }

    if(total_length>std::max(submission1.size(),submission2.size())*0.3) result[0]=1;
    else result[0]=0;
    result[1]=total_length;
    result[2]=longest;
    result[3]=a;
    result[4]=b;

    return result;
    // End TODO
}