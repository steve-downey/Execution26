// include/beman/execution26/detail/connect_result_t.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_RESULT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_RESULT

#include <beman/execution26/detail/connect.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
/*!
 * \brief Type alias to determine the operation state type returned from `connect()`.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
template <typename Sender, typename Receiver>
using connect_result_t = decltype(::beman::execution26::connect(::std::declval<Sender>(), ::std::declval<Receiver>()));
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
