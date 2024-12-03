/*
 *  Copyright 2008-2013 NVIDIA Corporation
 *  Modifications Copyright© 2019-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <thrust/functional.h>
#include <thrust/iterator/retag.h>
#include <thrust/partition.h>

#include "test_header.hpp"


//temp includes
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <limits>

#define SETUP_SEQUENTIAL(x, l, r)                                       \
    using T = typename TestFixture::input_type;                         \
    using v = typename std::vector<T>;                                  \
    size_t n = x;                                                       \
    v is_heap_pos(n), not_heap_pos(n), is_heap_neg(n), not_heap_neg(n); \
    int neg_i = 0;                                                      \
    for(int i = n-1; i >= 0; i--){                                      \
        is_heap_pos[i] = not_heap_pos[i] = (T)i;                        \
        is_heap_neg[i] = not_heap_neg[i] = (T)neg_i--;                  \
    }                                                                   \
    std::make_heap(is_heap_pos.begin() + l, is_heap_pos.begin() + r);   \
    std::make_heap(is_heap_neg.begin() + l, is_heap_neg.begin() + r);   \

#define SETUP_RANDOM(x, l, r)                                                               \
    using T = typename TestFixture::input_type;                                             \
    using v = typename std::vector<T>;                                                      \
    int n = x;                                                                              \
    v is_heap_pos(n), is_heap_neg(n);                                                       \
    static std::random_device rd;                                                           \
    std::mt19937 rng(rd());                                                                 \
    std::uniform_real_distribution<double>                                                  \
        dis((double)std::numeric_limits<T>::min(), (double)std::numeric_limits<T>::max());  \
    for(int i = 0; i < n; i++){                                                             \
        T rNum = (T) dis(rng);                                                              \
        is_heap_pos[i] = rNum;                                                              \
        is_heap_neg[i] = -rNum;                                                             \
    }                                                                                       \
    std::make_heap(is_heap_pos.begin() + l, is_heap_pos.begin() + r);                       \
    std::make_heap(is_heap_neg.begin() + l, is_heap_neg.begin() + r);                       \



TESTS_DEFINE(IsHeapTests, NumericalTestsParams)


TYPED_TEST(IsHeapTests, TestIsHeapSequentialSmall){
    SETUP_SEQUENTIAL(10, 0, 10)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
    
    ASSERT_FALSE(std::is_heap(not_heap_pos.begin(), not_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(not_heap_neg.begin(), not_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapSequentialLarge){
    SETUP_SEQUENTIAL(1000000, 0, 1000000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
    
    ASSERT_FALSE(std::is_heap(not_heap_pos.begin(), not_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(not_heap_neg.begin(), not_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapSequentialLeft){
    SETUP_SEQUENTIAL(10000, 0, 5000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.begin() + 5000));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.begin() + 5000));
    
    ASSERT_FALSE(std::is_heap(not_heap_pos.begin(), not_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(not_heap_neg.begin(), not_heap_neg.end()));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapSequentialMiddle){
    SETUP_SEQUENTIAL(10000, 2500, 7500)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin() + 2500, is_heap_pos.begin() + 7500));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin() + 2500, is_heap_neg.begin() + 7500));
   
    ASSERT_FALSE(std::is_heap(not_heap_pos.begin(), not_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(not_heap_neg.begin(), not_heap_neg.end()));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}


TYPED_TEST(IsHeapTests, TestIsHeapSequentialRight){
    SETUP_SEQUENTIAL(10000, 5000, 10000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin() + 5000, is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin() + 5000, is_heap_neg.end()));
    
    ASSERT_FALSE(std::is_heap(not_heap_pos.begin(), not_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(not_heap_neg.begin(), not_heap_neg.end()));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}


TYPED_TEST(IsHeapTests, TestIsHeapRandomSmall){
    SETUP_RANDOM(10, 0, 10)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapRandomLarge){
    SETUP_RANDOM(100000, 0, 100000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapRandomLeft){
    SETUP_RANDOM(10000, 0, 5000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin(), is_heap_pos.begin() + 5000));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin(), is_heap_neg.begin() + 5000));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}

TYPED_TEST(IsHeapTests, TestIsHeapRandomMiddle){
    SETUP_RANDOM(10000, 2500, 7500)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin() + 2500, is_heap_pos.begin() + 7500));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin() + 2500, is_heap_neg.begin() + 7500));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}


TYPED_TEST(IsHeapTests, TestIsHeapRandomRight){
    SETUP_RANDOM(10000, 5000, 10000)

    ASSERT_TRUE(std::is_heap(is_heap_pos.begin() + 5000, is_heap_pos.end()));
    ASSERT_TRUE(std::is_heap(is_heap_neg.begin() + 5000, is_heap_neg.end()));

    ASSERT_FALSE(std::is_heap(is_heap_pos.begin(), is_heap_pos.end()));
    ASSERT_FALSE(std::is_heap(is_heap_neg.begin(), is_heap_neg.end()));
}