#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <iterator>
#include <utility>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"
#define ll long long


// lower_bound(const g) Returns an iterator to the first element that is equivalent to ‘g’ or definitely will not go before the element ‘g’ in the set.
// upper_bound(const g)    Returns an iterator to the first element that will go after the element ‘g’ in the set.
// OPTIONAL: Add your helper functions and data structures here
const int BASE = 31;
const int MODULUS = 1e9 + 9;
ll exponent(ll base, ll power)
{
    ll result = 1;
    while (power > 0)
    {
        if (power % 2 == 1)
        {
            result = (result * base) % MODULUS;
        }
        base = (base * base) % MODULUS;
        power /= 2;
    }
    return result;
}

// This function computes the hash of a substring of the submission using polynomial hashing
ll compute_hash(std::vector<int> &submission, int start, int end)
{
    ll hash = 0;
    for (int i = start; i < end; i++)
    {
        hash = (hash * BASE + submission[i]) % MODULUS;
    }
    return hash;
}

// This function updates the hash value of a substring when a character is removed and another is added efficiently in constant time 
// rather than going over the length of the string
ll update_hash(ll old_hash, int old_char, int new_char, int length)
{
    ll new_hash = (old_hash - old_char * exponent(BASE, length - 1)) % MODULUS;
    if (new_hash < 0)
    {
        new_hash += MODULUS;
    }
    new_hash = (new_hash * BASE + new_char) % MODULUS;
    return new_hash;
}

