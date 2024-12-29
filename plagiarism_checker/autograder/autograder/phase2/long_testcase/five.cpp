#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <math.h> 
#include <unordered_set>
#include <unordered_map>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

const int BASE = 257; 
const int MOD = 1e9 + 7;

//Fuzzy algorithm
void fuzzy_approximate_match(std::array<int, 5>& result_accurate, std::vector<int>& submission1, std::vector<int>& submission2, int max_mismatches) {
    int n = submission1.size();
    int m = submission2.size();
    int longest_fuzzy_match = 0; // Track the longest fuzzy match
    int index_i = -1;
    int index_j = -1;

    // Iterate through both vectors
   for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int curr_match_length = 0;
            int mismatches = 0;

            // Check for matches starting from indices i and j
            int x = i, y = j;

            while (x < n && y < m) {
                if (submission1[x] != submission2[y]) {
                    mismatches++;
                }

                // Check mismatch rate condition
                if (curr_match_length > 30) {

                    int mismatch_rate = (mismatches * 100) / double(curr_match_length+mismatches);

                    if (mismatch_rate > max_mismatches) {
                        // Allow matching for an extra threshold length
                        int extend_length = 30; // Change this value based on your tolerance
                        
                        bool improved = false;
                        
                        int last_mismatches = mismatches;
                        
                        int last_match_length = curr_match_length;
                        
                        for (int k = 0; k < extend_length && x + k < n && y + k < m; ++k) {
                            if (submission1[x + k] != submission2[y + k]) {
                                last_mismatches++;
                            }
                            else{last_match_length++;}

                            if (((last_mismatches * 100) / double(last_match_length+mismatches )) <= max_mismatches) {

                                curr_match_length = last_match_length;
                                mismatches = last_mismatches;
                                
                                x += k;
                                y += k;
                            
                                improved = true;
                            
                                break;
                           
                            
                            }
                        
                        }


                        if (!improved) {
                            break; // Break if no improvement within the allowed extension
                        }
                    }
                }

                if(submission1[x] == submission2[y])  curr_match_length++ ;
                x++;
                y++;
            }

            int mismatch_rate = (mismatches * 100) / double(curr_match_length+mismatches);

            if (curr_match_length + mismatches > longest_fuzzy_match && mismatch_rate <= max_mismatches) {
                    longest_fuzzy_match = curr_match_length + mismatches;
                    index_i = i;
                    index_j = j;
                }
        }
    }
    // Store results
    result_accurate[2] = longest_fuzzy_match;
    result_accurate[3] = index_i;
    result_accurate[4] = index_j;

}

void fuzzy_approx (std::array<int, 5>& result_accurate, std::vector<int>& submission1, std::vector<int>& submission2, int max_mismatches) {
    int n = submission1.size();
    int m = submission2.size();
    int longest_fuzzy_match = 0; // Track the longest fuzzy match
    int index_i = -1;
    int index_j = -1;


   for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int mismatches = 0;
            int matches = 0 ; 
            // Check for matches starting from indices i and j
            int x = i, y = j;

            while (x < n && y < m) {

                if (submission1[x] != submission2[y]) {
                    mismatches++;
                    y++ ;
                }
                else { 
                    x++ ;
                    y++ ;
                    matches++ ;
                }

                if ( matches > 25 ) { 
                    int total_len = matches + mismatches ;
                    double perc_err = mismatches*100/double(total_len) ;
                    if (  perc_err <= max_mismatches ) { 

                        if ( total_len > longest_fuzzy_match ) { 

                            longest_fuzzy_match = total_len ;
                            index_i = i;
                            index_j = j;

                        }

                    }
                
                }
            }
        }
   }




    result_accurate[2] = longest_fuzzy_match;
    result_accurate[3] = index_i;
    result_accurate[4] = index_j;



    m = submission1.size();
    n = submission2.size();
    index_i = -1;
    index_j = -1;
    longest_fuzzy_match = 0 ;

   for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int mismatches = 0;
            int matches = 0 ; 
            // Check for matches starting from indices i and j
            int x = i, y = j;

            while (x < n && y < m) {

                if (submission2[x] != submission1[y]) {
                    mismatches++;
                    y++ ;
                }
                else { 
                    x++ ;
                    y++ ;
                    matches++ ;
                }

                if ( matches > 25 ) { 
                    int total_len = matches + mismatches ;
                    double perc_err = mismatches*100/double(total_len) ;
                    if (  perc_err <= max_mismatches ) { 

                        if ( total_len > longest_fuzzy_match ) { 

                            longest_fuzzy_match = total_len ;
                            index_i = i;
                            index_j = j;

                        }

                    }
                
                }
            }
        }
   }


    if ( longest_fuzzy_match > result_accurate[2]) { 
    result_accurate[2] = longest_fuzzy_match;
    result_accurate[3] = index_j;
    result_accurate[4] = index_i;

    }


}
//Tried for approximate matches using winnowing and rolling hash
/*
int hashWindow(const std::vector<int>& vec, int start, int windowSize) {
    int hash = 0;
    for (int i = start; i < start + windowSize; ++i) {
        hash = (1LL * hash * 31 + vec[i]) % MOD;
    }
    return hash;
}

std::unordered_set<int> winnowing(const std::vector<int>& vec, int windowSize) {
    std::unordered_set<int> fingerprints;
    int n = vec.size();
    
    // Loop over the vector with a sliding window
    for (int i = 0; i <= n - windowSize; ++i) {
        int hashValue = hashWindow(vec, i, windowSize);
        fingerprints.insert(hashValue); // Store the minimum hash in each window
    }
    return fingerprints;
}

bool longestApproximateMatch(const std::vector<int>& vec1, const std::vector<int>& vec2, int windowSize, int& matchLength) {
    std::unordered_set<int> fingerprints1 = winnowing(vec1, windowSize);
    std::unordered_set<int> fingerprints2 = winnowing(vec2, windowSize);

    // Find the common fingerprints
    std::vector<int> commonFingerprints;
    for (const int& fp : fingerprints1) {
        if (fingerprints2.find(fp) != fingerprints2.end()) {
            commonFingerprints.push_back(fp);
        }
    }
    
    matchLength = commonFingerprints.size();
    return matchLength;
}

*/



