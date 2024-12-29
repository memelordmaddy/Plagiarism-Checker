#include <iostream>
#include <vector>
#include <array>
#include <utility>  // For std::pair
#include <algorithm>
#include <set>

// Function to build the LPS array for the pattern
void buildLPS(const std::vector<int>& pattern, std::vector<int>& lps) {
    int m = pattern.size();
    lps[0] = 0; // lps[0] is always 0
    int length = 0; // Length of the previous longest prefix suffix
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1]; // Use the previous lps value
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// Function to find all occurrences of a pattern vector in a text vector
void KMP(const std::vector<int>& text, const std::vector<int>& pattern, 
         std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>>& matches, 
         int start, int end) {
    
    int n = text.size();
    int m = pattern.size();
    std::vector<int> lps(m);

    // Build the LPS array for the pattern
    buildLPS(pattern, lps);

    int i = 0; // Index for text
    int j = 0; // Index for pattern

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) { // Found a match
            matches.push_back({{start, end}, {i - j, i - 1}}); // Store starting index and length of the match
            j = lps[j - 1]; // Use LPS to find the next match
            break;
        } else if (i < n && pattern[j] != text[i]) { // Mismatch after j matches
            if (j != 0) {
                j = lps[j - 1]; // Use the LPS array
            } else {
                i++;
            }
        }
    }
}

// Function to check if two intervals overlap
bool overlaps(const std::pair<int, int>& a, const std::pair<int, int>& b) {
   // return a.first < b.second && b.first < a.second; // Check overlap
   return (a.first <= b.first && b.first <= a.second && a.second <= b.second) || (b.first <= a.first && a.first <= b.second && b.second <= a.second);
}

// Function to merge two intervals
std::pair<std::pair<int, int>, std::pair<int, int>> mergeIntervals(
    const std::pair<std::pair<int, int>, std::pair<int, int>>& a,
    const std::pair<std::pair<int, int>, std::pair<int, int>>& b) {
    return {
        {std::min(a.first.first, b.first.first), std::max(a.first.second, b.first.second)},
        {std::min(a.second.first, b.second.first), std::max(a.second.second, b.second.second)}
    };
}

// Function to combine overlapping intervals without sorting
std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> combineIntervals(
    const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& intervals) {

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> merged;

    for (const auto& current : intervals) {
        bool mergedFlag = false;

        // Check against all merged intervals
        for (size_t j = 0; j < merged.size(); ++j) {
            // If there's an overlap, merge the intervals
            if (overlaps(merged[j].first, current.first) && overlaps(merged[j].second, current.second)) {
               
                merged[j] = mergeIntervals(merged[j], current);
               
                mergedFlag = true;
                break; // Break since we only want to merge with one interval
            }
        }

        // If no merge happened, add the current interval
        if (!mergedFlag) {
            merged.push_back(current);
        }
    }

    return merged;
}

// Function to check if `a` is a subset of `b` (These were not removed in case of merging of overlapped interval)
bool isSubset(const std::pair<std::pair<int, int>, std::pair<int, int>>& a,
              const std::pair<std::pair<int, int>, std::pair<int, int>>& b) {
    return ((a.first.first >= b.first.first && a.first.second <= b.first.second) &&
           (a.second.first >= b.second.first && a.second.second <= b.second.second)) || (a.second.first >= b.second.first && a.second.second <= b.second.second);
}

// Function to remove subset pairs from indices
void removeSubsets(std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& indices) {
    for (auto it = indices.begin(); it != indices.end(); ) {
        bool erased = false;  // Track if the current element is erased
        for (auto it2 = indices.begin(); it2 != indices.end(); ++it2) {
            if (it != it2 && isSubset(*it, *it2)) {
                
                // Remove the smaller range and use the iterator returned by erase
                it = indices.erase(it);
                erased = true;
                break; // Break to avoid invalidated iterator
            }
        }
        
        // Only increment if no element was erased
        if (!erased) {
            ++it;
        }
    }
}

// Modified to return the start and end indices of the longest subsequence
std::tuple<int, int, int> longestSubsequenceWith80PercentOnes(const std::vector<int>& binaryVector) {
    int maxLength = 0;
    int startIdx = -1; // Start index of the subsequence within the diagonal
    int endIdx = -1;   // End index of the subsequence within the diagonal
    int n = binaryVector.size();
    double requiredPercentage = 0.8;

    int left = 0, onesCount = 0;

    for (int right = 0; right < n; ++right) {
        // Expand the window by including the current element at right pointer
        if (binaryVector[right] == 1) {
            onesCount++;
        }

        // Adjust the window if the percentage of ones is below the threshold
        int length = right - left + 1;
        while (left <= right && static_cast<double>(onesCount) / length <= requiredPercentage) {
            // Shrink the window from the left
            if (binaryVector[left] == 1) {
                onesCount--;
            }
            left++;
            length = right - left + 1;
        }

        // Check if the current window has more than 80% ones and is the longest so far
        if (static_cast<double>(onesCount) / length > requiredPercentage && length > maxLength) {
            maxLength = length;
            startIdx = left;
            endIdx = right;
        }
    }

    return std::make_tuple(maxLength, startIdx, endIdx);
}


