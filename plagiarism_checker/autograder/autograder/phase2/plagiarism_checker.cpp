#include "plagiarism_checker.hpp"
#define PRIME 31
#define MAX_FILE_COUNT 1000

// You should NOT add ANY other includes to this file.
void plagiarism_checker_t::plag_caller(int submission_id){
//a function to flag a submission if it is plagiarised after checking it's not a base submission
    if(submission_id>=base_count and !plagged[submission_id]){
        plagged[submission_id] = true;
        if (submissions[submission_id].first->student != nullptr) {
            submissions[submission_id].first->student->flag_student(submissions[submission_id].first);
        }
        if (submissions[submission_id].first->professor != nullptr) {
            submissions[submission_id].first->professor->flag_professor(submissions[submission_id].first);
        }
    }
}

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    submissions[submission_count] = (std::make_pair(__submission, std::chrono::system_clock::now() - start_time));
    submission_count++;
}

plagiarism_checker_t::plagiarism_checker_t() {
    start_time = std::chrono::system_clock::now();//stores the start of the class for relative time of submissions
    base_count = 0;//stores the number of base submissions
    submission_count = 0;//submission_counter
    running = true;//a variable that keeps the threads running
    submissions.resize(MAX_FILE_COUNT);//a vector of pairs of submissions and their time of submission
    database.resize(MAX_FILE_COUNT);//a vector of vectors of hashed tokens of submissions
    global_matches.resize(MAX_FILE_COUNT);//matches for patchwork verification over global database
    database_written.resize(MAX_FILE_COUNT, false);//a vector to check if the database has been written for a submission
    comparison_table.resize(MAX_FILE_COUNT);//a table to check if the comparison has been done for a pair of submissions
    plagged.resize(MAX_FILE_COUNT, false);//a vector to check if a submission has been flagged
    for (int i = 0; i < MAX_FILE_COUNT; ++i) {
        comparison_table[i].resize(MAX_FILE_COUNT, 0);
        comparison_table[i][i] = 1;
    }
    current_threads.push_back(std::thread(&plagiarism_checker_t::global_data_writer, this));
    current_threads.push_back(std::thread(&plagiarism_checker_t::global_comparator, this));
}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions) {
    //constructor to initialise the class with a vector of submissions
    start_time = std::chrono::system_clock::now();
    base_count = __submissions.size();
    submission_count = __submissions.size();
    running = true;
    submissions.resize(MAX_FILE_COUNT);
    database.resize(MAX_FILE_COUNT);
    global_matches.resize(MAX_FILE_COUNT);
    database_written.resize(MAX_FILE_COUNT, false);
    comparison_table.resize(MAX_FILE_COUNT);
    plagged.resize(MAX_FILE_COUNT, false);
    for (int i = 0; i < MAX_FILE_COUNT; ++i) {
        if (i < __submissions.size()) {
            comparison_table[i].resize(MAX_FILE_COUNT, 1);
        } else {
            comparison_table[i].resize(MAX_FILE_COUNT, 0);
            comparison_table[i][i] = 1;
        }
    }
    for (int i = 0; i < __submissions.size(); i++) {
        auto submission = __submissions[i];
        submissions[i] = std::make_pair(submission, std::chrono::seconds(0));
    }
    current_threads.push_back(std::thread(&plagiarism_checker_t::global_data_writer, this));
    current_threads.push_back(std::thread(&plagiarism_checker_t::global_comparator, this));
}

