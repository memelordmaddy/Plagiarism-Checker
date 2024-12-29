#include "structures.hpp"
// -----------------------------------------------------------------------------
#include <vector>
#include <cmath> // check
#include <map>
#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
//---------------------
#include <iostream>

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
    std::chrono::time_point<std::chrono::steady_clock> initial_time;

    std::vector<long long int> rolling_hash(std::vector<int> &input, int window_length);
    std::map<long long int, std::vector<long int>> map_75_hashes;
    std::map<long long int, std::vector<long int>> map_15_hashes;
    std::map<long, double> submission_times;
    std::map<long, std::shared_ptr<submission_t>> submission_id_to_ptr;
    void add_new_hash(std::vector<long long int> & input_hashes, long submission_id, std::map<long long int, std::vector<long>> & map_of_hash_values);
    bool exact75_check(long current_submission_id, std::vector<long long int> & input_hashes, std::map<long long int, std::vector<long>> & map_of_hash_values);
    bool exact15_check(long current_submission_id, std::vector<long long int> & input_hashes, std::map<long long int, std::vector<long>> & map_of_hash_values);

    // threading stuff
    std::mutex mtx;

    std::thread worker;
    std::queue<std::shared_ptr<submission_t>> sub_queue;
    bool stop;
    std::condition_variable cv;

    std::map<long, bool> flagged;
    void handle_submission();
    // End TODO
};
