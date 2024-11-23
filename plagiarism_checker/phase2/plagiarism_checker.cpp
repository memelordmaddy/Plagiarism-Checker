#include "plagiarism_checker.hpp"
#include "../tokenizer.hpp"

// RollingHash Implementation
RollingHash::RollingHash(int base, int mod) : base(base), mod(mod), base_power(1) {}

void RollingHash::add_token(int token) {
    hash = (hash * base + token) % mod;
    tokens.push_back(token);
    if (tokens.size() > window_size) {
        hash = (hash - tokens.front() * base_power % mod + mod) % mod;
        tokens.pop_front();
    } else {
        base_power = (base_power * base) % mod;
    }
}

int RollingHash::get_hash() const {
    return hash;
}

void RollingHash::set_window_size(int size) {
    window_size = size;
}

// Plagiarism Checker Implementation
plagiarism_checker_t::plagiarism_checker_t()
    : stop_worker(false), worker_thread(&plagiarism_checker_t::process_flags, this) {}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : plagiarism_checker_t() {
    for (auto& submission : __submissions) {
        add_submission(submission);
    }
}

plagiarism_checker_t::~plagiarism_checker_t() {
    stop_worker = true;
    cv.notify_all();
    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    std::vector<int> tokens;
    try {
        tokenizer_t tokenizer(__submission->codefile);
        tokens = tokenizer.get_tokens();
    } catch (...) {
        // Handle tokenization failure gracefully
        return;
    }

    std::unique_lock<std::mutex> lock(mutex);
    submissions.push({__submission, tokens});
    cv.notify_all();
}

void plagiarism_checker_t::process_flags() {
    while (!stop_worker) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return stop_worker || !submissions.empty(); });

        while (!submissions.empty()) {
            auto data = submissions.front();
            submissions.pop();
            lock.unlock();
            check_plagiarism(data);
            lock.lock();
        }
    }
}

void plagiarism_checker_t::check_plagiarism(const submission_data_t& data) {
    auto& tokens = data.tokens;
    auto& submission = data.submission;

    RollingHash rh_75(BASE, MOD), rh_15(BASE, MOD);
    rh_75.set_window_size(WINDOW_75);
    rh_15.set_window_size(WINDOW_15);

    int matches_75 = 0, matches_15 = 0;
    std::unordered_set<std::shared_ptr<submission_t>> sources;

    for (int token : tokens) {
        rh_75.add_token(token);
        rh_15.add_token(token);

        // Exact match (length 75)
        if (tokens.size() >= WINDOW_75) {
            int hash_75 = rh_75.get_hash();
            if (pattern_store_75[hash_75].count(submission)) {
                matches_75++;
                sources.insert(pattern_store_75[hash_75].begin(),
                               pattern_store_75[hash_75].end());
            }
            pattern_store_75[hash_75].insert(submission);
        }

        // Patchwork detection (length 15)
        if (tokens.size() >= WINDOW_15) {
            int hash_15 = rh_15.get_hash();
            if (pattern_store_15[hash_15].count(submission)) {
                matches_15++;
                sources.insert(pattern_store_15[hash_15].begin(),
                               pattern_store_15[hash_15].end());
            }
            pattern_store_15[hash_15].insert(submission);
        }
    }

    if (matches_75 >= 10 || sources.size() >= 20) {
        flag_submission(submission);
    }
}

void plagiarism_checker_t::flag_submission(std::shared_ptr<submission_t> submission) {
    if (submission->student) {
        submission->student->flag_student(submission);
    }
    if (submission->professor) {
        submission->professor->flag_professor(submission);
    }
}