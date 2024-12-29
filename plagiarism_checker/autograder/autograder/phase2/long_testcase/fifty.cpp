#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

#include <unordered_set>
#include <algorithm>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// -----------------------------------------------------------------------------
// OPTIONAL: Add your helper functions and data structures here

// ---------------------------------------------------------------------
std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    // std::array<int, 5> result = {0, 0, 0, 0, 0};

//-----------------------------------------------------------------------

struct Vector3 {
    int x, y, z,l,m;

    // Constructor for convenience
    Vector3(int x, int y, int z,int l, int m) : x(x), y(y), z(z),l(l),m(m) {}
};


    int plagiarism_flag = 0;
    int total_exact_match_length = 0;
    int longest_approx_match_length = 0;
    int start_index_file1 = -1;
    int start_index_file2= -1;
  
    const int MIN_SHORT_LEN = 10;
    const int MAX_SHORT_LEN = 20;
    const int MIN_LONG_LEN = 30;
    const double APPROX_THRESHOLD = 0.8;
    std::unordered_set<int> matched_indices_file1, matched_indices_file2;
    

    std::vector<Vector3>  file1;
    // std:: cout<< submission1.size() << " "<< submission2.size() << std::endl;
    

    // Step 1: Exact match detection (10-20 tokens)
    for (int len = MAX_SHORT_LEN; len >= MIN_SHORT_LEN; --len) {
        for (size_t i = 0; i + len <= submission1.size(); ++i) {
            bool unmatched1 =true;
            for(size_t x1 =i; x1 < i+ len; x1++ ) {
                if (matched_indices_file1.count(x1)) {unmatched1 = false; break;}
            }
            if(unmatched1) {
                
                std::vector<int> pattern1(submission1.begin() + i, submission1.begin() + i + len);
                for (size_t j = 0; j + len <= submission2.size(); ++j) {
                    bool unmatched2 = true;
                    for(size_t x2 = j; x2 < j + len; x2++ ) {
                        if (matched_indices_file2.count(x2)) { unmatched2 = false; break;}
                    }
                    if ( unmatched2) {
                         std::vector<int> pattern2(submission2.begin() + j, submission2.begin() + j + len);
                        if (pattern1 == pattern2) {
                            total_exact_match_length += len;
                            for (int k = 0; k < len; ++k) {
                                matched_indices_file1.insert(i + k);
                                matched_indices_file2.insert(j + k);
                                file1.push_back(Vector3(len, i+k, j+k, pattern1[k], pattern2[k]));
                          
                            }
                          
                          
                            
                            break;
                        }
                    }    
                   
                }
            }
            
        }
    }

// step2: condition to check whether it is flag or not
int min;

if (submission1.size() > submission2.size()) { min = submission2.size(); } 
else {  min = submission1.size(); }

// for(int i =0; i< min; i++) {
//      std::cout<< submission1[i] << " "<< submission2[i] << std::endl;
//  }
if (total_exact_match_length >= (min * 0.6) || total_exact_match_length>= 800) {  plagiarism_flag = 1; }


  // Step 2: approximate match detection

// 1.Sort the vector based on the 'y' value of each Vector3
std::sort(file1.begin(), file1.end(), [](const Vector3& a, const Vector3& b) {
    return a.y < b.y;  // Sort in ascending order of y
});


    // // Print the contents to verify
    //    for (const auto& vec : file1) {
    //    std::cout << "Vector3(" << vec.x << ", " << vec.y << ", " << vec.z <<" " <<  vec.l <<" " << vec.m<<")\n" ;
    //   }


bool found_match = false; // Flag to indicate if a match has been found

for (int len = total_exact_match_length; len >= 30 && !found_match; --len) { // Check flag in loop condition

    int appr_length1 =0;
    int appr_length2 =0;
    bool lessthan = true;
    for(int i =0; i+len <= total_exact_match_length -1 && !found_match; i++) {
        int min_value =0;
        int max_value =0;
        for(int j=i; j< i+len-1; j++) {
            if( file1[j].z > file1[j+1].z) {lessthan = false; break;}   // making sure they are in increasing order
        }
        if(lessthan) {
            appr_length1 = file1[i+len -1].y - file1[i].y;
            appr_length2 = file1[i+len -1].z - file1[i].z;

            if(  (len >= 0.8*appr_length1) && (len >= 0.8*appr_length2)) { 

                int f1 = file1[i].y; //starting index of file1
                int f2 = file1[i].z; // starting index of file2
                int check_length =0;
                int maximum ;

                if(appr_length1 < appr_length2) {maximum = appr_length2;}
                else {maximum= appr_length1;}

                for(int i1 =0; i1<maximum; i1++) {
                    if(submission1[i1+f1] == submission2[i1+f2])  { check_length++;}
                }
                if(check_length >= 0.8*(maximum) )  { // checking if the values which we got really satisfy the approximate property or not
                    found_match = true;
                    longest_approx_match_length = maximum;
                    start_index_file1= file1[i].y;
                    start_index_file2 =file1[i].z;
                }
               
           
            }

        }  

    }      
}

 return {plagiarism_flag, total_exact_match_length, longest_approx_match_length, start_index_file1, start_index_file2};
//-----------------------------------------------------------------------
    // return result;
    // End TODO
}