#include "utils.hpp"

#ifdef MULTICORE
    #define polynomial_multiplication polynomial_multiplication_parallel
#else   
    #define polynomial_multiplication polynomial_multiplication_serial
#endif

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
    FieldT omega = libff::get_root_of_unity<FieldT>(2*n);

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
void polynomial_multiplication_serial(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    std::cout << "\t[*] processing Serial FFT";
    std::cout.flush();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    polynomial_multiplication_on_FFT_serial<FieldT>(a, b, c);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    auto minutes = duration.count() / 60;
    auto seconds = duration.count() % 60;

    std::cout << std::setw(_print_align) << std::left  << "\r\t[+] Serial FFT process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << std::endl;

    return;
}

template <typename FieldT>
void polynomial_multiplication_parallel(const std::vector<FieldT>& a, const std::vector<FieldT>& b, std::vector<FieldT>& c)
{
    std::cout << "\t[*] processing Parallel FFT";
    std::cout.flush();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    polynomial_multiplication_on_FFT_parallel<FieldT>(a, b, c);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    auto minutes = duration.count() / 60;
    auto seconds = duration.count() % 60;

    std::cout << std::setw(_print_align) << std::left  << "\r\t[+] Parallel FFT process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s)" << std::endl;

    return;
}

int main() {
    std::vector<FieldT> a;
    std::vector<FieldT> b;
    std::vector<FieldT> c;

    std::cout << "-- Start Main --" << std::endl;

    #ifdef MULTICORE
        std::cout << "# Parallel Mode" << std::endl;
        const size_t num_cpus = omp_get_max_threads();
        const size_t log_cpus = ((num_cpus & (num_cpus - 1)) == 0 ? log2(num_cpus) : log2(num_cpus) - 1);
        std::cout << "[i] num_cpus : " << num_cpus << std::endl;
        std::cout << "[i] log_cpus : " << log_cpus << std::endl;
    #else        
        std::cout << "# Serial Mode" << std::endl;
    #endif

    bls12_381_pp::init_public_params();

    if(!read_polynomial("data/input_a.txt", a)) return 1;
    if(!read_polynomial("data/input_b.txt", b)) return 1;
    polynomial_multiplication(a, b, c);
    if(!write_polynomial("data/input_c.txt", c)) return 1;

    print_polynomial(a);
    print_polynomial(b);
    print_polynomial(c);

    std::cout << "-- End Main --" << std::endl;
    return 0;
}
