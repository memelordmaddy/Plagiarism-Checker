#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include<map>
#include<set>
#include<utility>
#include<algorithm>
#include<unordered_map>
// Function to compute the length of the longest common subsequence (LCS) and to obtain their positions of the respective subseqeunce(max)
std::pair<int, std::vector<std::pair<int, int>>> longestCommonSubsequenceWithPositions(const std::vector<int>& submission1, const std::vector<int>& submission2) {
    int size_1=submission1.size();
    int size_2=submission2.size();
    std::vector<std::vector<int>> dp(size_1 + 1, std::vector<int>(size_2 + 1, 0));
    for (int i=1;i<=size_1;i++){
        for (int j=1;j<=size_2;j++){
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
    int i=size_1;
    int j=size_2;
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
    return {dp[size_1][size_2],lcs_positions};
}
// Function to find the maximum matching pattern length from a given position (i) in submission1. Also updates the positions in submission2 such that the matched things are not recounted twice.
//std::map<int,std::set<int>>& submission2_positions , used set to track positions , since O(logn) deletion and O(logn) search for set.
int find_max_string_from(std::map<int,std::set<int>>& submission2_positions,std::vector<int>& submission1,int i,int size){
    int count=0;
    std::vector<int> remover;
    for(int index_in_2:submission2_positions[submission1[i]]){
        std::vector<int> tester;
        int rotor=1;
        tester.push_back(index_in_2);
//iterating over the next indexes and checking whether the index needed is present in the next numbers positions set in submission2
        while(i+rotor<submission1.size() && index_in_2+rotor<size && submission2_positions[submission1[i+rotor]].count(index_in_2+rotor)){
            tester.push_back(index_in_2+rotor);
            rotor=rotor+1;
        }
        if(rotor>count){
            remover=tester;
            count=rotor;
        }
    }
    //erasing already matched positions .to get no overlap
    if(count>=10){
        for(int j=0;j<remover.size();j++){
            submission2_positions[submission1[i+j]].erase(remover[j]);
        }
    }
    else{
        return 1;
    }
    return count;
}
// Main function to compare two submissions and calculate various metrics related to similarity.
std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    //map for storing all the positions of a particular integer in the given submission2.map a{integer}->{positions}
    std::map<int,std::set<int>>submission2_positions;
    int submission_2_size=submission2.size();
    int plag=0;
    for(int i=0;i<submission2.size();i++){ // Storing positions of each element in submission2
        submission2_positions[submission2[i]].insert(i);
    }
    int sum=0;
    int start=0;
    for(int i=0;i<submission1.size();i++){
        int k=find_max_string_from(submission2_positions,submission1,i,submission_2_size);
        if(k>1){
        i=i+k-1;
        sum=sum+k;}
    }
    int size1=submission1.size(); // Determining plagiarism threshold based on matching content ratio
    int size2=submission2.size();
    // used method determining plag 
    //(n1/n1+n2)(x/n2)+(n2/n1+n2)(x/n1)) , where n1,n2 are sizes and x is  the sum attained (more the n_i , more the chance of overlap with the other even there is no plag, )
    //so tried to clear it out adding their ratios in front making it more robust .
    if ((static_cast<double>(size1) / (size1 + size2)) * (static_cast<double>(sum) / size2) + (static_cast<double>(size2) / (size1 + size2)) * (static_cast<double>(sum) / size1) > 0.25) {
        plag = 1;
    }
    auto result = longestCommonSubsequenceWithPositions(submission1,submission2);// Obtaining the LCS length and positions of matched elements
    int length = result.first;
    std::vector<std::pair<int, int>> positions = result.second;
    //identifying the pattern , which starts with integer which is matching and ends with a integer which is matching , and which holds 80% matches elements 
    //from the other pattern.
    int max_len=0;
    int position_1=0;
    int position_2=0;
    for(int i=0;i<positions.size();i++){
        for(int j=i+23;j<positions.size();j++){
            if(j-i+1>(double)(0.8*(positions[j].first-positions[i].first+1))){
                if(j-i+1>(double)(0.8*(positions[j].second-positions[i].second+1))){
                    if(std::max(positions[j].first-positions[i].first+1,positions[j].second-positions[i].second+1)>max_len){
                        //updating the start positions whenever new max is found
                        position_1=positions[i].first;
                        position_2=positions[i].second;
                        max_len=std::max({positions[j].first-positions[i].first+1,positions[j].second-positions[i].second+1});
                    }
                }
            }
        }
    }
    // The function returns an array with 5 values: plagiarism flag, sum of matched subsequences,maximum length of contiguous match, and the starting positions of the longest matching subsequence in both submissions
    std::array<int, 5> result_2= {plag,sum,max_len,position_1,position_2};
    // std::cerr << "Result: [";
    // for (size_t i = 0; i < result_2.size(); ++i) {
    //     std::cerr << result_2[i];
    //     if (i < result_2.size() - 1) std::cerr << ", ";
    // }
    // std::cerr << "]" << std::endl;
    return result_2; 
}
