/*
    Default tests for using the default hashing algorithm
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>  /* roundf */
#include <string.h>
#include "timing.h"  /* URL: https://github.com/barrust/timing-c */
#include "../src/bloom.hpp"

#define ELEMENTS 50000
#define FALSE_POSITIVE_RATE 0.05
#define KEY_LEN 10

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KCYN  "\x1B[36m"

/* private functions */
int check_known_values(bloom_filter *bf, int mult);
int check_known_values_alt(bloom_filter *bf, int mult, int mult2, int* used);
int check_unknown_values(bloom_filter *bf, int mult);
int check_unknown_values_alt(bloom_filter *bf, int mult, int mult2, int offset, int* used);
int check_unknown_values_alt_2(bloom_filter *bf, int mult, int mult2, int offset, int* used);
int success_or_failure(int res);
void populate_bloom_filter(bloom_filter *bf, unsigned long long elements, int mult);
static uint64_t __fnv_1a_mod(const uint8_t *key, const size_t str_len);
static uint64_t* __default_hash_mod(int num_hashes, const uint8_t *str, const size_t str_len);

int main() {
    Timing tm;
    timing_start(&tm);

    printf("Testing bloom_filter version %s\n\n", bloom_filter::get_version());
    bloom_filter bf(ELEMENTS, FALSE_POSITIVE_RATE);
    printf("Hello World\n");
    int cnt, used;
    populate_bloom_filter(&bf, ELEMENTS, 2);
    printf("Hello World1\n");
    printf("Bloom Filter insertion: ");
    if (bf.false_positive_probability == (float)FALSE_POSITIVE_RATE && bf.elements_added == ELEMENTS && roundf(100 * bf.current_false_positive_rate()) / 100 <= bf.false_positive_probability) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
        // TODO: add why these failed!
    }
    printf(KCYN "NOTE:" KNRM "Bloom Filter Current False Positive Rate: %f\n", bf.current_false_positive_rate());

    bf.stats();

    printf("Bloom Filter: Check known values (all should be found): ");
    cnt = check_known_values(&bf, 2);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter: Check known values (all should be either not found or false positive): ");
    cnt = check_unknown_values(&bf, 2);

    if ((float)cnt / ELEMENTS <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits! Or %f%%\n", cnt, (float)cnt / ELEMENTS);


    printf("Bloom Filter export: ");
    int ex_res = bf.bf_export("./test_bloom.blm");
    if (ex_res == 0)
        success_or_failure(ex_res);
    else
        return success_or_failure(ex_res);

    printf("Clear Bloom Filter: ");
    bf.clear();
    assert(bf.false_positive_probability == (float)FALSE_POSITIVE_RATE);
    assert(bf.elements_added == 0);  // should be empty!
    unsigned long u;
    cnt = 0;
    for(u = 0; u < bf.bloom_length; ++u) {
        if(bf.bloom[u] != 0) {
            cnt++;
        }
    }
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Cleanup original Bloom Filter: ");
    bf.destroy();
    success_or_failure(0);  // there is no failure mode for destroy

    /* import in the exported bloom filter and re-run tests */
    printf("Import from file: ");
    bloom_filter bfi;
    bfi.import("./test_bloom.blm");
    if (bfi.false_positive_probability == (float)FALSE_POSITIVE_RATE && bfi.elements_added == ELEMENTS && roundf(100 * bfi.current_false_positive_rate()) / 100 <= bfi.false_positive_probability) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
        // TODO: add why these failed!
    }

    printf("Bloom Filter Imported: Check known values (all should be found): ");
    cnt = check_known_values(&bfi, 2);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);


    printf("Bloom Filter Imported: Check known values (all should be either not found or false positive): ");
    cnt = check_unknown_values(&bfi, 2);
    if ((float)cnt / ELEMENTS <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits! Or %f%%\n", cnt, (float)cnt / ELEMENTS);


    printf("Export Bloom Filter as hex string: ");
    char* bloom_hex = bfi.export_hex_string();
    // printf("\n%s\n\n\n", bloom_hex);
    if (bloom_hex != NULL) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }


    printf("Bloom Filter Hex Import: ");
    bloom_filter bfh;
    bfh.import_hex_string(bloom_hex);
    if (bfh.false_positive_probability == (float)FALSE_POSITIVE_RATE && bfh.elements_added == ELEMENTS && roundf(100 * bfh.current_false_positive_rate()) / 100 <= bfh.false_positive_probability) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
        // TODO: add why these failed!
    }

    printf(KCYN "NOTE:" KNRM " Free bloom hex string\n");
    free(bloom_hex);

    printf("Bloom Filter Hex: Check same as imported: ");
    int qres = 0;
    if (bfh.false_positive_probability != bfi.false_positive_probability || bfh.elements_added != bfi.elements_added || bfh.current_false_positive_rate() != bfi.current_false_positive_rate()) {
        qres = -1;
    }
    uint64_t t;
    for (t = 0; t < bfh.bloom_length; ++t) {
        if (bfh.bloom[t] != bfi.bloom[t]) {
            qres = 1;
            break;
        }
    }
    if (qres == 0) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    printf("Cleanup imported Bloom Filter: ");
    bfi.destroy();
    success_or_failure(0);  // there is basically no failure mode

    printf("Bloom Filter Hex: Check known values (all should be found): ");
    cnt = check_known_values(&bfh, 2);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter Hex: Check known values (all should be either not found or false positive): ");
    cnt = check_unknown_values(&bfh, 2);
    if ((float)cnt / ELEMENTS <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits! Or %f%%\n", cnt, (float)cnt / ELEMENTS);


    printf("Cleanup hex Bloom Filter: ");
    bfh.destroy();
    success_or_failure(0);  // there is basically no failure mode

    printf("Bloom Filter initialize On Disk: ");
    bloom_filter bfd;
    bfd.import_on_disk("./test_bloom.blm");
    if (bfd.false_positive_probability == (float)FALSE_POSITIVE_RATE && bfd.elements_added == ELEMENTS && roundf(100 * bfd.current_false_positive_rate()) / 100 <= bfd.false_positive_probability) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
        // TODO: add why these failed!
    }

    printf("Bloom Filter On Disk: Check known values (all should be found): ");
    cnt = check_known_values(&bfd, 2);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter On Disk: Check known values (all should be either not found or false positive): ");
    cnt = check_unknown_values(&bfd, 2);
    if ((float)cnt / ELEMENTS <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits! Or %f%%\n", cnt, (float)cnt / ELEMENTS);

    printf("Cleanup On Disk Bloom Filter: ");
    bfd.destroy();
    success_or_failure(0);  // there is basically no failure mode

    bloom_filter res;
    bloom_filter bf1;
    bloom_filter bf2;
    printf("Bloom Filter Union / Intersection / Jaccard Index: setup Bloom Filters: ");
    res.init(ELEMENTS * 4, FALSE_POSITIVE_RATE);
    bf1.init(ELEMENTS * 4, FALSE_POSITIVE_RATE);
    bf2.init(ELEMENTS * 4, FALSE_POSITIVE_RATE);

    populate_bloom_filter(&bf1, ELEMENTS * 2, 2);
    populate_bloom_filter(&bf2, ELEMENTS * 2, 3);
    cnt = check_known_values(&bf1, 2);
    cnt += check_known_values(&bf2, 3);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter Union: \n");
    printf("Bloom Filter Union: known values: ");
    res.bf_union(&res, &bf1, &bf2);
    cnt = check_known_values(&res, 2);
    cnt += check_known_values(&res, 3);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter Union: unknown values: ");
    cnt = check_unknown_values_alt(&res, 2, 3, 11, &used);
    if ((float)cnt / used <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits out of %d elements! Or %f%%\n", cnt, used, (float)cnt / used);
    res.stats();

    printf("Bloom Filter Union: count set bits without storing: ");
    if (bf1.count_union_bits_set(&bf1, &bf2) == res.count_set_bits()) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    res.clear();
    printf("Bloom Filter Intersection: \n");
    printf("Bloom Filter Intersection: known values: ");
    res.clear();
    res.intersect(&res, &bf1, &bf2);
    cnt = check_known_values_alt(&res, 2, 3, &used);
    if (cnt == 0)
        success_or_failure(cnt);
    else
        return success_or_failure(cnt);

    printf("Bloom Filter Intersection: unknown values: ");
    cnt = check_unknown_values_alt_2(&res, 2, 3, 23, &used);
    if ((float)cnt / used <= (float) FALSE_POSITIVE_RATE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " %d flagged as possible hits out of %d elements! Or %f%%\n", cnt, used, (float)cnt / used);
    res.stats();

    printf("Bloom Filter Intersection: reset inserted elements: ");
    if (res.elements_added == res.estimate_elements()) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    printf("Bloom Filter Intersection: count set bits without storing: ");
    if (bf1.count_intersection_bits_set(&bf1, &bf2) == res.count_set_bits()) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    printf("Bloom Filter Jaccard Index: \n");
    printf("Bloom Filter Jaccard Index: same Bloom Filter: ");
    if (res.jaccard_index(&res, &res) == 1) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " similarity score: %f\n", res.jaccard_index(&res, &res));

    printf("Bloom Filter Jaccard Index: ~30 percent similar Bloom Filter: ");
    if (bf1.jaccard_index(&bf1, &bf2) < .35) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " similarity score: %f\n", bf1.jaccard_index(&bf1, &bf2));

    printf("Bloom Filter Jaccard Index: empty union: ");
    bloom_filter empty(500, 0.05);
    if (empty.jaccard_index(&empty, &empty) == 1.0) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    empty.destroy();


    printf("Bloom Filter Unable to Union or Intersect: \n");
    bf.init(ELEMENTS, FALSE_POSITIVE_RATE);
    printf("Bloom Filter Unable to Union or Intersect: Different number bits: ");
    if (bf.bf_union(&bf, &bf, &bf1) == BLOOM_FAILURE && bf.number_hashes == bf1.number_hashes) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    bf2.destroy();
    bf2.init(ELEMENTS, FALSE_POSITIVE_RATE - 0.01);
    printf("Bloom Filter Unable to Union or Intersect: Different number hashes: ");
    if (bf.bf_union(&bf, &bf, &bf2) == BLOOM_FAILURE && bf.number_hashes != bf2.number_hashes) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }

    // add one that uses a different hash function
    bf2.destroy();
    bf2.init_alt(ELEMENTS, FALSE_POSITIVE_RATE, &__default_hash_mod);
    printf("Bloom Filter Unable to Union or Intersect: Different hash functions: ");
    if (bf.bf_union(&bf, &bf, &bf2) == BLOOM_FAILURE) {
        success_or_failure(0);
    } else {
        return success_or_failure(-1);
    }
    printf(KCYN "NOTE:" KNRM " this is actually the same hash function, just a different location/pointer; perhaps this should really test the hash function?\n");

    printf("Cleanup Bloom Filter: ");
    bf.destroy();
    res.destroy();
    bf1.destroy();
    bf2.destroy();
    success_or_failure(0);

    timing_end(&tm);
    printf("\nCompleted Bloom Filter tests in %f seconds!\n", timing_get_difference(tm));
    printf("\nCompleted tests!\n");
    return 0;

}


