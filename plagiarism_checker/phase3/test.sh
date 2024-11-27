# ./test.sh checker_file.hpp input_file1.cpp input_file2.cpp

if [ "$#" -ne 3 ]; then
    echo "Error: Exactly 3 arguments are required."
    exit 1  # Exit with an error code
fi

cp $1 match_submissions.hpp

clang++ -std=gnu++20 -g -Wall -Wshadow -Wuninitialized -Wtype-limits -Wno-unused-variable -I/usr/lib/llvm-18/include -lclang -L/usr/lib/llvm-18/lib -rpath /usr/lib/llvm-18/lib main.cpp tokenizer.cpp -o main

./main $2 $3