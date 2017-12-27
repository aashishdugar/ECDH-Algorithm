//by Aashish Dugar
#ifndef __ecdh_header
#define __ecdh_header

#include <gmp.h>

/**
 * Struct to represent a point in the prime field
 *
 * x and y are the co-ordinates of the point
 */
struct Point {
    mpz_t x;
    mpz_t y;
};

/**
 * Struct to represent an ellitic curve in a prime field
 * The curves are represented by the equation y^2 = x^3 + a*x + b
 * with the additional restriction that 4a^3 + 27b^2 != 0.
 *
 * All the fields below, except for key_size_bits, are public
 * knowledge and are available for various standard elliptic curves.
 * The key_size_bits is an implementation detail and can be
 * public as well.
 *
 * prime is the prime number defining the field.
 * a is the curve parameter a in the equation above.
 * b is the curve parameter b in the equation above.
 * G is the generator point of the curve and is public knowledge.
 * order is the order of the curve.
 * cofactor is the cofactor of the curve.
 * key_size_bits is the size in bits for the private keys.
 */
struct Curve {
    mpz_t prime;
    mpz_t a;
    mpz_t b;
    struct Point *G;
    mpz_t order;
    mpz_t cofactor;
    unsigned int key_size_bits;
};

/**
 * A collection of curves implemented by the code
 */
enum Curves {
    SECP_192_K1,
    SECP_192_R1
};


/**
 * Struct representing a public-private key pair
 *
 * private is the private key
 * public is the public key as a hexadecimal string
 * ec is the elliptic curve on which the key works
 */
struct KeyPair {
    mpz_t private;
    char *public;
    struct Curve *ec;
};

/* Functions for struct KeyPair */
struct KeyPair *gen_key_pair(enum Curves curve);
void free_key(struct KeyPair *key);

/* Functions for point arithmetic and conversions */
struct Point *point_add(struct Point *j, struct Point *k, struct Curve *ec);
struct Point *point_double(struct Point *p, struct Curve *ec);
struct Point *scalar_mult(struct Point *p, mpz_t k, struct Curve *ec);
struct Point *str_to_point(const char *str);
char *point_to_str(struct Point *point, size_t *len);
struct Point *create_point(void);
void free_point(struct Point *point);
struct Point *copy_point(struct Point *point);

/* Functions for struct Curve */
struct Curve *get_secp192k1_curve(void);
struct Curve *get_secp192r1_curve(void);
void free_curve(struct Curve *curve);

#endif
