#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_set>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here


// Function to calculate the hash value af a string
long long rollinghash(const std::span<int> &P,int m){
    int B=37;
    long long Q=1000000007; 
    long long hash=0;
    for(int i=0;i<m;i++){
        hash=(hash*B+P[i])%Q;
    }
    return hash;
}

// Function to calculate the hash value of the next substring using the hash of previous substring
long long next_hash(const int &excluded,const int &included,int m,long long &prev_hash){
    int B=37;
    long long Q=1000000007;
    long long B_m = 1;
    for (int i = 0; i < m; ++i) {
        B_m = (B_m * B) % Q;
    }
    long long hash = (B * prev_hash + included - (B_m * excluded) % Q + Q) % Q;

    return hash;
}

// Function to find minimum element in a span
long long min_element(const std::span<long long> &w_span) {
    long long min_value = w_span[0];

    for (const auto& value : w_span) {
        if (value < min_value) {
            min_value = value;
        }
    }
    return min_value;
}

// Function to decrease the size fingerprint of a sequence using winnowing for approximate match
std::vector<long long> winnowing(const std::span<long long> &kgrams, int w) {
    std::vector<long long> fingerprint;
    int n = kgrams.size();

    for (int i = 0; i <= n - w; i++) {
        fingerprint.push_back(min_element(kgrams.subspan(i, w)));
    }

    return fingerprint;
}

// Function to calculate the matched percentage b/w two fingerprints
double percentage_matched(const std::vector<long long> &fingerprint1, const std::vector<long long> &fingerprint2){
    std::unordered_set<long long> set1(fingerprint1.begin(), fingerprint1.end());
    std::unordered_set<long long> set2(fingerprint2.begin(), fingerprint2.end());

    int common = 0;
    for (const auto& hash : set1) {
        if (set2.count(hash)) ++common;
    }

    return (static_cast<double>(common) *100 *2) / (set1.size()+set2.size());
}

// Helper function to sort the vector of pair of ints on the basis of second element
void sortPairsBySecond(std::vector<std::pair<int, int>>& matches) {
    std::sort(matches.begin(), matches.end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second; // Sort by the second element
        });
}

