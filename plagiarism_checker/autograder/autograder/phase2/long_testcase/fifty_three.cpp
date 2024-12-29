#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include<unordered_map>
#include<queue>
#include<algorithm>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
// Constants for hash calculation
const int base = 31;  // Base for polynomial hashing
const int mod = 1e9 + 7;  // Modulus to avoid overflow in hash calculations

// Hash function for a window of data in the vector
int hash(const std::vector<int> &data, int start, int window_size) {int64_t hash = 0;
    for (int i = 0; i < window_size; ++i) {
        hash = (hash * base + data[start + i]) % mod;
    }
    return hash;
}

// Function to calculate the next hash by sliding the window
int hash_nxt(const std::vector<int> &data, int index, int prev_hash){
    int window_size = 10;
    if (index + window_size >= data.size()) {
        std::cout<<index<<" "<<window_size<<" "<<data.size()<<std::endl;
        throw std::out_of_range("Index and window size exceed data bounds.");
    }
    long long int new_hash = (prev_hash - data[index] * (1LL * static_cast<int64_t>(std::pow(base, window_size - 1))) % mod + mod) % mod;
    new_hash = (new_hash * base + data[index + window_size]) % mod;
    return new_hash;
}

// Main function to check for similarity between two submissions
std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0}; // Result array to hold output values
    std::unordered_map<int,std::vector<int>> hash_values;  // Hash map to store hashes of `submission2`
    std::unordered_map<int,std::vector<int>> indices; // Stores indices of matching hashes

    // Precompute hashes for `submission2`
    int hash_value=hash(submission2,0,10);
    for(int b=0;b+10<submission2.size();b++){
        //hash_value=hash(submission2,i,10);
        hash_values[hash_value].push_back(b);
        hash_value=hash_nxt(submission2,b,hash_value);
    }
    hash_values[hash_value].push_back(submission2.size()-10);
    int i=0;
    int val=hash(submission1,0,10); // Compute initial hash for `submission1`

    // Matching hashes in `submission1` to `submission2`
    while(i+9<submission1.size()){
        if(hash_values[val].size()==0){
            if(i+9<submission1.size())val=hash_nxt(submission1,i,val);
            i++;
            continue;
        }
        else{
            indices[i]=hash_values[val];
            result[1]+=10; // Increase matching score
            i+=10;
            if(i<submission1.size()-9)val=hash(submission1,i,10);
        }
    }

    // Find longest approximate matching pattern between both submissions
    int k=std::min(submission1.size(),submission2.size());
    int p=0,q=0;
    bool found=false;
    while(!found && k>=30){
        while(p+k<=submission1.size()){
            std::vector<int> vec1(submission1.begin()+p,submission1.begin()+p+k);
            std::vector<int> vec2(submission2.begin()+q,submission2.begin()+q+k);
        int hash_value2=hash(vec2,0,10);
        std::vector<int> v2;
        for(int l=0;l<vec2.size()-10;l++){
        v2.push_back(hash_value2);
        hash_value2=hash_nxt(vec2,l,hash_value2);
    }
    v2.push_back(hash_value2);
    int hash_value1=hash(vec1,0,10);
    std::vector<int> v1;
    for(int l=0;l<vec1.size()-10;l++){
        v1.push_back(hash_value1);
        hash_value1=hash_nxt(vec1,l,hash_value1);
    }
    v1.push_back(hash_value1);
    std::vector<int> v11,v22;

    for(int l=0;l+9<v1.size();l++){
        auto min_it1 = std::min_element(vec1.begin() + l, vec1.begin() + l + 10);
        auto min_it2 = std::min_element(vec2.begin() + l, vec2.begin() + l + 10);
        v11.push_back(*min_it1);
    v22.push_back(*min_it2);

    }
    
    int count=0;
    sort(v11.begin(),v11.end());
    sort(v22.begin(),v22.end());

    // Count matching elements in the two vectors
    for(int l=0;l<v11.size();l++){
        auto it = std::find(v22.begin(), v22.end(), v11[l]);

    if (it != v22.end()) {
        int index = std::distance(v22.begin(), it);
        count++;
        v22.erase(v22.begin()+index);
    }

    }
    // Check if similarity threshold is met
    if((double(count)/double(2*v11.size()-count))>=0.999){
        found=true;
        break;
    }
    else{
        if(q+20+k>submission2.size()){
            q=0;p=p+20;
        }
        else q=q+20;
    }
        }
        if(!found){
            k=k-30; // Decrease pattern length if not found
        p=0;
        q=0;

        }
        
    }

    // Update results if pattern length found
    if(k>=30){
        result[2]=k;
    result[3]=p;
    result[4]=q;
    }
    else{
        result[2]=0;
    result[3]=-1;
    result[4]=-1;
    }

    // Calculate plagiarism score and update result[0]
    int min_size=std::min(submission1.size(),submission2.size());
    double plag=(double(result[1])/double(min_size));
    if(min_size > 1000 && plag >= 0.5) result[0] = 1;
    else if(min_size > 500 && plag >= 0.6) result[0] = 1;
    else if(min_size > 300 && plag >= 0.7) result[0] = 1;
    else if(min_size < 300 && plag >= 0.8) result[0] = 1;
    else result[0]=0;
    return result;
    // End TODO
}