// STREAMHPC TODO implement
#pragma once


#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC
#include <thrust/system/hip/config.h>

#include <thrust/detail/config.h>
#include <thrust/pair.h>

#include <thrust/detail/cstdint.h>
#include <thrust/detail/temporary_array.h>
#include <thrust/system/hip/detail/util.h>
#include <thrust/detail/raw_reference_cast.h>
#include <thrust/detail/type_traits/iterator/is_output_iterator.h>
#include <thrust/system/hip/detail/par_to_seq.h>
#include <thrust/system/hip/detail/get_value.h>
#include <thrust/functional.h>
#include <thrust/device_vector.h>
#include <thrust/detail/minmax.h>
#include <thrust/distance.h>
#include <thrust/detail/alignment.h>
#include <thrust/system/hip/detail/get_value.h>

// rocprim include
#include <rocprim/rocprim.hpp>

BEGIN_NS_THRUST

template <typename DerivedPolicy,
typename InputIterator1,
typename InputIterator2,
typename OutputIterator1,
typename OutputIterator2,
typename BinaryPredicate>
thrust::pair<OutputIterator1, OutputIterator2>
__host__ __device__
reduce_by_key(
              const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
              InputIterator1                                              keys_first,
              InputIterator1                                              keys_last,
              InputIterator2                                              values_first,
              OutputIterator1                                             keys_output,
              OutputIterator2                                             values_output,
              BinaryPredicate                                             binary_pred);

namespace hip_rocprim{

    namespace __reduce_by_key {

        template <class Derived,
                  class KeysInputIt,
                  class ValuesInputIt,
                  class KeysOutputIt,
                  class ValuesOutputIt,
                  class EqualityOp,
                  class ReductionOp>
        pair<KeysOutputIt, ValuesOutputIt>
        THRUST_HIP_RUNTIME_FUNCTION
        reduce_by_key(execution_policy<Derived>& policy,
                      KeysInputIt                keys_first,
                      KeysInputIt                keys_last,
                      ValuesInputIt              values_first,
                      KeysOutputIt               keys_output,
                      ValuesOutputIt             values_output,
                      EqualityOp                 equality_op,
                      ReductionOp                reduction_op)
        {
            typedef size_t size_type;
            size_type    num_items          = static_cast<size_type>(thrust::distance(keys_first, keys_last));
            void *       d_temp_storage     = NULL;
            size_t       temp_storage_bytes = 0;
            hipStream_t  stream             = hip_rocprim::stream(policy);
            size_type *  d_num_runs_out     = NULL;
            bool         debug_sync         = THRUST_HIP_DEBUG_SYNC_FLAG;

            if (num_items == 0)
              return thrust::make_pair(keys_output, values_output);

            hip_rocprim::throw_on_error(
                rocprim::reduce_by_key(d_temp_storage,
                                       temp_storage_bytes,
                                       keys_first,
                                       values_first,
                                       num_items,
                                       keys_output,
                                       values_output,
                                       d_num_runs_out,
                                       reduction_op,
                                       equality_op,
                                       stream,
                                       debug_sync),
                "reduce_by_key failed on 1st step");

            // Allocate temporary storage.
            void *ptr = get_memory_buffer(policy, sizeof(size_type) + temp_storage_bytes);

            d_num_runs_out = detail::aligned_reinterpret_cast<size_type*>(ptr);
            d_temp_storage = static_cast<void*>(&(((size_type*)ptr)[1]));

            hip_rocprim::throw_on_error(
                rocprim::reduce_by_key(d_temp_storage,
                                       temp_storage_bytes,
                                       keys_first,
                                       values_first,
                                       num_items,
                                       keys_output,
                                       values_output,
                                       d_num_runs_out,
                                       reduction_op,
                                       equality_op,
                                       stream,
                                       debug_sync),
                "reduce_by_key failed on 2nd step");

            size_type num_runs_out = hip_rocprim::get_value(policy, d_num_runs_out);

            return_memory_buffer(policy, ptr);

            return thrust::make_pair(keys_output + num_runs_out, values_output + num_runs_out);
        }

    } // namespace __reduce_by_key