/* private function definitions */
void populate_bloom_filter(bloom_filter *bf, unsigned long long elements, int mult) {
    for (unsigned long long i = 0; i < elements * mult; i+=mult) {
        char key[KEY_LEN] = {0};
        sprintf(key, "%llu", i);
        bf->add_string(key);
    }
}

int check_known_values(bloom_filter *bf, int mult) {
    int i, cnt = 0;
    for (i = 0; i < ELEMENTS * mult; i+=mult) {
        char key[KEY_LEN] = {0};
        sprintf(key, "%d", i);
        if (bf->check_string(key) == BLOOM_FAILURE) {
            cnt++;
        }
    }
    return cnt;
}

int check_known_values_alt(bloom_filter *bf, int mult, int mult2, int* used) {
    int i, cnt = 0;
    int j = 0;
    for (i = 0; i < ELEMENTS * mult; i+=mult) {
        if (i % mult2 == 0 && i % mult == 0) {
            char key[KEY_LEN] = {0};
            sprintf(key, "%d", i);
            if (bf->check_string(key) == BLOOM_FAILURE) {
                cnt++;
            }
            j++;
        }
    }
    *used = j;
    return cnt;
}

int check_unknown_values(bloom_filter *bf, int mult) {
    int i, cnt = 0;
    for (i = 1; i < ELEMENTS * mult; i+=mult) {
        char key[KEY_LEN] = {0};
        sprintf(key, "%d", i);
        if (bf->check_string(key) == BLOOM_SUCCESS) {
            cnt++;
        }
    }
    return cnt;
}

