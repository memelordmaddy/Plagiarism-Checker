#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <map>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

int perfect_match_counter(std::vector<int> &A, std::vector<int> &B){

    int Size1 = A.size();
    int Size2 = B.size();
    int matchCount = 0;
    std::vector<bool> matched1(Size1, false);
    std::vector<bool> matched2(Size2, false);

    for (int i = 0; i < Size1; ++i) {
        for (int j = 0; j < Size2; ++j) {

            int matchLength = 0;
            bool overlap = false;

            while (i + matchLength < Size1 && j + matchLength < Size2 && A[i + matchLength] == B[j + matchLength])
            {
                if(matched1[i + matchLength] || matched2[j + matchLength])
                {
                    overlap = true;
                    break;
                }
                matchLength++;
            }

            if (matchLength >= 10 && overlap==false)
            {
                matchCount += matchLength; 
                for (int o = 0; o < matchLength; o++)
                {
                    matched1[o+i] = true;
                    matched2[o+j] = true;
                }
            }

        }
    }
    return matchCount;

}

double plag_bound(double x)
{
    if(x>=5000)return 200.0;
    double a = -1.0 / 64000;
    double b = 9.0 / 80;
    double c = 225.0 / 8;
    return std::max(50.0,std::min(200.0,a * x * x + b * x + c));
}
bool Smart_plag_detector(double exact,double approx,double size)
{
    if(exact*5>=2*size)
    {
        double approx_bound=std::max(30.0,std::min(50.0,30.0+20.0*(size-200)/800.0));
        if(approx>=approx_bound)return true;
    }
    return false;
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2)
{
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    int Size1 = submission1.size();
    int Size2 = submission2.size();

// Calculation for result[1] (Total match length)
    result[1] = perfect_match_counter(submission1,submission2);


// Calculation for result[2], result[3] and result[4]

    int best_length = 0;
    int best_start1 = 0;
    int best_start2 = 0;

    std::vector<std::vector<bool>> Visited (Size1+1, std::vector<bool> (Size2+1, 0));
    std::map<int, std::vector<int>> Positions_to_be_checked;

    for(int i=0;i<Size2;i++)
    {
        Positions_to_be_checked[submission2[i]].push_back(i);
    }

    for(int i=0;i<Size1-29;i++)
    {
        for(int j: Positions_to_be_checked[submission1[i]])
        {
            if(Visited[i][j]==1)continue;
            int Correctly_matched = 0;
            int approx_length = 0;
            while(i+approx_length<Size1 && j+approx_length<Size2 && approx_length<29)
            {
                if(submission1[i+approx_length] == submission2[j+approx_length])
                {
                    Correctly_matched++;
                    approx_length++;
                }
                else approx_length++;
                
            }
            while(i+approx_length<Size1 && j+approx_length<Size2)
            {
                if(submission1[i+approx_length] == submission2[j+approx_length])
                {
                    Correctly_matched++;
                    approx_length++;
                    if(5*Correctly_matched>=4*approx_length)
                    {
                        if(approx_length>best_length)
                        {
                            best_length = approx_length;
                            best_start1 = i;
                            best_start2 = j;
                            Visited[i+approx_length][j+approx_length]=1;
                        }
                    }
                    if(approx_length>60 && 4*Correctly_matched<approx_length) break;
                }
                else approx_length++;
            }
        }
    }

    result[2]=best_length;
    result[3]=best_start1;
    result[4]=best_start2;

// Calculation for result[0]
    int minsize=std::min(Size1,Size2);
    double allowed_bound = plag_bound(1.0*std::min(Size1, Size2));
    result[0] = (result[2] >= allowed_bound);
    result[0] = result[0] || (2*result[1]>= minsize) || (Smart_plag_detector(1.0*result[1],1.0*result[2],1.0*minsize));
    return result;
}