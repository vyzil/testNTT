#include "utils.hpp"

// Function to generate random polynomial and write to file
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
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto minutes = duration.count() / 60000;
        auto seconds = (duration.count() % 60000) / 1000;
        auto milliseconds = duration.count() % 1000;

        std::cout << std::dec;
        std::cout << std::setw(_print_align) << std::left  << "\r[+] Polynomial written to " + filename 
                  << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;
    } else {
        std::cerr << "\r[-] Unable to open file " + filename << std::endl;
    }
}

void print_polynomial(const std::vector<FieldT>& poly)
{
    std::cout << "[i] Polynomial Info" << std::endl;
    #define print_line() std::cout << "\t" << std::string(150, '-') << std::endl
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

/* Function to read polynomial from a file in binary format */
bool read_polynomial_from_file(const std::string& filename, std::vector<FieldT>& poly)
{
    std::ifstream input_file(filename, std::ios::binary);
    if (input_file.is_open()) {
        while (!input_file.eof()) {
            bigint<4> bigint_value;
            input_file.read(reinterpret_cast<char*>(bigint_value.data), sizeof(bigint_value.data));
            if (input_file.gcount() == 0) break;  // 파일의 끝에 도달한 경우
            FieldT value(bigint_value);
            poly.push_back(value);
        }
        input_file.close();
        return true;
    } else {
        return false;
    }
}

/* Wrapper function to read polynomial from a file with timing in binary format */
bool read_polynomial(const std::string& filename, std::vector<FieldT>& poly)
{
    std::cout << "[*] Reading " << filename;
    std::cout.flush();

    auto start_time = std::chrono::high_resolution_clock::now();

    bool success = read_polynomial_from_file(filename, poly);
    if (!success) {
        std::cerr << "\r[-] Unable to open " << filename << std::endl;
        return false;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;

    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left  << "\r[+] Read " + filename
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;
    return true;
}


/* Function to write polynomial to a file in binary format */
bool write_polynomial_to_file(const std::string& filename, const std::vector<FieldT>& poly)
{
    std::ofstream output_file(filename, std::ios::binary);
    if (output_file.is_open()) {
        for (const auto& value : poly) {
            const auto& bigint_value = value.as_bigint();
            output_file.write(reinterpret_cast<const char*>(bigint_value.data), sizeof(bigint_value.data));
        }
        output_file.close();
        return true;
    } else {
        return false;
    }
}

/* Wrapper function to write polynomial to a file with timing in binary format */
bool write_polynomial(const std::string& filename, const std::vector<FieldT>& poly)
{
    std::cout << "[*] Writing " << filename;
    std::cout.flush();

    auto start_time = std::chrono::high_resolution_clock::now();

    bool success = write_polynomial_to_file(filename, poly);
    if (!success) {
        std::cerr << "\r[-] Unable to write to " << filename << std::endl;
        return false;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;

    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left  << "\r[+] Written " + filename
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;
    return true;
}
