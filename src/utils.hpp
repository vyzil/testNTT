#ifndef FFT_OPERATIONS_HPP
#define FFT_OPERATIONS_HPP

#include <chrono>
#include <vector>
#include <gmpxx.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <omp.h>

#include <libff/algebra/curves/bls12_381/bls12_381_pp.hpp>
#include <libff/algebra/field_utils/bigint.hpp>
#include <libfqfft/polynomial_arithmetic/basic_operations.hpp>
#include <libfqfft/evaluation_domain/domains/basic_radix2_domain_aux.hpp>

#define _print_align 40

using namespace libfqfft;
using namespace libff;

typedef bls12_381_Fr FieldT;
typedef Fp_model<4, bls12_381_modulus_r> Fp256;

void print_polynomial(const std::vector<FieldT>& poly);

bool read_polynomial_from_file(const std::string& filename, std::vector<FieldT>& poly);
bool read_polynomial(const std::string& filename, std::vector<FieldT>& poly);
bool write_polynomial_to_file(const std::string& filename, const std::vector<FieldT>& poly);
bool write_polynomial(const std::string& filename, const std::vector<FieldT>& poly);



#endif // FFT_OPERATIONS_HPP