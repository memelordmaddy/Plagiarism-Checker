#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <unordered_map>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
// Define a structure for nodes in the suffix tree


// Node for a suffix tree
/*
The below class is a Node and is by the Trie data structure
----> Each node contains an unordered map, with the value being Pointer to the children and key being it's value in the vector (submission 1 here).
----> Also contains , isWordEnd to mark end word in the trie
----> Index store the index of that node in the vector 
*/
class Node {
    public : 
        std::unordered_map<int,Node*> children;
        int index ; // To store the index of the token , that is it's position in the submission 1 in our case , used to report result 3 and result 4
        bool isWordEnd ; 
        Node() {isWordEnd = false ; index = -1 ;} // Constructor
};

/*
Contains a constructor and a destructor to avoid memory overflow 
---> Contains root , Insert method and a buildsuffixTree that passes all the suffix to Insert method to make a suffix trie of a vector
 */
class SuffixTree {
    public:
        Node* root = nullptr;
        SuffixTree();
        ~SuffixTree() ;
        Node * getNode() ; 
        void deleteNode(Node* node) ;
        void Insert(const std::vector<int> keys, int n) ; 
        void buildsuffixTree(std::vector<int> & pattern) ;

};

SuffixTree::SuffixTree() {
    root = getNode(); // Initialize root here
}

SuffixTree::~SuffixTree() {
    deleteNode(root);
}

void SuffixTree::deleteNode(Node* node) {
    if (!node) return;
    for (auto& child : node->children) {
        deleteNode(child.second) ;
    }
    delete node; 
}
        
Node* SuffixTree::getNode(){
    Node * newNode = new Node(); 
    return newNode;
}

// n is the size of the vector whose suffix trie is being build
void SuffixTree::Insert(const std::vector<int> keys,  int n){
    Node * temp = root ;
    bool is_empty = root->children.empty() ; 
    int curr_suffix_size = keys.size() ; 

    for(int i=0 ;i<keys.size() ; i++) {	
        // If node for the key not found, then insert one for it.
        if(temp->children.find(keys[i]) == temp->children.end()) {
            temp->children[keys[i]] = SuffixTree::getNode();
        }

        // Store the first occurence in the suffix tree only 
        if (temp->children[keys[i]]->index == -1) 
            temp->children[keys[i]]->index = n-curr_suffix_size+i ; // Storing the index of the token in the submission 1
        
        temp = temp->children[keys[i]] ; 
        if(i==keys.size()-1) temp->isWordEnd = true ;

    }
}

void SuffixTree::buildsuffixTree(std::vector<int> & pattern){
    for(int i = 1 ; i<=pattern.size() ; i++){
        std::vector<int> sliced_vec(pattern.end()-i,pattern.end()) ; 
        Insert(sliced_vec,pattern.size()) ; 
    } 
}

// Declaration here , defination below 
void fix_overlapping(std::vector<bool> & matched , std::vector<int> & match_index , int & curr_match_len , int & overlap_start , int & overlap_end ,Node * temp, int & j) ; 

/*
-----> matched : keeps check of which tokens in the submission1 (in our case) is matched
-----> match_index : keeps the track that the tokens in the sub2 is matched to which index token in sub1
-----> curr match len : keeps track of the length of the token that has been matched  
-----> j : is the index in the sub2 where the match ended
-----> temp : pointer to the last matched index wrt submission 1 (onw whose trie is build) in the TRIE 
*/
void add_to_match(std::vector<bool> & matched , std::vector<int> & match_index , int & curr_match_len,int & j,Node* temp){
    int overlap_start = -1 ; 
    int overlap_end = -1;
    for (int i = 0; i<curr_match_len ; i++){
        if(matched[temp->index-i] == false){
            match_index[j-i] = temp->index-i ;
            matched[temp->index-i] = true ;
        }
        else {
            if(overlap_start == -1){
                overlap_start = i ;  
            }
            overlap_end = i ;
        }
    }

    // Calling the function to check in which region overlapping is there 
    if(overlap_start != -1){
        // Overlapping is present
        fix_overlapping(matched , match_index , curr_match_len , overlap_start , overlap_end , temp , j) ; 
    }

    return ; 
}


/*
Same arguements almost
-----> overlap start : denotes the index from where overlap starts  
-----> overlap end : denotes the index where overlap ends 
***** -> Overlap means that if there is match between sub1 and sub2 currently , and that region of the sub1 was already matched
         with some other part of sub2, then I declare a overlap
*/
void fix_overlapping(std::vector<bool> & matched , std::vector<int> & match_index , int & curr_match_len , int & overlap_start , int & overlap_end ,Node * temp, int & j){

    // If overlap is started at a point and it ends before the last index of current matched pattern then the current matched pattern is longer 
    // than the previous pattern so remove that and write the current one 
    if(overlap_end < curr_match_len -1 ){
        for(int i = overlap_start ; i<= overlap_end ; i++){
            matched[temp->index-i] = true ; 
            match_index[j-i] = temp->index - i ; 
        }
    }
    else{
        if(overlap_start <= 9) {
            // In this case most of this sub sequence is already matched with someone else so drop this and continue 
            // So change the matched variable for these <=9 tokens to false
            for(int i = 0 ; i<overlap_start ; i++) {
                match_index[j-i] = -1 ;
                matched[temp->index-i] = false ;
            }
            return ; 
        } 
    }
    return ;    
}