void update( int&curr_match  , int&total_length_match, int&min_length , int&i , int &j , std::unordered_map<int , int>&match_length , std::vector<bool>&match_at_index_i , std::vector<bool>&match_at_index_j)  {

    if ( curr_match >= min_length) { 

        if ( match_length[i] < curr_match ) { 

            int increment = curr_match - match_length[i] ;
            
            total_length_match +=  increment ;
            
            match_length[i] = curr_match ;

            // std::cout << " match found at " << i << " " << j << " of length: " << curr_match << " " << increment << std::endl ;

            for ( int idx = i ; idx < i + curr_match ; idx++ ) { 
                match_at_index_i[idx] = true ;
            }

            for ( int idx = j ; idx < j + curr_match ; idx++ ) { 
                match_at_index_j[idx] = true ;
            }
        }
    }

    curr_match = 0 ;

}




void match ( std::vector<int> &submission1 , std::vector<int> &submission2 , std::vector<bool> &match_at_index_i , std::vector<bool> &match_at_index_j , std::unordered_map<int , int> &match_length_at_i , int &min_length , int &total_match_length ) { 


    for ( int i  = 0 ; i< submission1.size() ; i++) { 
        
        if ( match_at_index_i[i] ) continue ;

        for ( int j = 0 ; j < submission2.size() ; j++ ) { 
            
            if ( match_at_index_j[j] ) continue ;

            //current running match's length
            int curr_match = 0 ; 

            int M = std::min(submission1.size() - i , submission2.size() - j) ;
            for ( int l = 0 ; l < std::min(M ,20) ; l++ ) { 
                
                if ( submission1[i+l] == submission2[j+l] ) { 
                    curr_match++ ;

                    if ( l == std::min(M ,20) - 1 ) { 
                        update(curr_match , total_match_length, min_length , i , j , match_length_at_i , match_at_index_i , match_at_index_j)  ;
                        j = j + l ;
                        break ;
                    }

                }
                
                else { 
                        update(curr_match , total_match_length, min_length , i , j , match_length_at_i , match_at_index_i , match_at_index_j)  ;
                    j = j + l ;
                    break ;                  
                }

            }


        }

    }   


}









void accurate_matching( std::array<int,5> &result, std::vector<int> &submission1 , std::vector<int> &submission2  ) { 

    int min_length = 10 ; 
    int total_length_match = 0 ; 
   
    std::vector<bool> match_at_index_i(submission1.size() , false) ;
    std::vector<bool> match_at_index_j(submission2.size() , false) ;

    std::unordered_map<int , int> match_length_at_i ;

    match( submission1 , submission2 , match_at_index_i , match_at_index_j , match_length_at_i , min_length , total_length_match ) ;

    
    result[1] = total_length_match ;

    return ;
}










// Calling main function
std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};





    // /*----------------------------uncomment for debugging----------------------
    
    //  for ( int i = 0 ; i < submission1.size() ; i++) { Counting objects: 100% (7/7), done.
    //      std::cout << i << " "<< submission1[i]  << std::endl; 
    //  }

    //  for ( int i = 0 ; i < submission2.size() ; i++) { 
    //      std::cout << i << " "<< submission2[i]  << std::endl; 
    //  }


    // ------------------------------------------------------------------------*/




    //Accurate matching
    accurate_matching( result , submission1 , submission2 ) ;




    /* Aditi and aakash please write the case for longer strings with 80% matchings here ie fuzzy matching and stuff ;)*/
    fuzzy_approx(result,submission1,submission2,20);
    
    int total=std::min(submission1.size(),submission2.size());
    if ( (result[1] > 0.5*total &&  result[2] > 0.3*total ) || result[1] > 0.7*total || result[2] > 200 )   { 
        result[0] = 1 ; 
    }

    // if(longestMatch>=threshold) result_accurate[0]=1;
    // if(longestMatch>=result_accurate[2]) result_accurate[2]=longestMatch;
    // else result_accurate[2]=longest_match;
    // result_accurate[2]=longestMatch;
    // std::cout << result_accurate[2] << std::endl ;
    // std::cout << longestMatch << std::endl ;
    // std::cout << submission1.size() << std::endl ;
    // std::cout << submission2.size() << std::endl ;




    return result;
    // End TODO

}