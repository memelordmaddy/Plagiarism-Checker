#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

#include <algorithm>
#include <set>
#include <map>
#include <stack>

#define MAX_MOD 1000000007

#ifdef DEBUG
#undef DEBUG
#endif

#ifndef NDEBUG
#define NDEBUG
#endif

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

// Implementing winnowing

// Step1 : obtain the sequence of k-grams
// Step2 : compute hashes of them using polynomial hashing
// Step3 : for window of size t, check out matches, and preserve with index
/**
 * RBTree class is taken from https://gist.github.com/m-mizutani/1135351 with slight modifications
 */

int size_gridblock = 10;

class Interval;

typedef std::vector<std::vector<std::set<Interval>>> gridType;

enum partial_order
{
    less,
    more,
    equal,
    uncomp
};
class Interval
{
public:
    int start1;
    int start2;
    int end1;
    int end2;
    float matching;

    bool operator<(const Interval &i2) const
    {
        partial_order p = TotalComp(*this, i2);
        if (p == partial_order::less)
        {
            return true;
        }
        else
            return false;
    }

    bool operator==(const Interval &i2) const
    {
        partial_order p = PartialComp(*this, i2);
        return (p == partial_order::equal);
    }

    friend partial_order PartialComp(const Interval &i1, const Interval &i2);
    friend partial_order TotalComp(const Interval &i1, const Interval &i2);
};
struct CompareIntervalp
{
    bool operator()(Interval *ip1, Interval *ip2)
    {
        return (*ip1 < *ip2);
    }
};
partial_order PartialComp(const Interval &i1, const Interval &i2)
{
    if (i2.start1 > i1.end1 && i2.start2 > i1.end2)
    {
        return partial_order::less;
    }
    else if (i1.start1 > i2.end1 && i1.start2 > i2.end2)
    {
        return partial_order::more;
    }
    else if (i1.start1 == i2.start1 && i1.start2 == i2.start2 && i1.end1 == i2.end1 && i1.end2 == i2.end2)
    {
        return partial_order::equal;
    }
    else
    {
        return partial_order::uncomp;
    }
}
partial_order TotalComp(const Interval &i1, const Interval &i2)
{
    if (i1.start1 * i1.start2 == i2.start1 * i2.start2)
    {
        return ((i1.matching > i2.matching) ? partial_order::less : partial_order::more);
    }
    else
    {
        return ((i1.start1 * i1.start2) < (i2.start1 * i2.start2) ? partial_order::less : partial_order::more);
    }
}

void generate_grid(gridType &grid, const std::vector<std::vector<Interval>> &kmp_result)
{
    const int size1 = kmp_result.size();
    const int size2 = (size1> 0) ? kmp_result[0].size() : 0;
        
    for (int i = 0; i < size1; i++)
    {
        for (int j = 0; j < size2; j++)
        {
            if (kmp_result[i][j].start1 < kmp_result[i][j].end1 && kmp_result[i][j].start2 < kmp_result[i][j].end2)
                grid[i / size_gridblock][j / size_gridblock].insert(kmp_result[i][j]);
        }
    }
}

float computeMerge(Interval& merged, const Interval& intPre, const Interval& intPost)
{
    merged.start1 = intPre.start1;
    merged.start2 = intPre.start2;
    merged.end1 = intPost.end1;
    merged.end2 = intPost.end2;

    int len_substr1 = std::max(intPre.end1 - intPre.start1, intPre.end2 - intPre.start2);
    int len_substr2 = std::max(intPost.end1 - intPost.start1, intPost.end2 - intPost.start2);
    float len_match = len_substr1 * intPre.matching + len_substr2 * intPost.matching;
    int total_len1 = intPost.end1 - intPre.start1;
    int total_len2 = intPost.end2 - intPre.start2;
    // std::cout << total_len1 << " " << len_match << " " << total_len2 <<"\n" ;
    float frac_match = (total_len1 < total_len2) ? len_match / total_len2 : len_match / total_len1;
    merged.matching = frac_match;
    return frac_match;
}

