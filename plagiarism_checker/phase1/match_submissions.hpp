#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <map>
#include <tuple>
#include <algorithm>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// function to compute (a^m)%p

long long int modular_exponentation(long long int a, int m, long long int p)
{
    a %= p;
    if (m == 0)
        return 1;
    if (m == 1)
        return a % p;
    long long int res = modular_exponentation(a, m / 2, p);
    if (m % 2 == 0)
        return (res * res) % p;
    return (a * ((res * res) % p)) % p;
}

// function to compute modular inverse.
// Citation: lab exam 2 : Q 3

long long int modular_inverse(long long int a, long long int prime)
{
    return a <= 1 ? a : prime - (long long)(prime / a) * modular_inverse(prime % a, prime) % prime;
}

// function to compute rolling hash for all 10 length substrings in a vector of integers
// Inspiration from the Rabin-Karp Algorithm

void compute_rolling_hash(std::vector<int> &input, int prime, long long int modulo, std::map<long long int, int> &hash_values)
{
    int n = input.size();
    if (n < 10)
        return;
    long long int x = 1;
    long long int polynomial = 0;
    for (int i = 0; i < 10; i++)
    {
        polynomial += (x * (input[i] % modulo)) % modulo;
        x = (x * prime) % modulo;
    }
    hash_values[polynomial] = 0;

    long long int prime_power = modular_exponentation(prime, 9, modulo);
    long long int inverse = modular_inverse(prime, modulo);
    for (int end_index = 10; end_index < n; end_index++)
    {
        polynomial = (polynomial - input[end_index - 10] + modulo) % modulo;
        polynomial = (polynomial * inverse) % modulo;
        polynomial = (polynomial + (prime_power * input[end_index]) % modulo) % modulo;
        hash_values[polynomial] = end_index - 9;
    }
}

// function to compute the polynomial for the first 10 length substring

long long int initial_polynomial(std::vector<int> &input, int prime, int modulo, int start)
{
    long long int x = 1;
    long long int polynomial = 0;
    for (int i = start; i < 10 + start; i++)
    {
        polynomial += (x * (input[i] % modulo)) % modulo;
        x = (x * prime) % modulo;
    }
    return polynomial;
}

// returns if two intervals overlap

bool overlap(std::pair<int, int> a, std::pair<int, int> b)
{
    return !(a.second < b.first || b.second < a.first);
}

// function to compute exact matches of length 10 or above

int check_exact_matches(std::vector<int> &file1, std::vector<int> &file2, int prime, int modulo)
{
    int file1size = file1.size();
    int file2size = file2.size();

    std::vector<int> &small_file = (file1size < file2size) ? file1 : file2;
    std::vector<int> &big_file = (file1size < file2size) ? file2 : file1;

    int small_file_size = small_file.size();
    int big_file_size = big_file.size();

    if (small_file_size < 10)
        return 0;

    long long int prime_power = modular_exponentation(prime, 9, modulo);
    long long int inverse = modular_inverse(prime, modulo);

    std::map<long long int, int> hash_values;
    compute_rolling_hash(big_file, prime, modulo, hash_values);

    // store a pair for each match, the first index in file1 and the last index in file1

    std::vector<std::pair<int, int>> matches;

    int index = 0;

    while (index < small_file_size - 10 + 1)
    {
        long long int polynomial = initial_polynomial(small_file, prime, modulo, index);

        while ((index < small_file_size - 10) && (hash_values.find(polynomial) == hash_values.end()))
        {
            polynomial = (polynomial - small_file[index] + modulo) % modulo;
            polynomial = (polynomial * inverse) % modulo;
            polynomial = (polynomial + (prime_power * small_file[index + 10]) % modulo) % modulo;

            index++;
        }

        // if we couldn't find a match of length 10, there won't be a match of length longer than 10
        // hence, we are done

        if (index == small_file_size - 10)
        {
            // check if the last substring of length 10 is a match
            if (hash_values.find(polynomial) != hash_values.end())
            {
                matches.push_back(std::make_pair(hash_values[polynomial], hash_values[polynomial] + 9));
            }
            break;
        }

        // if we reached here
        // we found a match, trying to extend

        int original_index = hash_values[polynomial] + 10;
        index += 10;
        int length = 10;

        while (original_index < big_file_size && index < small_file_size && big_file[original_index] == small_file[index])
        {
            original_index++;
            index++;
            length++;
        }

        // add pair to matches

        matches.push_back(std::make_pair(original_index - length, original_index - 1));
    }

    // construct a graph where each node is an interval
    // and there is an edge between two nodes if the intervals overlap

    int num_matches = matches.size();

    // adjacency list
    std::vector<std::vector<int>> graph(num_matches, std::vector<int>());

    // store degree of each vertex

    std::vector<int> degree(num_matches, 0);

    for (int i = 0; i < num_matches; i++)
    {
        for (int j = i + 1; j < num_matches; j++)
        {
            if (overlap(matches[i], matches[j]))
            {
                graph[i].push_back(j);
                graph[j].push_back(i);
                degree[i]++;
                degree[j]++;
            }
        }
    }

    // choose pairs to maximize length covered but no two pairs overlap
    std::vector<std::pair<int, int>> chosen_pairs;

    // keep track of the total length of chosen_pairs
    int total_length = 0;

    // store weights/degree of each vertex
    std::vector<std::pair<int, float>> weights_div_degree; // index, weight/degree
    for (int i = 0; i < num_matches; i++)
    {
        if (degree[i] == 0)
        {
            chosen_pairs.push_back(matches[i]);
            total_length += matches[i].second - matches[i].first + 1;
        }
        else
        {
            int size = matches[i].second - matches[i].first + 1;
            weights_div_degree.push_back(std::make_pair(i, float(size) / float(degree[i])));
        }
    }

    // sort the vertices based on weight/degree
    std::sort(weights_div_degree.begin(), weights_div_degree.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b)
              { return a.second > b.second; });

    int non_zero_degree = weights_div_degree.size();

    for (int i = 0; i < non_zero_degree; i++)
    {
        index = weights_div_degree[i].first;
        bool overlapping = false;
        for (const auto &pair : chosen_pairs)
        {
            if (overlap(matches[index], pair))
            {
                overlapping = true;
                break;
            }
        }
        if (!overlapping)
        {
            chosen_pairs.push_back(matches[index]);
            total_length += matches[index].second - matches[index].first + 1;
        }
    }

    return total_length;
}

