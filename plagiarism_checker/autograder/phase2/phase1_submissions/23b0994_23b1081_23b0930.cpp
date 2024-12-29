#include <array>
#include <iostream>
#include <span>
#include <vector> 
#include <cmath>
// -----------------------------------------------------------------------------
#include <unordered_map>
#include <algorithm>
#include <queue>
# define maxx_size 500
#include <utility>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"
// OPTIONAL: Add your helper functions and data structures here



int prime1 = 10e9+7 , prime2 = 999999937; 

int power(const long long int &n , const long long int &p , const long long int &prime) // calculates (n^p)%prime
{
    if(p == 0) return 1;
    if(p == 1) return n;
    long long int x = power(n , p/2, prime);
    return ((((x*x)%prime)*power(n , p%2, prime))%prime);
}

// calculates 1/n mod prime, i.e n^(prime-2) mod prime, n^(prime-1) mod prime is 1 we know
int modulo(const long long int &n , const long long int & prime) 
{
    return power(n , prime-2 , prime);
}

// computes the rolling hashes for the vector of tokens with window size 10 
std::vector<int> compute_hashes_10(const std::vector<int> &v , const int &prime) 
{
    long long int n = v.size() , hash = 0 , p = 1 , x = modulo(maxx_size , prime);
    for(int i = 0 ; i < 10 ; i++)
    {
        hash += v[i]*p;
        hash %= prime;
        p *= maxx_size;
        p %= prime;
    }
    p *= x;
    p %= prime;
    std::vector<int> hashes(n-9);
    hashes[0] = hash;
    for(int i = 1 ; i < n-9 ; i++)
    {
        hash -= v[i-1];
        hash = (hash+prime)%prime;
        hash *= x;
        hash += v[i+9]*p;
        hash %= prime;
        hashes[i] = hash;
    }
    return hashes;
}

// computes the rolling hashes for the vector of tokens with window size 4, can use compute_hashes_5 instead of 4 as well
std::vector<int> compute_hashes_4(const std::vector<int> &v , const int &prime) 
{                                                                              
    long long int n = v.size() , hash = 0 , p = 1 , x = modulo(maxx_size , prime);
    for(int i = 0 ; i < 4 ; i++)
    {
        hash += v[i]*p;
        hash %= prime;
        p *= maxx_size;
        p %= prime;
    }
    p *= x;
    p %= prime;
    std::vector<int> hashes(n-3);
    hashes[0] = hash;
    for(int i = 1 ; i < n-3 ; i++)
    {
        hash -= v[i-1];
        hash = (hash+prime)%prime;
        hash *= x;
        hash += v[i+3]*p;
        hash %= prime;
        hashes[i] = hash;
    }
    return hashes;
}





