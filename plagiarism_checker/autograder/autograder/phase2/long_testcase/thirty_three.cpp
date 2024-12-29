#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <set>
#include <iterator> // ---- ADDED by TA TEJAS SHARMA to prevent compilation errors.
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

/**
 * @brief Computes the Jaccard similarity between two lists of integers.
 *
 * The Jaccard similarity is defined as the size of the intersection divided by the size of the union of the sample sets.
 * It is a measure of the similarity between two sets, with a value between 0 and 1.
 *
 * @param list1 The first list of integers.
 * @param list2 The second list of integers.
 * @return A double representing the Jaccard similarity between the two lists.
 *         Returns 1.0 if both lists are empty.
 */
double jaccardSimilarity(const std::vector<int> &list1, const std::vector<int> &list2)
{
    std::set<int> set1(list1.begin(), list1.end());
    std::set<int> set2(list2.begin(), list2.end());

    std::set<int> intersection;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                          std::inserter(intersection, intersection.begin()));

    std::set<int> unionSet;
    std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(),
                   std::inserter(unionSet, unionSet.begin()));

    return unionSet.empty() ? 1.0 : (double)intersection.size() / unionSet.size();
}

/**
 * @brief Finds an approximate match between two lists based on a similarity threshold.
 *
 * This function searches for a sublist in `list1` that approximately matches a sublist in `list2`.
 * The similarity between sublists is measured using the Jaccard similarity coefficient.
 * The function returns a vector containing the size of the matching sublist, the starting index
 * of the matching sublist in `list1`, and the starting index of the matching sublist in `list2`.
 * If no match is found, an empty vector is returned.
 *
 * @param list1 The first list of integers to be compared.
 * @param list2 The second list of integers to be compared.
 * @param threshold The similarity threshold for considering a match (default is 0.95).
 * @return A vector containing the size of the matching sublist, the starting index in `list1`,
 *         and the starting index in `list2`. If no match is found, an empty vector is returned.
 */
std::vector<int> approx_match(const std::vector<int> &list1, const std::vector<int> &list2, double threshold = 0.95)
{
    int len1 = list1.size();
    int len2 = list2.size();
    std::vector<int> ans;

    for (int k = std::min(len1, len2); k >= 30; k -= 10)
    {
        for (int i = 0; i <= len1 - k; ++i)
        {
            std::vector<int> sublist1(list1.begin() + i, list1.begin() + i + k);

            for (int j = 0; j <= len2 - k; ++j)
            {
                std::vector<int> sublist2(list2.begin() + j, list2.begin() + j + k);
                double similarity = jaccardSimilarity(sublist1, sublist2);

                if (similarity >= threshold)
                {
                    ans.push_back(sublist2.size());
                    ans.push_back(i);
                    ans.push_back(j);
                    return ans;
                }
            }
        }
    }
    return {};
}

/**
 * @brief Computes the longest prefix which is also suffix (LPS) array for the given pattern.
 *
 * This function populates the LPS array (also known as the "partial match" table) for the given pattern.
 * The LPS array is used in the KMP (Knuth-Morris-Pratt) pattern matching algorithm to find occurrences
 * of a pattern within a text efficiently.
 *
 * @param pattern The pattern for which the LPS array is to be computed.
 * @param h A reference to the vector that will store the computed LPS array. It should be of the same size as the pattern.
 */
void H_ARRAY(const std::vector<int> &pattern, std::vector<int> &h)
{
    int length = 0;
    h[0] = 0;
    int N = pattern.size();
    int i = 1;
    while (i < N)
    {
        if (pattern[i] == pattern[length])
        {
            length++;
            h[i] = length;
            i++;
        }
        else
        {
            if (length != 0)
                length = h[length - 1];
            else
            {
                h[i] = 0;
                i++;
            }
        }
    }
}

/**
 * @brief Performs KMP (Knuth-Morris-Pratt) search algorithm to find the first occurrence of a pattern in a given text.
 *
 * @param text The text in which to search for the pattern.
 * @param pattern The pattern to search for in the text.
 * @return int The index of the first occurrence of the pattern in the text, or -1 if the pattern is not found.
 */
