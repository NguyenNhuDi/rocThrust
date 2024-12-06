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


#define SETUP_SEQUENTIAL(x)                         \
    using T = typename TestFixture::input_type;     \
    using v = typename std::vector<T>;              \
    size_t n = std::min((double)x,                  \
        (double)std::numeric_limits<T>::max());     \
    v test_heap(n);                                 \
    for(size_t i = 0; i < n; i++)                   \
        test_heap[i] = (T)i;                        \

#define SETUP_RANDOM(x)                                                                     \
    using T = typename TestFixture::input_type;                                             \
    using v = typename std::vector<T>;                                                      \
    size_t n = std::min((double)x,                                                          \
        (double)std::numeric_limits<T>::max());                                             \
    v test_heap(n);                                                                         \
    static std::random_device rd;                                                           \
    std::mt19937 rng(rd());                                                                 \
    std::uniform_real_distribution<double>                                                  \
        dis((double)std::numeric_limits<T>::min(), (double)std::numeric_limits<T>::max());  \
    for(size_t i = 0; i < n; i++){                                                          \
        T rNum = (T) dis(rng);                                                              \
        test_heap[i] = rNum;                                                                \
    }                                                                                       \
    sort(test_heap.begin(), test_heap.end());                                               \

TESTS_DEFINE(IsHeapUntilTests, NumericalTestsParams)


TYPED_TEST(IsHeapUntilTests, TestIsHeapSequentialSmall){
    SETUP_SEQUENTIAL(10)
    for(size_t position = 1; position <= 10; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}

TYPED_TEST(IsHeapUntilTests, TestIsHeapSequentialMedium){

    SETUP_SEQUENTIAL(100)
    for(size_t position = 1; position <= 100; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}

TYPED_TEST(IsHeapUntilTests, TestIsHeapSequentialLarge){
    SETUP_SEQUENTIAL(100000)
    for(size_t position = 1; position <= n; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}

TYPED_TEST(IsHeapUntilTests, TestIsHeapRandomSmall){
    SETUP_RANDOM(10)
    for(size_t position = 1; position <= 10; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}

TYPED_TEST(IsHeapUntilTests, TestIsHeapRandomMedium){

    SETUP_RANDOM(100)
    for(size_t position = 1; position <= 100; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}

TYPED_TEST(IsHeapUntilTests, TestIsHeapRandomLarge){
    SETUP_RANDOM(100000)
    for(size_t position = 1; position <= n; position++){
        
        std::make_heap(test_heap.begin(), test_heap.begin() + position);   
        auto pos_end = test_heap.begin() + position;

        ASSERT_EQ(pos_end, std::is_heap_until(test_heap.begin(), test_heap.end()));
    }
}