/*
This matches the sub2 and sub1 using the suffix trie of sub1 
*/
void ShortMatching(Node * root, std::vector<int> & sub2 , std::vector<int> & match_index,int & idx,std::vector<bool> & matched)
{
    Node * temp = root ; 
    int curr_match_len = 0 ;

    for(int j = idx ; j<sub2.size() ; j++){
        
        if(temp->children.find(sub2[j]) != temp->children.end()){ 
            temp = temp->children[sub2[j]] ; 
            curr_match_len ++ ;
            if(temp->isWordEnd == 1){  
                // Case when the token is matched and suffix's end is reached 
                if(curr_match_len > 10){
                    add_to_match(matched,match_index,curr_match_len,j,temp) ; 
                    idx += curr_match_len;
                    return ; 
                    // No need to go to suffix link as we have already matched the token at the word end
                }
                else {
                    idx ++ ; 
                    return ; 
                }
            }
        }
        // If match failed then check a short pattern was detected or not 
        else if(curr_match_len > 10){
            j = j-1 ; // Beacuse in last loop when temp was intialised j was incremented by one after that due to loop 
            add_to_match(matched,match_index,curr_match_len,j,temp) ; 
            idx += curr_match_len;
            return ; 
        }
        else{
            idx ++ ; 
            return ; 
        }
    }
}


void LongestMatching(const std::vector<int>& match_index, std::array<int, 5>& result) {
    int max_len = 0;         
    int first_non_neg = -1 , first_non_neg_index = -1 ; // Index of the first non -1 in the current segment 
    int total_count = 0 , non_neg_count = 0;    // Count of non -1 and total values in the current segment
    int min_sub1 = 1000000 , max_sub1 = -1000000 ; // setting large values 
    bool just_after_last_min_one =  0 ; 
    bool index_after_minus_one = 0 ;
    int reset_idx = - 1 ;

    for (int i = 0; i < match_index.size(); i++) {
        if (match_index[i] != -1) { 
            if (total_count == 0) { 
                first_non_neg = match_index[i]; // corresponds to the result 3 values 
                first_non_neg_index = i; // coressponds to the result 4 values 
            }

            if(index_after_minus_one){
                index_after_minus_one = 0 ;
                reset_idx = i ; 
            }

            min_sub1 = std::min(min_sub1,match_index[i]) ; 
            max_sub1 = std::max(max_sub1,match_index[i]) ; 

            non_neg_count++;
            total_count++;
            just_after_last_min_one  = 1 ;

        } 
        if(match_index[i] == -1 || i == match_index.size()-1) { 
            if(just_after_last_min_one) { 
                int max_diff = std::max(max_sub1-min_sub1 , total_count) ; 
                double percentage_match = non_neg_count * 100.0 / max_diff ; 
                if (percentage_match >= 80.0 && max_diff >= 30) { // Check if non -1 count is > 80%
                    if (max_diff > max_len) { // Check if this segment is the longest so far
                        max_len = max_diff;
                        result[2] = max_len ; 
                        result[3] = first_non_neg ; 
                        result[4] = first_non_neg_index ;
                    }
                }
                else if(percentage_match < 80.0) {
                    total_count = 0 ; 
                    non_neg_count = 0  ;
                    min_sub1 = 1000000 ;
                    max_sub1 = -1000000 ; 
                    i = reset_idx ; 
                    i-- ; 
                    just_after_last_min_one = 0  ;
                    continue;
                } 
            }
            if (total_count > 0) {
                total_count++;
            }
            just_after_last_min_one = 0  ;
            index_after_minus_one = 1 ; 
        }
    }
    // If no valid segment found then return, the default values.
    if (max_len == 0) { 
        result[2] = 0;
        result[3] = -1;
        result[4] = -1;
    }
    return ; 
}        

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, -1, -1};

    if (submission1.empty() || submission2.empty()) {
        return result; 
    }

    // Make a trie for the sub 1 
    SuffixTree trie ; 
    trie.buildsuffixTree(submission1) ;
    Node * root = trie.root ;
    int idx = 0 ; // match the submission2 from this index

    std::vector<int> match_index(submission2.size(),-1) ; // To store the index of the matched token in the submission 1
    std::vector<bool> matched(submission1.size(),false) ; // To ensure no character is matched twice

    while (idx < submission2.size()) {
        ShortMatching(root, submission2, match_index, idx,matched);
    }

    // Process the result[0]
    for(auto i : match_index){
        if(i != -1) result[1] ++ ; 
    }

    // setting the values of result [0]
    int max_size = 0 ; 
    if(submission1.size() > submission2.size()) max_size = submission1.size() ;
    else max_size = submission2.size() ; 

    LongestMatching(match_index,result) ; 

    float percentage_exact_match =  (float(result[1])/float(max_size))*100  ;
    float percentage_long_total = (float(result[2])/float(max_size))*100 ; 
    if (percentage_exact_match > 65) result[0] = 1 ;
    else if (percentage_exact_match > 40 && percentage_long_total > 25) result[0] = 1 ;
    else result[0] = 0 ;   

    return result; // dummy return
    // End TODO
}