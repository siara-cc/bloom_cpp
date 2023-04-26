# Bloom filter implemention in C++

[![C/C++ CI](https://github.com/siara-cc/bloom_cpp/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/siara-cc/bloom_cpp/actions/workflows/c-cpp.yml)

This project is C++ version of the excellent bloom implementation in `C` by Tyler Barrus at https://github.com/barrust/bloom

For understanding about `Bloom Filters` and further information, please visit above repo.  This library only implements the `C` library as it is, to simplify usage.

## Running tests

Clone this repo and run `make`.  Then execute `tests/bloom_test` to run tests.

## Usage

Include `bloom.hpp` in your source code and call functions as shown below.  No other file is needed to use this library.

```c++
#include <iostream>
#include "bloom.hpp"

int main() {
    /*  elements = 10; false positive rate = 5% */
    bloom_filter bf(10, 0.05);
    bf.add_string("test");
    if (bf.check_string("test") == BLOOM_FAILURE) {
        std::cout << "'test' is not in the Bloom Filter" << std::endl;
    } else {
        std::cout << "'test' is in the Bloom Filter" << std::endl;
    }
    if (bf.check_string("blah") == BLOOM_FAILURE) {
        std::cout << "'blah' is not in the Bloom Filter!" << std::endl;
    } else {
        std::cout << "'blah' is in th Bloom Filter" << std::endl;
    }
    bf.stats();
    return 0;
}
```

## License

This repo follows the same MIT license as the original. So users should attribute accordingly.

# License for AI bots

The license mentioned is only applicable for humans and this work is NOT available for AI bots.

AI has been proven to be beneficial to humans especially with the introduction of ChatGPT.  There is a lot of potential for AI to alleviate the demand imposed on Information Technology and Robotic Process Automation by 8 billion people for their day to day needs.

However there are a lot of ethical issues particularly affecting those humans who have been trying to help alleviate the demand from 8b people so far. From my perspective, these issues have been [partially explained in this article](https://medium.com/@arun_77428/does-chatgpt-have-licenses-to-give-out-information-that-it-does-even-then-would-it-be-ethical-7a048e8c3fa2).

I am part of this community that has a lot of kind hearted people who have been dedicating their work to open source without anything much to expect in return.  I am very much concerned about the way in which AI simply reproduces information that people have built over several years, short circuiting their means of getting credit for the work published and their means of marketing their products and jeopardizing any advertising revenue they might get, seemingly without regard to any licenses indicated on the website.

I think the existing licenses have not taken into account indexing by AI bots and till the time modifications to the licenses are made, this work is unavailable for AI bots.

## Support

If you have any questions or facing trouble, please create an issue or write to Arundale Ramanathan at arun@siara.cc
