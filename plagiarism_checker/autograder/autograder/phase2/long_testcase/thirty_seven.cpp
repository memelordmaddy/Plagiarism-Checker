#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>


// this file contains the implementation of the `match_submissions` function which is used to compare two sequences of tokens (integers) and report the degree of matching between them
// the function 'kmp' uses the Knuth-Morris-Pratt (KMP) algorithm to find matching sub-patterns between the two sequences and returns the starting indices of the matches
// the function 'short_match' finds the matches of short length and returns the total matched length
// the function 'long_match' finds the longest match  and starting indices of the longest match in both sequences
// the function 'match_submissions' returns an array of 5 integers which are flag to plagiarism,total_matched_length,longest_match,starting_index1 of first sequence,starting_index2 of second sequence

// -----


// this is the implementation of kmp algorithm which first creates an LPS array for the pattern P
// then it uses this array to find where P appears in T and gives the starting index in T where P occurs for the first time

std::vector<int>kmp(std::vector<int>&P,std::vector<int>&T){
    int m=P.size();   
    int n=T.size();  
    std::vector<int>found;
    std::vector<int>lps(m,0);
    int length=0;
    lps[0]=0;

    // generating the lps vector for the pattern

    for (int i=1;i<m;i++){
        if (P[i]==P[length]){
            length++;
            lps[i]=length;
            continue;
        }
        else{
            if(length!=0){
                length=lps[length-1];
                i--;
            } 
            else{
                lps[i]=0;
            }
        }
    }
    
    // we match the pattern using calculated lps values

    int i=0;  // to iterate for T
    int j=0;  // to iterate for P

    // loop to find the pattern in T 

    while(i<n){
    if(P[j]==T[i]){
            j++;
            i++;    
        if(j==m){
            found.push_back(i-j);
            j=lps[j-1];
            }
        }
        else{
            if(j!=0){
                j=lps[j-1];
            }
            else{
                i++;
            }
        }
    }
    return found;
}


// we are using the naive approach in basic way to find the matches
// this function finds the matching patterns of short lengths 10-20 between two sequences and gives the total matched length

int short_match(std::vector<int>&submission1,std::vector<int>&submission2,std::vector<int>&visited1,std::vector<int>&visited2){
    int m=submission1.size();
    int n=submission2.size();
    int total_matched_length=0;

// this is for finding matching in 10-20 length and length here refers to the pattern length
    for(int length=10; length<=20; length++){
        for(int starting_index1=0; starting_index1<=m-length; starting_index1++){

         //if they are already considered we have to skip it

            if(visited1[starting_index1]==1){
                 continue;
                 }

//first we see submission1's pattern and we need to look for it in submission2 so
  
        // creating a segment of length "length" from submission1

        std::vector<int>pattern(submission1.begin()+starting_index1,submission1.begin()+starting_index1+length);

        //we are using kmp to search in submission2

            std::vector<int>match_positions=kmp(pattern,submission2); 

//if it is not marked we use it so we check the matched segments using loop

            for(auto i:match_positions){

        // If the segment is already visited, continue

                if(visited2[i]==1){ 
                    continue;
                }

// we need to ignore if the segment is matched with the previously seen matches ie not to double count

                int double_count_check=0;
                for(int position=0;position<length;position++){
                    if(visited1[starting_index1+position]==1||visited2[i+position]==1){
                    double_count_check++;
                        break;
                    }
                }
        //if it is not visited before then we need to add it to the total length that is matched

                if(double_count_check==0){
                    total_matched_length+=length;

                // marking the visited segments

                for(int position=0;position<length;position++){
                        visited1[starting_index1+position]=1;
                        visited2[i+position]=1;
                    }

        //break statement has to be written to go to the next one in the first submission
                break; 
                }
            }
        }
    }
    return total_matched_length;
}

// this function finds the longest match of atleast 30 elements with 80% similarity between two sequences

int long_match(std::vector<int>&submission1,std::vector<int>&submission2,int& starting_index1_30,int& starting_index2_30){
    int m=submission1.size();
    int n=submission2.size();

    int longest_match=0; 
    starting_index1_30=0;
    starting_index2_30=0;   

    for(int i=0;i<=m-30;i++){
        for(int j=0;j<=n-30;j++){
            int match_count=0,length=0;

//we need to continue it till the time those are different so we use while loop here
        while(i+length<m && j+length<n){
                if(submission1[i+length]==submission2[j+length]){
                    match_count++;
                }
                length++;

//if the requirements are met ie length is >=30 and atleast 80% similar and length>length of last found longest_match,then we need to update the longest_match
    if(length>=30 && match_count>=length*0.8 && length>longest_match){
                    longest_match=length;
                    starting_index1_30=i;
                    starting_index2_30=j;
                }
            }
        }
    }
    return longest_match;
}

// this is the match submissions function

std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    
    // for this function we need to return 5 values 
    // 1 if the 2 submissions are similar then 1 (plag) if not then 0
    // 2 the length of the total matched patterns
    // 3 length of the longest match
    // 4 starting index in one
    // 5 starting index in second

     std::array<int, 5> result = {0, 0, 0, 0, 0};
    int m=submission1.size();
    int n=submission2.size();

// we created a visited array to see the matched segments are again considered or not ie not to double count
     std::vector<int>visited1(m,0);
     std::vector<int>visited2(n,0);

    // for short matches(length 10-20)
    int total_matched_length=short_match(submission1,submission2,visited1,visited2);

    // for long matches(length>=30)
    int starting_index1_30=0;
    int starting_index2_30=0;
    int longest_match=long_match(submission1,submission2,starting_index1_30,starting_index2_30);

    int shorter_length=std::min(m,n);  //to find the shortest submission length

    // flag as plagiarism based on matched percentage or longest match
    if (longest_match>=0.6*shorter_length|| total_matched_length>=0.4*shorter_length){
        result[0] = 1;
    } 
    else {
        result[0] = 0;
    }
    result[1]=total_matched_length;
    result[2]=longest_match;
    result[3]=starting_index1_30;
    result[4]=starting_index2_30;
    return result;
}

