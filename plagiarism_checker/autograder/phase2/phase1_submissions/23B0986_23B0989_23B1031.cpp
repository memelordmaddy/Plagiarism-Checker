#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include<queue>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Helper function to calculate LPS array for KMP
void calculateHtable(std::vector<int> &submission1, int index, std::vector<int>&htable){
    int i=1,j=0;
    htable[0] = -1;
    int  l = htable.size();
    while(i < l-1){
        if( submission1[i + index] != submission1[j + index]){
            htable[i] = j;
            while(j >= 0 && submission1[i + index] != submission1[j + index] ){
                j = htable[j];
            }
        }else{
            htable[i] = htable[j];
        }
        i++;j++;
    }
    htable[l-1] = j;
}

// implementation of KMP algorithm returning the index of found match
int KMP(std::vector<int>&submission1,std::vector<int> &submission2,int index,int length,std::vector<bool>&visited2){
    int i=0,j=0;
    int found = 0;
    std::vector<int>htable(length+1);
    calculateHtable(submission1,index,htable);
    int size2 = submission2.size();
    while(i < size2){
        if (visited2[i])
        {
            i++;
            j = 0;
            continue;
        }
        
        if(submission2[i] == submission1[j + index]){
            i+=1;
            j+=1;
            if(j==length){
                return i-j;
            }
        }
        else{
            j = htable[j];
            if(j<0){i++;j++;}
        }
    }
    return -1;

}

// Finds the total length of common subsequence having length between 10-20
int findingShort(std::vector<int> &submission1, std::vector<int> &submission2,std::vector<std::pair<int,int>>&matching_index){
    int size1 = submission1.size();
    int size2 = submission2.size();
    int tot_length=0;
    int startShort=10;
    int endShort=20;
    std::vector<bool> visited1(size1,false);
    std::vector<bool> visited2(size2,false);
    
    for(int start_ind=0;start_ind<=size1-5;start_ind++){
        
        bool check = false;
        for(int k=start_ind;k<start_ind+5;k++){
            if(visited1[k]){
                check = true;
                break;
            }
        }
        if(check) continue; 

        // finding the index of subsequence in submission2
        int found = KMP(submission1,submission2,start_ind,5,visited2);
        if(found>=0){
            matching_index.push_back({start_ind,found});
            for(int i=0;i<5;i++){
                visited1[start_ind+i] = true;
                visited2[found+i] = true;
            }
        }
    }

    std::fill(visited1.begin(),visited1.end(),false);
    std::fill(visited2.begin(),visited2.end(),false);
    for(int length = endShort; length>=startShort;length--){
         // iterating over subsequence of length between 10-20
        for(int start_ind=0;start_ind<=size1-length;start_ind++){

            bool check = false;
            for(int k=start_ind;k<start_ind+length;k++){
                if(visited1[k]){
                    check = true;
                    break;
                }
            }
            if(check) continue; 
            // finding the index of subsequence in submission2
            int found = KMP(submission1,submission2,start_ind,length,visited2);

            if(found>=0){
                
                for(int i=0;i<length;i++){
                    visited1[start_ind+i] = true;
                    visited2[found+i] = true;
                }
                tot_length+=length;
            }

        }
    }
    return tot_length;

}

//approximately finding longest common subsequence by searching near shorter exact matches
std::pair<int, std::pair<int, int>> findLong(std::vector<int> &submission1, std::vector<int> &submission2,std::vector<std::pair<int,int>>&matching_index){
    int size1 = submission1.size();
    int size2 = submission2.size();
    int maxsize = std::max(size1,size2);
    int max_length=0;

    int start1=-1,start2=-1;

    //matching index is the vector of pairs of indices of exact matching subsequences
    for(auto x:matching_index){
        int s1=x.first,s2=x.second;
        int length=-1;
        std::vector<std::vector<int>> dp(size1-s1+1,std::vector<int>(size2-s2+1,0));
        for (int i = 1; i <= size1-s1; ++i) {
            for (int j = 1; j <= size2-s2; ++j) {
                if (submission1[s1+i-1] == submission2[s2+j - 1])
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                else
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                if(0.8*std::max(i,j) <= dp[i][j] ){
                    length = std::max(std::min(i,j),length);
                }
            }
        }
        
        if(length>max_length && length>=30){
            max_length = length;
            start1 = s1;
            start2 = s2;
        }
            
        
    }
    return {max_length,{start1,start2}};
    
}

//function to detect plagiarism based on other parameters
int detectPlag(int n,int m, int short_len,int long_len){
    double th1=0.23;
    double th2=0.41;
    double longer=m>n?m:n;   
    if(((double)short_len)/longer>th1 || ((double)long_len)/longer>th2){
        return 1;
    }
    return 0;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {

    
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::vector<std::pair<int,int>>matching_index;
    int short_len=findingShort(submission1,submission2,matching_index);
    std::pair<int, std::pair<int, int>> long_result = findLong(submission1,submission2,matching_index);
    result[1]=short_len;
    result[2] = long_result.first;
    result[3] = long_result.second.first;
    result[4] = long_result.second.second;
    result[0]=detectPlag(submission1.size(),submission2.size(),short_len,long_result.first);
    return result; 
    // End TODO
}