void EXtend_interval(Interval current, std::set<Interval> &s, gridType &grid)
{

    float frac = 0.6;

    Interval next;
    next.start1 = current.end1;
    next.start2 = current.end2;
    next.matching = frac - 0.00001;

    std::set<Interval> &snext = grid[next.start1 / size_gridblock][next.start2 / size_gridblock];

    auto it1 = std::upper_bound(snext.begin(), snext.end(), next);
    for (int i = 0; i < size_gridblock / 2; i++)
    {

        if (it1 == snext.end())
            break;

        Interval to_merge = *it1;
        if (PartialComp(next, to_merge) != partial_order::less)
        {
            next = to_merge;
            continue;
        }
        Interval merged;
        float frac_match = computeMerge(merged, current, to_merge);

        if (frac_match > frac)
        {
            std::set<Interval>::iterator it2 = s.find(current);
            s.erase(it2);
            s.insert(merged);
            break;
        }
    }

}

void grid_layer_compute(gridType &grid, int layer)
{

    int m = grid.size() - layer;
    int n = grid[0].size() - layer;

    for (int j = 0; j <= n; j++)
    {
        std::set<Interval> &s = grid[m][j];
        for (auto current : s)
        {
            EXtend_interval(current, s, grid);
        }
    }
    for (int i = 0; i <= m; i++)
    {
        std::set<Interval> &s = grid[i][n];
        for (auto current : s)
        {
            EXtend_interval(current, s, grid);
        }
    }
}

void EXtendMatching(gridType &grid)
{
    int num_layers = std::min(grid.size(), grid[0].size());
    for (int i = 1; i <= num_layers; i++)
    {
        grid_layer_compute(grid, i);
    }
}

std::vector<int> find_longest_match(const gridType &grid)
{
    // returns vector of size of match, index1, index2
    std::vector<int> longest(3);

    const int size1 = grid.size();
    const int size2 = grid[0].size();

    for (int i = 0; i < size1; i++)
    {
        for (int j = 0; j < size2; j++)
        {
            std::set<Interval> set_ij = grid[i][j];
            for (auto interval : set_ij)
            {

                if (interval.matching < 0.8)
                    continue;

                int len = std::max(interval.end1 - interval.start1, interval.end2 - interval.start2);
                if (len > longest[0])
                {
                    longest[0] = len;
                    longest[1] = interval.start1;
                    longest[2] = interval.start2;
                }
            }
        }
    }

    return longest;
}

void KMP_Table(std::vector<int> &pattern, std::vector<int> &kmp_array)
{
    int i = 1;
    int j = 0;
    kmp_array[0] = -1;
    const int pat_size = pattern.size();
    while (i < pat_size)
    {
        if (pattern[i] != pattern[j])
        {
            kmp_array[i] = j;
            while (j >= 0 && pattern[i] != pattern[j])
            {
                j = kmp_array[j];
            }
        }
        else
        {
            kmp_array[i] = kmp_array[j];
        }
        i++;
        j++;
    }
    kmp_array[pat_size] = j;
    return;
}

void KMP(std::vector<int> &text, std::vector<int> &pattern, std::set<int> &start_indices)
{
    int i = 0;
    int j = 0;

    const int pat_size = pattern.size();
    const int text_size = text.size();

    std::vector<int> kmptable(pat_size + 1);
    KMP_Table(pattern, kmptable);
    while (i < text_size)
    {
        if (pattern[j] == text[i])
        {
            i++;
            j++;
            if (j == pat_size)
            {
                start_indices.insert(i - j);
                j = kmptable[j];
            }
        }
        else
        {
            j = kmptable[j];
            if (j < 0)
            {
                i++;
                j++;
            }
        }
    }

    return;
}

int match_kmp_length(int pattern_size, std::set<int> &start_indices)
{
    int length = 0;
    int start;
    int end;
    bool first = true;
    for (int index : start_indices)
    {
        if (first)
        {
            start = index;
            end = index + pattern_size - 1;
            first = false;
        }
        if (index <= end)
        {
            end = index + pattern_size - 1;
        }
        else
        {
            length += end - start + 1;

#ifdef DEBUG
            std::cout << "start: " << start << " end: " << end << std::endl;
#endif
            start = index;
            end = index + pattern_size - 1;
        }
    }
    length += end - start + 1;
#ifdef DEBUG
    std::cout << "start: " << start << " end: " << end << std::endl;
#endif
    return length;
}

