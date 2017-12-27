# ECDH-Algorithm
This is an implementation of the Elliptic-Curve Diffie-Hellman Cryptographic Algorithm.

This repo was initially a part of the course project CSCI-GA 3033-020 Practical Computer Security at New York University under
Prof. T. Srivatanakul, but can be further extended to research in the field of IoT.

The goal of the project was to try and reduce the computational resources
used by an cryptographic algortihm such that it is suitable for use in IoT
devices,by in turn sacrificing some acceptable amount of resources used in the operational scheme of the device. The code in the repository consists of two implementations of the Elliptic Curve Diffie-Hellman (ECDH) algorithms- 

1) The base implementation is done using OpenSSL APIs, which serves as a
   benchmark since it is a popular library for encrypting messages over the
   network. This implementation is in the ``ecdh-openssl.c`` file. It requires
   OpenSSL library and headers to be installed.

2) The implementation that we developed as part of the project is in
   ``ecdh.c``, ``ecdh.h`` and ``primefield.h`` files. It uses GMP for arbitrary
   precision arithmetic(i.e limited by the host machine). GMP is required to run and compile the code.
   

## Compilation
This code is meant only for systems running Linux or with a Linux-compatible
``/dev/urandom`` device available on the system.

The code requires OpenSSL library and development headers and gmplib. Install
them using your package manager, if not already installed.

| To compile both implementations, run ``make``.
| To compile only OpenSSL version, run ``make ecdh-openssl``.
| To compile only our version, run ``make ecdh``.

A recent version of ``gcc`` is required for compilation. If the compiler
complains about ``-Wall`` as unrecognized option or the complains about
``-std=c99``, run the command as ``CC=gcc CFLAGS='-std=c99' make``


## Running
To run the code, execute ``./ecdh``. The code will generate two keypairs and validate
the ecdh exchange between them. To see the keypairs generated, execute ``./ecdh -v``.

To run the OpenSSL version, execute ``./ecdh-openssl``. There is no option to see the
keypairs generated here.


## Benchmarking
To get statistics on memory and CPU usage, run ``./utils/benchmark [executable] [iterations]``.
The benchmark script requires GNU time to run, not the shell built-in time.

