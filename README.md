# Bloom filter implemention in C++

[![C/C++ CI](https://github.com/siara-cc/bloom_cpp/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/siara-cc/bloom_cpp/actions/workflows/c-cpp.yml)

This project is C++ version of the excellent bloom implementation in `C` by Tyler Barrus at https://github.com/barrust/bloom

For understanding about `Bloom Filters` and further information, please visit above repo.  This library only implements the `C` library as it is, to simplify usage.

## Running tests

Clone this repo and run `make`.  Then execute `tests/bloom_test` to run tests.

## Usage

Include `bloom.hpp` in your source code and call functions as shown below.  No other file is needed to use this library.

```c++
#include "bloom.hpp"

int main() {
    /*  elements = 10; false positive rate = 5% */
    bloom_filter bf(10, 0.05);
    bf.add_string("test");
    if (bf.check_string("test") == BLOOM_FAILURE) {
        printf("'test' is not in the Bloom Filter\n");
    } else {
        printf("'test' is in the Bloom Filter\n");
    }
    if (bf.check_string("blah") == BLOOM_FAILURE) {
        printf("'blah' is not in the Bloom Filter!\n");
    } else {
        printf("'blah' is in th Bloom Filter\n");
    }
    bf.stats();
    return 0;
}
```

## License

This repo follows the same MIT license as the original. So users should attribute accordingly.

## Support

If you have any questions or facing trouble, please create an issue or write to Arundale Ramanathan at arun@siara.cc
