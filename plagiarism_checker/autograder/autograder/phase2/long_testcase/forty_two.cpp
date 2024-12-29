#include <array>
#include <cmath>
#include <iostream>
#include <span>
#include <vector>
// -----------------------------------------------------------------------------

#include <algorithm>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

std::pair<int, int>
exact_matching(std::vector<int> &vec1, std::vector<int> &vec2, int min_matches,
               std::vector<std::pair<int, int>> &possible_starts) {

  int currt_matches = 0;
  int total_matches = 0;
  int max_matches = 0;
  int index_max = -1;

  std::vector<int> shift(vec1.size(), 1);

  for (int i = 0; i < vec2.size(); i++) {
  next_i:

    int j = 0;
    int match_index = j;

    while (j < vec1.size()) {
      // if the characters haven't been matched, then
      if (shift[j] == 1) {
        // if the characters are the same, then update count, and shift
        if ((i + currt_matches < vec2.size()) &&
            vec1[j] == vec2[i + currt_matches]) {
          currt_matches = currt_matches + 1;
          j += shift[j];
        }
        // if the characters are not the same, then check the current count
        else if (currt_matches >= min_matches) {
          // case 1 : when the match ends at the end of vector, j exits the
          // while loop
          if (j > currt_matches)
            possible_starts.push_back({j - currt_matches, i});
          // case 2 :
          else if (vec1.size() - currt_matches > 0)
            possible_starts.push_back({vec1.size() - currt_matches, i});
          else
            possible_starts.push_back({0, i});

          total_matches = total_matches + currt_matches;

          if (currt_matches > max_matches) {
            max_matches = currt_matches;
            index_max = i;
          }

          // shift value for the matched index is set to the current match
          shift[match_index] = currt_matches;
          // increment the index to skip the matched tokens
          i = i + currt_matches;
          // reset the matches
          currt_matches = 0;

          if (i >= vec2.size()) {
            return {total_matches, index_max};
          }

          // used to skip i++ in the for loop
          goto next_i;
        }
        // if the current count is less than the minimum required matches, then
        else {
          currt_matches = 0;
          j += shift[j];
          match_index = j;
        }
      }
      // the current characters have been matched, then skip the characters
      else {

        if (currt_matches >= min_matches) {

          if (j > currt_matches)
            possible_starts.push_back({j - currt_matches, i});
          else if (vec1.size() - currt_matches > 0)
            possible_starts.push_back({vec1.size() - currt_matches, i});
          else
            possible_starts.push_back({0, i});

          total_matches = total_matches + currt_matches;

          if (currt_matches > max_matches) {
            max_matches = currt_matches;
            index_max = i;
          }

          shift[match_index] = currt_matches;
          i = i + currt_matches;
          currt_matches = 0;

          if (i >= vec2.size()) {
            return {total_matches, index_max};
          }

          goto next_i;
        }

        currt_matches = 0;
        j += shift[j];
        match_index = j;
      }
    }
    if (currt_matches >= min_matches) {

      if (j > currt_matches)
        possible_starts.push_back({j - currt_matches, i});
      else if (vec1.size() - currt_matches > 0)
        possible_starts.push_back({vec1.size() - currt_matches, i});
      else
        possible_starts.push_back({0, i});

      total_matches = total_matches + currt_matches;

      if (currt_matches > max_matches) {
        max_matches = currt_matches;
        index_max = i;
      }

      shift[match_index] = currt_matches;
      i = i + currt_matches;
      currt_matches = 0;

      if (i >= vec2.size()) {
        return {total_matches, index_max};
      }
    }
  }
  return {total_matches, index_max};
}

// levenstein_distance is being used
// references : https://www.geeksforgeeks.org/edit-distance-dp-5/

int max_valid_window(const std::vector<int> &vec1, const std::vector<int> &vec2,
                     int &max_len1, int &max_len2, int &max_match1,
                     int &max_match2,
                     std::vector<std::pair<int, int>> &possible_starts) {
  int n = vec1.size();
  int m = vec2.size();
  int max_length = 0;

  // 2D DP table to store distances for current and previous lengths
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

  for (auto it : possible_starts) {

    int match_index1 = it.first;
    int match_index2 = it.second;

    std::fill(dp.begin(), dp.end(), std::vector<int>(m + 1, 0));

    for (int len1 = 1; match_index1 + len1 <= n; len1++) {
      for (int len2 = 1; match_index2 + len2 <= m; len2++) {

        int i = match_index1 + len1 - 1;
        int j = match_index2 + len2 - 1;

        if (vec1[i] == vec2[j]) {
          dp[len1][len2] = dp[len1 - 1][len2 - 1];
        } else {
          dp[len1][len2] = 1 + std::min({
                                   dp[len1 - 1][len2],    // deletion from vec1
                                   dp[len1][len2 - 1],    // insertion into vec1
                                   dp[len1 - 1][len2 - 1] // substitution
                               });
        }

        // check threshold condition
        int window_len = std::min(len1, len2);
        double threshold = 0.2 * window_len;

        if (dp[len1][len2] <= threshold) {
          max_length = std::max(max_length, window_len);
          if (max_length == window_len) {
            max_len1 = match_index1;
            max_len2 = match_index2;
          }
        }

        if (dp[len1][len2] <= 0.1 * window_len) {
          max_match1 = std::max(max_match1, window_len);
        }
        if (dp[len1][len2] <= 0.05 * window_len) {
          max_match2 = std::max(max_match2, window_len);
        }
      }
    }
  }
  return max_length;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2) {
  // TODO: Write your code here

  std::array<int, 5> result = {0, 0, 0, 0, 0};
  std::vector<std::pair<int, int>> possible_starts;
  std::pair<int, int> x =
      exact_matching(submission1, submission2, 10, possible_starts);

  result[1] = x.first;

  std::vector<std::pair<int, int>> updated_possible_starts;

  std::vector<int> poss_index1;
  std::vector<int> poss_index2;
  for (int i = 0; i < 5; i++) {
    poss_index1.push_back(i * 5);
    poss_index2.push_back(i * 5);
  }
  int y = 25;
  while (y < std::min(submission1.size(), submission2.size())) {
    poss_index1.push_back(y);
    poss_index2.push_back(y);
    y = (int)y * 1.6;
  }

  for (auto i : poss_index1) {
    for (auto j : poss_index2) {
      if (i < submission1.size() && j < submission2.size()) {
        updated_possible_starts.push_back({i, j});
      }
    }
  }

  int k = 5;
  for (auto it : possible_starts) {
    for (int i = 0; i < 10; i++) {
      if (it.first - i * k >= 0 && it.second - i * k >= 0 &&
          it.second - i * k < submission2.size() &&
          it.first - i * k < submission1.size()) {
        updated_possible_starts.push_back(
            {it.first - i * k, it.second - i * k});
      } else {
        break;
      }
    }
  }

  int len90 = 0;
  int len95 = 0;

  result[2] = max_valid_window(submission1, submission2, result[3], result[4],
                               len90, len95, updated_possible_starts);

  if(result[2] < 30){result[2] = 0; result[3] = -1;result[4]=-1;}

  if (result[1] >= 0.7 * std::min(submission1.size(), submission2.size()))
    result[0] += 1;
  if (result[2] >= 0.4 * std::min(submission1.size(), submission2.size()))
    result[0] += 1;
  if (len90 >= 0.3 * std::min(submission1.size(), submission2.size()))
    result[0] += 1;

  result[0] = result[0] >= 2 ? 1 : 0;

  return result;
  // End TODO
}
