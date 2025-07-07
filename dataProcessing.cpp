#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
namespace fs = std::filesystem;

int main() {

    // We make a new directory to parsed data
    std::string dir_name = "data_Corrupted";
    fs::create_directory("./clean_"+dir_name);
    const fs::path path = "./" + dir_name;

    // Take every file name in a given directory
    for (auto const& dir_entry: fs::directory_iterator(path)) {
        if (fs::is_regular_file(dir_entry.path())) {
            // Create new file for each old file
            std::string file_name = dir_entry.path().filename().string();
            std::ifstream file(dir_name+"/"+file_name);

            if (file_name == ".DS_Store") { continue; }

            if (file.is_open()) {
                std::cout << "Parsing " + file_name << std::endl;
                std::ofstream new_file("./clean_"+dir_name+"/"+file_name);

                std::string current;

                int prev_channel    = 1;
                int preprev_channel = 1;

                long prev_time    = 0;
                long preprev_time = 0;
                long last_written = 0;

                while (std::getline(file, current)) {
                    if (current[0] == '#') { continue; }

                    long time   = std::stol(current.substr(0, current.find(';')));
                    int channel = std::stoi(current.substr(current.find(';')+1));

                    // std::cout << time << " " << channel << std::endl;

                    // When we loop over line j we inspect lines j-1 and j-2
                    // If j-1 and j-2 have different channels we always register line j-1
                    // Then we check and register which arrival time is smaller between j and j-2 (if j and j-2 have same channel)
                    // If they belong to different channels (so j-1 and j-2 have same channel) we register always j-2

                    if (prev_channel != preprev_channel && last_written != preprev_time) {
                        new_file << prev_time << ";" << prev_channel << std::endl;

                        if (channel == preprev_channel) {
                            if (abs(time-prev_time) < abs(preprev_time-prev_time)) { 
                                new_file << time << ";" << channel<<std::endl;
                            } else {
                                new_file << preprev_time << ";" << preprev_channel<<std::endl;
                            }

                        } else {
                            new_file << preprev_time << ";" << preprev_channel << std::endl;
                        }
                        last_written = prev_time;
                    }

                    preprev_channel = prev_channel;
                    prev_channel = channel;

                    preprev_time = prev_time;
                    prev_time = time;
                }

                new_file.close();
            }
        }
    }

    std::cout << "Done" << std::endl;

    return 0;
}