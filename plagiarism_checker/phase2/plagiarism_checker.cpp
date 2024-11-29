#include "plagiarism_checker.hpp"
// You should NOT add ANY other includes to this file.
// Do NOT add "using namespace std;".



std::vector<long long int> plagiarism_checker_t:: rolling_hash(std::vector<int> &input, int window_length) 
{
    const long long  int MOD = 1e9 + 7;
    const int BASE = 31; // You can choose any base (usually a small prime)
    int n = input.size();
    if (window_length > n || window_length <= 0) {
       return {};
    }

    std::vector<long long int> hashes; // Resultant rolling hashes
    long long current_hash = 0; // Current window's hash value
    long long power_base = 1;   // Precompute powers of BASE mod MOD

    // Compute the hash for the first window
    for (int i = 0; i < window_length; i++) {
        current_hash = (current_hash * BASE + input[i]) % MOD;
        if (i < window_length - 1) {
            power_base = (power_base * BASE) % MOD;
        }
    }
    hashes.push_back(current_hash);

    // Slide the window and compute the rolling hash
    for (int i = window_length; i < n; i++) {
        current_hash = (current_hash - (input[i - window_length] * power_base) % MOD + MOD) % MOD; // Remove the leftmost element
        current_hash = (current_hash * BASE + input[i]) % MOD; // Add the new element
        hashes.push_back(current_hash);
    }

    return hashes;
}

void plagiarism_checker_t:: add_new_hash(std::vector<long long int> & input_hashes, long submission_id, std::map<long long int, std::vector<long>> & map_of_hash_values)
{

    for(auto hash_value: input_hashes)
    {
        if(map_of_hash_values.find(hash_value)==map_of_hash_values.end())
        {
            map_of_hash_values[hash_value] = {submission_id};
        }
        else
        {
            map_of_hash_values[hash_value].push_back(submission_id);
        }
    }
}

bool plagiarism_checker_t:: exact75_check(long current_submission_id, std::vector<long long int> & input_hashes, std::map<long long int, std::vector<long>> & map_of_hash_values)
{
    for(auto hash_value: input_hashes)
    {
        if(map_of_hash_values.find(hash_value)!=map_of_hash_values.end())
        {
            for(auto previous_submission_id : map_of_hash_values[hash_value])
            {
                if (flagged[previous_submission_id]) continue;
                
                if(std::abs(submission_times[current_submission_id] - submission_times[previous_submission_id]) <= 1.0) // changed from 1.0 
                {
                    if(submission_id_to_ptr[previous_submission_id]->student) // null pointer check
                        submission_id_to_ptr[previous_submission_id]->student->flag_student(submission_id_to_ptr[previous_submission_id]);
                    if(submission_id_to_ptr[previous_submission_id]->professor)
                        submission_id_to_ptr[previous_submission_id]->professor->flag_professor(submission_id_to_ptr[previous_submission_id]);
                
                    // std::cerr << "Match of length 75 with previous submission id :" << previous_submission_id << std::endl;
                }
            }

            flagged[current_submission_id] = true;
            if(submission_id_to_ptr[current_submission_id]->student)
                submission_id_to_ptr[current_submission_id]->student->flag_student(submission_id_to_ptr[current_submission_id]);
            if(submission_id_to_ptr[current_submission_id]->professor)
                submission_id_to_ptr[current_submission_id]->professor->flag_professor(submission_id_to_ptr[current_submission_id]);
            
            // std::cerr << "Match of length 75 with submission id : " << current_submission_id << std::endl;
            return true; 
        }
    }

    return false;
}

