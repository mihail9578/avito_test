#include <iostream>
#include "file.h"
#include "sorter.h"



int main(const int argc, const char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./freq [input_file] [output_file]" << std::endl;
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    try{
        auto fm = FileManager(input_path);
        auto file_data = fm.GetData();

        auto fs = FrequencySorter(file_data, 3);

        WriteToFile(output_path, fs.GetSortedVector());

    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
    }
}