// This implements the O(n^4) solution
void solve(std::unordered_map<ll, std::set<int>> &flags, std::vector<int> &submission1, std::vector<int> &submission2, int &start_index1, int &start_index2, int &best, int index1, int index2)
{
    int length_text_1 = submission1.size() - index1;
    int m = submission2.size() - index2;
    std::vector<std::vector<int>> dpp(31, std::vector<int>(31, 0));
    for (int i = 1; i <= 30 && index1 + i - 1 < submission1.size(); ++i)
    {
        for (int j = 1; j <= 30 && index2 + j - 1 < submission2.size(); ++j)
        {
            if (submission1[index1 + i - 1] == submission2[index2 + j - 1])
                dpp[i][j] = dpp[i - 1][j - 1] + 1;
            else
                dpp[i][j] = std::max(dpp[i - 1][j], dpp[i][j - 1]);
        }
    }
    if (dpp[std::min(30, int(submission1.size() - index1))][std::min(30, int(submission2.size() - index2))] < 21)
        return;

    std::vector<std::vector<int>> dp(length_text_1 + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= length_text_1; ++i)
    {
        for (int j = 1; j <= m; ++j)
        {
            if (submission1[index1 + i - 1] == submission2[index2 + j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
        }
    }
    for (int f = index2 + 30; f < submission2.size(); f++)
    {
        std::set<int> indices = flags[submission2[f]];
        auto it = indices.upper_bound(index1);
        for (; it != indices.end(); ++it)
        {
            int index = *it;
            double ratio = double(index - index1) / double(f - index2);
            if (ratio > 1.25 || ratio < 0.8)
                continue;
            int lcs = dp[index - index1][f - index2];
            if (double(lcs) >= (0.8) * double(std::max(index - index1, f - index2)) && lcs >= 30)
            {
                // if(index1 == 183 && index2 == 1) cout << index << ' ' << ratio << ' ' << lcs << ' ' << f << '\n';
                if (std::min(index - index1, f - index2) > best)
                {
                    start_index1 = index1;
                    start_index2 = index2;
                    best = std::min(index - index1, f - index2);
                }
            }
        }
    }
}

// This function implements the O(n^2) solution using dynamic programming to find the longest common subsequence and for heuristics assuming that
// the longest approximate match is a part of the longest common subsequence
std::vector<int> longest_approximate_match_length(std::vector<int> &submission1, std::vector<int> &submission2)
{
    int length_input_1 = submission1.size();
    int length_input_2 = submission2.size();
    std::vector<std::vector<int>> dp_table(length_input_1 + 1, std::vector<int>(length_input_2 + 1, 0));
    std::vector<std::vector<std::pair<int, int>>> parent(length_input_1 + 1, std::vector<std::pair<int, int>>(length_input_2 + 1, {0, 0}));

    // dp[m][n] represents the length of the longest common subsequence between the first m characters of submission1 and the first n characters of submission2 
    for (int e = 0; e <= length_input_1; e++)
    {
        dp_table[e][0] = e;
    }
    for (int e = 0; e <= length_input_2; e++)
    {
        dp_table[0][e] = e;
    }
    for (int a = 1; a <= length_input_1; a++)
    {
        for (int b = 1; b <= length_input_2; b++)
        {
            int cost = 2;
            if (submission1[a - 1] == submission2[b - 1])
                cost = 0;
            dp_table[a][b] = std::min(dp_table[a - 1][b] + 1, std::min(dp_table[a][b - 1] + 1, dp_table[a - 1][b - 1] + cost));
            if (dp_table[a][b] == dp_table[a - 1][b - 1] + cost)
            {
                parent[a][b] = {a - 1, b - 1};
            }
            if (dp_table[a][b] == dp_table[a - 1][b] + 1)
            {
                parent[a][b] = {a - 1, b};
            }
            if (dp_table[a][b] == dp_table[a][b - 1] + 1)
            {
                parent[a][b] = {a, b - 1};
            }
        }
    }
    int overall_best = 0;
    int final_start_index1 = 0, final_start_index2 = 0;

    for (int i = length_input_1; i >= (int) ((0.6) * length_input_1); i -= (int) ((0.12) * length_input_1))
    {
        for (int j = length_input_2; j >= (int) (0.6)*length_input_2; j -= (int) ((0.12) * length_input_2))
        {
            if(i < 0 || j < 0) continue;
            int a = i, b = j;
            std::vector<std::pair<int, int>> matches;
            while (a > 0 && b > 0)
            {
                int temp_a = parent[a][b].first, temp_b = parent[a][b].second;
                if (temp_a == a - 1 && temp_b == b - 1 && dp_table[a][b] == dp_table[a - 1][b - 1])
                    matches.push_back({a - 1, b - 1});
                a = temp_a;
                b = temp_b;
            }
           
            int ms = matches.size();
            int best = 0;
            int start_index1 = 0, start_index2 = 0;
            for (int t = 0; t < ms; t++)
            {
                for (int w = t + 1; w < ms; w++)
                {
                    if (std::max(matches[t].second - matches[w].second + 1, matches[t].first - matches[w].first + 1) < 30)
                        continue;
                    double ratio = double(w - t + 1) / double(std::max(matches[t].second - matches[w].second + 1, matches[t].first - matches[w].first + 1));
                    if (ratio >= 0.8)
                    {
                        int score = std::max(matches[t].second - matches[w].second + 1, matches[t].first - matches[w].first + 1);
                        int old_score = score;
                        score = (int)(double(w - t + 1) / 0.8);
                        score = std::min(score, std::min(length_input_1, length_input_2));
                        if (best < score)
                        {
                            
                            start_index1 = std::max(matches[w].first + old_score - score, length_input_1 - score);
                           
                            start_index2 = std::max(matches[w].second + old_score - score, length_input_2 - score);
                            best = score;
                        }
                    }
                }
            }
           
            if (overall_best < best)
            {
                overall_best = best;
                final_start_index1 = start_index1;
                final_start_index2 = start_index2;
            }
        }
    }
   
    return {overall_best, final_start_index1, final_start_index2};
   
}

std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2)
{
    int length_input_1 = submission1.size();
    int length_input_2 = submission2.size();

    
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int plagiarism_flag = 0;
    int total_matching_length = 0;
    int longest_approx_match = 0;
    int start_index1 = 0;
    int start_index2 = 0;
    
    std::unordered_map<ll, ll> indice_to_hash;
    std::unordered_map<ll, std::vector<ll>> hashes;
   
    ll window_length = 10;
    ll hash1 = compute_hash(submission1, 0, window_length);
    ll rolling_hash = hash1;

    // This for loop implements the idea of the Rabin-Karp algorithm by calculating hashes for all substrings of length = window_length
    for (int i = 0; i <= length_input_1 - window_length; i++)
    {
        
        hash1 = rolling_hash;
        hashes[hash1].push_back(i);
        indice_to_hash[i] = hash1;
       

        if (i + window_length < length_input_1)
        {
            rolling_hash = update_hash(hash1, submission1[i], submission1[i + window_length], window_length);
           
        }
      
    }
    // This loops finds indices which have the same hash value and then finds the longest common substring between them
    ll hash2 = compute_hash(submission2, 0, window_length);
    for (int j = 0; j <= length_input_2 - window_length; j++)
    {
        hash2 = compute_hash(submission2, j, j + window_length);

       
        if (hashes.find(hash2) != hashes.end())
        {

            std::vector<ll> match_indices = hashes[hash2];
            if (match_indices.empty())
                continue;
           
            int max_length_matching = 0;
            int index_of_max_match = -1;
            for (auto match_index : match_indices)
            {
                int length_matching = 0;
                while (match_index + length_matching < length_input_1 && j + length_matching < length_input_2 && submission1[match_index + length_matching] == submission2[j + length_matching])
                {
                    length_matching++;
                }
                if (length_matching > max_length_matching)
                {
                    max_length_matching = length_matching;
                    index_of_max_match = match_index;
                }
            }

            // This loop removes the indices of the matching substring from the hash map such that we dont use them again and we only consider non-overlapping substrings
            for (int it = index_of_max_match + 1; it < index_of_max_match + max_length_matching; it++)
            {
                auto ptr = std::find(hashes[indice_to_hash[it]].begin(), hashes[indice_to_hash[it]].end(), it);
                if (ptr != hashes[indice_to_hash[it]].end())
                    hashes[indice_to_hash[it]].erase(ptr);
            }
            
            total_matching_length += max_length_matching;
            j += max_length_matching;
           
        }
        
    }


    
    auto approx_matching_stats = longest_approximate_match_length(submission1, submission2);
    longest_approx_match = approx_matching_stats[0];
    start_index1 = approx_matching_stats[1];
    start_index2 = approx_matching_stats[2];
    // std::cout << (double (total_matching_length) / std::min(length_input_1, length_input_2)) << '\n';
    // std::cout << (double (longest_approx_match) / std::min(length_input_1, length_input_2)) << '\n';

    // Given enough testcase data we can actually implement a logistic regression model to predict the plagiarism flag based on the these features
    if (total_matching_length > 0.4141 * std::min(length_input_1, length_input_2) || (longest_approx_match > 0.314159 * std::min(length_input_1, length_input_2)))
    {
        plagiarism_flag = 1;
    }
    else
    {
        plagiarism_flag = 0;
    }

    /* This is an optimized O(n^4) algorithm which gives the accurate results for sure but is not efficient for practical purposes */

    // n to the power of 4 algo
    // First build the map that stores the
    // int best = 0;
    // int s1 = -1, s2 = -1;
    // std::unordered_map<ll, std::set<int>> flags;
    // for (int ind = 0; ind < length_input_1; ind++)
    // {
    //     (flags[submission1[ind]]).insert(ind);
    // }

    // int c = 0;
    // // Now we iterate through it as a way of finding the indices matching for a certain index in the second set
    // for (int it = 0; it < m; it++)
    // {
    //     for (auto ind : flags[submission2[it]])
    //     {
    //         // std::cout << ind << ' ';
    //         // cout << c << '\n';
    //         // solve(flags, submission1, submission2, s1, s2, best, ind, it);
    //     }
    //     c++;
    //     // cout << c;
    //     // std::cout << '\n';
    // }
    // start_index1 = s1;
    // start_index2 = s2;

    // longest_approx_match = best;

    result[0] = plagiarism_flag;
    result[1] = total_matching_length;
    result[2] = longest_approx_match;
    result[3] = start_index1;
    result[4] = start_index2;

    return result; 
   
}
