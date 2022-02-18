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

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);
    std::cout << "Copying from " << args.input_path << " to " << args.output_path << '\n';
    app::App application{};
    copier::Metrics metrics{};
    //TODO: ensure proper file closing. Use fstream instead?
    auto src_f = fopen(args.input_path.c_str(), "rb");
    auto dst_f = fopen(args.output_path.c_str(), "wb");

    application.run(src_f, dst_f, metrics);

    fclose(src_f);
    fclose(dst_f);
    metrics.stop();
    std::cout << "Copied " << metrics.total_size << " bytes in " << metrics.total_ms << " ms with average speed " << metrics.rate_MBps() << " MB/s \n";
    return 0;
}