// Taken from https://www.geeksforgeeks.org/z-algorithm-linear-time-pattern-searching-algorith with slight modifications
int GetZArray(int idx, const std::vector<int> &str, std::vector<int> &Z)
{
    int count=0;
    int n = str.size() - idx;
    int L, R, k;

    L = R = 0;
    for (int i = 1; i < n; ++i)
    {
        if (i > R)
        {
            L = R = i;
            while (R < n && str[R - L + idx] == str[R + idx])
                R++;
            Z[i] = R - L;
            R--;
        }
        else
        {
            k = i - L;
            if (Z[k] < R - i + 1)
                Z[i] = Z[k];
            else
            {
                L = i;
                while (R < n && str[R - L + idx] == str[R + idx])
                    R++;
                Z[i] = R - L;
                R--;
            }
        }
        if(Z[i] > 15)
            count++;
    }
    return count;
}

int longest_match_any_index(std::vector<int>& submission1, const std::vector<int> &submission2,
                             std::vector<std::vector<int>> &answer)
{
    int count = 0;
    // std::vector<std::vector<int>> answer(submission1.size(), std::vector<int>(submission2.size(), -1));
    int s1Size = submission1.size();
    submission1.push_back(-1);
    submission1.insert(submission1.end(), submission2.begin(), submission2.end());
    for (int i = 0; i < s1Size; i++)
    {
        std::vector<int> za(submission1.size() - i + 1, 0);
        count += GetZArray(i, submission1, za);
        std::copy(za.begin() + s1Size - i + 2, za.end(), answer[i].begin());
    }
    submission1.resize(s1Size);
    return count;
}

void ReGrid(const gridType &oldGrid, gridType &newGrid, int newGridSize, int sizeThreshold, float matchThreshold, bool extendedBlock=false)
{
    const int ogsize1 = oldGrid.size();
    const int ogsize2 = oldGrid[0].size();

    for (int i = 0; i < ogsize1; i++)
    {
        for (int j = 0; j <ogsize2; j++)
        {
            for (Interval interval : oldGrid[i][j])
            {
                int int_s = std::max(interval.end1 - interval.start1, interval.end2 - interval.start2);
                if (int_s >= sizeThreshold && interval.matching >= matchThreshold)
                {
                    int xNew = interval.start1 / newGridSize;
                    int yNew = interval.start2 / newGridSize;
                    newGrid[xNew][yNew].insert(interval);
                    if(extendedBlock)
                    {
                        if(xNew - 1 >= 0 )
                        {
                            newGrid[xNew-1][yNew].insert(interval);
                            if(yNew - 1 >= 0)
                            {
                                newGrid[xNew][yNew-1].insert(interval);
                                newGrid[xNew-1][yNew-1].insert(interval);
                            }
                        }
                        else if(yNew - 1 >= 0)
                        {
                            newGrid[xNew][yNew-1].insert(interval);
                        }
                    }
                }
            }
        }
    }
}

void GetIntervalFromMatch(std::vector<std::vector<int>>& match, std::vector<std::vector<Interval>>& grid,int size1,int size2)
{
    for (int i = 0; i < size1; i++)
    {
        for (int j = 0; j < size2; j++)
        {
            Interval in;
            in.start1 = i;
            in.start2 = j;
            in.end1 = i + match[i][j];
            in.end2 = j + match[i][j];
            in.matching = 1;
            grid[i][j] = in;
        }
    }
}