std::array<int,1> exact_match_length(std::vector<int> &submission1, std::vector<int> &submission2) { 

     //Citation: My algorithm is based on Rabin-Karp algorithm, and greedy string tiling (Wise's algorithm)
     //Tiling means marking, if a subsequence in above vector matches with another in below vector, then we tile(mark) both

    //Calculating rolling hashes(window size 10) for the vectors of the two submissions
    std::vector<int> hashes1 = compute_hashes_10(submission1, prime1); 
    std::vector<int> hashes2 = compute_hashes_10(submission2, prime1);

    //Creating a hash table that stores for a particular rolling hash value of vector 1, 
    //the indices of vector 2 that have same hash value, either normally, or in increasing order
    std::unordered_map<int, std::vector<int> > hash_table;
    for (int i = 0; i < hashes2.size(); i++) {
        hash_table[hashes2[i]].push_back(i);
    }
    

    //I am going to store whether a token is already marked(call it as tiled) or not in 
    // the tokens of submissions or not using the vector "tiled1", "tiled2"
    std::vector<bool> tiled1(submission1.size(), false);
    std::vector<bool> tiled2(submission2.size(), false);
    int exact_match=0; //The total no of positions that are exactly matched


    //Iterating over the first vector and checking for matching subsequences from second vector
    for(int i=0;i<hashes1.size();i++){
        if(tiled1[i]==true || hash_table.find(hashes1[i])==hash_table.end()){
            continue;
        }
        else{
            
            for(auto j: hash_table[hashes1[i]]){ //indices of second vector where the subsequence matches 
                if(tiled2[j]==true){
                    continue; 
                }
                else{                   
                    //Now extend at these positions to check if further match, we can also initialise extend = 10 instead of extend = 0 below, 
                    //I put 0 so that it just checks the matched 10 things also again(for confirmation)
                    int extend=0;
                    while(i+extend< tiled1.size() && j+extend< tiled2.size() && tiled1[i+extend]==false && tiled2[j+extend]==false && submission1[i+extend]==submission2[j+extend]){
                        extend++;                       
                    }
                    std::fill(tiled1.begin() + i, tiled1.begin() + i+extend , true); 
                    std::fill(tiled2.begin() + j, tiled2.begin() + j+extend , true); 
                    exact_match+=extend;

                    break; //Only once per i, so after finding one j for this i, break
                }
            }
        }
    }


    // float percent_match = exact_match*2.0/(submission1.size()+submission2.size());
    // int plag_or_not = (percent_match>0.5)?1:0;
    std::array<int, 1> result = {exact_match};
    return result;
    
}




//See the longest_approx_match function which follows this, and then see this function
//We are doing only forward tiling for checking approx match, i.e no criss-cross tiling, because in most of the cases the 80% matched subsequences 
//will be in order, if they are not in order and they are longer lengths, then they will be caught by exact_match anyways, but if not in order 
//and shorter length then it doesn't make sense to tag them as plag, so thats why the criss-cross tiling is not needed for approx matching
//In below function, we are finding longest approx match(forward tiling only) that starts at startpos1, startpos2 positions in vectors 1,2 respectively
int longest_approx_match_recur(std::vector<int> &submission1, std::vector<int> &submission2, std::vector<int> &hashes1, std::vector<int> &hashes2, std::unordered_map<int, std::vector<int> > &hash_table, int startpos1, int startpos2) {

    int max_matched_len = 0;
    std::vector<bool> tiled1(submission1.size()-startpos1, false); //Whether tiled or not, position "startpos1" of submission1 is the 0 index here 
    std::vector<bool> tiled2(submission2.size()-startpos2, false);
    int i = 0; //this is representative of the startpos1 position of submission1, i+startpos1 gives the actual position in submission1 vector
    int j = 0; //this is representative of the startpos2 position of submission2

    while(i+startpos1<hashes1.size() && j+startpos2<hashes2.size()){

        while(i+startpos1<hashes1.size() && j+startpos2<hashes2.size() && submission1[i+startpos1]==submission2[j+startpos2] ){

            tiled1[i] = true;
            tiled2[j]  = true;
            i++;
            j++;            
        }

        //if(startpos1==26 && startpos2==24) std::cout<<i<<" "<<j<<std::endl;
        if(!(i+startpos1<hashes1.size() && j+startpos2<hashes2.size())){
            break;
        }
        else if(hash_table.find(hashes1[i+startpos1])==hash_table.end()){ 
            i = i+1;
        }
        else{  
            int got_in = 0; 
            for(auto jnew: hash_table[hashes1[i+startpos1]]){ //Forward tiling, so jnew-startpos2 shud be > j, and if the jnew is soo much ahead of 
                                                              //current j, then lot of gap, so we dont want such jnew, 
                if(jnew-startpos2<j || jnew-startpos2>j+20){  //parameter check: change 20 here if needed
                    continue;
                }
                else{
                    got_in = 1;
                    j = jnew-startpos2;
                    break;
                }
            }
            if(got_in==0){
                i = i+1;  
            }
        }

    }
    
    // Forward tiling is done, now I will check below for longest approx match starting at position i in 1, j in 2


    float total_count1=0; //1 means for vector1
    float tiled_count1 = 0;
    float total_count2= 0;
    float tiled_count2 = 0;
    int iter1= 0;   //this is startpos1 position of submission1 
    int iter2 = 0; //this is startpos2 position of submission2 
    
    while(iter1<submission1.size()-startpos1 && iter2<submission2.size()-startpos2){
        if(tiled1[iter1] && tiled2[iter2]){
            iter1++;
            iter2++;
            total_count1++;
            tiled_count1++;
            total_count2++;
            tiled_count2++;
            //whenever percentage tiled till that place is >85% then update the longest approx match length
            if((tiled_count1/total_count1)>0.80 && (tiled_count2/total_count2)>0.80){       //parameter check: change this 0.75(75%) later 
                max_matched_len = std::max(total_count1, total_count2);
            }
        }

        else{ //If some position is not tiled, move forward until u reach some tiled position, and that will definitely correspond to current tiled position 
                //of other vector because we did forward tiling
            
            while(iter1<submission1.size()-startpos1 && iter2<submission2.size()-startpos2 && !tiled1[iter1]){
                iter1++;
                total_count1++;

                if((tiled_count1/total_count1)>0.80 && (tiled_count2/total_count2)>0.80){       //check: change this 0.75(75%) later 

                    max_matched_len = std::max(total_count1, total_count2);
                }
               
            }
            while(iter1<submission1.size()-startpos1 && iter2<submission2.size()-startpos2 && !tiled2[iter2]){
                iter2++;
                total_count2++;

                if((tiled_count1/total_count1)>0.80 && (tiled_count2/total_count2)>0.80){       //check: change this 0.75(75%) later 
                    
                    max_matched_len = std::max(total_count1, total_count2);
                }
            }

        }


    }
    
    return max_matched_len;

}