int KMPSearch(const std::vector<int> &text, const std::vector<int> &pattern)
{
    int m = pattern.size();
    int n = text.size();

    std::vector<int> h(m); // Longest prefix suffix array
    H_ARRAY(pattern, h);

    int i = 0; // index for text
    int j = 0; // index for pattern
    while (i < n)
    {
        if (pattern[j] == text[i])
        {
            i++;
            j++;
        }
        if (j == m)
        {
            return i - j; // Return the first matching index
        }
        else if (i < n && pattern[j] != text[i])
        {
            if (j != 0)
                j = h[j - 1]; // Move the pattern
            else
                i++;
        }
    }
    return -1; // Return -1 if no match is found
}

/**
 * @brief Finds 10-token matches between two sequences and records their indices.
 *
 * This function searches for 10-token subsequences from `text2` within `text1` and records the starting indices
 * of these matches in the provided `match_indices` pair. The first element of the pair contains the starting
 * indices in `text1`, and the second element contains the corresponding starting indices in `text2`.
 *
 * @param text1 The first sequence of tokens to search within.
 * @param text2 The second sequence of tokens to search for 10-token subsequences.
 * @param match_indices A pair of vectors to store the starting indices of matches in `text1` and `text2`.
 */
void find_10_matches(const std::vector<int> &text1, const std::vector<int> &text2, std::pair<std::vector<int>, std::vector<int>> &match_indices)
{
    int segmentSize = 10;

    // Iterate over each possible 10-token subsequence in text2
    for (int i = 0; i <= text2.size() - segmentSize; ++i)
    {
        // Create a sliding window of 10 tokens starting at index i
        std::vector<int> subpattern(text2.begin() + i, text2.begin() + i + segmentSize);

        // Call KMPSearch for each 10-token subsequence, obtaining start indices in text1
        int index = KMPSearch(text1, subpattern);

        // If there are matches, add them to the match_indices vector
        if (index != -1)
        {
            match_indices.first.push_back(index);
            match_indices.second.push_back(i);
        }
    }
}

/**
 * @brief Joins overlapping or consecutive matches into a single range.
 *
 * This function takes a sorted vector of start indices and merges overlapping
 * or consecutive ranges of matches into a single range. Each match is assumed
 * to span 10 units from its start index.
 *
 * @param start_indices A sorted vector of integers representing the start indices of matches.
 * @return A vector of pairs, where each pair represents the start and end indices of a merged match range.
 */
std::vector<std::pair<int, int>> join_matches(std::vector<int> &start_indices)
{
    // assuming the start_indices are sorted
    // This can be ensured by taking the second output of find_10_matches, as they are pushed back into increasing order
    std::vector<std::pair<int, int>> ans;
    for (int i = 0; i < start_indices.size(); i++)
    {
        if (i == 0)
        {
            ans.push_back({start_indices[i], start_indices[i] + 10});
            continue;
        }
        if (start_indices[i] <= ans.back().second)
        {
            ans.back().second = start_indices[i] + 10;
        }
        else
        {
            ans.push_back({start_indices[i], start_indices[i] + 10});
        }
    }
    return ans;
}

// OPTIONAL: Add your helper functions and data structures here

std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2)
{
    // TODO: Write your code here
    // calculating length of exact matches
    std::pair<std::vector<int>, std::vector<int>> match_indices;
    find_10_matches(submission1, submission2, match_indices);

    // sort the match_indices according to the first vector
    std::sort(match_indices.first.begin(), match_indices.first.end());
    // join the matches

    // join the matches
    std::vector<std::pair<int, int>> matches = join_matches(match_indices.first);

    // calculating the length of the matches
    int exact_length = 0;
    for (int i = 0; i < matches.size(); i++)
    {
        exact_length += matches[i].second - matches[i].first;
    }

    std::vector<int> approx_result = approx_match(submission1, submission2);

    int is_plag = 0;
    int len = std::max(submission1.size(), submission2.size());
    if (submission1.size() > 0.7 * len && submission2.size() > 0.7 * len) // optimise 0.4 and 0.5
    {
        if (approx_result[0] >= 0.4 * len || exact_length >= 0.5 * len)
        {
            is_plag = 1;
        }
    }
    std::array<int, 5> result;

    result[0] = is_plag;
    result[1] = exact_length;
    result[2] = approx_result[0];
    result[3] = approx_result[1];
    result[4] = approx_result[2];

    return result;
    // End TODO
}