// function to find the longest common subsequence. Citation: https://www.geeksforgeeks.org/longest-common-subsequence-dp-4/
std::vector<int> longestCommonSubsequence(const std::vector<int> &v1, const std::vector<int> &v2, int &start1, int &end1, int &start2, int &end2)
{
    // Define the subarray lengths
    int len1 = end1 - start1;
    int len2 = end2 - start2;

    // Initialize a 2D DP table with dimensions (len1 + 1) x (len2 + 1)
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

    // Fill the DP table
    for (int i = 1; i <= len1; ++i)
    {
        for (int j = 1; j <= len2; ++j)
        {
            if (v1[start1 + i - 1] == v2[start2 + j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            }
            else
            {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    // Reconstruct the longest common subsequence
    std::vector<int> lcs;
    int i = len1, j = len2;
    int lcs_start1 = -1, lcs_start2 = -1; // Track start indices in original vectors

    // Backtrack to build the LCS and find the starting indices
    while (i > 0 && j > 0)
    {
        if (v1[start1 + i - 1] == v2[start2 + j - 1])
        {
            if (lcs.empty())
            { // Set end positions once at the beginning of LCS reconstruction
                end1 = start1 + i - 1;
                end2 = start2 + j - 1;
            }
            lcs.push_back(v1[start1 + i - 1]);
            lcs_start1 = start1 + i - 1;
            lcs_start2 = start2 + j - 1;
            --i;
            --j;
        }
        else if (dp[i - 1][j] > dp[i][j - 1])
        {
            --i;
        }
        else
        {
            --j;
        }
    }

    // Reverse the LCS as we built it backwards
    std::reverse(lcs.begin(), lcs.end());

    // Update start indices after LCS reconstruction
    start1 = lcs_start1;
    start2 = lcs_start2;

    return lcs;
}

// function to find the longest subsequence with more than 80% match
std::vector<int> checkLarge(std::vector<int> v1, std::vector<int> v2)
{
    int _start_1 = 0;
    int _start_2 = 0;
    int _end_1 = v1.size();
    int _end_2 = v2.size();

    int len1 = _end_1 - _start_1;
    int len2 = _end_2 - _start_2;

    std::vector<int> &v_short = (len1 <= len2) ? v1 : v2;
    std::vector<int> &v_long = (len1 > len2) ? v1 : v2;

    // set values of start1, end1, start2 and end2
    int start1 = (len1 > len2) ? _start_2 : _start_1;
    int end1 = (len1 > len2) ? _end_2 : _end_1;
    int start2 = (len1 > len2) ? _start_1 : _start_2;
    int end2 = (len1 > len2) ? _end_1 : _end_2;

    while (start1 < v_short.size() && start2 < v_long.size() && end1 > start1 && end2 > start2)
    {

        std::vector<int> temp = longestCommonSubsequence(v_short, v_long, start1, end1, start2, end2);

        if (temp.size() * 10 >= 8 * std::max(end1 - start1, end2 - start2))
        {
            std::vector<int> op;
            op.push_back(temp.size());
            op.push_back(start1);
            op.push_back(start2);
            return op;
        }
        else
        {
            // original values of start1 and end1
            int start_1 = start1;
            int end_1 = end1;
            // original values of start2 and end2
            int start_2 = start2;
            int end_2 = end2;

            // find nearest match from either end(excluding the ends)
            // If not found from either end, shrink the window being checked to within the original lcs

            // look for temp[1] in shorter file (assume v1)
            int dist_start = 0;
            for (int index = start2 + 1; index < end2; index++)
            {
                dist_start++;
                if (v_long[index] == temp[1])
                {
                    break;
                }
            }

            int size = temp.size();
            int dist_end = 0;
            for (int index = end2 - 1; index > start2; index--)
            {
                dist_end++;
                if (v_long[index] == temp[size - 2])
                {
                    break;
                }
            }

            if (dist_start < dist_end)
            {
                start2 += dist_start;
                std::vector<int> temp_start = longestCommonSubsequence(v_short, v_long, start1, end1, start2, end2);

                if (temp_start.size() * 10 > 8 * std::max(end1 - start1, end2 - start2))
                {
                    std::vector<int> op;
                    op.push_back(temp_start.size());
                    op.push_back(start1);
                    op.push_back(start2);
                    return op;
                }
                else
                {
                    start1 = start_1;
                    start2 = start_2;
                    end1 = end_1;
                    end2 = end_2;

                    end2 -= dist_end;
                    std::vector<int> temp_end = longestCommonSubsequence(v_short, v_long, start1, end1, start2, end2);

                    if (temp_end.size() * 10 > 8 * std::max(end1 - start1, end2 - start2))
                    {
                        std::vector<int> op;
                        op.push_back(temp_end.size());
                        op.push_back(start1);
                        op.push_back(start2);
                        return op;
                    }
                    else
                    {
                        start1 = start_1;
                        start2 = start_2;
                        end1 = end_1;
                        end2 = end_2;

                        start2 += dist_start;
                        end2 -= dist_end;
                    }
                }
            }
            else
            {
                end2 -= dist_end;
                std::vector<int> temp_end = longestCommonSubsequence(v_short, v_long, start1, end1, start2, end2);

                if (temp_end.size() * 10 > 8 * std::max(end1 - start1, end2 - start2))
                {
                    std::vector<int> op;
                    op.push_back(temp_end.size());
                    op.push_back(start1);
                    op.push_back(start2);
                    return op;
                }
                else
                {
                    start1 = start_1;
                    start2 = start_2;
                    end1 = end_1;
                    end2 = end_2;

                    start2 += dist_start;
                    std::vector<int> temp_start = longestCommonSubsequence(v_short, v_long, start1, end1, start2, end2);

                    if (temp_start.size() * 10 > 8 * std::max(end1 - start1, end2 - start2))
                    {
                        std::vector<int> op;
                        op.push_back(temp_start.size());
                        op.push_back(start1);
                        op.push_back(start2);
                        return op;
                    }
                    else
                    {
                        start1 = start_1;
                        start2 = start_2;
                        end1 = end_1;
                        end2 = end_2;

                        start2 += dist_start;
                        end2 -= dist_end;
                    }
                }
            }
        }
    }

    std::vector<int> op;
    return op;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2)
{

    std::array<int, 5> result = {0, 0, 0, 0, 0};
    int prime = 307;                // prime number for hashing
    long long int modulo = 1e9 + 7; // modulo for hashing

    int len_short_matches = check_exact_matches(submission1, submission2, prime, modulo); // checking for exact matches
    std::vector<int> temp = checkLarge(submission1, submission2);                         // checking for approximate matches
    if ((len_short_matches * 10 >= 3 * std::min(submission1.size(), submission2.size())) || (temp.size() * 2 >= std::min(submission1.size(), submission2.size())))
        result[0] = 1;
    else
        result[0] = 0;
    result[1] = len_short_matches;
    result[2] = temp[0];
    result[3] = temp[2];
    result[4] = temp[1];

    return result;
}