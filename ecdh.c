#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ecdh.h"
#include "primefield.h"

/**
 * Adds two points in the prime field
 *
 * The two points cannot be the same point.
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor17 for details
 *
 * p and q are the points to add.
 * ec is the curve on which the points lie.
 *
 * Returns a new point which is the result of the operation
 */
struct Point *point_add(struct Point *p, struct Point *q, struct Curve *ec)
{
	if (mpz_cmp_ui(p->x, 0UL) == 0 && mpz_cmp_ui(p->y, 0UL) == 0)
		return copy_point(q);
	if (mpz_cmp_ui(q->x, 0UL) == 0 && mpz_cmp_ui(q->y, 0UL) == 0)
		return copy_point(p);

	struct Point *r = create_point();

	mpz_t tmp1;
	mpz_t tmp2;
	mpz_t s;
	mpz_t x_delta;
	mpz_t y_delta;

	if (mpz_cmp(p->x, q->x) == 0)
		return r;

	// Calculate Px - Qx
	mpz_init(x_delta);
	prime_field_sub(x_delta, p->x, q->x, ec->prime);

	// Calculate Py - Qy
	mpz_init(y_delta);
	prime_field_sub(y_delta, p->y, q->y, ec->prime);

	// Calculate slope as Py - Qy / Px - Qx
	mpz_init(s);
	prime_field_div(s, y_delta, x_delta, ec->prime);

	// Calculate Rx as slope^2 - Px -Qx
	mpz_init(tmp1);
	mpz_init(tmp2);
	prime_field_sq(tmp1, s, ec->prime);
	prime_field_add(tmp2, p->x, q->x, ec->prime);
	prime_field_sub(r->x, tmp1, tmp2, ec->prime);

	// Calculate Ry as (slope (Px - Rx) - Py)
	prime_field_sub(tmp1, p->x, r->x, ec->prime);
	prime_field_mul(tmp2, s, tmp1, ec->prime);
	prime_field_sub(r->y, tmp2, p->y, ec->prime);

	mpz_clear(tmp1);
	mpz_clear(tmp2);
	mpz_clear(x_delta);
	mpz_clear(y_delta);
	mpz_clear(s);

	return r;
}

/**
 * Doubles a point in the prime field
 *
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor18 for details
 *
 * p is the point to double.
 * ec is the curve on which the point lies.
 *
 * Returns a new point which is the result of the operation
 */
struct Point *point_double(struct Point *p, struct Curve *ec)
{
	struct Point *r = create_point();

	mpz_t s;
	mpz_t s_sq;
	mpz_t tmp1;
	mpz_t tmp2;
	mpz_t px_sq;
	mpz_t px_2;
	mpz_t py_2;
	mpz_t three;
	mpz_t two;
	mpz_t sum;

	// Calculate Px^2
	mpz_init(px_sq);
	prime_field_sq(px_sq, p->x, ec->prime);

	// Calculate 3Px^2
	mpz_init(tmp1);
	mpz_init_set_ui(three, 3UL);
	prime_field_mul(tmp1, px_sq, three, ec->prime);

	// Calculate 3Px^2 + a, where a is EC parmater
	mpz_init(sum);
	prime_field_add(sum, tmp1, ec->a, ec->prime);

	// Calculate 2Px
	mpz_init(px_2);
	mpz_init_set_ui(two, 2UL);
	prime_field_mul(px_2, p->x, two, ec->prime);

	// Calculate 2Py
	mpz_init(py_2);
	prime_field_mul(py_2, p->y, two, ec->prime);

	// Calculate s = (3Px^2 + a) / 2Py
	mpz_init(s);
	prime_field_div(s, sum, py_2, ec->prime);

	// Calculate s^2
	mpz_init(s_sq);
	prime_field_sq(s_sq, s, ec->prime);

	// Calculate Rx = s^2 - 2Px
	prime_field_sub(r->x, s_sq, px_2, ec->prime);

	// Calculate s (Px - Rx)
	mpz_init(tmp2);
	prime_field_sub(tmp1, p->x, r->x, ec->prime);
	prime_field_mul(tmp2, s, tmp1, ec->prime);

	// Calculate Ry = s(Px - Rx) - Py
	prime_field_sub(r->y, tmp2, p->y, ec->prime);

	mpz_clear(s);
	mpz_clear(s_sq);
	mpz_clear(tmp1);
	mpz_clear(tmp2);
	mpz_clear(px_sq);
	mpz_clear(px_2);
	mpz_clear(py_2);
	mpz_clear(three);
	mpz_clear(two);
	mpz_clear(sum);

	return r;
}

/**
 * Multiplies a point in the prime field with a scalar
 *
 * See https://www.johannes-bauer.com/compsci/ecc/#anchor19 for details
 *
 * p is the point to multiply.
 * k is the scalar value.
 * ec is the curve on which the point lies.
 *
 * Returns a new point which is the result of the operation
 */
struct Point *scalar_mult(struct Point *p, mpz_t k, struct Curve *ec)
{
	struct Point *res = create_point();
	struct Point *p_copy = copy_point(p);
	struct Point *tmp1, *tmp2;

