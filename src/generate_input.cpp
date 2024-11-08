#include "utils.hpp"

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
    generate_polynomial_to_file("data/input_a.txt", degree);
    generate_polynomial_to_file("data/input_b.txt", degree);

    return 0;
}
