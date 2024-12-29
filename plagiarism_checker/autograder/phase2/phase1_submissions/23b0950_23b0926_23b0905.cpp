#include <array>
#include <iostream>
#include <span>
#include <vector>
// -----------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <unordered_set>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

//-------------------------------------------------
//------------------------------------------------

/*
 * The present code utilizes KMP to find all disjoint exact matches between
 * given vectors. Also it uses dynamic programming to find the longest
 * approximate match. Based on these two results we judge if plagiarism is
 * present or not.
 */

// Uses Knuth-Morris-Pratt algorithm to find exact matching
class KMP {
private:
  // HELPER Functions
  // To avoid overlapping between the matches found in submission2 (the text
  // vector to be matched) Hence we find the unmatched part, which can
  // accomodate the present pattern
  static int
  findNextUnmatchedPart(int patsize, int txtsize, int start,
                        const std::unordered_set<int> &matchedIndices) {
    bool found = 0;
    int i = start;

    while (!found && i < txtsize) {
      // if current pattern were to be matched, the end index of pattern must be
      // unmatched
      if (matchedIndices.count(i + patsize - 1)) {
        i = i + patsize;
        continue;
      }

      // exhaustive check to find if any pattern matching has already been done,
      // in between start and end index
      else if (matchedIndices.count(i + 10)) {
        i = i + 11;
        continue;
      }

      // if present index is already matched
      else if (matchedIndices.count(i)) {
        i++;
        continue;
      }
      found = 1;
    }
    if (!found) {
      return -1;
    }
    return i; // Returns starting index of unmatched part
  }

  // Uses Kmp like approach to ci=ompute kmp-table
  // Fills h[] for given pattern pat[0..M-1]
  static void computeKMPTable(const std::vector<int> &pat, int M,
                              std::vector<int> &h) {
    h[0] = -1;
    int i = 1;
    int j = 0;

    while (i < M) {
      if (pat[i] != pat[j]) {
        h[i] = j;
        // precompute for next iteration
        while (j >= 0 && pat[i] != pat[j])
          j = h[j];
      } else
        h[i] = h[j];

      i++;
      j++;
    }
    h[i] = j;
  }

public:
  // MAIN Function
  static int KMPSearch(const std::vector<int> &pat, const std::vector<int> &txt,
                       std::unordered_set<int> &matchedIndices) {
    int j = 0;
    int m = pat.size();
    int n = txt.size();

    // This function finds the starting index first contiguous part of txt
    // which is atleast the size of pat and completely unmatched
    int i = findNextUnmatchedPart(m, n, 0, matchedIndices);
    if (i == -1) // Means No remaining place found, where matching can be done.
    {
      return 0;
    }
    std::vector<int> h(m + 1);
    computeKMPTable(pat, m, h);
    while (i < n) {
      // If the current index is already matched, we need to find next unmatched
      // part and then restart kmp by setting j=0
      if (matchedIndices.count(i)) {
        i = findNextUnmatchedPart(m, n, i, matchedIndices);
        j = 0;
        continue;
      }
      if (pat[j] == txt[i]) {
        i++;
        j++;
        if (j == m) {
          // Adding the matched pattern indices in matchedIndices
          for (int k = i - j; k < i; k++)
            matchedIndices.insert(k);
          return j; // returns the length of the pattern matched
        }
      } else {
        // shifting j to the point where the matched portion repeats
        j = h[j];
        if (j < 0) // no portion of pattern is matched, move index
        {
          i++;
          j++;
        }
      }
    }
    return 0;
  }
};

//---------------------------------------
//----------------------------------------------

// Using Longest Common Subsequence to find longest approximate matching
class LCS {
private:
  std::vector<int> ind1, ind2; // Indices of LCS in both vectors