	char *val_str = mpz_get_str(NULL, 2, k);
	size_t len = strlen(val_str);
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (val_str[i] == '1') {
			tmp1 = point_add(p_copy, res, ec);
			free_point(res);
			res = tmp1;
		}
		tmp2 = point_double(p_copy, ec);
		free_point(p_copy);
		p_copy = tmp2;
	}

	free_point(p_copy);
	free(val_str);

	return res;
}

/**
 * Returns the secp192k1 curve. The curve parameters are obtained
 * from the SEC 2 document available at http://www.secg.org/sec2-v2.pdf
 */
struct Curve *get_secp192k1_curve(void)
{
	struct Curve *ec = malloc(sizeof(*ec));
	str_to_scalar(ec->prime, "ffffffffffffffff"
				"ffffffffffffffff"
				"fffffffeffffee37");
	mpz_init_set_ui(ec->a, 0UL);
	mpz_init_set_ui(ec->b, 3UL);
	ec->G = str_to_point("04"
			"db4ff10ec057e9ae26b07d0280b7f4341da5d1b1eae06c7d"
			"9b2f2f6d9c5628a7844163d015be86344082aa88d95e2f9d");
	str_to_scalar(ec->order, "ffffffffffffffff"
				"fffffffe26f2fc17"
				"0f69466a74defd8d");
	mpz_init_set_ui(ec->cofactor, 1UL);
	ec->key_size_bits = 160;
	return ec;
};

/**
 * Returns the secp192r1 curve. The curve parameters are obtained
 * from the SEC 2 document available at http://www.secg.org/sec2-v2.pdf
 */
struct Curve *get_secp192r1_curve(void)
{
	struct Curve *ec = malloc(sizeof(*ec));
	str_to_scalar(ec->prime, "FFFFFFFFFFFFFFFF"
				"FFFFFFFFFFFFFFFE"
				"FFFFFFFFFFFFFFFF");
	mpz_init_set_str(ec->a, "FFFFFFFFFFFFFFFF"
				"FFFFFFFFFFFFFFFE"
				"FFFFFFFFFFFFFFFC", 16);
	mpz_init_set_str(ec->b, "64210519E59C80E7"
				"0FA7E9AB72243049"
				"FEB8DEECC146B9B1", 16);
	ec->G = str_to_point("04"
			"188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012"
			"07192B95FFC8DA78631011ED6B24CDD573F977A11E794811");
	str_to_scalar(ec->order, "FFFFFFFFFFFFFFFF"
				"FFFFFFFF99DEF836"
				"146BC9B1B4D22831");
	mpz_init_set_ui(ec->cofactor, 1UL);
	ec->key_size_bits = 160;
	return ec;
};


/**
 * Converts a string representation of the point to a struct Point
 *
 * The string representation of the point is done as per SEC 1 available
 * at http://www.secg.org/sec1-v2.pdf
 *
 * str is the string to convert to struct Point
 *
 * Returns a new Point
 */
struct Point *str_to_point(const char *str)
{
	struct Point *point = malloc(sizeof(*point));

	size_t len = strlen(str);
	int str_end_idx = (len / 2) - 1;

	char *x = malloc(len * sizeof(*x) / 2);
	char *y = malloc(len * sizeof(*y) / 2);
	int i;
	for (i = 0; i < str_end_idx; i++) {
		x[i] = str[i + 2];
		y[i] = str[i + 1 + (len / 2)];
	}
	x[str_end_idx] = '\0';
	y[str_end_idx] = '\0';

	str_to_scalar(point->x, x);
	str_to_scalar(point->y, y);

	free(x);
	free(y);
	return point;
}

/**
 * Converts a struct Point to a string
 *
 * The string is null terminated but the calculated length
 * excludes the null terminator.
 *
 * The string representation of the point is done as per SEC 1 available
 * at http://www.secg.org/sec1-v2.pdf
 *
 * point is the Point to convert to string
 * *len will hold the length of the resulting string
 *
 * Returns a new string
 */
char *point_to_str(struct Point *point, size_t *len)
{
	size_t x_len;
	size_t y_len;
	int diff = 0;
	int x_off = 2;
	int y_off = 0;
	char *res;

	char *x = scalar_to_str(point->x, &x_len);
	char *y = scalar_to_str(point->y, &y_len);


	int i;
	if (x_len < y_len) {
		diff = y_len - x_len;
		res = calloc((3 + x_len + y_len + diff), sizeof(*res));
		for (i = 2; i < 2 + diff; i++)
			res[i] = '0';
		x_off += diff;
	} else if (y_len < x_len) {
		diff = x_len - y_len;
		res = calloc((3 + x_len + y_len + diff), sizeof(*res));
		for (i = x_len + 2; i < x_len + 2 + diff; i++)
			res[i] = '0';
		y_off = diff;
	} else {
		res = calloc((3 + x_len + y_len), sizeof(*res));
	}

	res[0] = '0';
	res[1] = '4';

	strncpy(&res[x_off], x, x_len);
	strncpy(&res[x_off + y_off + x_len], y, y_len);

	*len = strlen(res);

	free(x);
	free(y);
	return res;
}