std::vector<int> GetApproxMatch(std::vector<int> &submission1, std::vector<int> &submission2, std::vector<std::vector<int>>& match)
{
    int iteration = 0;
    int max_iterations = 5;
    size_gridblock = 5;
    int min_size = 1;
    std::vector<std::vector<Interval>> z_result(submission1.size(), std::vector<Interval>(submission2.size()));
    // longest_match_any_index(submission1, submission2, z_result);
    GetIntervalFromMatch(match, z_result,submission1.size(), submission2.size());
    gridType grid(submission1.size() / size_gridblock + 1,
                  std::vector<std::set<Interval>>(submission2.size() / size_gridblock + 1));
    generate_grid(grid, z_result);
    z_result.clear();
    EXtendMatching(grid);
    std::vector<int> result_base = find_longest_match(grid);
    size_gridblock = 30;
    min_size = 20;
    while (iteration < max_iterations)
    {

#ifdef DEBUG
        std::cout << "Starting Iteration " << iteration << "\n";
        std::cout << "min_size = " << min_size << " " << "size_gridblock = " << size_gridblock << "\n";
#endif

        // re-grid
        gridType newGrid(submission1.size() / size_gridblock + 1,
                         std::vector<std::set<Interval>>(submission2.size() / size_gridblock + 1));
        ReGrid(grid, newGrid, size_gridblock, min_size, 0.3, iteration >= 2);
        grid = std::move(newGrid);

        EXtendMatching(grid);
        std::vector<int> result_iter = find_longest_match(grid);
        if (result_iter[0] > result_base[0])
        {
            result_base = result_iter;
        }

#ifdef DEBUG
        std::cout << "GRID AFTER MERGING\n";

        for (auto x : grid)
        {
            for (auto s : x)
            {
                std::cout << s.size() << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";

#endif

        iteration++;
        size_gridblock *= 2;
        size_gridblock = std::max(size_gridblock, result_base[0] / 4);
        min_size = 0.5 * size_gridblock;

#ifdef DEBUG

        std::cout << "This Iteration Result: " << result_iter[0] << " " << result_iter[1] << " " << result_iter[2] << "\n";

#endif

        if (result_base[0] < min_size)
            break;
    }
    return result_base;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1,
                                     std::vector<int> &submission2)
{

    std::array<int, 5> result = {0, 0, 0, 0, 0};

    const float exThreshold = 0.2;
    const float apThreshold = 0.2;
    const int exAbThreshold = 75;

    const int sub1Size = submission1.size();
    const int sub2Size = submission2.size();

    std::vector<std::vector<int>> zArr(sub1Size, std::vector<int>(sub2Size,0));
    int count_match = longest_match_any_index(submission1, submission2, zArr);
#ifdef DEBUG
    // Printing Tokens
    for (auto x : submission1)
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    for (auto x : submission2)
    {
        std::cout << x << " ";
    }
    std::cout << "\n";

    std::cout<<submission1.size()<<" "<<submission2.size()<<std::endl;

    std::cout<<submission1.size()<<" "<<submission2.size()<<std::endl;
#endif //DEBUG
    // Detecting short matches

	int pattern_size = 10;
    std::set<int> all_start_indices1;
    std::set<int> all_start_indices2;
    for(int i=0;i<sub1Size;i++)
    {
        for(int j=0;j<sub2Size;j++)
        {
            if(zArr[i][j] >= pattern_size)
            {
                all_start_indices1.insert(i);
                all_start_indices2.insert(j);
            }
        }
    }


    // for (int i = 0; i < submission2.size() - pattern_size + 1; i++)
    // {
    //     std::vector<int> pattern(submission2.begin() + i, submission2.begin() + i + pattern_size);
    //     KMP(submission1, pattern, all_start_indices);
    // }
    int answer = 0;
    if(!all_start_indices1.empty())
        answer = match_kmp_length(pattern_size, all_start_indices1);

#ifdef DEBUG
    for(int interate : all_start_indices){
        std::cout<<interate<<" ";
    }
    std::cout<<std::endl;
    std::cout<<answer<<std::endl;
#endif

    // for (int i = 0; i < submission1.size() - pattern_size + 1; i++)
    // {
    //     std::vector<int> pattern(submission1.begin() + i, submission1.begin() + i + pattern_size);
    //     KMP(submission2, pattern, all_indices2);
    // }
    int answer1 = 0;
    if(!all_start_indices2.empty())
        answer1 = match_kmp_length(pattern_size, all_start_indices2);

#ifdef DEBUG
    for(int interate : all_indices2){
        std::cout<<interate<<" ";
    }
    std::cout<<std::endl;
    std::cout<<answer1<<std::endl;
#endif

    result[1] = std::min(answer,answer1);

    // // Detecting long match

    std::vector<int> longest_match = {0,0,0};
    if(sub1Size > 0 && sub2Size > 0)
        longest_match = GetApproxMatch(submission1, submission2,zArr);

    result[2] = longest_match[0];
    result[3] = longest_match[1];
    result[4] = longest_match[2];

    int exactMatch = result[1];
    int appMatch = result[2];

    if (exactMatch > sub1Size * exThreshold|| exactMatch > sub2Size * exThreshold || 
         appMatch > sub1Size * apThreshold|| appMatch > sub2Size * apThreshold ||
         exactMatch > exAbThreshold )
        result[0] = 1;

    // for (int i = 0; i < 5; i++)
    //     std::cout << result[i] << " ";
    // std::cout << std::endl;

    return result;
    // End TODO 
}
