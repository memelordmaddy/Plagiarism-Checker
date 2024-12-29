#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include<algorithm>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
void KMPTable(std::vector<int> &submission1,int start,int length,int* h){
    int i=1;
    int j=0;
    h[0]=-1;
    while(i<length){
        if(submission1[start+j]!=submission1[start+i]) {
            h[i]=j;
            while((j>=0)&&(submission1[start+j]!=submission1[start+i])){
                j=h[j];
            }
        }
        else {
            h[i]=h[j];
        }
        i++;
        j++;
    }
    h[length]=j;
}
int KMP(std::vector<int> &submission1,std::vector<int> &submission2,int start,int length,std::vector<bool>visited1,std::vector<bool>visited2){
    bool already_done=false;
    for(int i=start;i<start+length;i++){
        already_done=already_done||visited1[i];
    }
    if(already_done) return -1;
    int h[length+1];
    KMPTable(submission1,start,length,h); 
    int i=0;
    int j=0;
    std::vector<int> found;
    while(i<submission2.size()){
        if(submission1[start+j]==submission2[i]){
            i++;
            j++;
            if(j==length){
                found.push_back(i-j);
                j=h[j];
            }
        }
        else{
            j=h[j];
            if(j<0){
                i++;
                j++;
            }
        }
    }
    for(int j=0;j<found.size();j++){
        bool done=false;
        for(int i=0;i<length;i++){
            done=done||visited2[found[j]+i];
        }
        if(!done) return found[j];
    }
    return -1;

}

int val1(std::vector<int> &submission1, std::vector<int> &submission2){
    std::vector<bool> visited1(submission1.size(),false);
    std::vector<bool> visited2(submission2.size(),false);
    int order[] = {20,11,12,13,14,15,16,17,18,19,10};
    int answer=0;
    for(int i = 0; i<11;i++){
        int length=order[i];
        for(int start=0;start<=submission1.size()-length;start++){
            int found = KMP(submission1,submission2,start,length,visited1,visited2);

            if(found!=-1){
                for(int j=0;j<length;j++){
                    visited1[start+j] = true;
                    visited2[found+j] = true;
                }
                answer=answer+length;
            }

        }
    }
    return answer;

}
int longestsequence(std::vector<int> submission1,std::vector<int> submission2,int &m,int&n){
    std::vector<std::vector<double>> mem(submission1.size()+1, std::vector<double>(submission2.size()+1, 0));
    std::vector<std::vector<int>> mem_len(submission1.size()+1, std::vector<int>(submission2.size()+1, 0));
    std::vector<std::vector<int>> from_where(submission1.size()+1, std::vector<int>(submission2.size()+1, 0));
    for(int i=0;i<submission1.size();i++){
        for(int j=0;j<submission2.size();j++){
            int value1;//from L,1
            int value2;//from R,2
            int value3;//from diagonal,3
            if(submission1[i]==submission2[j]){
                value1=mem[i+1][j]+1;
                value2=mem[i][j+1]+1;
                value3=mem[i][j]+1;
            }
            else{
                value1=mem[i+1][j]-2;
                value2=mem[i][j+1]-2;
                value3=mem[i][j]-4;
            }
            if(true){
                mem[i+1][j+1]=value3;
                from_where[i+1][j+1]=3;
                mem_len[i+1][j+1]=mem_len[i][j];
            }
            double dummyvalue1=value1/4.0+mem_len[i+1][j];
            double dummyvalue2=value2/4.0+mem_len[i][j+1];
            double dummyvalue3=value3/4.0+mem_len[i][j];
            if(dummyvalue1>=dummyvalue2&&dummyvalue1>dummyvalue3&&submission1[i]!=(j>0?submission2[j-1]:0)){
                mem[i+1][j+1]=value1;
                from_where[i+1][j+1]=1;
                mem_len[i+1][j+1]=mem_len[i+1][j];
            }
            else if(dummyvalue2>dummyvalue3&&dummyvalue2>=dummyvalue1&&submission2[j]!=(i>0?submission1[i-1]:0)){
                mem[i+1][j+1]=value2;
                from_where[i+1][j+1]=2;
                mem_len[i+1][j+1]=mem_len[i][j+1];
            }
            if(submission1[i]==submission2[j]){
                mem_len[i+1][j+1]++;
            }
            if(mem_len[i+1][j+1]+mem[i+1][j+1]/4<=0){
                mem[i+1][j+1]=0;
                from_where[i+1][j+1]=0;
                mem_len[i+1][j+1]=1;
            }

        }
    }
    int maxval=0;
    int max_i=0;
    int max_j=0;
    for(int i=0;i<submission1.size();i++){
        for(int j=0;j<submission2.size();j++){
            if(maxval<mem_len[i+1][j+1]+mem[i+1][j+1]/4) {
                maxval=mem_len[i+1][j+1]+mem[i+1][j+1]/4;
                max_i=i+1;
                max_j=j+1;  
            }
        }
    }
    // for(int i=0;i<=submission1.size();i++){
    //     for(int j=0;j<=submission2.size();j++){
    //         std::cout<<mem_len[i][j]<<","<<mem[i][j]<<"  ";
    //     }
    //     std::cout<<std::endl;
    // }


    
    while(from_where[max_i][max_j]!=0){
        if(from_where[max_i][max_j]==1) max_j--;
        else if(from_where[max_i][max_j]==2) max_i--;
        else{
            max_i--;
            max_j--;
        }
    }
    m=max_i;
    n=max_j;
    return maxval;

}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
    std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    result[1]=val1(submission1,submission2);
    int m=0;
    int n=0;
    result[2]=longestsequence(submission1,submission2,m,n);
    result[3]=m;
    result[4]=n;
    if(submission1.size()>submission2.size()) m=submission2.size();
    else m=submission1.size();
    
    if (result[2]>m) {
        result[2]=m;
    }

    if(result[1]>1*m/2 || result[2]>7*m/10) result[0]=1;
    // auto [longestMatchLength, startIdx1, startIdx2] = findLongestApproximateMatch(submission1, submission2);
    // result[2]=longestMatchLength;
    // result[3]=startIdx1;
    // result[4]=startIdx2;
    // int values[3];
    // longest_match(submission1,submission2,values);
    // result[2]=values[0];
    // result[3]=values[1];
    // result[4]=values[2];

    return result; // dummy return

    // End TODO
}