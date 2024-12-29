#include <iostream>
#include <vector>
#include <span>
#include <array>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <limits.h>

// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

struct segment {
    int i;
    int j;
    int length_i;
    int length_j;
    int score;

    segment(int i, int j, int length_i, int length_j, int score)
        : i(i), j(j), length_i(length_i), length_j(length_j), score(score) {}
};

std::ostream& operator<<(std::ostream& os, const segment& seg) {
    os << "Segment(i: " << seg.i 
       << ", j: " << seg.j 
       << ", length_i: " << seg.length_i
       << ", length_j: " << seg.length_j 
       << ", score: " << seg.score 
       << ")";
    return os;
}

class PlagChecker {
private:
    const std::vector<int>& seq1;
    const std::vector<int>& seq2;
    const int m, n;
    std::vector<segment> exact_match_segments;

    std::pair<int, std::pair<int, int>> needleman_wunsch_with_boost(
        std::pair<int&, int&> start, std::pair<int&, int&> end,
        int start_boost = 0, int match = 1, int mismatch_penalty = -4
    ) {
        // The above algorithm is a slight modification of the 'Needleman Wunsch algorithm' which finds the best fit 
        // for the given strings managing matches, mismatches and gaps to maximise the score. The slight modification
        // is to give a offset of score bump at the start so as to get the point where the score becomes zero.
        // 
        // the arguments are the starting and the ending points of the required subsequence and optional start boost 
        // as mentioned above. This function returns the ending score and the point where the score becomes zero (again
        // if starting at 0) and if not found, returns {-1, -1}

        const int& i_start = start.first;   const int& j_start = start.second;
        const int& i_end = end.first;       const int& j_end = end.second;
        int alignment_score;
        std::pair<int, int> zero_crossing_index = {-1, -1};
 
        if (i_start == i_end) {
            // only gaps along a coloumn
            alignment_score =  -4 * (j_end - j_start);
            if(alignment_score + start_boost <=0)
                zero_crossing_index = {i_start, (start_boost/4) + j_start};

        } else if (j_start == j_end) {
            // only gaps along a row
            alignment_score =  mismatch_penalty-4 * (i_end - i_start);
            if(alignment_score + start_boost <=0)
                zero_crossing_index = {j_start, (start_boost/4) + i_start};
        } else {
            // else
            int n_local = i_end - i_start; // Length of the first subsequence segment
            int m_local = j_end - j_start; // Length of the second subsequence segment
            
            std::vector<int> prev_row(m_local + 1, 0);
            std::vector<int> curr_row(m_local + 1, 0);
            
            prev_row[0] = start_boost;
            for (int j = 1; j <= m_local; ++j) { prev_row[j] = prev_row[j - 1] + mismatch_penalty; } // Initialize the first row
                    
            for (int i = 1; i <= n_local; ++i) {
                curr_row[0] = prev_row[0] + mismatch_penalty; // Initialize first column of current row
                
                for (int j = 1; j <= m_local; ++j) {                    
                    int match_mismatch = prev_row[j - 1] + (
                        (this->seq1[i_start + i - 1] == this->seq2[j_start + j - 1]) ? match : mismatch_penalty
                    );
                    int gap_seq1 = prev_row[j] + mismatch_penalty;
                    int gap_seq2 = curr_row[j - 1] + mismatch_penalty;
                    
                    curr_row[j] = std::max({match_mismatch, gap_seq1, gap_seq2});
                    
                    // Update cumulative score and check for zero crossing
                    if (curr_row[j] == 0 && zero_crossing_index.first == -1) { zero_crossing_index = {i, j}; }
                }                
                prev_row = curr_row;
            }
            alignment_score = curr_row[m_local];
        }
    
        return {alignment_score, zero_crossing_index}; // Return as a pair
    }

