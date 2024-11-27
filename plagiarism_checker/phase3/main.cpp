#include "match_submissions.hpp"
#include "../tokenizer.hpp"

// You should NOT modify ANYTHING in this file.
extern std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2);

void test_checker(std::string file1, std::string file2) {
	tokenizer_t file_one(file1);
	tokenizer_t file_two(file2);
	std::vector<int> submission1 = file_one.get_tokens();
	std::vector<int> submission2 = file_two.get_tokens();
	std::array<int, 5> output = match_submissions(submission1, submission2);
	
	for (int i = 0; i < 5; i++){
		std::cout << output[i] << " ";
	}
	std::cout << std::endl;

	return;
}

int main(int argc, char* argv[]) {

	if (argc >= 3){	
		std::string file1 = argv[1];
		std::string file2 = argv[2];

		test_checker(file1, file2);
	}
	else{
		std::cout << "Not enough arguments passed." << std::endl;
	}
	
	return 0;
}