// Function to merge all adjacent patterns in matches1 and matches2
void mergeMatchedList(std::vector<std::pair<int, int>>& matches1, std::vector<std::pair<int, int>>& matches2) {
    std::vector<bool> visited(matches1.size(), false);
    std::vector<std::pair<int, int>> m1, m2;

    for (size_t i = 0; i < matches1.size(); ++i) {
        if (visited[i]) continue; // Skip already visited patterns

        visited[i] = true;
        int start1 = matches1[i].first;
        int end1 = start1 + matches1[i].second - 1;
        int start2 = matches2[i].first;
        int end2 = start2 + matches2[i].second - 1;

        // Check for adjacent patterns
        for (size_t j = i + 1; j < matches1.size(); ++j) {
            if (visited[j]) continue; // Skip already visited patterns

            if (end1 + 1 == matches1[j].first && end2 + 1 == matches2[j].first) {
                // Merge adjacent patterns
                end1 = matches1[j].first + matches1[j].second - 1;
                end2 = matches2[j].first + matches2[j].second - 1;
                visited[j] = true; // Mark as visited
            } 
            else if (start1 - 1 == matches1[j].first + matches1[j].second && start2 - 1 == matches2[j].first + matches2[j].second) {
                // Merge adjacent patterns to the left
                start1 = matches1[j].first;
                start2 = matches2[j].first;
                visited[j] = true; // Mark as visited
            }
        }
        
        // Add the merged pattern to the result lists
        m1.emplace_back(start1, end1 - start1 + 1);
        m2.emplace_back(start2, end2 - start2 + 1);
    }

    sortPairsBySecond(m1);
    sortPairsBySecond(m2);

    // Replace the original matches with the merged results
    matches1 = std::move(m1);
    matches2 = std::move(m2);
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};    

    int l1=submission1.size();
    int l2=submission2.size();
        
    // To store accurate matches
    std::vector<std::pair<int,int>> matches1,matches2;   

    int m=22;

    while(--m>=10){

        long long P_hash;
        bool skipped1=false;

        // Loop for pattern matching
        for(int i=0;i<=l1-m;i++){
            
            // Checking whether the pattern is already matched or not to avoid multiple matching of patterns
            bool already_matched1=false;
            for(const auto &matched : matches1){
                if(!(i+m-1<matched.first || i>matched.first+matched.second-1)) {
                    i = matched.first+matched.second-1;
                    already_matched1=true;
                    skipped1=true;
                    break;
                }
            }
            if(already_matched1)  continue;
            

            // Calculate the hash of the substring that we want to search using rolling hash method
            if(i==0) P_hash=rollinghash(std::span<int>(submission1).subspan(i,m),m);
            else if(!skipped1) P_hash=next_hash(submission1[i-1],submission1[i+m-1],m,P_hash);
            else {
                P_hash=rollinghash(std::span<int>(submission1).subspan(i,m),m);
                skipped1=false;
            }

            long long hash;
            bool skipped2=false;

            for(int j=0;j<=l2-m;j++){

                // Checking whether the pattern is already matched or not to avoid multiple matching of patterns
                bool already_matched2=false;
                for(const auto &matched : matches2){
                if(!(j+m-1<matched.first || j>matched.first+matched.second-1)) {
                            j = matched.first+matched.second-1;
                            already_matched2=true;
                            skipped2=true;
                            break;
                        }
                }
                    
                if(already_matched2) continue;

                // Calculate the hash of the substring using rolling hash method 
                if(j==0) {hash=rollinghash(std::span<int>(submission2).subspan(0,m),m);}
                else if(!skipped2) hash=next_hash(submission2[j-1],submission2[j+m-1],m,hash);
                else {
                    hash=rollinghash(std::span<int>(submission2).subspan(j,m),m);
                    skipped2=false;
                }

                // Stores the value of start index and length of substring that are accurately matched in a vector
                if(P_hash==hash) {  
                    matches1.push_back(std::make_pair(i,m));
                    matches2.push_back(std::make_pair(j,m));
                    break;                  
                }

            }
        }
    }

    // Merge adjacent accurate matches
    mergeMatchedList(matches1,matches2);

    for (const auto &p : matches1) {
        result[1] += p.second;
    }
    

    std::vector<long long> kgram1, kgram2;
    int k = 30;  // Length of k-grams

    // Compute initial hash for k-grams 
    if(std::min(l1,l2)>30){
        kgram1.push_back(rollinghash(std::span<int>(submission1).subspan(0, k), k));
        kgram2.push_back(rollinghash(std::span<int>(submission2).subspan(0, k), k));
    

        // Compute rolling hash for all k-grams in both submissions
        for (int i = 1; i <= l1 - k; i++) {
            kgram1.push_back(next_hash(submission1[i - 1], submission1[i + k - 1], k, kgram1[i - 1]));
        }
        for (int i = 1; i <= l2 - k; i++) {
            kgram2.push_back(next_hash(submission2[i - 1], submission2[i + k - 1], k, kgram2[i - 1]));
        }
    }

    // Compute p-grams and l-grams using different gram sizes (p = 7, l = 13) to increase granularity
    std::vector<long long> pgram1, pgram2, lgram1, lgram2;
    int p = 7;
    int l = 13;

    // Generate p-grams for both submissions
    pgram1.push_back(rollinghash(std::span<int>(submission1).subspan(0, p), p));
    pgram2.push_back(rollinghash(std::span<int>(submission2).subspan(0, p), p));

    for (int i = 1; i <= l1 - p; i++) {
        pgram1.push_back(next_hash(submission1[i - 1], submission1[i + p - 1], p, pgram1[i - 1]));
    }
    for (int i = 1; i <= l2 - p; i++) {
        pgram2.push_back(next_hash(submission2[i - 1], submission2[i + p - 1], p, pgram2[i - 1]));
    }

    // Generate l-grams for both submissions
    lgram1.push_back(rollinghash(std::span<int>(submission1).subspan(0, l), l));
    lgram2.push_back(rollinghash(std::span<int>(submission2).subspan(0, l), l));

    for (int i = 1; i <= l1 - l; i++) {
        lgram1.push_back(next_hash(submission1[i - 1], submission1[i + l - 1], l, lgram1[i - 1]));
    }
    for (int i = 1; i <= l2 - l; i++) {
        lgram2.push_back(next_hash(submission2[i - 1], submission2[i + l - 1], l, lgram2[i - 1]));
    }

    // Binary search to find the largest value of m (sequence length) with a match over the threshold
    int low=30;
    if( !matches1.empty() && matches1[0].second>30 ) low=matches1[0].second;
    int high = l1;
    int best_m = 0;
    int start1 = -1, start2 = -1;
    int step = 10;
    if (std::min(l1, l2) < 200) step = 5;

    while (low <= high) {
        int mid = (low + high) / 2;
        bool found = false;
        double max_percentage = 0;

        // Use different criteria based on length of sequence
        if (mid > 160) {
            // Large m, so use k-grams for fingerprinting with window size (k + 1)
            for (int i = 0; i <= l1 - mid; i += step) {
                std::vector<long long> fingerprint1 = winnowing(std::span<long long>(kgram1).subspan(i, mid - k + 1), k + 1);

                for (int j = 0; j <= l2 - mid; j += step) {
                    std::vector<long long> fingerprint2 = winnowing(std::span<long long>(kgram2).subspan(j, mid - k + 1), k + 1);

                    double percentage = percentage_matched(fingerprint1, fingerprint2);
                    if (percentage >= 91.5 && percentage > max_percentage) {
                        max_percentage = percentage;
                        start1 = i;
                        start2 = j;
                        best_m = mid;
                        found = true;
                    }
                }
            }
        } else if (mid > 100) {
            // Medium m, use smaller winnowing window size (1)
            for (int i = 0; i <= l1 - mid; i += step) {
                std::vector<long long> fingerprint1 = winnowing(std::span<long long>(kgram1).subspan(i, mid - k + 1), 1);

                for (int j = 0; j <= l2 - mid; j += step) {
                    std::vector<long long> fingerprint2 = winnowing(std::span<long long>(kgram2).subspan(j, mid - k + 1), 1);

                    double percentage = percentage_matched(fingerprint1, fingerprint2);
                    if (percentage >= 84 && percentage > max_percentage) {
                        max_percentage = percentage;
                        start1 = i;
                        start2 = j;
                        best_m = mid;
                        found = true;
                    }
                }
            }
        } else if (mid < 50) {
            // Small m, use p-grams
            for (int i = 0; i <= l1 - mid; i += step) {
                std::vector<long long> fingerprint1 = winnowing(std::span<long long>(pgram1).subspan(i, mid - p + 1), 1);

                for (int j = 0; j <= l2 - mid; j += step) {
                    std::vector<long long> fingerprint2 = winnowing(std::span<long long>(pgram2).subspan(j, mid - p + 1), 1);

                    double percentage = percentage_matched(fingerprint1, fingerprint2);
                    if (percentage >= 75 && percentage > max_percentage) {
                        max_percentage = percentage;
                        start1 = i;
                        start2 = j;
                        best_m = mid;
                        found = true;
                    }
                }
            }
        } else {
            // Other cases, use l-grams
            for (int i = 0; i <= l1 - mid; i += step) {
                std::vector<long long> fingerprint1 = winnowing(std::span<long long>(lgram1).subspan(i, mid - l + 1), 1);

                for (int j = 0; j <= l2 - mid; j += step) {
                    std::vector<long long> fingerprint2 = winnowing(std::span<long long>(lgram2).subspan(j, mid - l + 1), 1);

                    double percentage = percentage_matched(fingerprint1, fingerprint2);
                    if (percentage >= 79.5 && percentage > max_percentage) {
                        max_percentage = percentage;
                        start1 = i;
                        start2 = j;
                        best_m = mid;
                        found = true;
                    }
                }
            }
        }
        
        // Adjust binary search based on whether a match was found
        if (found) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }


    // Store results and calculate percentage match
    result[2] = best_m;
    result[3] = start1;
    result[4] = start2;

    int min_length = std::min(l1, l2);
    float percentage = float(result[1] * 100) / min_length;

    // Thresholds based on sequence length
    if (min_length<50){
        result[0] = percentage > 90 ? 1 : 0;
    } else if (min_length < 100) {
        result[0] = percentage > 85 ? 1 : 0;
    } else if (min_length < 140) {
        result[0] = percentage > 80 ? 1 : 0;
    } else if (min_length < 200) {
        result[0] = percentage > 75 ? 1 : 0;
    } else if (min_length < 300) {
        result[0] = percentage > 68 ? 1 : 0;
    } else if (min_length < 400) {
        result[0] = percentage > 65 ? 1 : 0;
    } else if (min_length < 700) {
        result[0] = percentage > 60 ? 1 : 0;
    } else if (min_length < 1200) {
        result[0] = percentage > 50 ? 1 : 0;
    } else {
        result[0] = percentage > 46 ? 1 : 0;
    }
    
    return result; 
    // End TODO
}