    std::vector<segment> _filter_matching_subpatterns() {
        // this function takes the this->exact_match_segments which might have segments out of order (the positions j)
        // as it is possible to have matching segments back of the previous matching segments in order and thus
        // it is important to pick the subsequence which has the corresponding segments in both (seq1 & seq2) in order
        // with the maximum sum of lengths. Running time is O(n^2) 

        if (exact_match_segments.empty()) return {}; 

        int n_local = this->exact_match_segments.size();
        std::vector<int> dp(n_local);   // stores the maximum total length possible ending at index i
        std::vector<int> prev(n_local, -1); // prev[i] stores the previous index in the optimal sequence ending at i
        
        dp[0] = exact_match_segments[0].length_i;   // Initialize first segment
        
        for(int i = 1; i < n_local; i++) {
            // loop inviariant: dp[i] and prev[i] contain lengths and prev index upto i
            dp[i] = exact_match_segments[i].length_i;            
            for(int j = 0; j < i; j++) {
                // If current segment's j is greater than previous segment's j
                // and including it gives better total length
                if(
                    exact_match_segments[i].j > exact_match_segments[j].j && 
                    dp[j] + exact_match_segments[i].length_i > dp[i]
                ){
                    dp[i] = dp[j] + exact_match_segments[i].length_i;
                    prev[i] = j;
                }
            }
        }
        
        int maxIdx = 0;
        for(int i = 1; i < n_local; i++) { if(dp[i] > dp[maxIdx]) maxIdx = i; }
        
        // Reconstruct the sequence
        std::vector<segment> filtered_matches;
        for(int i = maxIdx; i != -1; i = prev[i]) { filtered_matches.push_back(exact_match_segments[i]); }
        
        // Reverse to get segments in correct order
        std::reverse(filtered_matches.begin(), filtered_matches.end());
        
        return filtered_matches;
    }  

    std::pair<int, int> _max_weight_subarray(const std::vector<segment>& segments) {
        // given the vector of segments, this returns the optimum bounds for which the sum of scores is non negative
        // and the length is maximised
        // note: the end i.e. bound.second is non inclusive

        int segment_count = segments.size();
        if (segment_count == 0) return {{}, 0};
        
        // Build prefix sums for scores and weights
        std::vector<int> score_prefix(segment_count + 1, 0);
        std::vector<int> weight_prefix(segment_count + 1, 0);
        
        for (int i = 0; i < segment_count; ++i) {
            score_prefix[i + 1] = score_prefix[i] + segments[i].score;
            weight_prefix[i + 1] = weight_prefix[i] + std::max(segments[i].length_i,segments[i].length_j);
        }
        
        // Map to store the earliest index for each score prefix
        std::unordered_map<int, int> min_index_for_score;
        
        int best_weight = 0;
        int best_start = 0;
        int best_end = 0;
        
        // Check all possible end points
        for (int end = 0; end <= segment_count; ++end) {
            int curr_score = score_prefix[end];
            
            // Iterate over prefix scores and find the longest subarray with non-negative score
            for (const auto& [prefix_score, start] : min_index_for_score) {
                if (prefix_score <= curr_score) {
                    int weight = weight_prefix[end] - weight_prefix[start];
                    
                    if (weight > best_weight) {
                        best_weight = weight;
                        best_start = start;
                        best_end = end;
                    }
                }
            }
            
            // Add the current prefix sum to the map if it's the earliest occurrence
            if (min_index_for_score.find(curr_score) == min_index_for_score.end() || end < min_index_for_score[curr_score]) {
                min_index_for_score[curr_score] = end;
            }
        }
        
        // Collect the best subarray
        std::vector<segment> best_subarray(segments.begin() + best_start, segments.begin() + best_end);
        
        return {best_start, best_end};
    }

