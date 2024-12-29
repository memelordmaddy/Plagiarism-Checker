#include <array>
#include<iostream>
#include <span>
#include <vector>
#include <cmath>

// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// this fucntion compute rolling hashed values of a given vector,with a given window size
std::vector<int> rolling_hash(const std::vector<int>& vec, int window_size=12) {
    
    const int BASE = 257;       
    const int MOD = 1e9 + 7;

    std::vector<int> hashed_values;

    int b =vec.size();

    if (b < window_size) {
        return hashed_values; 
    }

    long long current_hash = 0;
    long long base_pow = 1;  

    for (int i = 0; i < window_size; i++) {
        current_hash = (current_hash * BASE + vec[i]) % MOD;
        if (i < window_size - 1) {
            base_pow = (base_pow * BASE) % MOD;
        }
    }
    hashed_values.push_back(current_hash);

    for (size_t i = window_size; i < vec.size(); ++i) {


        current_hash = (current_hash - vec[i - window_size] * base_pow % MOD + MOD) % MOD;

        current_hash = (current_hash * BASE + vec[i]) % MOD;

        hashed_values.push_back(current_hash);
    }

    return hashed_values;
}

int Short_Using_Hash(std::vector<int> submission1, std::vector<int> submission2){
    std::vector<int > Hash1=rolling_hash(submission1);
    std::vector<int > Hash2=rolling_hash(submission2);
    int n1 = submission1.size();
    int n2 = submission2.size();
    int total_matched_length=0;
    for(int i=0;i<n2;i++){
        for(int j=i; j < n1 ;j++){
            if(Hash1[j]==Hash2[i]){
                total_matched_length +=12;
                i+=12;
                j+=11;
            }
        }
    }
    return total_matched_length;
}


// this function takes 2 submission vectors and return total short length matches
int Short_Using_Hash_new(std::vector<int> submission1, std::vector<int> submission2){
    std::vector<int > Hash1=rolling_hash(submission1);
    std::vector<int > Hash2=rolling_hash(submission2);
    int n1 = Hash1.size();
    int n2 = Hash2.size();
    int total_matched_length1=0;
    int total_matched_length2=0;
    int last_Updated_j=0;
    int last_Updated_j1=0;
    int value1=0;
    int value2=0;
    for(int i=0;i<n2;i++){
        for(int j=last_Updated_j; j < n1 ;j++){
            if(Hash1[j]==Hash2[i]){
                total_matched_length1 +=12;
                i+=12;
                j+=11;
                last_Updated_j=j;
            }
        }
    }
    value1= total_matched_length1;
    for(int i=0;i<n1;i++){
        for(int j=last_Updated_j1; j < n2 ;j++){
            if(Hash1[i]==Hash2[j]){
                total_matched_length2 +=12;
                i+=12;
                j+=11;
                last_Updated_j1=j;
            }
        }
    }
    value2= total_matched_length2;

    if(value1>=value2){
        return value1;
    }
    return value2;

}

// this function takes 2 submission vectors and return vector of vector (starting index in submission 1 , starting index in submission 2 , ending index in submission 1 , ending index in submission 2)
std::vector<std::vector<int>> Short_Using_Hash_new_Vector(std::vector<int> submission1, std::vector<int> submission2){
    int hash_length=3;
    std::vector<int> Hash1=rolling_hash(submission1,hash_length);
    std::vector<int> Hash2=rolling_hash(submission2,hash_length);
    int n1 = Hash1.size();
    int n2 = Hash2.size();
    int total_matched_length1=0;
    int total_matched_length2=0;
    int last_Updated_j=0;
    int last_Updated_j1=0;
    int value1=0;
    int value2=0;
    std::vector<std::vector<int>>v1;
    std::vector<std::vector<int>>v2;
    for(int i=0;i<n2;i++){
        for(int j=last_Updated_j; j < n1 ;j++){
            if(Hash1[j]==Hash2[i]){
                total_matched_length1 +=hash_length;
                std::vector<int> r ;
                r={j,i,j+hash_length-1,i+hash_length-1};
                v1.push_back(r);
                i+=hash_length;
                j+=hash_length-1;
                last_Updated_j=j;
            }
        }
    }
    value1= total_matched_length1;


    for(int i=0;i<n1;i++){
        for(int j=last_Updated_j1; j < n2 ;j++){
            if(Hash1[i]==Hash2[j]){
                total_matched_length2 +=12;
                std::vector<int> r ;
                r={i,j,i+hash_length-1,j+hash_length-1};
                v2.push_back(r);
                i+=hash_length;
                j+=hash_length-1;
                last_Updated_j1=j;
            }
        }
    }
    value2= total_matched_length2;

    if(value1>=value2){
        return v1;
    }
    return v2;

}



