#include <iostream>
#include <app.hpp>
#include <metrics.hpp>

struct Arguments {
    std::string input_path {"random_file"};
    std::string output_path {"output_file"};
};

Arguments parse_args(int argc, char* argv[]) {
    Arguments retval{};
    if(argc >= 2) retval.input_path = argv[1];
    if(argc >= 3) retval.output_path = argv[2];
    return retval;
}

// RAII for source and destination files
struct FileHandler {
    FileHandler(const std::string& path, const std::string&& mode) {
        file = fopen(path.c_str(), mode.c_str());
    }

    ~FileHandler() {
        fclose(file);
    }

    FILE* file;
};

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);
    std::cout << "Copying from " << args.input_path << " to " << args.output_path << '\n';
    app::App application{};
    copier::Metrics metrics{};
    {
        FileHandler src{args.input_path, "rb"};
        FileHandler dst{args.output_path, "wb"};
        application.run(src.file, dst.file, metrics);
    }

    auto rate = metrics.stop();
    std::cout << "Copied " << metrics.total_size << " bytes in " << metrics.total_ms << " ms with average speed " << rate << " MB/s \n";
    return 0;
}

