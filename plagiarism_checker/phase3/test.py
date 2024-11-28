def modify_cpp_file(input_file, output_file):
    try:
        with open(input_file, 'r') as infile:
            content = infile.readlines()

        # Insert "junk++" in every alternate line
        modified_content = []
        for i, line in enumerate(content):
            modified_content.append(line)
            if i % 2 == 1:  # Every alternate line (1, 3, 5, ...)
                modified_content.append('junk++;\n')

        with open(output_file, 'w') as outfile:
            outfile.writelines(modified_content)

        print(f"Successfully modified the file. Output written to {output_file}")

    except FileNotFoundError:
        print(f"The file {input_file} does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
modify_cpp_file('file1.cpp', 'file2.cpp')