    //-------------------------
    // Thrust API entry points
    //-------------------------

    __thrust_exec_check_disable__
    template <class Derived,
              class KeyInputIt,
              class ValInputIt,
              class KeyOutputIt,
              class ValOutputIt,
              class BinaryPred,
              class BinaryOp>
    pair<KeyOutputIt, ValOutputIt> THRUST_HIP_FUNCTION
    reduce_by_key(execution_policy<Derived> &policy,
                  KeyInputIt                 keys_first,
                  KeyInputIt                 keys_last,
                  ValInputIt                 values_first,
                  KeyOutputIt                keys_output,
                  ValOutputIt                values_output,
                  BinaryPred                 binary_pred,
                  BinaryOp                   binary_op)
    {
        pair<KeyOutputIt, ValOutputIt> ret = thrust::make_pair(keys_output, values_output);

        THRUST_HIP_PRESERVE_KERNELS_WORKAROUND((
            __reduce_by_key::reduce_by_key<Derived, KeyInputIt, ValInputIt, KeyOutputIt, ValOutputIt, BinaryPred, BinaryOp>));

#if __THRUST_HAS_HIPRT__

        ret = __reduce_by_key::reduce_by_key(policy,
                                                 keys_first,
                                                 keys_last,
                                                 values_first,
                                                 keys_output,
                                                 values_output,
                                                 binary_pred,
                                                 binary_op);

#else // __THRUST_HAS_HIPRT__

            ret = thrust::reduce_by_key(cvt_to_seq(derived_cast(policy)),
                                        keys_first,
                                        keys_last,
                                        values_first,
                                        keys_output,
                                        values_output,
                                        binary_pred,
                                        binary_op);
#endif // __THRUST_HAS_HIPRT__

        return ret;
    }


    __thrust_exec_check_disable__
    template <class Derived,
    class KeyInputIt,
    class ValInputIt,
    class KeyOutputIt,
    class ValOutputIt,
    class BinaryPred>
    pair<KeyOutputIt, ValOutputIt> THRUST_HIP_FUNCTION
    reduce_by_key(execution_policy<Derived> &policy,
                  KeyInputIt                 keys_first,
                  KeyInputIt                 keys_last,
                  ValInputIt                 values_first,
                  KeyOutputIt                keys_output,
                  ValOutputIt                values_output,
                  BinaryPred                 binary_pred)
    {
        typedef typename thrust::detail::eval_if<
        thrust::detail::is_output_iterator<ValOutputIt>::value,
        thrust::iterator_value<ValInputIt>,
        thrust::iterator_value<ValOutputIt>
        >::type value_type;

        return hip_rocprim::reduce_by_key(policy,
                                          keys_first,
                                          keys_last,
                                          values_first,
                                          keys_output,
                                          values_output,
                                          binary_pred,
                                          plus<value_type>());
    }

    __thrust_exec_check_disable__
    template <class Derived,
    class KeyInputIt,
    class ValInputIt,
    class KeyOutputIt,
    class ValOutputIt>
    pair<KeyOutputIt, ValOutputIt> __host__ __device__
    reduce_by_key(execution_policy<Derived> &policy,
                  KeyInputIt                 keys_first,
                  KeyInputIt                 keys_last,
                  ValInputIt                 values_first,
                  KeyOutputIt                keys_output,
                  ValOutputIt                values_output)
    {
        typedef typename thrust::iterator_value<KeyInputIt>::type KeyT;
        return hip_rocprim::reduce_by_key(policy,
                                          keys_first,
                                          keys_last,
                                          values_first,
                                          keys_output,
                                          values_output,
                                          equal_to<KeyT>());
    }

} // namespace  hip_rocprim

END_NS_THRUST

//#include <thrust/memory.h>
//#include <thrust/reduce.h>

#endif
