#include <algorithm>
#include <functional>

#include <libfqfft/evaluation_domain/domains/basic_radix2_domain_aux.hpp>
#include <libfqfft/kronecker_substitution/kronecker_substitution.hpp>
#include <libfqfft/tools/exceptions.hpp>

using namespace libfqfft;

/* Polynomial Multiplication via FFT */
template <typename FieldT>
void polynomial_multiplication_on_FFT_example ()
{

    /* Polynomial a = 1 + 2x + 3x^2 + x^3 */
    /* Polynomial b = 1 + 2x + x^2 + x^3 */
    std::vector<FieldT> a = { 1, 2, 3, 2 };
    std::vector<FieldT> b = { 1, 2, 2, 2 };

    const size_t n = libff::get_power_of_two(a.size() + b.size() - 1);
    FieldT omega = libff::get_root_of_unity<FieldT>(n);

    std::vector<FieldT> u(a);
    std::vector<FieldT> v(b);
    std::vector<FieldT> c(1, FieldT::zero());

    u.resize(n, FieldT::zero());
    v.resize(n, FieldT::zero());
    c.resize(n, FieldT::zero());

    _basic_serial_radix2_FFT(u, omega);
    _basic_serial_radix2_FFT(v, omega);

    std::transform(u.begin(), u.end(), v.begin(), c.begin(), std::multiplies<FieldT>());

    _basic_serial_radix2_FFT(c, omega.inverse());

    /* Print out the polynomial in human-readable form */
    for (size_t i = 0; i < c.size(); i++)
    {
        unsigned long coefficient = c[i].as_ulong();

        if (i == 0) std::cout << coefficient << " + ";
        else if (i < 6) std::cout << coefficient << "x^" << i << " + ";
        else std::cout << coefficient << "x^" << i << std::endl;
    }
}

int main()
{
  polynomial_multiplication_on_FFT_example<libff::Double> ();
}