  void findLCSIndices(const std::vector<int> &vec1,
                      const std::vector<int> &vec2) {
    int m = vec1.size();
    int n = vec2.size();

    // Maintain a table for calculating LCS using dynamic programming
    // Ref: https://www.geeksforgeeks.org/longest-common-subsequence-dp-4/
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++) {
      for (int j = 1; j <= n; j++) {
        if (vec1[i - 1] == vec2[j - 1]) {
          dp[i][j] = dp[i - 1][j - 1] + 1; // Characters match
        } else {
          dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]); // Choose the maximum
        }
      }
    }

    // Backtrack to find the LCS elements and their indices
    // Ref: https://www.geeksforgeeks.org/printing-longest-common-subsequence/
    int i = m, j = n;
    while (i > 0 && j > 0) {
      // If current character in vec1 and vec2 are same, then
      // current character is part of LCS
      if (vec1[i - 1] == vec2[j - 1]) {
        ind1.push_back(i - 1);
        ind2.push_back(j - 1);
        i--;
        j--;
      }
      // If not same, then find the larger of two and
      // go in the direction of larger value
      else if (dp[i - 1][j] > dp[i][j - 1]) {
        i--; // Move up
      } else {
        j--; // Move left
      }
    }

    // Since we constructed lcs backwards, reverse it
    std::reverse(ind1.begin(), ind1.end());
    std::reverse(ind2.begin(), ind2.end());
  }

public:
  // MAIN Function
  // Returns length of longest match and starting indices in both vectors
  std::vector<int> longest_approximate_match(const std::vector<int> &vec1,
                                             const std::vector<int> &vec2) {
    int m = vec1.size();
    int n = vec2.size();

    findLCSIndices(vec1, vec2);

    int ans1 = 0;
    int ans2 = 0;
    int ans3 = 0;

    // Look for the largest subsequence first
    int indSize = ind1.size();
    for (int k = 0; k < indSize; k++) {
      // optimization to break the current loop if it cannot give longer
      // match than we already have
      if (indSize - ind1[k] <= ans1 || indSize - ind2[k] <= ans1)
        break;

      for (int h = indSize - 1; h > k; h--) {
        // If match found is small, discard it and continue to next iteration
        int minStringLen = std::min((ind1[h] - ind1[k]), (ind2[h] - ind2[k]));
        if (minStringLen < 30)
          break;

        int maxStringLen = std::max((ind1[h] - ind1[k]), (ind2[h] - ind2[k]));
        double fracMatch = double(h - k + 1) / (maxStringLen + 1);

        // Fraction of matching subsequence should be at least 80% of longer
        // substring.
        if (fracMatch > 0.8 && maxStringLen + 1 > ans1) {
          ans1 = maxStringLen + 1;
          ans2 = ind1[k];
          ans3 = ind2[k];
          break;
        }
      }
    }
    return {ans1, ans2, ans3};
  }
};
//---------------------------------------
//---------------------------------------

// HELPER Function for exact matching
// Returns total match length
int total_match(const std::vector<int> &submission1,
                const std::vector<int> &submission2) {
  // MatchedIndices required to avoid non overlapping
  std::unordered_set<int> matchedIndices;

  int totalMatch = 0;

  for (int i = 0; i < (int)submission1.size(); ++i) {
    // Check patterns of length between 20 and 10 (start from larger patterns
    // first)
    for (int len = 20; len >= 10; --len) {
      if (i + len > submission1.size())
        continue; // Remove out of bound errors

      std::vector<int> pattern(submission1.begin() + i,
                               submission1.begin() + i + len);
      int match = KMP::KMPSearch(pattern, submission2, matchedIndices);
      totalMatch += match;

      if (match > 0) {
        i = i + len - 1;
        break; // Skip smaller patterns if a larger one is found
      }
    }
  }
  return totalMatch;
}

// MAIN Function of this file
// Finds matches between two tokenized files
std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2) {
  // ensure 1 is the smaller one
  if (submission1.size() > submission2.size())
    return match_submissions(submission2, submission1);

  // Corner case where the cpp file is too small
  if (submission1.size() < 10)
    return {0, 0, 0, 0, 0};

  // Exact Matching function
  int totalMatch = total_match(submission1, submission2);

  // Approximate matching function
  LCS lcs;
  std::vector<int> approxMatch =
      lcs.longest_approximate_match(submission1, submission2);

  // Report plag based on total and approx match
  int n1 = submission1.size(), n2 = submission2.size();
  double n3 = std::max(n1, n2);
  double fracExactPlag = totalMatch / n3;
  double fracApproxPlag = approxMatch[0] / n3;

  // The Exact matching can be due to essential parts of codes,
  // so the threshold should depend on the length of code,
  // as in smaller codes, a small threshold can detect false plag
  double threshold = (5.0) / std::pow(n3, 0.43);
  int hasPlag =
      ((fracExactPlag > threshold || fracApproxPlag > 0.4) && totalMatch > 40);
  std::array<int, 5> result = {hasPlag, totalMatch, approxMatch[0],
                               approxMatch[1], approxMatch[2]};
  return result;
}