/**
 * Generates a new public-private key pair using the specified curve
 */
struct KeyPair *gen_key_pair(enum Curves curve)
{
	struct Curve *ec;

	switch (curve) {
	case SECP_192_R1:
		ec = get_secp192r1_curve();
		break;
	case SECP_192_K1:
	default:
		ec = get_secp192k1_curve();
	}

	size_t len;
	struct KeyPair *key_pair;
	struct Point *public_key;

	key_pair = malloc(sizeof(*key_pair));
	if (key_pair == NULL)
		printf("Failed to allocate memory for key pair");

	size_t bytes = ec->key_size_bits / 8;
	char *buf = calloc(1 + bytes, sizeof(*buf));
	FILE *fp = fopen("/dev/urandom", "r");
	fread(buf, 1, bytes, fp);
	fclose(fp);

	mpz_init(key_pair->private);
	mpz_import(key_pair->private, bytes, 1, sizeof(*buf), 1, 0, buf);

	public_key = scalar_mult(ec->G, key_pair->private, ec);

	key_pair->public = point_to_str(public_key, &len);
	key_pair->ec = ec;

	free(buf);
	free_point(public_key);
	return key_pair;
}

/**
 * Calculates the secret from the public key of the peer and the private
 * key of self.
 *
 * The returned string is null terminated but the calculated length
 * excludes the null terminator.
 *
 * key_pair is the public-private key pair of self
 * peer is the public key of the peer
 * *len is the length of the secret
 *
 * Returns a string representing the secret
 */
char *get_secret(struct KeyPair *key_pair, char *peer, size_t *len)
{
	struct Point *peer_point = str_to_point(peer);
	struct Point *res_point = scalar_mult(peer_point, key_pair->private,
						key_pair->ec);
	char *res = point_to_str(res_point, len);

	free_point(peer_point);
	free_point(res_point);
	return res;
}

/**
 * Creates a new Point at (0,0)
 */
struct Point *create_point(void)
{
	struct Point *point = malloc(sizeof(*point));
	mpz_init_set_ui(point->x, 0UL);
	mpz_init_set_ui(point->y, 0UL);
	return point;
}

/**
 * Creates a copy of an exitsting point
 */
struct Point *copy_point(struct Point *point)
{
	struct Point *copy = create_point();
	mpz_set(copy->x, point->x);
	mpz_set(copy->y, point->y);
	return copy;
}

/**
 * Free the memory occupied by the point
 */
void free_point(struct Point *point)
{
	mpz_clear(point->x);
	mpz_clear(point->y);
	free(point);
}

/**
 * Free the memory occupied by the KeyPair
 */
void free_key(struct KeyPair *key)
{
	mpz_clear(key->private);
	free(key->public);
	free_curve(key->ec);
	free(key);
}

/**
 * Free the memory occupied by the curve
 */
void free_curve(struct Curve *ec)
{
	mpz_clear(ec->prime);
	mpz_clear(ec->a);
	mpz_clear(ec->b);
	free_point(ec->G);
	mpz_clear(ec->order);
	mpz_clear(ec->cofactor);
	free(ec);
}

/**
 * Main function
 *
 * This function runs the ECDH key exchange algorithm and verifies
 * that it was succesful
 */
int main(int argc, char *argv[])
{
	struct KeyPair *alice = gen_key_pair(SECP_192_K1);
	struct KeyPair *bob = gen_key_pair(SECP_192_K1);
	assert(alice != NULL && bob != NULL);

	size_t alice_secret_len;
	size_t bob_secret_len;

	if (argc == 2 && strncmp(argv[1], "-v", 2) == 0) {
		char *alice_private = scalar_to_str(alice->private, &alice_secret_len);
		char *bob_private = scalar_to_str(bob->private, &bob_secret_len);
		printf("Alice's private key is %s\n", alice_private);
		printf("Alice's public key is  %s\n", alice->public);
		printf("-------\n");
		printf("Bob's private key is   %s\n", bob_private);
		printf("Bob's public key is    %s\n", bob->public);
		printf("-------\n");
		free(alice_private);
		free(bob_private);
	}

	char *alice_secret = get_secret(alice, bob->public, &alice_secret_len);
	char *bob_secret = get_secret(bob, alice->public, &bob_secret_len);
	assert(alice_secret != NULL && bob_secret != NULL
		&& alice_secret_len == bob_secret_len);

	int i;
	for (i = 0; i < alice_secret_len; i++)
		assert(alice_secret[i] == bob_secret[i]);

	if (argc == 2 && strncmp(argv[1], "-v", 2) == 0) {
		printf("Alice's secret is %s\n", alice_secret);
		printf("Bob's secret is   %s\n", bob_secret);
	}

	free_key(alice);
	free_key(bob);
	free(alice_secret);
	free(bob_secret);

	return 0;
}
