#include "utils.hpp"

void print_polynomial(const std::vector<FieldT>& poly)
{
    
    #define print_line() std::cout << std::string(200, '-') << std::endl
    /* Print out the polynomial in human-readable form */
    print_line();
    for (size_t i = 0; i < poly.size(); i++)
    {
        bigint<4> coefficient_bigint = poly[i].as_bigint();
        mpz_class coefficient;
        mpz_import(coefficient.get_mpz_t(), 4, -1, sizeof(coefficient_bigint.data[0]), 0, 0, coefficient_bigint.data);
        std::cout << "\t" << i << " : " << "0x" << coefficient.get_str(16) << " (" << coefficient.get_str() << ")" <<  std::endl;
    }
    print_line();
    #undef print_line
}

/* Function to read polynomial from a file */
bool read_polynomial_from_file(const std::string& filename, std::vector<FieldT>& poly)
{
    std::ifstream input_file(filename);
    if (input_file.is_open()) {
        FieldT value;
        while (input_file >> value) {
            poly.push_back(value);
        }
        input_file.close();
        return true;
    } else {
        return false;
    }
}

/* Wrapper function to read polynomial from a file with timing */
bool read_polynomial(const std::string& filename, std::vector<FieldT>& poly)
{
    std::cout << "\t[*] Reading " << filename;
    std::cout.flush();

    auto start_time = std::chrono::high_resolution_clock::now();

    bool success = read_polynomial_from_file(filename, poly);
    if (!success) {
        std::cerr << "\r\t[-] Unable to open " << filename << std::endl;
        return false;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    auto minutes = duration.count() / 60;
    auto seconds = duration.count() % 60;

    std::cout << std::setw(30) << std::left  << "\r\t[+] Read " + filename
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << std::endl;
    return true;
}

/* Function to write polynomial to a file */
bool write_polynomial_to_file(const std::string& filename, const std::vector<FieldT>& poly)
{
    std::ofstream output_file(filename);
    if (output_file.is_open()) {
        for (const auto& value : poly) {
            output_file << value << "\n";
        }
        output_file.close();
        return true;
    } else {
        return false;
    }
}

/* Wrapper function to write polynomial to a file with timing */
bool write_polynomial(const std::string& filename, const std::vector<FieldT>& poly)
{
    std::cout << "\t[*] Writing " << filename;
    std::cout.flush();

    auto start_time = std::chrono::high_resolution_clock::now();

    bool success = write_polynomial_to_file(filename, poly);
    if (!success) {
        std::cerr << "\r\t[-] Unable to write to " << filename << std::endl;
        return false;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    auto minutes = duration.count() / 60;
    auto seconds = duration.count() % 60;

    std::cout << std::setw(30) << std::left  << "\r\t[+] Written " + filename
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << std::endl;
    return true;
}