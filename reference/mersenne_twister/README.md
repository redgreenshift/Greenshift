# Mersenne Twister Reference Sources

This directory contains original Mersenne Twister source files retained for historical reference, benchmarking, experimentation, and source provenance.

These files are not compiled directly as part of Project Greenshift.

## Purpose

The source files preserved here document the origins of later Greenshift Mersenne Twister implementations and provide a stable reference for comparison and verification.

Retaining the original source files makes it possible to determine whether behavior originated in the upstream Mersenne Twister sources or was introduced later by Greenshift modifications.

## Files

mt19937ar.c -- Original MT19937 reference implementation with the improved initialization algorithm introduced on 2002-01-26.

mt19937ar-cok.c -- Optimized MT19937 variant incorporating work by Shawn Cokus, Matthe Bellew, and Isaku Wada.

## Relationship to Greenshift

The following Greenshift implementations were derived from the source files contained in this directory:

| Greenshift Class | Derived From |
|----------|----------|
| `MTRand2002` | mt19937ar.c |
| `MTRand2002Cokus` | mt19937ar-cok.c |

These Greenshift classes contain additional wrappers, modernization, testing infrastructure, and utility functions.

The files contained in this directory are preserved reference copies and are not modified by Greenshift.


## Licensing History
According to the original authors:

 - GNU Public License (1997)
 - Artistic License (2001-04-06)
 - BSD License (2002-01-26)

The source files preserved in this directory originate from the 3-clause BSD-licensed MT2002 distribution.

See [`LICENSE.txt`](LICENSE.txt) for the complete license text.

## Provenance
The reference files currently stored in this directory were recovered from historical MT2002 distributions preserved by the Internet Archive Wayback Machine.

Primary references:

 - [MT2002 project page](https://web.archive.org/web/20031008112004/http://www.math.keio.ac.jp/~matumoto/MT2002/emt19937ar.html)
 - 2003 Archived source distributions
    - [http://www.math.keio.ac.jp/matumoto/CODES/MT2002/mt19937ar.c](https://web.archive.org/web/20031015153702/http://www.math.keio.ac.jp/matumoto/CODES/MT2002/mt19937ar.c)
    - [http://www.math.keio.ac.jp/matumoto/CODES/MT2002/MTARCOK/mt19937ar-cok.c](https://web.archive.org/web/20031017081801/http://www.math.keio.ac.jp/matumoto/CODES/MT2002/MTARCOK/mt19937ar-cok.c)
 - 2004 Archived source distributions
    - [http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c](https://web.archive.org/web/20040614201228/http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c)
    - [http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/MTARCOK/mt19937ar-cok.c](https://web.archive.org/web/20040609124912/http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/MTARCOK/mt19937ar-cok.c)
 - Original source files

> NOTE: The exact MT2002 revision represented by these recovered files has not yet been verified against the [currently published Hiroshima University copies](https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/emt19937ar.html).

These references were used to verify licensing, source lineage, and historical context during Greenshift's Mersenne Twister investigation.

Additional original documentation from the MT2002 distribution has been preserved in [`readme-mt.txt`](readme-mt.txt).

## Related Work

The original authors later released [SIMD-oriented Fast Mersenne Twister (SFMT)](https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/index.html).

`SFMT` improves performance and statistical properties relative to earlier MT19937 implementations while remaining part of the Mersenne Twister family.

During investigation of MT2002-based implementations, `SFMT` was discovered to provide a newer and significantly faster implementation, ultimately becoming Greenshift's default Mersenne Twister implementation.

This effectively ended further development of `MTRand2002`, which was retained as a benchmarking and reference implementation.

