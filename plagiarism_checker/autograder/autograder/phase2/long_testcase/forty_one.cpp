#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

struct VectorHash {
    std::size_t operator()(const std::vector<int>& vec) const {
        std::size_t hash = 0;
        for (const int& i : vec) {
            hash ^= std::hash<int>()(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2); // Combine hashes
        }
        return hash;
    }
};

struct VectorEqual {
    bool operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) const {
        return lhs == rhs; // Uses std::vector's built-in comparison
    }
};

std::vector<int> computeLPS(const std::vector<int> &pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int length = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;

        } else {
            if (length != 0) {
                length = lps[length - 1];
            } 
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

int extendMatch(const std::vector<int> &largest, const std::vector<int> &smallest, int startLarge, int startSmall, int l) {
    int matchLength = l;
    int totalLength = l;

    // Use a loop to calculate matches and handle the 80% match condition
    while (startLarge + totalLength < largest.size() && startSmall + totalLength < smallest.size()) {
        // Check for direct match
        if (largest[startLarge + totalLength] == smallest[startSmall + totalLength]) {
            matchLength++;
            totalLength++;
        } 
        else {
            // Calculate the current matching ratio
            int matchedElements = matchLength; // count of directly matched elements
            totalLength++; // include the current pair being compared

            // Calculate the match percentage
            double matchPercentage = (static_cast<double>(matchedElements) / totalLength) * 100;

            // If match percentage is greater than 80%, consider it a match
            if (matchPercentage < 80.0) {
                totalLength--;
                break; // Exit if match percentage is not met
            }
        }
    }

    return totalLength;
}



int KMPMatch(const std::vector<int> &text, const std::vector<int> &pattern, int &startIdx) {
    std::vector<int> lps = computeLPS(pattern);
    int i = 0;
    int j = 0;
    int maxMatchLength = 0;
    int tempStartIdx = -1;

    while (i < text.size()) {
        if (pattern[j] == text[i]) {
            if (j == 0) tempStartIdx = i; // potential start index
            i++;
            j++;
            if (j > maxMatchLength) {
                maxMatchLength = j;
                startIdx = tempStartIdx;
            }
        }
        if (j == pattern.size()) {
            break;
            j = lps[j - 1];
        } 
        else if (i < text.size() && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } 
            else {
                i++;
            }
        }
    }



    return maxMatchLength;
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // Determine the smallest and largest vectors
    auto &smallest = (submission1.size() < submission2.size()) ? submission1 : submission2;
    auto &largest = (submission1.size() >= submission2.size()) ? submission1 : submission2;

    int chunkSize = 20;
    int maxMatchLength = 0;
    int totalMatchedLength = 0;
    int maxMatchStartSmallest = -1;
    int maxMatchStartLargest = -1;
    

    int matchLength=0;
    std::vector<bool> x((int)smallest.size(),false);
    std::unordered_set<std::vector<int>, VectorHash, VectorEqual> y;

    for (int i = 0; i < smallest.size(); i += 1) {
        std::vector<int> smallChunk(smallest.begin() + i, smallest.begin() + std::min(i + chunkSize, (int)smallest.size()));
        int matchStartIdx = -1;
        matchLength = KMPMatch(largest, smallChunk, matchStartIdx);
        if (matchLength >= 6) {
                    int extendedMatchLength = extendMatch(largest, smallest, matchStartIdx, i, matchLength);
                    if (extendedMatchLength > maxMatchLength) {
                        maxMatchLength = extendedMatchLength;
                        maxMatchStartSmallest = i;
                        maxMatchStartLargest = matchStartIdx;
                    }
        }
        int endIdx = std::min(i + matchLength,    static_cast<int>(smallest.size()));
        if (i >= 0 && i < smallest.size() && endIdx > i) {
            std::vector<int> slice(smallest.begin() + i, smallest.begin() + endIdx);
            if (y.find(slice) == y.end()) {
                y.insert(slice);
                if(matchLength>=10 && matchLength<=20){
                    for(int k=i;k<i+matchLength;k++){
                        if(!x[k]){
                            x[k]=true;
                            totalMatchedLength++;
                        } 
                    }
                }
            }
        }
    }
    double matchpercentage = ((double)totalMatchedLength/smallest.size()) * 100;
    double approxmatchpercentage= ((double)maxMatchLength/smallest.size()) * 100;
    // Set the result array based on gathered information
    result[0] = (matchpercentage > 30 || approxmatchpercentage > 40 || totalMatchedLength > 80 || approxmatchpercentage > 100) ? 1 : 0;
    result[1] = totalMatchedLength;
    result[2] = maxMatchLength;
    result[3] = maxMatchStartSmallest;
    result[4] = maxMatchStartLargest;

    return result;

}