bool plagiarism_checker_t:: exact15_check(long current_submission_id, std::vector<long long int> & hash_values, std::map<long long int, std::vector<long>> & map_of_hash_values)
{
    int no_unique_patterns=0;
    std::map<long, int> frequency_count;
    int i = 0;
    while(i < hash_values.size())
    {
        if(map_of_hash_values.find(hash_values[i])!=map_of_hash_values.end())
        {
            no_unique_patterns++; // hash values could repeat
            // to detect patchwork plagiarism
            if(no_unique_patterns>=20) // check number
            {
                flagged[current_submission_id] = true;
                if(submission_id_to_ptr[current_submission_id]->student)
                    submission_id_to_ptr[current_submission_id]->student->flag_student(submission_id_to_ptr[current_submission_id]);
                if(submission_id_to_ptr[current_submission_id]->professor)
                    submission_id_to_ptr[current_submission_id]->professor->flag_professor(submission_id_to_ptr[current_submission_id]);
                
                // debugging
                // std::cerr << "Patchwork plag detected with id " << current_submission_id << std::endl;
                
                return true;
            }

            // to detect copies from the same file
            std::vector<long int> matching_files = map_of_hash_values[hash_values[i]];
            for(auto previous_submission_id : matching_files)
            {
                frequency_count[previous_submission_id]++;

                if(frequency_count[previous_submission_id]>=10) // plag between two files checking
                {
                    if(std::abs(submission_times[current_submission_id] - submission_times[previous_submission_id]) <= 1.0) // changed from 1.0 
                    {
                        if (flagged[previous_submission_id]) continue;

                        if(submission_id_to_ptr[previous_submission_id]->student)
                            submission_id_to_ptr[previous_submission_id]->student->flag_student(submission_id_to_ptr[previous_submission_id]);
                        if(submission_id_to_ptr[previous_submission_id]->professor)
                            submission_id_to_ptr[previous_submission_id]->professor->flag_professor(submission_id_to_ptr[previous_submission_id]);

                        // std::cerr << "Flagged previous submission for copies from same file " << previous_submission_id << std::endl;
                    }
                    flagged[current_submission_id] = true;
                    if(submission_id_to_ptr[current_submission_id]->student)
                        submission_id_to_ptr[current_submission_id]->student->flag_student(submission_id_to_ptr[current_submission_id]);
                    if(submission_id_to_ptr[current_submission_id]->professor)
                        submission_id_to_ptr[current_submission_id]->professor->flag_professor(submission_id_to_ptr[current_submission_id]);

                    // std::cerr << "Flagged submission with id " << current_submission_id << " for copies from same file." << std::endl;
                    return true;
                }
            }

            i+=15; // 15
        }
        else
        {
            i++;
        }
    }

    return false;
}

plagiarism_checker_t::plagiarism_checker_t()
{
    stop = false;
    worker = std::thread(&plagiarism_checker_t::handle_submission, this);

    initial_time = std::chrono::steady_clock::now();
}

plagiarism_checker_t:: plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
{
    stop = false;
    worker = std::thread(&plagiarism_checker_t::handle_submission, this);

    initial_time = std::chrono::steady_clock::now();

    for(auto file: __submissions)
    {
        std::string code = file->codefile;
        tokenizer_t tokenizer(code);
        std::vector<int> tokens = tokenizer.get_tokens();

        std::vector<long long int> hash75 = rolling_hash(tokens, 75);
        std::vector<long long int> hash15 = rolling_hash(tokens, 15);

        add_new_hash(hash75, file->id, map_75_hashes);
        add_new_hash(hash15, file->id, map_15_hashes);

        submission_times[file->id] = -100.0;
        submission_id_to_ptr[file->id]= file;
    }
}

plagiarism_checker_t:: ~plagiarism_checker_t()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }

    cv.notify_one();
    worker.join();
}

void plagiarism_checker_t::handle_submission(){
    while (true) {
        std::shared_ptr<submission_t> __submission;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]{
                return (sub_queue.empty() == false) || stop;
            });

            if (stop && sub_queue.empty())
                break;

            __submission = sub_queue.front();
            sub_queue.pop();
        }

        // --------------------------------------------------------
        auto time_now = std::chrono::steady_clock::now();
        double submission_time  = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - initial_time).count() / 1000.0;
        {
            std::unique_lock<std::mutex> lock(mtx);    
            submission_times[__submission->id] = submission_time;
            submission_id_to_ptr[__submission->id] = __submission;
        }

        std::string code = __submission->codefile;
        tokenizer_t tokenizer(code);
        std::vector<int> tokens = tokenizer.get_tokens();
        std::vector<long long int> hash75 = rolling_hash(tokens, 75);
        std::vector<long long int> hash15 = rolling_hash(tokens, 15);
        

        bool caught_75 = exact75_check(__submission->id, hash75, map_75_hashes);
        
        // check only if there was no match of length 75
        if (caught_75 == false){
            exact15_check(__submission->id, hash15, map_15_hashes);
        }

        {
            std::unique_lock<std::mutex> lock(mtx);

            // add the new file 
            add_new_hash(hash75, __submission->id, map_75_hashes);
            add_new_hash(hash15, __submission->id, map_15_hashes);
        }

        // debugging
        // std::cerr << "Received submission with id " << __submission->id << " at time " << submission_time << std::endl;
        // --------------------------------------------------------

    }
}

void plagiarism_checker_t:: add_submission(std::shared_ptr<submission_t> __submission)
{
    // std::cerr << "Called add submission on id " << __submission->id << std::endl;

    {
        std::unique_lock<std::mutex> lock(mtx);

        sub_queue.push(__submission);    
    }

    cv.notify_one();
}