plagiarism_checker_t::~plagiarism_checker_t() {
    //destructor to join the threads
    running = false;
    for (auto& thread : current_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

std::vector<int> plagiarism_checker_t::get_tokens(const std::string& file_path) {
    //a function to get the tokens from a file
    tokenizer_t tokenizer(file_path);
    return tokenizer.get_tokens();
}

void plagiarism_checker_t::global_data_writer() {
    {
        std::unique_lock<std::mutex> lock(mtx);
    //a function to write the database for each submission
    bool finish=false;//a variable to check if the function has finished after the destructor was called
    while (running or !finish) {
        int sum=0;//a variable to check if the database has been written for all submissions
        for (int i = 0; i < submission_count; i++) {
            if (!database_written[i] && !submissions[i].first->codefile.empty()) {
                auto tokens = get_tokens(submissions[i].first->codefile);
                //implementing rolling hash for easier comparison
                long long int hashval = 0;
                std::vector<long long int> hashed_tokens;
                long long int prime_mod=1;
                for(int j=0;j<5;j++){
                    hashval=(hashval*PRIME+tokens[j]);
                    prime_mod=(prime_mod*PRIME);
                }
                hashval=hashval%prime_mod;
                hashed_tokens.push_back(hashval);
                for (int j = 5; j < tokens.size(); j++) {
                    hashval = (hashval * PRIME + tokens[j]) % prime_mod;
                    hashed_tokens.push_back(hashval);
                }
                database[i] = hashed_tokens;
                database_written[i] = true;
            }
            if (database_written[i]) {
                sum++;
            }
        }
        if(sum==submission_count){
            finish=true;
        }
    }
    }
}

void plagiarism_checker_t::global_comparator() {
    {
        std::unique_lock<std::mutex> lock(mtx);
    //a function to compare all submissions
    bool finish = false;
    while (running or !finish) {
        int sum = 0;
        for (int i = 0; i < submission_count; i++) {
            for (int j = 0; j < submission_count; j++) {
                if (!comparison_table[i][j] && database_written[i] && database_written[j]) {
                    comparison_table[i][j] = 1;
                    comparison_table[j][i] = 1;
                    primary_comparator(i, j);
                }
                if (comparison_table[i][j]==1) {
                    sum++;
                }
            }
        }
        if (sum == submission_count * submission_count and !running) {
            finish = true;
        }
    }
    }
}

void plagiarism_checker_t::primary_comparator(int submission_id1, int submission_id2) {
    //a function to compare two submissions for plagiarism
    std::vector<std::pair<int, int>> matches;//stores the matches between the two submissions
    int max_size = 0;//stores the maximum consecutive matches size between the two submissions

    std::vector<int> plag_flaggers;//stores the submissions that are to be flagged for plagiarism
    //skipping the check for base submissions, since it will be done in global_comparator
    if (std::chrono::duration_cast<std::chrono::seconds>(abs(submissions[submission_id1].second-submissions[submission_id2].second)).count()<1){
        plag_flaggers.push_back(submission_id1);
        plag_flaggers.push_back(submission_id2);
    }
    else {
        if (submissions[submission_id1].second <= submissions[submission_id2].second ) {
            plag_flaggers.push_back(submission_id2);
        }
        else{
            plag_flaggers.push_back(submission_id1);
        }
    }
    for (int i = 0; i < database[submission_id1].size() - 10; i++) {
        for (int j = 0; j < database[submission_id2].size() - 10; j++) {
            if (database[submission_id1][i] == database[submission_id2][j] and database[submission_id1][i+10] == database[submission_id2][j+10] and database[submission_id1][i+5] == database[submission_id2][j+5]) {
                for (int k = 0; k < plag_flaggers.size(); k++) {
                    //updating the global_matches for patchwork verification
                    if(plag_flaggers[k] == submission_id1){
                        global_matches[submission_id2].push_back(j);
                    }
                    else{
                        global_matches[submission_id1].push_back(i);
                    }
                }
                //updating the matches for the current comparison
                matches.push_back(std::make_pair(i, j));
                //updating the size of the longest match
                if (max_size < 75) {
                    int nmax_size = secondary_comparator(submission_id1, submission_id2, i, j);
                    if (nmax_size > max_size) {
                        max_size = nmax_size;
                    }
                }
            }
        }
    }
    bool plag_detected = false;
    if (max_size >= 75) {
        //flagging the submissions if the match size is greater than 75
        plag_detected = true;
    }
    std::sort(matches.begin(), matches.end());
    std::vector<std::pair<int, int>> final_matches;
    for (int k = 0; k < matches.size(); k++) {
        //removing the overlapping matches for better verification after sorting to reduce the time
        if(k == 0){
            final_matches.push_back(matches[k]);
        }
        else{
            if (matches[k].first - final_matches.back().first >= 15) {
                final_matches.push_back(matches[k]);
            }
        }
    }
    //condition to flag the submissions if there are at least 10 local matches
    if (final_matches.size() >= 10) {
        plag_detected = true;
    }
    //calling the plag_caller function to flag the submissions
    if (plag_detected) {
        for (int k = 0; k < plag_flaggers.size(); k++) {
            if (plagged[plag_flaggers[k]]) {
                continue;
            }
            plag_caller(plag_flaggers[k]);
        }
    }
    //finally checking for global matches and flagging the submissions if there are at least 20 global matches for patchwork verification
    std::vector<int> submissions_to_check = {submission_id1, submission_id2};
    for (int l = 0; l < submissions_to_check.size(); l++) {
        if (plagged[submissions_to_check[l]]) {// checking if the submission has already been flagged
            continue;
        }
        else{
            //sorting the global matches for faster verification
            std::sort(global_matches[submissions_to_check[l]].begin(), global_matches[submissions_to_check[l]].end());
            int s = 1;
            while (s < global_matches[submissions_to_check[l]].size() && s < 21) {
                //removing the overlapping matches for better verification
                if(global_matches[submissions_to_check[l]][s] - global_matches[submissions_to_check[l]][s - 1] < 15){
                    global_matches[submissions_to_check[l]].erase(global_matches[submissions_to_check[l]].begin() + s);
                }
                else{
                    s++;
                }
            }
            if (global_matches[submissions_to_check[l]].size() >= 20) {
                plag_caller(submissions_to_check[l]);//flagging the submission if there are at least 20 global matches
            }
        }
    }
}

int plagiarism_checker_t::secondary_comparator(int submission_id1, int submission_id2, int i, int j) {
    //a function to return the size of the match between two submissions starting at indices i and j respectively
    int size = 0;
    while(i < database[submission_id1].size() && j  < database[submission_id2].size() and size<80){
        if(database[submission_id1][i] == database[submission_id2][j]) {
            size += 5;
            i += 5;
            j += 5;
        }
        else{
            break;
        }
    }
    return size;
}


// #include "plagiarism_checker.hpp"
// #include <iostream>
// #define MOD 1000000007
// #define PRIME 31
// #define MAX_FILE_COUNT 1000

// std::mutex mtx;

// // ANSI color codes
// #define RESET   "\033[0m"
// #define RED     "\033[31m"
// #define GREEN   "\033[32m"
// #define YELLOW  "\033[33m"
// #define BLUE    "\033[34m"
// #define MAGENTA "\033[35m"
// #define CYAN    "\033[36m"
// #define WHITE   "\033[37m"

// // You should NOT add ANY other includes to this file.
// void plagiarism_checker_t::plag_caller(int submission_id){
//     if(plagged[submission_id]){
//         // std::cerr << RED << "Submission " << submission_id << " already flagged." << RESET << std::endl;
//         return;
//     }
//     if(submission_id >= base_count){
//         plagged[submission_id] = true;
//         std::cerr << RED << "Flagging submission " << submission_id << RESET << std::endl;
//         if (submissions[submission_id].first->student != nullptr) {
//             submissions[submission_id].first->student->flag_student(submissions[submission_id].first);
//         }
//         if (submissions[submission_id].first->professor != nullptr) {
//             submissions[submission_id].first->professor->flag_professor(submissions[submission_id].first);
//         }
//     }
// }

// void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
//     // std::cerr << GREEN << "Adding submission " << submission_count << RESET << std::endl;
//     submissions[submission_count] = (std::make_pair(__submission, std::chrono::system_clock::now() - start_time));
//     submission_count++;
// }

// plagiarism_checker_t::plagiarism_checker_t() {
//     start_time = std::chrono::system_clock::now();
//     submissions.resize(MAX_FILE_COUNT);
//     database.resize(MAX_FILE_COUNT);
//     global_matches.resize(MAX_FILE_COUNT);
//     database_written.resize(MAX_FILE_COUNT, false);
//     plagged.resize(MAX_FILE_COUNT, false);
//     check.resize(MAX_FILE_COUNT, false);
//     global_database.clear();
//     local_matches.clear();
//     patchwork_matches.clear();
//     base_count = 0;
//     submission_count = 0;
//     running = true;
//     comparison_table.resize(MAX_FILE_COUNT);
//     for (int i = 0; i < MAX_FILE_COUNT; ++i) {
//         comparison_table[i].resize(MAX_FILE_COUNT, 0);
//         comparison_table[i][i] = 1;
//     }
//     current_threads.push_back(std::thread(&plagiarism_checker_t::global_data_writer, this));
//     current_threads.push_back(std::thread(&plagiarism_checker_t::global_comparator, this));
// }

// plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions) {
//     start_time = std::chrono::system_clock::now();
//     submissions.resize(MAX_FILE_COUNT);
//     database.resize(MAX_FILE_COUNT);
//     global_matches.resize(MAX_FILE_COUNT);
//     database_written.resize(MAX_FILE_COUNT, false);
//     plagged.resize(MAX_FILE_COUNT, false);
//     check.resize(MAX_FILE_COUNT, false);
//     global_database.clear();
//     local_matches.clear();
//     patchwork_matches.clear();
//     base_count = __submissions.size();
//     submission_count = base_count;
//     running = true;
//     comparison_table.resize(MAX_FILE_COUNT);
//     for (int i = 0; i < MAX_FILE_COUNT; ++i) {
//         if (i < __submissions.size()) {
//             comparison_table[i].resize(MAX_FILE_COUNT, 1);
//         } else {
//             comparison_table[i].resize(MAX_FILE_COUNT, 0);
//             comparison_table[i][i] = 1;
//         }
//     }
//     for (int i = 0; i < __submissions.size(); i++) {
//         auto submission = __submissions[i];
//         submissions[i] = std::make_pair(submission, std::chrono::seconds(0));
//     }
//     current_threads.push_back(std::thread(&plagiarism_checker_t::global_data_writer, this));
//     current_threads.push_back(std::thread(&plagiarism_checker_t::global_comparator, this));
// }

// int plagiarism_checker_t::secondary_comparator(int submission_id1, int submission_id2, int i, int j) {
//     int size = 0;
//     while (i < database[submission_id1].size() && j < database[submission_id2].size() and size<80) {
//         if (database[submission_id1][i] == database[submission_id2][j]) {
//             size += 15;
//             i += 15;
//             j += 15;
//         } else {
//             // std::cout << "Mismatch at submission " << submission_id1 << " index " << i << " and submission " << submission_id2 << " index " << j << std::endl;
//             break;
//         }
//     }
//     std::cerr << "Match size: " << size << " between submission " << submission_id1 << " and submission " << submission_id2 << " at indices " << i << " and " << j << std::endl;
//     return size;
// }

// plagiarism_checker_t::~plagiarism_checker_t() {
//     running = false;
//     for (auto& thread : current_threads) {
//         if (thread.joinable()) {
//             thread.join();
//         }
//     }
// }

// std::vector<int> plagiarism_checker_t::get_tokens(const std::string& file_path) {
//     tokenizer_t tokenizer(file_path);
//     return tokenizer.get_tokens();
// }

// void plagiarism_checker_t::global_data_writer(){
//     long long int power = 1;
//     bool finish = false;
//     for (int s = 0; s < 15 - 1; ++s){
//         power = (power * PRIME) % MOD;
//     }
//     while(running or !finish){ 
//         int sum = 0;
//         for(int i = 0; i < submission_count; i++){
//             if(database_written[i]){
//                 sum++;
//             }       
//             if(!database_written[i] and !submissions[i].first->codefile.empty()){
//                 std::vector<std::pair<int,int>> matches;
//                 std::vector<int> tokens = get_tokens(submissions[i].first->codefile);
//                 std::vector<long long int> hashes;
//                 long long int hashval = 0;
//                 for(int j = 0; j < 15; j++){
//                     hashval = (hashval * PRIME + tokens[j]) % MOD;
//                 }
//                 hashes.push_back(hashval);
//                 for(int j = 15; j < tokens.size(); j++){
//                     hashval = (hashval - tokens[j-15] * power % MOD + MOD) % MOD;
//                     hashval = (hashval * PRIME + tokens[j]) % MOD;
//                     hashes.push_back(hashval);
//                     std::vector<std::pair<int,int>> current_matches = global_database[hashval];
//                     for(auto match : current_matches){
//                         local_matches[std::make_pair(i, match.first)].emplace_back(j, match.second);
//                     }
//                     global_database[hashval].push_back(std::make_pair(i, j));
//                 }
//                 database_written[i] = true;
//                 database[i] = hashes;
//                 // std::cerr << CYAN << "Database written for submission " << i << RESET << std::endl;
//             }
//         }
//         if(sum == submission_count and !running){
//             finish = true;
//         }
//     }
// }

// void plagiarism_checker_t::global_comparator(){
//     bool finish = false;
//     while(running or !finish){
//         int sum = 0;
//         for(int i = 0; i < submission_count; i++){
//             for(int j = 0; j < submission_count; j++){
//                 if(comparison_table[i][j] == 1){
//                     sum+=1;
//                     continue;
//                 }
//                 if(!(database_written[i] and !submissions[i].first->codefile.empty()))continue;
//                 // std::cerr << YELLOW << "Comparing submission " << i << " with submission " << j << RESET << std::endl;
//                 bool other_file = (j >= base_count) && (abs(submissions[j].second - submissions[i].second) <= std::chrono::seconds(1));
//                 auto local_match = local_matches[std::make_pair(i, j)];
//                 std::sort(local_match.begin(), local_match.end());
//                 std::vector<std::pair<int,int>> final_matches;
//                 int max_size = 0;
//                 for (int k = 0; k < local_match.size(); k++){
//                     if(k == 0){
//                         final_matches.push_back(local_match[k]);
//                         patchwork_matches[i].insert(local_match[k].first);
//                         if(other_file){
//                             patchwork_matches[j].insert(local_match[k].second);
//                         }
//                     }
//                     else{
//                         if(local_match[k].first - final_matches.back().first >= 15){
//                             final_matches.push_back(local_match[k]);
//                             patchwork_matches[i].insert(local_match[k].first);
//                             if(other_file){
//                                 patchwork_matches[j].insert(local_match[k].second);
//                             }
//                         }
//                     }
//                     if(max_size < 75){
//                         max_size = std::max(secondary_comparator(i, j, local_match[k].first, local_match[k].second), max_size);
//                         // std::cerr << MAGENTA << "Max size for submission " << i << " with submission " << j << " is " << max_size << RESET << std::endl;
//                     }
//                 }
//                 if(final_matches.size() >= 10){
//                     std::cerr << RED << "Flagging submission " << i << " due to at least 10 local matches with submission " << j << RESET << std::endl;
//                     if(other_file){
//                         std::cerr << RED << "Flagging submission " << j << " due to at least 10 local matches with submission " << i << RESET << std::endl;
//                         plag_caller(j);
//                     }
//                     plag_caller(i);
//                 }

//                 if(patchwork_matches[i].size() >= 20){
//                     std::cerr << RED << "Flagging submission " << i << " due to at least 20 global matches" << RESET << std::endl;
//                     plag_caller(i);
//                 }
//                 if(other_file and patchwork_matches[j].size() >= 20){
//                     std::cerr << RED << "Flagging submission " << j << " due to at least 20 global matches" << RESET << std::endl;
//                     plag_caller(j);
//                 }
//                 if(max_size >= 75){
//                     std::cerr << RED << "Flagging submission " << i << " due to perfect match >= 75 with submission " << j << RESET << std::endl;
//                     if(other_file){
//                         std::cerr << RED << "Flagging submission " << j << " due to perfect match >= 75 with submission " << i << RESET << std::endl;
//                         plag_caller(j);
//                     }
//                     plag_caller(i);
//                 }
//                 check[i] = true; 
//                 comparison_table[i][j] = 1;
//                 comparison_table[j][i] = 1;            
//             }
//         }

//         if(sum == submission_count*submission_count and  !running){
//             finish = true;
//         }
//     }
// }