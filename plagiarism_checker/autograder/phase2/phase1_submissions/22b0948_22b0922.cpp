#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <algorithm>
#include<map>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here



// Function to find exact matches of subsequences of a specified length
int findExactMatches(const std::vector<int>& s1, const std::vector<int>& s2, int minLength, int maxLength) {
    int totalLength = 0;
    int i=0;
    while(i<s1.size()-minLength){
        bool check=0;
        int temp=0;
        for(int j=0;j<s2.size()-minLength;j++){
            check=0;
            temp=0;
            for(int k=0;k<minLength;k++){
                if(s1[i+k]==s2[j+k]){}
                else{check=1;break;}
            }
            if(check==0){
                temp=minLength;
                for(int k=minLength;k<maxLength;k++){
                    if(i+k<s1.size() && j+k<s2.size() && s1[i+k]==s2[j+k]){temp++;}
                    else{break;}
                }
                i=i+temp;
                totalLength=totalLength+temp;
                break;
            }
            
        }
        if(check==1){i=i+1;}
        

    }
    return totalLength;
}






int findExactMatches2(const std::vector<int>& s1, const std::vector<int>& s2, int minLength, int maxLength) {
    int totalLength = 0;
    int i=0;
    std::vector< std::pair<int,int> > ans;
    while(i<s1.size()-minLength){
        bool check=0;
        int temp=0;
        for(int j=0;j<s2.size()-minLength;j++){
            check=0;
            temp=0;
            for(int k=0;k<minLength;k++){
                if(s1[i+k]==s2[j+k]){}
                else{check=1;break;}
            }
            if(check==0){
                temp=minLength;
                for(int k=minLength;k<maxLength;k++){
                    if(i+k<s1.size() && j+k<s2.size() && s1[i+k]==s2[j+k]){temp++;}
                    else{break;}
                }
                i=i+temp;
                ans.push_back({j,temp});
                // totalLength=totalLength+temp;
                break;
            }
            
        }
        if(check==1){i=i+1;}
        

    }
    int sum = 0;
    int previousFirst = -1; // Initialize with the smallest possible integer

    for (const auto& p : ans) {
        if (p.first >= previousFirst) {
            sum += p.second;
            previousFirst = p.first;
        }
    }
    totalLength=sum;
    return totalLength;
}


// Function to find approximate matches (LCS based)
int findLongestApproximateMatch(const std::vector<int>& s1, const std::vector<int>& s2, int& start1, int& start2) {
    int n = s1.size();
    int m = s2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    int longestMatch = 0;
    start1 = -1;
    start2 = -1;

    int low = 1;
    int high = std::min(m,n);

    while(low <= high){
        int mid = (low + high)/2;
        int check=0;
        for(int i=0;i<n-mid;i=i+10){
            for(int j=0;j<m-mid;j=j+10){
                std::vector<int> vec1(s1.begin()+i, s1.begin()+i+mid);
                std::vector<int> vec2(s2.begin()+j, s2.begin()+j+mid);
                int temp=findExactMatches2(vec1,vec2,10,20);
                if(temp>0.8*mid){check=1;longestMatch=mid;start1=i;start2=j;break;}

            }
            if(check==1){break;}
        }
        if(check==1){
            low = mid+1;
        }
        else{
            high = mid - 1;
        }

    
    }
    return longestMatch;
}



std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // Step 1: Find exact matches
    int totalExactMatchLength = findExactMatches(submission1, submission2, 10, 20);
    result[1] = totalExactMatchLength;

    // Step 2: Find longest approximate match
    int start1, start2;
    int longestApproxMatchLength = findLongestApproximateMatch(submission1, submission2, start1, start2);
    if (longestApproxMatchLength >= 30) {
        result[2] = longestApproxMatchLength;
        result[3] = start1;
        result[4] = start2;
    }

    // Step 3: Determine if plagiarism is flagged
    if (totalExactMatchLength > 0.2*std::min(submission1.size(),submission2.size()) || longestApproxMatchLength > 0.5*std::min(submission1.size(),submission2.size())) {
        result[0] = 1; // Flag as plagiarized
    } else {
        result[0] = 0; // Not plagiarized
    }

    return result; // dummy return
    // End TODO
}







