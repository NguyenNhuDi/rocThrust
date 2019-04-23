/*
 *  Copyright 2018 NVIDIA Corporation
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

#pragma once

#include <thrust/detail/preprocessor.h>

/**
 * Dispatch between 32-bit and 64-bit index based versions of the same algorithm
 * implementation. This version assumes that callables for both branches consist
 * of the same tokens, and is intended to be used with Thrust-style dispatch
 * interfaces, that always deduce the size type from the arguments.
 */
#define THRUST_INDEX_TYPE_DISPATCH(status, call, count, arguments) \
    if (count <= std::numeric_limits<thrust::detail::int32_t>::max()) { \
        thrust::detail::int32_t THRUST_PP_CAT2(count, _fixed) = count; \
        status = call arguments; \
    } \
    else { \
        thrust::detail::int64_t THRUST_PP_CAT2(count, _fixed) = count; \
        status = call arguments; \
    }

/**
 * Dispatch between 32-bit and 64-bit index based versions of the same algorithm
 * implementation. This version allows using different token sequences for callables
 * in both branches, and is intended to be used with CUB-style dispatch interfaces,
 * where the "simple" interface always forces the size to be `int` (making it harder
 * for us to use), but the complex interface that we end up using doesn't actually
 * provide a way to fully deduce the type from just the call, making the size type
 * appear in the token sequence of the callable.
 *
 * See reduce_n_impl to see an example of how this is meant to be used.
 */
#define THRUST_INDEX_TYPE_DISPATCH2(status, call_32, call_64, count, arguments) \
    if (count <= std::numeric_limits<thrust::detail::int32_t>::max()) { \
        thrust::detail::int32_t THRUST_PP_CAT2(count, _fixed) = count; \
        status = call_32 arguments; \
    } \
    else { \
        thrust::detail::int64_t THRUST_PP_CAT2(count, _fixed) = count; \
        status = call_64 arguments; \
    }