//The idea is, for every matched positions pair i,j in vectors1,2 we find the longest approx match that starts at these positions
//and find the overall maximum over all such
std::array<int,3> longest_approx_match(std::vector<int> &submission1, std::vector<int> &submission2) {

    std::vector<int> hashes1 = compute_hashes_4(submission1, prime1);
    std::vector<int> hashes2 = compute_hashes_4(submission2, prime1);
    std::unordered_map<int, std::vector<int> > hash_table;

    for (int i = 0; i < hashes2.size(); i++) {
        hash_table[hashes2[i]].push_back(i);
    }
    
    int longest_match = 0;
    int index1=0; //index of longest match in first vector
    int index2=0;   

    //Iteratively find longest match that starts at positions i,j over all i,j such that hashes_5 match at i,j
    for(int i=0;i<hashes1.size();i++){
        if(hash_table.find(hashes1[i])==hash_table.end()){ 
            continue;
        }
        else{
            for(auto j: hash_table[hashes1[i]]){
                int tempp = longest_approx_match_recur(submission1,submission2, hashes1, hashes2, hash_table,i , j);
                if(tempp>longest_match){
                    longest_match = tempp;
                    index1 = i;
                    index2 = j;
                }
            }
        }
    }


    std::array<int,3> resultt{0,0,0};
    resultt = {longest_match,index1, index2} ;
    return resultt;

}






std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    
    int plag=0; 
    int total_exact_match = exact_match_length(submission1, submission2)[0];
    std::array<int,3> result_approx = longest_approx_match(submission1,submission2);
    int longest_approxmatch = result_approx[0];
    int index1 = result_approx[1];
    int index2 = result_approx[2];
    if(longest_approxmatch<30){
        longest_approxmatch=0;
        index1 = 0;
        index2 = 0;
    }
    float plag_exact = float(total_exact_match)*2.0/float(submission1.size()+submission2.size()) ;
    float plag_approx = float(longest_approxmatch)*2.0/float(submission1.size()+submission2.size()) ;

    if( plag_exact>0.6 || plag_approx>0.6 || plag_approx*0.8 + plag_exact*0.6 > 0.5 ){ //parameter check: modify these values if needed
        plag = 1;
    }
   
    
    std::array<int, 5> result = {plag, total_exact_match, longest_approxmatch, index1, index2};
    return result; 

}

