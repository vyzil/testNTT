#include "utils.hpp"

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

// Code from libff
template<typename FieldT>
void baseline_serial_ntt(std::vector<FieldT> &a, const FieldT &omega)
{
    const size_t n = a.size(), logn = log2(n);
    if (n != (1u << logn)) throw DomainSizeException("expected n == (1u << logn)");

    /* swapping in place (from Storer's book) */
    for (size_t k = 0; k < n; ++k)
    {
        const size_t rk = libff::bitreverse(k, logn);
        if (k < rk)
            std::swap(a[k], a[rk]);
    }

    size_t m = 1; // invariant: m = 2^{s-1}
    for (size_t s = 1; s <= logn; ++s)
    {
        // w_m is 2^s-th root of unity now
        const FieldT w_m = omega^(n/(2*m));

        asm volatile  ("/* pre-inner */");
        for (size_t k = 0; k < n; k += 2*m)
        {
            FieldT w = FieldT::one();
            for (size_t j = 0; j < m; ++j)
            {
                const FieldT t = w * a[k+j+m];
                a[k+j+m] = a[k+j] - t;
                a[k+j] += t;
                w *= w_m;
            }
        }
        asm volatile ("/* post-inner */");
        m *= 2;
    }
}

template<typename FieldT>
void baseline_parallel_ntt(std::vector<FieldT> &a, const FieldT &omega, const size_t log_cpus)
{
    const size_t num_cpus = 1ul<<log_cpus;

    const size_t m = a.size();
    const size_t log_m = log2(m);
    if (m != 1ul<<log_m) throw DomainSizeException("expected m == 1ul<<log_m");

    if (log_m < log_cpus)
    {
        _basic_serial_radix2_FFT(a, omega);
        return;
    }

    std::vector<std::vector<FieldT> > tmp(num_cpus);
    for (size_t j = 0; j < num_cpus; ++j)
    {
        tmp[j].resize(1ul<<(log_m-log_cpus), FieldT::zero());
    }

    #pragma omp parallel for
    for (size_t j = 0; j < num_cpus; ++j)
    {
        const FieldT omega_j = omega^j;
        const FieldT omega_step = omega^(j<<(log_m - log_cpus));

        FieldT elt = FieldT::one();
        for (size_t i = 0; i < 1ul<<(log_m - log_cpus); ++i)
        {
            for (size_t s = 0; s < num_cpus; ++s)
            {
                // invariant: elt is omega^(j*idx)
                const size_t idx = (i + (s<<(log_m - log_cpus))) % (1u << log_m);
                tmp[j][i] += a[idx] * elt;
                elt *= omega_step;
            }
            elt *= omega_j;
        }
    }

    const FieldT omega_num_cpus = omega^num_cpus;

    #pragma omp parallel for
    for (size_t j = 0; j < num_cpus; ++j)
    {
        _basic_serial_radix2_FFT(tmp[j], omega_num_cpus);
    }

    #pragma omp parallel for
    for (size_t i = 0; i < num_cpus; ++i)
    {
        for (size_t j = 0; j < 1ul<<(log_m - log_cpus); ++j)
        {
            // now: i = idx >> (log_m - log_cpus) and j = idx % (1u << (log_m - log_cpus)), for idx = ((i<<(log_m-log_cpus))+j) % (1u << log_m)
            a[(j<<log_cpus) + i] = tmp[i][j];
        }
    }
}

int test(int k) {
    size_t degree = 1 << k;

    // Print Process Info
    const size_t num_cpus = omp_get_max_threads();
    const size_t log_cpus = ((num_cpus & (num_cpus - 1)) == 0 ? log2(num_cpus) : log2(num_cpus) - 1);
    std::cout << "[i] Mode : Parallel" << std::endl;
    std::cout << "\t- num_cpus : " << num_cpus << std::endl;
    std::cout << "\t- log_cpus : " << log_cpus << std::endl;

    // Read Polynomial & Print Parameter
    std::vector<FieldT> a;
    std::vector<FieldT> v(a);
    std::vector<FieldT> u(a);
    bls12_381_pp::init_public_params();
    generate_polynomial_to_file("data/input_a_2.txt", degree);
    if(!read_polynomial("data/input_a_2.txt", a)) return 1;
    const size_t n = libff::get_power_of_two(a.size());
    FieldT omega = libff::get_root_of_unity<FieldT>(n);
    std::cout << "[i] NTT Parameter" << std::endl;
    std::cout << "\t - Polynomial Size : " << "2^" <<  log2(n) << std::endl;
    std::cout << "\t - Omega : 0x" << std::hex << omega << std::endl;
    std::cout << "\t - O_inv : 0x" << std::hex << omega.inverse() << std::endl;

    u.resize(n, FieldT::zero());
    v.resize(n, FieldT::zero());
    // Serial Timing Measure
    {
    std::cout << "[*] processing Serial FFT";
    std::cout.flush();
    auto start_time = std::chrono::high_resolution_clock::now();
    baseline_serial_ntt(v, omega);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;

    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left << "\r[+] Serial process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;
    }
    
    // Parallel Timing Measure
    {
    std::cout << "[*] processing Parallel FFT";
    std::cout.flush();
    auto start_time = std::chrono::high_resolution_clock::now();
    baseline_parallel_ntt(u, omega, log_cpus);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    auto minutes = duration.count() / 60000;
    auto seconds = (duration.count() % 60000) / 1000;
    auto milliseconds = duration.count() % 1000;
    
    std::cout << std::dec;
    std::cout << std::setw(_print_align) << std::left << "\r[+] Parallel process complete"
              << std::setw(10) << std::right << " (" << minutes << "m " << seconds << "s " << milliseconds << "ms)" << std::endl;
    }
    
    if (v != u) std::cout << "Serial and Parallel Results are different" << std::endl;

    if(!write_polynomial("data/output_a_ntt.txt", u)) return 1;

    return 0;
}

int main(){
    int i;

    for(i = 27; i < 28; i++) {
        std::cout << "# Test " << i << std::endl;
        test(i);
        std::cout << std::endl;
    }

    return 0;
}