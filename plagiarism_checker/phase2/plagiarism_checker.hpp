#ifndef PLAGIARISM_CHECKER_HPP
#define PLAGIARISM_CHECKER_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "structures.hpp"

// Helper class for rolling hash computation
class RollingHash {
public:
    RollingHash(int base, int mod);
    void add_token(int token);
    int get_hash() const;
    void set_window_size(int size);

private:
    int base, mod, hash = 0, base_power = 1, window_size = 0;
    std::deque<int> tokens;
};

// Plagiarism Checker Class
class plagiarism_checker_t {
public:
    plagiarism_checker_t();
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions);
    ~plagiarism_checker_t();

    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    struct submission_data_t {
        std::shared_ptr<submission_t> submission;
        std::vector<int> tokens;
    };

    std::queue<submission_data_t> submissions; // Queue for new submissions
    std::unordered_map<int, std::unordered_set<std::shared_ptr<submission_t>>> pattern_store_75; // Exact match store
    std::unordered_map<int, std::unordered_set<std::shared_ptr<submission_t>>> pattern_store_15; // Patchwork match store
    std::mutex mutex; // Protect shared data
    std::condition_variable cv; // Condition variable for worker thread
    std::atomic<bool> stop_worker; // Stop signal for worker thread
    std::thread worker_thread; // Background thread

    const int BASE = 257;        // Rolling hash base
    const int MOD = 1000000007;  // Rolling hash modulus
    const int WINDOW_75 = 75;    // Length for exact match
    const int WINDOW_15 = 15;    // Length for patchwork match

    void process_flags(); // Worker thread function
    void check_plagiarism(const submission_data_t& data);
    void flag_submission(std::shared_ptr<submission_t> submission);
};

#endif // PLAGIARISM_CHECKER_HPP