std::vector<std::vector<int>> Short_Using_Hash_vector(std::vector<int> submission1, std::vector<int> submission2){
    int hash_length=2;
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    std::vector<std::vector<int>> vec ;
    int n1 = Hash1.size();
    int n2 = Hash2.size();
    // int total_matched_length=0;
    int last_Updated_j=0;
    for(int i=0;i<n2;i++){
        for(int j=last_Updated_j ; j < n1 ;j++){ 
            if(Hash1[j]==Hash2[i]){
                // total_matched_length +=hash_length;
                std::vector<int> r ;
                r={j,i,j+hash_length-1,i+hash_length-1};
                vec.push_back(r);
                i+=hash_length;
                j+=hash_length-1;
                last_Updated_j=j;
            }
        }
    }
    return vec;
}

// int Short_matching(std::vector<int> submission1, std::vector<int> &submission2){
//     int n1=submission1.size();
//     int n2 = submission2.size();
//     std::cout <<n1<<" "<<n2<<std::endl;

//     int i1=0;
//     int i2=0;
//     int total_matched_length=0;
//     int indxLastMatch=-1;

//     if(n1 > n2 ){
//         while(i1 <n1 || i2<n2){
//             if(i1>=n1 ){
//                 cout <<"reach"<<endl;
//                 i2++;
//                 i1=indxLastMatch+1;
//                 if(i1>=n1){
//                     break;
//                 }
//             }
//             for(int k=i1; k < n1 || i2+k-i1 <n2; k++){
//                 if(k-i1+1 >=20){
//                     cout<<"Hello"<<endl;
//                     total_matched_length+=k-i1;
//                     cout <<"added1 :"<<k-i1<<endl;
//                     int temp = i1;
//                     i1=k;
//                     i2 +=k-temp;
//                     break;
//                 }
//                 if(submission1[k]==submission2[i2-i1+k]){
//                     indxLastMatch=k;
//                     cout <<k<<" "<<i1<<" "<<i2<<endl;
//                     cout<<"Matched Length "<<k-i1+1<<endl;
//                     if((k>=n1-1 && (k-i1 >=10)) || (k-i1+i2 >=n2-1&& (k-i1 >=10))){
//                         total_matched_length+=k-i1+1;
//                         // std::cout <<"added2 :"<<k-i1+1<<endl;
//                         int temp = i1;   
//                         i1=k+1;
//                         i2 +=k-temp;
//                         break;
//                     }
//                     else if(k >= n1-1 || k-i1+i2 >=n2){
//                         cout <<"hello"<<endl;
//                         int temp = i1;   
//                         i1=k+1;
//                         i2 +=k-temp;
//                         // std::cout << i1 <<" "<<i2 <<" " <<k<<endl;
//                         break;
//                     }
//                     continue;
//                 }
//                 else{
//                     if(k-i1+1 >10){
//                         int temp = i1;   
//                         total_matched_length+=k-i1;
//                         std::cout <<"added3 :"<<k-i1<<endl;
//                         i1=k+1;
//                         i2 +=k-temp;
//                         break;
//                     }
//                     else{
//                         int temp = i1;                   
//                         i1=k+1;
//                         i2 += k-temp;
//                         break;
//                     }
//                 }
//             }
//         }
//     }
//     return total_matched_length;
// } 




