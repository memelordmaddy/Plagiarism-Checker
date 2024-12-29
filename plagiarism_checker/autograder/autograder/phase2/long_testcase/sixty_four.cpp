#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <deque>
#include <cassert>

std::vector<bool> visited;
// -----------------------------------------------------------------------------
// citation: used kmp algorithm from slides of course cs213
std::vector<int> computeKMPTable(const std::deque<int>& pattern) {
    // Standard KMP algorithm
    int m = pattern.size();
    std::vector<int> table(m+1);
    table[0] = -1;
    int j = 0;
    for (int i = 1; i < m; ++i,++j) {
        if (pattern[i] != pattern[j])
        {
            table[i] = j;
            while (j >= 0 && pattern[i] != pattern[j]) {
                j = table[j];
            }
        }
        else
        {
            table[i] = table[j];
        }
    }
    table[m] = j;
    return table;
}

bool kmpSearch(const std::vector<int>& text, const std::deque<int>& pattern,int & index) {
    std::vector<int> table = computeKMPTable(pattern); 
      
    int j = 0 , i = 0;
    bool flag;
    
    while (i < text.size())
    {
        if (text[i] == pattern[j]) {
            ++j;
            ++i;
            if (j == pattern.size())
            {
                flag=false;
                // Checking if the pattern is overlapping or not with an already matched pattern (using visited array)

                for(int k = i - pattern.size() ; k < i; k++)
                {
                    if(visited[k] == true)
                    {
                        j = table[j]; 
                        flag=true; 
                        break;
                    }
                }
                if(flag) continue;

                index=i;
                return 1;
            }
        }

        else
        {
            j = table[j];
            if (j < 0)
            {    
                i++,j++;
            }
        }
    }
    return 0;
}
// -----------------------------------------------------------------------------
//this fuction find the accurate matches between two submissions
//it returns the total length of the matches
//it uses kmp algorithm to find the matches
//it also uses visited array to check if the pattern is overlapping with an already matched pattern
//it also uses a deque to store the current pattern
//it also uses a minLen parameter to set the minimum length of the pattern

int find_accurate_matches(std::vector<int>& submission1, std::vector<int>& submission2, int minLen = 10) {

    int totalMatchLength = 0;
    int n = submission1.size();
    int i = 0;
   
    std::deque<int> cur_pattern;
    for (int j = 0 ; j < minLen && j < n; j ++)
    {
        cur_pattern.push_back(submission1[j]);
    }

    while(i<(n-minLen)){
        // i represents the starting index of the pattern which is checked

        for(int j = i + cur_pattern.size(); cur_pattern.size() < minLen && j < n; j++)
            cur_pattern.push_back(submission1[j]);

           
        int index;
        // i will extract the pattern from submission1 and search it in submission2 using kmp algorithm 
        //index will store the index of the pattern in submission2 if it is found so that we can mark it as visited
        if(kmpSearch(submission2,cur_pattern,index)){
           
            while(kmpSearch(submission2,cur_pattern,index) && cur_pattern.size() < 20 && i+cur_pattern.size() < submission1.size())
            { 
                cur_pattern.push_back(submission1[i+cur_pattern.size()]);
            }
         
            for(int k = index - cur_pattern.size() ; k < index && k<submission2.size(); k++)
            {
                visited[k] = true;
            }
            
            totalMatchLength += cur_pattern.size();
            i+=cur_pattern.size();
            cur_pattern.clear();

        }
        else 
        {
            i++;
            cur_pattern.pop_front();
        }
    }
        

    return totalMatchLength;
}

std::array<int,3> long_match(std::vector<int> &submission1, std::vector<int> &submission2 , double threshold = 0.8) 
{
    std::array<int,3> result;
    
    int length_variation = 10; // maximum variation in "longest length" allowed. increasing this will reduce the time taken
    
    std::vector<std::vector<int> > dp(submission1.size()+1,std::vector<int>(submission2.size()+1));
    for (int i = 0 ; i <= submission1.size() ; i++)
    {
        dp[i][0] = i;
    }
    for (int i = 0 ; i <= submission2.size() ; i++)
    {
        dp[0][i] = i;
    }
    for (int i = 1 ; i <= submission1.size() ; i++)
    {
        for (int j = 1 ; j <= submission2.size() ; j++)
        {
            dp[i][j] = std::min(dp[i-1][j-1] + (submission1[i-1] != submission2[j-1]) , 1 + std::min(dp[i-1][j],dp[i][j-1])); // edit distance between the first i characters of the first string and first j characters of the second string.
        }
    }
 
    int lower_limit = 30 , upper_limit = std::min(submission2.size(),submission1.size()) , m ;
    while ( lower_limit < upper_limit - length_variation )
    {
        bool found = false;
        m = (lower_limit + upper_limit) >> 1;
        for (int i = m ; i <= submission1.size() && !found ; i++)
        {
            for (int j = m ; j <= submission2.size() && !found; j++)
            {
                if (dp[i][j] - dp[i-m][j-m] < (1 - threshold) * m) // checks if there is a substring of length m (starting from i-m+1 for first string and j-m+1 for second string)  which has very edit distance
                {
                    found = true;
                    result[0] = m;
                    result[1] = i-m;
                    result[2] = j-m;  
                }
            }
        }
        if (found)
        {
            lower_limit = m + 1;
        }
        else
        {
            upper_limit = m - 1;
        }
    }
    return result;
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {

    std::array<int, 5> result = {0, 0, 0, -1, -1};
    visited.clear();
    visited.resize(submission2.size(),false);

    result[1] = find_accurate_matches(submission1, submission2);
        
    auto [longest_match, start_index1, start_index2] = long_match(submission1, submission2);

    result[2] = longest_match;
    result[3] = start_index1;
    result[4] = start_index2;

    result[0] = (result[1] >= 0.45 * std::min(submission1.size(),submission2.size()) || result[2] >=  0.35 * std::min(submission1.size(),submission2.size()));
    return result;
}
