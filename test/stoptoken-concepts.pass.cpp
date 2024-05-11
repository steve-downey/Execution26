// Copyright © 2024 Beman Project
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/stop_token.hpp>
#include <type_traits>
#include "test/execution.hpp"

namespace test
{
    struct NonToken
    {
    };
    struct TokenNonCtorCallback
    {
        template <typename>
        struct callback_type
        {
        };
    };
    struct TokenWithOddCallback
    {
        template <typename>
        struct callback_type
        {
            template <typename Initializer>
            callback_type(TokenWithOddCallback const&, Initializer) {}
            template <typename Initializer>
            callback_type(TokenWithOddCallback&, Initializer) = delete;
        };
    };
    struct TokenWithValueCallback
    {
        template <typename>
        struct callback_type
        {
            template <typename Initializer>
            callback_type(TokenWithValueCallback const&, Initializer) {}
            template <typename Initializer>
            callback_type(TokenWithValueCallback&&, Initializer) = delete;
        };
    };
    struct Token
    {
        template <typename Fun>
        struct callback_type
        {
            template <typename Initializer>
            callback_type(Token const&, Initializer) {}
        };
    };
}

auto test_detail_stopppable_callback_for()
{
    struct Callback
    {
        struct Tag {};
        Callback() = default;
        explicit Callback(Tag) {}
        auto operator()() -> void {}
    };

    static_assert(not test_detail::stoppable_callback_for<int, test::Token>);
    static_assert(test_detail::stoppable_callback_for<Callback, test::Token>);

    static_assert(not test_detail::stoppable_callback_for<Callback, test::Token, int>);
    static_assert(test_detail::stoppable_callback_for<Callback, test::Token, Callback::Tag>);

    static_assert(not test_detail::stoppable_callback_for<Callback, test::NonToken>);
    static_assert(not test_detail::stoppable_callback_for<Callback, test::TokenNonCtorCallback>);
    static_assert(not test_detail::stoppable_callback_for<Callback, test::TokenWithValueCallback>);
    static_assert(not test_detail::stoppable_callback_for<Callback, test::TokenWithOddCallback>);
    static_assert(test_detail::stoppable_callback_for<Callback, test::Token>);
    static_assert(test_detail::stoppable_callback_for<Callback, test::Token, Callback::Tag>);
}

namespace stoppable_token
{
    struct no_callback_type
    {
        auto stop_requested() const noexcept -> bool;
        auto stop_possible() const noexcept -> bool;
        auto operator== (no_callback_type const&) const -> bool = default;
    };

    struct non_template_callback_type
    {
        struct callback_type {}; //-dk:TODO
        auto stop_requested() const noexcept -> bool;
        auto stop_possible() const noexcept -> bool;
        auto operator== (non_template_callback_type const&) const -> bool = default;
    };

    template <bool CopyNoexcept,
              typename Requested, bool RequestedNoexcept,
              typename Possible, bool PossibleNoexcept>
    struct token
    {
        template <typename> struct callback_type {};

        token(token const&) noexcept(CopyNoexcept);
        auto stop_requested() const noexcept(RequestedNoexcept) -> Requested;
        auto stop_possible() const noexcept(PossibleNoexcept) -> Possible;
        auto operator== (token const&) const -> bool = default;
    };

    struct non_assignable
    {
        template <typename> struct callback_type {};

        non_assignable(non_assignable const&) noexcept;
        auto operator=(non_assignable const&) -> non_assignable& = delete;
        auto stop_requested() const noexcept -> bool;
        auto stop_possible() const noexcept -> bool;
        auto operator== (non_assignable const&) const -> bool = default;
    };

    struct non_comparable
    {
        template <typename> struct callback_type {};

        auto stop_requested() const noexcept -> bool;
        auto stop_possible() const noexcept -> bool;
    };

    struct non_swappable
    {
        template <typename> struct callback_type {};

        non_swappable(non_swappable const&) noexcept = default;
        auto operator= (non_swappable&&) noexcept -> non_swappable& = delete;
        auto stop_requested() const noexcept -> bool;
        auto stop_possible() const noexcept -> bool;
        auto operator== (non_swappable const&) const -> bool = default;
    };

}

auto test_stoppable_token()
{
    static_assert(not test_std::stoppable_token<stoppable_token::no_callback_type>);
    static_assert(not test_std::stoppable_token<stoppable_token::non_template_callback_type>);

    static_assert(test_std::stoppable_token<stoppable_token::token<true, bool, true, bool, true>>);
    static_assert(not test_std::stoppable_token<stoppable_token::token<false, bool, true, bool, true>>);
    static_assert(not test_std::stoppable_token<stoppable_token::token<true, int, true, bool, true>>);
    static_assert(not test_std::stoppable_token<stoppable_token::token<true, bool, false, bool, true>>);
    static_assert(not test_std::stoppable_token<stoppable_token::token<true, bool, true, int, true>>);
    static_assert(not test_std::stoppable_token<stoppable_token::token<true, bool, true, bool, false>>);

    static_assert(not test_std::stoppable_token<stoppable_token::non_assignable>);
    static_assert(not test_std::stoppable_token<stoppable_token::non_comparable>);
    static_assert(not test_std::stoppable_token<stoppable_token::non_swappable>);
}

namespace unstoppable_token
{
    template <bool CopyNoexcept, bool Possible>
    struct token
    {
        template <typename> struct callback_type {};

        constexpr token() {}
        constexpr token(token const&) noexcept(CopyNoexcept);
        auto stop_requested() const noexcept -> bool;
        static constexpr auto stop_possible() noexcept -> bool { return Possible; }
        auto operator== (token const&) const -> bool = default;
    };
}

auto test_unstoppable_token() -> void
{
    static_assert(::test_std::unstoppable_token<::unstoppable_token::token<true, false>>);
    static_assert(not ::test_std::unstoppable_token<::unstoppable_token::token<false, false>>);
    static_assert(not ::test_std::unstoppable_token<::unstoppable_token::token<true, true>>);
}

auto main() -> int
{
    test_detail_stopppable_callback_for();
    test_stoppable_token();
    test_unstoppable_token();
}