std ::vector<std::vector<int>> Short_matching_vector_1(std::vector<int> submission1, std::vector<int> submission2){
    int hash_length=4;
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    int n1=Hash1.size();
    int n2 = Hash2.size();
    std::vector<std::vector<int>>vec;

    int i1=0 ;
    int i2=0;
    // int total_matched_length=0;
    int indxLastMatch=-1;


    if(true ){
        while(i1 <n1 || i2<n2){
            if(i1==n1){
                i2++;
                i1=indxLastMatch+1;
                if(i1==n1){
                    break;
                }
            }
            for(int k=i1; k < n1 || i2+k-i1 <n2; k++){
                if(Hash1[k]==Hash2[i2-i1+k]){
                    if(k-indxLastMatch >30){
                        i2++;
                        i1=indxLastMatch+1;
                        break;
                    }
                    indxLastMatch=k;
                    if((k>=n1-1 && (k-i1 >=0)) || (k-i1+i2 >=n2 -1&& (k-i1 >=0))){
                        // total_matched_length+=k-i1+1;
                        std::vector<int> r={i1,i2,k+hash_length-1,k-i1+i2+hash_length-1};
                        vec.push_back(r);
                        int temp = i1;   
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    else if(k >= n1-1 || k-i1+i2 >=n2){
                        int temp = i1;   
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    continue;
                }
                else{
                    if(k-i1+1 >1){
                        int temp = i1;   
                        std::vector<int> r={i1,i2,k-2+hash_length,k-i1+i2-2+hash_length};
                        vec.push_back(r);
                        // total_matched_length+=k-i1;
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    else{
                        int temp = i1;                   
                        i1=k+1;
                        i2 += k-temp;
                        break;
                    }
                }
            }
        }
    }
    return vec;
} 




std::vector<std::vector<int>> Short_matching_vector_H(std::vector<int> submission1, std::vector<int> submission2){
    int hash_length=4;
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    int n1=Hash1.size();
    int n2 = Hash2.size();
    std::vector<std::vector<int>>vec;

    int i1=0 ;
    int i2=0;
    // int total_matched_length=0;
    int indxLastMatch=-1;

    if(true ){
        while(i1 <n1 || i2<n2){
            if(i1==n1){
                i2++;
                i1=indxLastMatch+1;
                if(i1==n1){
                    break;
                }
            }
            for(int k=i1; k < n1 || i2+k-i1 <n2; k++){
                if(Hash1[k]==Hash2[i2-i1+k]){
                    if(k-indxLastMatch >30){
                        i2++;
                        i1=indxLastMatch+1;
                        break;
                    }
                    indxLastMatch=k;
                    if((k>=n1-1 && (k-i1 >=0)) || (k-i1+i2 >=n2 -1&& (k-i1 >=0))){
                        // total_matched_length+=k-i1+1;
                        std::vector<int> r={i1,i2,k+hash_length-1,k-i1+i2+hash_length-1};
                        vec.push_back(r);
                        int temp = i1;   
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    else if(k >= n1-1 || k-i1+i2 >=n2){
                        int temp = i1;   
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    continue;
                }
                else{
                    if(k-i1+1 >1){
                        int temp = i1;   
                        std::vector<int> r={i1,i2,k-2+hash_length,k-i1+i2-2+hash_length};
                        vec.push_back(r);
                        // total_matched_length+=k-i1;
                        i1=k+1;
                        i2 +=k-temp;
                        break;
                    }
                    else{
                        int temp = i1;                   
                        i1=k+1;
                        i2 += k-temp;
                        break;
                    }
                }
            }
        }
    }
    return vec;
} 

// this function takes a boolean array having 1,0 values either matched or not and returns true if in the given index range the fraction is greater than 80 percent 
bool fracInbetween(std::vector<std::vector<int>>vec,int l , int r ,bool match[],int n ){
    double count =0;
    int a=l;
    int b = r;
    for(int i=l ; i <= r ; i++ ){
        if(match[i]){
            count ++;
        }
    }
    double frac =((count) /( b-a+1)) ;

    return frac >=0.8;
}

std::vector<int> Longest_Aprx_Match(std::vector<int> submission1, std::vector<int> submission2){
    int hash_length=2;
    std::vector<std::vector<int>> vec =Short_Using_Hash_vector(submission1,submission2);
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    int n = submission1.size();
    // int total_matched_length=0;
    const int n1 =100000;
    bool match[n1]={false};
    for(auto a : vec ){
        // total_matched_length += a[2]-a[0]+1;
        for(int i=a[0];i<=a[2];i++){
            match[i]=true;
        }
    }
    int l= 0;
    int r = n-1;
    int lastFound=-1;
    int start1=-1;
    int start2=-1;
    while(l<r){
        int len = (l + r)/2;
        bool found = false;
        for(int i=0; (i <= n-len && !found );i++){
            bool bl=fracInbetween(vec,i,i+len-1,match,n);
            if(match[i] && bl){
                start1=i;
                lastFound=len;
                l= len +1;
                found = true;
            }
        }
        if(!found){
            r=len-1;}
        else{
            break;
        }
    }
    int key = Hash1[start1];
    int n2 = Hash2.size();
    for(int k=0 ; k < n2 ; k++){
        if(key==Hash2[k]){
            start2=k;//Check if two value is same 
        }
    }
    std::vector<int>v;
    v.push_back(lastFound);
    v.push_back(start1);
    v.push_back(start2);
    return v;
}

std::vector<int> Longest_Aprx_Match1(std::vector<int> submission1, std::vector<int> &submission2){
    int hash_length=4;
    std::vector<std::vector<int>> vec =Short_matching_vector_H(submission2,submission1);
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    int n = Hash2.size();
    const int n1=100000;
    bool match[n1];
    for(int i=0; i < n ;i++){
        match[i]=false;
    }
    // int total_matched_length=0;
    int start2=1000007;
    int start1=1000007;
    for(auto a : vec ){
        // total_matched_length += a[2]-a[0]+1;
        for(int i=a[0];i<=a[2];i++){
            match[i]=true;
            if(a[1]<start2){
                start2=a[1];
            }
            if(a[0] <start1){
                start1=a[0];
            }
        }
    }
    int l = 0;
    int r = n-1;
    int lastFound=-1;
    int len = n;
    while(l<r && len >0){
        bool found = false;
        for(int i=0; (i <= n-len && !found );i++){
            int l1=i;
            int r1=i+len-1;
            if(match[i] && fracInbetween(vec,l1,r1,match,n)){
                lastFound=len;
                len = r1-l1+1;
                l= len +1;
                found = true;
                break;
            }
        }
        if(!found){
            len=len-1;}
        else{
            break;
        }
    }
    std::vector<int>v;
    v.push_back(lastFound);
    v.push_back(start1);
    v.push_back(start2);
    return v;
}


//this function takes 2 submission vectors and returns longest length size with starting indices
std::vector<int> Longest_Aprx_Match12(std::vector<int> submission1, std::vector<int> &submission2){
    int hash_length=3;
    std::vector<std::vector<int>> vec =Short_Using_Hash_new_Vector(submission2,submission1);
    std::vector<int > Hash1=rolling_hash(submission1,hash_length);
    std::vector<int > Hash2=rolling_hash(submission2,hash_length);
    int n = Hash1.size();
    const int n1=100000;
    bool match[n1]={false};
    // int total_matched_length=0;
    int start2=1000007;
    int start1=1000007;
    for(auto a : vec ){
        // total_matched_length += a[2]-a[0]+1;
        for(int i=a[0];i<=a[2];i++){
            match[i]=true;
            if(a[1]<start2){
                start2=a[1];
            }
            if(a[0] <start1){
                start1=a[0];
            }
        }
    }
    int l = 0;
    int r = n-1;
    int lastFound=-1;
    while(l<r ){
        int len = (l+r)/2;
        bool found = false;
        for(int i=0; (i <= n-len && !found );i++){
            int l1=i;
            int r1=i+len-1;
            if(match[i] && fracInbetween(vec,l1,r1,match,n)){
                lastFound=len;
                len = r1-l1+1;
                l= len +1;
                found = true;
            }
        }
        if(!found){
            r=len-1;
            len=len-1;}
    }
    std::vector<int>v;
    v.push_back(lastFound);
    v.push_back(start1);
    v.push_back(start2);
    return v;
}
double mini(double a , double b){
    if(a<=b){
        return a;
    }
    return b;
}
double maxi(double a , double b){
    if(a>=b){
        return a;
    }
    return b;
}

// this function returns whether plag is done or not setting a threshold value by our own
bool plag_detect(std::vector<int> s1,std::vector<int> s2){
    int hash_size=4;
    int hash_size1=12;
    bool longer=false;
    bool shorter=false;
    std::vector<int> v= Longest_Aprx_Match12(s1, s2);
    double long_length=v[0];
    double short_length=Short_Using_Hash_new(s1,s2);
    double n1=s1.size();
    double n2=s2.size();
    double n_min=mini(n1-hash_size+1,n2-hash_size+1); 

    // we used min of two submission size because one may have plag, and size difference may be very large

    if(long_length/n_min > 0.2){
        longer=true;
    } 
    if(short_length/n_min > 0.25){
        shorter=true;
    }
    return bool(longer || shorter);
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int l1=submission1.size();
    int l2=submission2.size();
    if(l1<=l2){
        result[0]=plag_detect(submission1,submission2);
        result[1]=Short_Using_Hash_new(submission1,submission2);
        std::vector<int> v1= Longest_Aprx_Match12( submission1,  submission2);
        std::vector<int> v2= Longest_Aprx_Match12( submission2,  submission1);
        if(v1[0]>v2[0]){    
            result[2]=v1[0];
            result[3]=v1[1];
            result[4]=v1[2];
        }
        else{
            result[2]=v2[0];
            result[3]=v2[1];
            result[4]=v2[2];
        }
        return result;
    }
    else{
        result[0]=plag_detect(submission2,submission1);
        result[1]=Short_Using_Hash_new(submission1,submission2);
        std::vector<int> v1= Longest_Aprx_Match12( submission2,  submission1);
        std::vector<int> v2= Longest_Aprx_Match12( submission1,  submission2);
        if(v1[0]>v2[0]){    
            result[2]=v1[0];
            result[3]=v1[1];
            result[4]=v1[2];
        }
        else{
            result[2]=v2[0];
            result[3]=v2[1];
            result[4]=v2[2];
        }
        return result;
    }
}