    std::vector<segment> get_remaining_segments() {
        std::vector<segment> filtered_matches = _filter_matching_subpatterns();
        std::vector<segment> segments;

        // Process each match and gap as separate segments
        segment& current_match = filtered_matches[0];

        int gap_start_1 = 0;
        int gap_end_1 = current_match.i;
        int gap_start_2 = 0;
        int gap_end_2 = current_match.j;

        std::pair<int&, int&> gap_start(gap_start_1, gap_start_2);
        std::pair<int&, int&> gap_end(gap_end_1, gap_end_2);
        int nw_result = needleman_wunsch_with_boost(gap_start, gap_end).first;

        segments.push_back(
            segment(gap_start_1, gap_start_2, gap_end_1 - gap_start_1, gap_end_2 - gap_start_2, nw_result)
        );

        for (size_t i = 1; i < filtered_matches.size() ; ++i) {
            segments.push_back(current_match);
            segment& next_match = filtered_matches[i];

            // Calculate the gap positions
            gap_start_1 = current_match.i + current_match.length_i;
            gap_end_1 = next_match.i;
            gap_start_2 = current_match.j + current_match.length_j;
            gap_end_2 = next_match.j;

            // Compute the Needleman-Wunsch score for the gaps inbetween
            gap_start.first = gap_start_1; gap_start.second = gap_start_2;
            gap_end.first = gap_end_1; gap_end.second = gap_end_2;
            nw_result = needleman_wunsch_with_boost( gap_start, gap_end ).first;

            // Append the gap as its own segment
            segments.push_back(
                segment(gap_start_1, gap_start_2, gap_end_1 - gap_start_1, gap_end_2 - gap_start_2, nw_result)
            );

            current_match = next_match;
        }

        // Include the last match as its own segment
        segments.emplace_back(filtered_matches.back());

        gap_start_1 = current_match.i + current_match.length_i;
        gap_end_1 = this->m;
        gap_start_2 = current_match.j + current_match.length_j;
        gap_end_2 = this->n;

        gap_start.first = gap_start_1; gap_start.second = gap_start_2;
        gap_end.first = gap_end_1; gap_end.second = gap_end_2;
        nw_result = needleman_wunsch_with_boost(gap_start, gap_end).first;
        
        segments.push_back(
            segment(gap_start_1, gap_start_2, gap_end_1 - gap_start_1, gap_end_2 - gap_start_2, nw_result)
        );

        // Step 3: Output segments for review
        // for (const auto& segment : segments) { std::cout<<segment<<std::endl; }

        return segments;
    }

public:
    PlagChecker(const std::vector<int>& seq1, const std::vector<int>& seq2)
        : seq1(seq1), seq2(seq2), m(seq1.size()), n(seq2.size()) {}

    int get_exact_matches(const int len_threshold = 10){
        // returns the total sum of the exact matching patterns with the minimum length threshold = 10
        // and also stores the segments of the match in the 'exact segments match' 
        // note: run this function before get_approximate_matches()

        std::vector<bool> seq2_visited(n, false);
        int exact_matches_length = 0;

        int i = 0;
        while(i < m - len_threshold + 1){
            int j=0;

            while(j < n - len_threshold + 1){
                // checks for a valid 'j' which is not visited and matches with 'i'
                if(seq2_visited[j] || seq1[i] != seq2[j]){
                    j++; 
                    continue;
                }   

                int j_traverser = j;
                int i_traverser = i;
                
                // traverse untill either there is a mismatch or seq2[j] is already visited 
                while(
                    i_traverser < m && j_traverser < n &&
                    !seq2_visited[j] && 
                    seq1[i_traverser] == seq2[j_traverser]
                ){ i_traverser++; j_traverser++; }   

                // if matched length is >= threshold, add and then break out of the second while loop
                int match_length = i_traverser - i;
                if(match_length >= len_threshold){
                    for(int k=j; k<j_traverser; k++) { seq2_visited[k] = true; }

                    exact_matches_length += match_length;
                    segment exact_match_seg(i, j, match_length, match_length, match_length);
                    this->exact_match_segments.push_back(exact_match_seg);

                    i = i_traverser-1;
                    break;
                }   

                // if a exact match length is not greater than threshold
                j++;
            }
            i++;
        }
        return exact_matches_length;
    }

