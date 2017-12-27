//by Aashish Dugar
#ifndef __primefield_header
#define __primefield_header

#include <gmp.h>

/**
 * Adds two numbers which are in the prime field
 *
 * This is similar to normal addition except that the result
 * is bound between 0 and p.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor03 for details.
 *
 * res is the return variable. It must be initialized.
 * a and b are the numbers to add. They have to be within the prime field.
 * p is the prime number defining the field.
 */
void prime_field_add(mpz_t res, mpz_t a, mpz_t b, mpz_t p)
{
	mpz_t tmp;
	mpz_init(tmp);

	mpz_add(tmp, a, b);
	if (mpz_cmp(tmp, p) >= 0)
		mpz_sub(res, tmp, p);
	else if (mpz_cmp_ui(tmp, 0UL) < 0)
		mpz_add(res, tmp, p);
	else
		mpz_set(res, tmp);

	mpz_clear(tmp);
}

/**
 * Subtracts two numbers which are in the prime field
 *
 * This is similar to normal subtraction except that the result
 * is bound between 0 and p.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor03 for details.
 *
 * res is the return variable. It must be initialized.
 * a and b are the numbers to subtract. They have to be within the prime field.
 * p is the prime number defining the field.
 */
void prime_field_sub(mpz_t res, mpz_t a, mpz_t b, mpz_t p)
{
	mpz_t tmp;
	mpz_init(tmp);
	mpz_neg(tmp, b);
	prime_field_add(res, a, tmp, p);
	mpz_clear(tmp);
}

/**
 * Multiplies two numbers which are in the prime field
 *
 * The function loops copies b into a throwaway variable and loops
 * over the bits of b, starting with most significant bit. If the
 * bit is set, it adds the value of the copied throwaway to the result.
 * Then it doubles the value of the throwaway. All operations are
 * prime field operations.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor05 for details.
 *
 * res is the return variable. It must be initialized.
 * a and b are the numbers to multiply. They have to be within the prime field.
 * p is the prime number defining the field.
 */
void prime_field_mul(mpz_t res, mpz_t a, mpz_t b, mpz_t p)
{
	mpz_t copy;
	mpz_t tmp;
	mpz_init_set(copy, a);
	mpz_init(tmp);
	mpz_set_ui(res, 0UL);

	char *bits = mpz_get_str(NULL, 2, b);
	size_t bitlength = strlen(bits);

        int i;
	for (i = bitlength - 1; i >= 0; i--) {
		if (bits[i] == '1') {
			prime_field_add(tmp, res, copy, p);
			mpz_set(res, tmp);
		}
		prime_field_add(tmp, copy, copy, p);
		mpz_set(copy, tmp);
	}
	mpz_clear(copy);
	mpz_clear(tmp);
	free(bits);
}

/**
 * Divides two numbers which are in the prime field
 *
 * The function first calculates the inverse of b in the prime field,
 * and then multiplies a with that number to get the result.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor07 for details.
 *
 * res is the return variable. It must be initialized.
 * a is the dividend and b is the divisor. Both must be in the prime field.
 * p is the prime number defining the field.
 */
void prime_field_div(mpz_t res, mpz_t a, mpz_t b, mpz_t p)
{
	mpz_t q, r, s, t, u, v, copy_b, copy_p, u_new, v_new, tmp;
	mpz_init(q);
	mpz_init(r);
	mpz_init_set_ui(s, 1UL);
	mpz_init_set_ui(t, 0UL);
	mpz_init_set_ui(u, 0UL);
	mpz_init_set_ui(v, 1UL);
	mpz_init_set(copy_b, b);
	mpz_init_set(copy_p, p);
	mpz_init(u_new);
	mpz_init(v_new);
	mpz_init(tmp);

	while (mpz_cmp_ui(copy_p, 0UL) != 0) {
		mpz_fdiv_qr(q, r, a, copy_p);
		mpz_set(u_new, s);
		mpz_set(v_new, t);
		mpz_mul(tmp, q, s);
		mpz_sub(s, u, tmp);
		mpz_mul(tmp, q, t);
		mpz_sub(t, v, tmp);
		mpz_set(copy_b, copy_p);
		mpz_set(copy_p, r);
		mpz_set(u, u_new);
		mpz_set(v, v_new);
	}
	prime_field_mul(res, a, u, p);

	mpz_clear(q);
	mpz_clear(r);
	mpz_clear(s);
	mpz_clear(t);
	mpz_clear(u);
	mpz_clear(v);
	mpz_clear(copy_p);
	mpz_clear(copy_b);
	mpz_clear(u_new);
	mpz_clear(v_new);
	mpz_clear(tmp);
}

/**
 * Squares a number in the prime field
 *
 * This is uses the same approach as multiplication.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor09 for details
 *
 * res is the return variable. It must be initialized.
 * a is the number to square.
 * p is the prime number defining the field.
 */
void prime_field_sq(mpz_t res, mpz_t a, mpz_t p)
{
	mpz_t copy;
	mpz_t tmp;
	mpz_init_set(copy, a);
	mpz_init(tmp);
	mpz_set_ui(res, 1UL);

	char *bits = "10";

        int i;
	for (i = 1; i >= 0; i--) {
		if (bits[i] == '1') {
			prime_field_mul(tmp, res, copy, p);
			mpz_set(res, tmp);
		}
		prime_field_mul(tmp, copy, copy, p);
		mpz_set(copy, tmp);
	}
	mpz_clear(tmp);
	mpz_clear(copy);
}

/**
 * Converts a hex-string representation of a scalar to
 * a GMP integer
 *
 * scalar is an uninitialized pointer to the result
 * str is the hex string containing the number
 */
int str_to_scalar(mpz_t scalar, const char *str)
{
	return mpz_init_set_str(scalar, str, 16);
}

/**
 * Returns the hex-string for the given scalar
 *
 * The string is null terminated but the calculated length
 * excludes the null terminator.
 *
 * scalar is the number to convert
 * *len is a pointer which will hold the length of the result
 */
char *scalar_to_str(mpz_t scalar, size_t *len)
{
	*len = mpz_sizeinbase(scalar, 16) + 2;
	char *str = malloc((*len) * sizeof(*str));
	mpz_get_str(str, 16, scalar);
	*len = strlen(str);
	return str;
}

#endif
