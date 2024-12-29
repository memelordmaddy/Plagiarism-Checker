#include "structures.hpp"
#include <vector>
#include <thread>
#include<chrono>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <set>
#include<atomic>
#include<unordered_map>

// struct pair_hash {
//     template <typename T1, typename T2>
//     std::size_t operator()(const std::pair<T1, T2>& p) const {
//         // Combine the hashes of both elements
//         return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
//     }
// };

// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and classes here

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> 
                            __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // TODO: Add members and function signatures here
    std::vector<std::pair<std::shared_ptr<submission_t>, std::chrono::system_clock::duration>> submissions;
    std::vector<std::vector<long long int>> database;
    std::vector<std::thread> current_threads;
    std::vector<std::vector<bool>>comparison_table;
    std::vector<std::vector<int>>global_matches;
    std::vector<bool>database_written;
    std::vector<bool>plagged;
    std::vector<int>get_tokens(const std::string& file_path);
    std::chrono::system_clock::time_point start_time;
    int base_count;
    int submission_count;
    bool running;
    void primary_comparator(int submission_id1,int submission_id2);
    void plag_caller(int submission_id);
    int secondary_comparator(int submission_id1,int submissionid2, int i, int j);
    void global_data_writer();
    void global_comparator();

    // mutex
    std::mutex mtx;
    // std::vector<bool>check;
    // std::unordered_map<long long int,std::vector<std::pair<int,int>>>global_database;
    // std::unordered_map<std::pair<int,int>,std::vector<std::pair<int,int>>,pair_hash>local_matches;
    // std::unordered_map<int,std::set<int>>patchwork_matches;
    // End TODO
};
