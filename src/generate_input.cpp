#include "utils.hpp"

// Function to generate random polynomial and write to file
template <typename FieldT>
void generate_polynomial_to_file(const std::string& filename, size_t degree)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    std::chrono::seconds duration;
    int minutes;
    int seconds;

    std::cout << "[*] Generating " << filename << "...";
    std::cout.flush();

    // std::ofstream output_file(filename);

    std::ofstream output_file(filename, std::ios::binary);

    if (output_file.is_open()) {
        start_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < degree; ++i) {
            FieldT value = FieldT::random_element();
            const auto& bigint_value = value.as_bigint();            
            output_file.write(reinterpret_cast<const char*>(bigint_value.data), sizeof(bigint_value.data));
        }

        output_file.close();
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        minutes = duration.count() / 60;
        seconds = duration.count() % 60;
        std::cout << std::setw(_print_align) << std::left  << "\r[+] Polynomial written to " + filename 
                  << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << std::endl;
    } else {
        std::cerr << "\r[-] Unable to open file " + filename << std::endl;
    }
}

int main(int argc, char* argv[]) {

    int opt;
    size_t k = 4; // Default value for k
    size_t degree;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                k = std::stoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-n k]" << std::endl;
                return 1;
        }
    }

    degree = 1 << k;

    bls12_381_pp::init_public_params();
    std::cout << "Generating data with degree: 2^" << k << " (" << degree << " elements)" << std::endl;
    generate_polynomial_to_file<FieldT>("data/input_a.txt", degree);
    generate_polynomial_to_file<FieldT>("data/input_b.txt", degree);

    return 0;
}