std::tuple<int, int, int> longestApproximateMatch(const std::vector<int>& vector1, const std::vector<int>& vector2, double threshold = 0.8) {
    int len1 = vector1.size();
    int len2 = vector2.size();
    int maxMatchLength = 0;
    int finalStartIdx1 = -1; // To store the start index in vector1
    int finalStartIdx2 = -1; // To store the start index in vector2


    // Step 1: Create the binary matrix
    std::vector<std::vector<int>> binaryMatrix(len1, std::vector<int>(len2, 0));
    for (int i = 0; i < len1; ++i) {
        for (int j = 0; j < len2; ++j) {
            if (vector1[i] == vector2[j]) {
                binaryMatrix[i][j] = 1;
            }
        }
    }

    // Step 2: Traverse over difference diagonals (i - j = constant)

    // Diagonals starting from each row in the leftmost column
    for (int startRow = 0; startRow < len1; ++startRow) {
        std::vector<int> diagonal;
        int diagStartIdx1 = startRow;
        int diagStartIdx2 = 0;
        for (int i = startRow, j = 0; i < len1 && j < len2; ++i, ++j) {
            diagonal.push_back(binaryMatrix[i][j]);
        }
        
        if (diagonal.size() >= 30) {
            auto [tmpLength, diagStart, diagEnd] = longestSubsequenceWith80PercentOnes(diagonal);
            if (tmpLength >= 30 && tmpLength > maxMatchLength) {
                maxMatchLength = tmpLength;
                // Calculate the actual start indices in vector1 and vector2
                finalStartIdx1 = diagStartIdx1 + diagStart;
                finalStartIdx2 = diagStartIdx2 + diagStart;
            }
        }
    }

    // Diagonals starting from each column in the top row (excluding the first column)
    for (int startCol = 1; startCol < len2; ++startCol) {
        std::vector<int> diagonal;
        int diagStartIdx1 = 0;
        int diagStartIdx2 = startCol;
        for (int i = 0, j = startCol; i < len1 && j < len2; ++i, ++j) {
            diagonal.push_back(binaryMatrix[i][j]);
        }
        
        if (diagonal.size() >= 30) {
            auto [tmpLength, diagStart, diagEnd] = longestSubsequenceWith80PercentOnes(diagonal);
            if (tmpLength >= 30 && tmpLength > maxMatchLength) {
                maxMatchLength = tmpLength;
                // Calculate the actual start indices in vector1 and vector2
                finalStartIdx1 = diagStartIdx1 + diagStart;
                finalStartIdx2 = diagStartIdx2 + diagStart;
            }
        }
    }

    return std::make_tuple(maxMatchLength, finalStartIdx1, finalStartIdx2);
}

// Final Function to detect plagiarism
std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>> indices;

    // Loop over submission1 to extract different patterns
    for (int i = 0; i < submission1.size(); ++i) {
        for (int j = 10; j <= 20; ++j) { // Taking pattern lengths between 10 and 20
            if (i + j <= submission1.size()) { // Ensure the slice is within bounds
                std::vector<int> pattern(submission1.begin() + i, submission1.begin() + i + j);

                // Call KMP for the current pattern against submission2
                KMP(submission2, pattern, indices, i, i + j - 1);
            }
        }
    }
    
    auto mergedIntervals = combineIntervals(indices);
    removeSubsets(mergedIntervals);

   
    for (const auto& interval : mergedIntervals) {
       
        result[1] += (interval.second.second - interval.second.first + 1); // Add length of matches
       
    }
    
    std::set<std::pair<int, int>> uniqueIntervals;

    result[2] = std::get<0>(longestApproximateMatch(submission1, submission2));
    result[3] = std::get<1>(longestApproximateMatch(submission1, submission2));
    result[4] = std::get<2>(longestApproximateMatch(submission1, submission2));

//Taking flag of plagiarism as 1 when total length of exact tokens found is atleast half of minimum of two or longest approximate subsequence
//found is atleast half of minimum of two
    if(result[1] >= 0.6 * std::min(submission1.size(),submission2.size()) || result[2] >= 0.5 * std::min(submission1.size(),submission2.size())){
        result[0] = 1;
    }
    return result;
}

