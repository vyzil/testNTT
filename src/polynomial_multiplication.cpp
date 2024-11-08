#include "utils.hpp"

/* Polynomial Multiplication via FFT with output parameter */
template <typename FieldT>
void polynomial_multiplication_on_FFT_serial(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    // // -- # Cycle Convolution --
    // const size_t n = libff::get_power_of_two(a.size() + b.size() - 1);
    // FieldT omega = libff::get_root_of_unity<FieldT>(n);

    // std::vector<FieldT> u(a);
    // std::vector<FieldT> v(b);

    // u.resize(n, FieldT::zero());
    // v.resize(n, FieldT::zero());
    // c.resize(n, FieldT::zero());    
    // // ---------------------------

    // -- # Negative Wrapped Convolution --
    const size_t n = libff::get_power_of_two(a.size());
    FieldT omega = libff::get_root_of_unity<FieldT>(2*n);

    std::vector<FieldT> u(a);
    std::vector<FieldT> v(b);

    u.resize(n, FieldT::zero());
    v.resize(n, FieldT::zero());
    c.resize(n, FieldT::zero());
    // -------------------------------------

    _basic_serial_radix2_FFT(u, omega);
    _basic_serial_radix2_FFT(v, omega);    

    std::transform(u.begin(), u.end(), v.begin(), c.begin(), std::multiplies<FieldT>());
    
    _basic_serial_radix2_FFT(c, omega.inverse());  

    const FieldT sconst = FieldT(n).inverse();
    std::transform(c.begin(), c.end(), c.begin(), std::bind(std::multiplies<FieldT>(), sconst, std::placeholders::_1));
    _condense(c);

    return;
}

template <typename FieldT>
void polynomial_multiplication_serial(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    std::cout << "[*] processing Serial FFT";
    std::cout.flush();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    polynomial_multiplication_on_FFT_serial<FieldT>(a, b, c);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;

    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left  << "\r[+] Serial FFT process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << milliseconds << "ms)" << std::endl;

    return;
}

/* Polynomial Multiplication via FFT with output parameter */
template <typename FieldT>
void polynomial_multiplication_on_FFT_parallel(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    // // -- # Cycle Convolution --
    // const size_t n = libff::get_power_of_two(a.size() + b.size() - 1);
    // FieldT omega = libff::get_root_of_unity<FieldT>(n);

    // std::vector<FieldT> u(a);
    // std::vector<FieldT> v(b);

    // u.resize(n, FieldT::zero());
    // v.resize(n, FieldT::zero());
    // c.resize(n, FieldT::zero());    
    // // ---------------------------

    // -- # Negative Wrapped Convolution --
    const size_t n = libff::get_power_of_two(a.size());
    FieldT omega = libff::get_root_of_unity<FieldT>(n);

    std::vector<FieldT> u(a);
    std::vector<FieldT> v(b);

    u.resize(n, FieldT::zero());
    v.resize(n, FieldT::zero());
    c.resize(n, FieldT::zero());
    // -------------------------------------
 
    _basic_parallel_radix2_FFT(u, omega);
    _basic_parallel_radix2_FFT(v, omega);

    std::transform(u.begin(), u.end(), v.begin(), c.begin(), std::multiplies<FieldT>());
     
    _basic_parallel_radix2_FFT(c, omega.inverse());

    const FieldT sconst = FieldT(n).inverse();
    std::transform(c.begin(), c.end(), c.begin(), std::bind(std::multiplies<FieldT>(), sconst, std::placeholders::_1));
    _condense(c);

    return;
}

template <typename FieldT>
void polynomial_multiplication_parallel(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    std::cout << "[*] processing Parallel FFT";
    std::cout.flush();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    polynomial_multiplication_on_FFT_parallel<FieldT>(a, b, c);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;

    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left << "\r[+] Parallel FFT process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;

    return;
}

void parse_arguments(int argc, char *argv[], bool &multicore, bool &test_mode, bool &debug_mode) {
    multicore = false;
    test_mode = false;
    debug_mode = false;

    const char *short_opts = "mtd";
    const option long_opts[] = {
        {"multicore", no_argument, nullptr, 'm'},
        {"test", no_argument, nullptr, 't'},
        {"debug", no_argument, nullptr, 'd'},
        {nullptr, no_argument, nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'm':
                multicore = true;
                break;
            case 't':
                test_mode = true;
                break;
            case 'd':
                debug_mode = true;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-m|--multicore] [-t|--test] [-d|--debug]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {    
    bool multicore;
    bool test_mode;
    bool debug_mode;

    parse_arguments(argc, argv, multicore, test_mode, debug_mode);

    if (multicore) {
        const size_t num_cpus = omp_get_max_threads();
        const size_t log_cpus = ((num_cpus & (num_cpus - 1)) == 0 ? log2(num_cpus) : log2(num_cpus) - 1);
        std::cout << "[i] Mode : Parallel" << std::endl;
        std::cout << "\t- num_cpus : " << num_cpus << std::endl;
        std::cout << "\t- log_cpus : " << log_cpus << std::endl;
    } else {
        std::cout << "[i] Mode : Serial" << std::endl;
    }

    std::vector<FieldT> a;
    std::vector<FieldT> b;
    std::vector<FieldT> c;

    bls12_381_pp::init_public_params();

    if (!test_mode) { 
        if(!read_polynomial("data/input_a.txt", a)) return 1;
        if(!read_polynomial("data/input_b.txt", b)) return 1;
    } else {
        a = {1, 2};
        b = {3, 5};
    }

    const size_t n = libff::get_power_of_two(a.size());
    FieldT omega = libff::get_root_of_unity<FieldT>(n);
    std::cout << "[i] NTT Parameter" << std::endl;
    std::cout << "\t - Polynomial Size : " << n << std::endl;
    std::cout << "\t - Omega : 0x" << std::hex << omega << std::endl;
    std::cout << "\t - O_inv : 0x" << std::hex << omega.inverse() << std::endl;

    if(multicore) polynomial_multiplication_parallel(a, b, c);
    else polynomial_multiplication_serial(a, b, c);
    
    if (!test_mode) { 
        if(!write_polynomial("data/output_c.txt", c)) return 1;
    }

    if (test_mode || debug_mode) {
        print_polynomial(a);
        print_polynomial(b);
        print_polynomial(c);
    }
    
    return 0;
}