int check_unknown_values_alt(bloom_filter *bf, int mult, int mult2, int offset, int* used) {
    int i, cnt = 0;
    int j = 0;
    for (i = offset; i < ELEMENTS * offset; i+=offset) {
        if (i % mult2 == 0 || i % mult == 0) {
            // pass
        } else {
            char key[KEY_LEN] = {0};
            sprintf(key, "%d", i);
            if (bf->check_string(key) == BLOOM_SUCCESS) {
                cnt++;
            }
            j++;
        }
    }
    *used = j;
    return cnt;
}

int check_unknown_values_alt_2(bloom_filter *bf, int mult, int mult2, int offset, int* used) {
    int i, cnt = 0;
    int j = 0;
    for (i = offset; i < ELEMENTS * offset; i+=offset) {
        if (i % mult2 == 0 && i % mult == 0) {
            // pass
        } else {
            char key[KEY_LEN] = {0};
            sprintf(key, "%d", i);
            if (bf->check_string(key) == BLOOM_SUCCESS) {
                cnt++;
            }
            j++;
        }
    }
    *used = j;
    return cnt;
}

int success_or_failure(int res) {
    if (res == 0) {
        printf(KGRN "success!\n" KNRM);
    } else {
        printf(KRED "failure!\n" KNRM);
        return 1;
    }
    return 0;
}

/* NOTE: The caller will free the results */
static uint64_t* __default_hash_mod(int num_hashes, const uint8_t *str, const size_t str_len) {
    uint64_t *results = (uint64_t*)calloc(num_hashes, sizeof(uint64_t));
    int i;
    char *key = (char*)calloc(17, sizeof(char));  // largest value is 7FFF,FFFF,FFFF,FFFF
    results[0] = __fnv_1a_mod(str, str_len);
    for (i = 1; i < num_hashes; ++i) {
        sprintf(key, "%" PRIx64 "", results[i-1]);
        results[i] = __fnv_1a_mod((const uint8_t *) key, strlen(key));
    }
    free(key);
    return results;
}

static uint64_t __fnv_1a_mod(const uint8_t *key, const size_t len) {
    // FNV-1a hash (http://www.isthe.com/chongo/tech/comp/fnv/)
    size_t i;
    uint64_t h = 14695981039346656073ULL; // FNV_OFFSET 64 bit
    for (i = 0; i < len; ++i) {
        h = h ^ (unsigned char) key[i];
        h = h * 1099511628211ULL; // FNV_PRIME 64 bit
    }
    return h;
}