    segment get_approximate_match(){
        // returns the segment corresponding to the longest approximate match of the two given sequences
        // the segment returned will have the score '0' i.e. uninitialised, i and j will the start indexes in the 
        // seq1 and seq2, length_i and length_j are the lengths of the approximate matches in seq1 and seq2
        // 
        // note: run this only after the 'get_exact_matches()' so that the exact_match_segments is properly created
        
        std::vector<segment> segments = get_remaining_segments();
        std::pair<int,int> bounds = _max_weight_subarray(segments);

        int &start = bounds.first;  int& end = bounds.second;
        
        int length_i = 0; int length_j = 0;
        int score = 0;
        for(int i=start; i<end; i++){
            score += segments[i].score;
            length_i += segments[i].length_i;
            length_j += segments[i].length_j;
        }

        // std::cout<<"initial length_i: "<<length_i<<" initial length_i: "<<length_j<<std::endl;
        // std::cout<<"leftover score: "<<score<<std::endl;

        // the incorr_ratio or the incorrect ratio is the ratio of the negative score of the surrounding 
        // segments of the bounds to its length (max of the length_i and length_j). They remain INT_MIN if 
        // no segments found outside the optimum bounds
        // lesser negative ratio can give better pattern match-length for a given score
        float incorr_ratio_before = INT_MIN;
        float incorr_ratio_after = INT_MIN;

        
        
        if(start > 0){
            segment& before_segment = segments[start-1];
            if(std::max(before_segment.length_i, before_segment.length_j) != 0){
                // if there is a segment before the optimum bound
                incorr_ratio_before = before_segment.score / static_cast<float>(std::max(before_segment.length_i, before_segment.length_j));
            }
        }
        if(end < segments.size()){
            segment& after_segment = segments[end];
            if(std::max(after_segment.length_i, after_segment.length_j) != 0){
                // if there is a segment after the optimum bound
                incorr_ratio_after = after_segment.score / static_cast<float>(std::max(after_segment.length_i, after_segment.length_j));
            }
        }
                
        if(incorr_ratio_before == INT_MIN && incorr_ratio_after == INT_MIN){
            // if no bounds found then the entire sequence is the pattern
            return segment( 0, 0, m, n, 0 );
        } else if (incorr_ratio_after > incorr_ratio_before) {
            segment& after_segment = segments[end];
            // if there is/are segments outside the optimum approx segments, we give the score left 
            // by adding the scores of all the segmens in the optimum segments. this is given as a boost to 
            // the better segment to get an index where the score becomes zero. we will update this new set of 
            // indices. 
            // note: there is no possibility where the index returned be {-1,-1} i.e. not found. This will imply
            // that in the best path, the scores in the cells doesnt become zero which is a contradiction to the 
            // _max_weight_subarray function
            int gap_end_i = after_segment.i + after_segment.length_i; 
            int gap_end_j = after_segment.j + after_segment.length_j;
            std::pair<int&, int&> gap_start(after_segment.i , after_segment.j);
            std::pair<int&, int&> gap_end(gap_end_i, gap_end_j);
            std::pair<int,int> new_coeff_end = needleman_wunsch_with_boost(gap_start, gap_end, score).second;
                        
            return segment(
                segments[start].i,
                segments[start].j,
                new_coeff_end.first + segments[end].i - segments[start].i,
                new_coeff_end.second + segments[end].j - segments[start].j, 
                0
            );
        } else {
            segment& before_segment = segments[start-1];
            int gap_end_i = before_segment.i + before_segment.length_i; 
            int gap_end_j = before_segment.j + before_segment.length_j;
            std::pair<int&, int&> gap_start(before_segment.i , before_segment.j);
            std::pair<int&, int&> gap_end(gap_end_i, gap_end_j);
            std::pair<int,int> new_coeff_end = needleman_wunsch_with_boost(gap_start, gap_end, - score - before_segment.score).second;
            
            return segment(
                new_coeff_end.first,
                new_coeff_end.second,
                segments[end].i - new_coeff_end.first,
                segments[end].j - new_coeff_end.second, 
                0
            );
        }
    }

    bool is_plagged(const int &exact_match_count, const int& approx_match_i, const int& approx_match_j){
        float a = 0.6;
        float b = 0.2;
        float c = 0.2;
        float threshold = 0.4;

        float result = (a*exact_match_count)/(std::min(m, n)) + (b*approx_match_i)/m + (c*approx_match_j)/n;
        std::cout<< "combined result: "<<result<<"/1.0 (threshold : "<<threshold<<")"<<std::endl;
        return result >= threshold;
    }

    std::array<int, 5> getPlagParameters(){
        std::array<int, 5> params;
        params[1] = get_exact_matches();
        segment approx_segment = get_approximate_match();
        params[2] = std::max(approx_segment.length_i, approx_segment.length_j);
        params[3] = approx_segment.i;
        params[4] = approx_segment.j;
        params[0] = is_plagged(params[1], approx_segment.length_i, approx_segment.length_j);

        // // uncomment below to check the parameters
        // std::cout
        //     << "flag:\t" << params[0]
        //     << "\nexact:\t" << params[1]
        //     << "\napprox:\t" << params[2]
        //     << "\ni:\t" << params[3]
        //     << "\nj:\t" << params[4] 
        // <<std::endl;

        return params;
    }
};

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    PlagChecker plgchk(submission1, submission2);
    return plgchk.getPlagParameters();
}
