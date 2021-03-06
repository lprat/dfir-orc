//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// Copyright © 2020 ANSSI. All Rights Reserved.
//
// Author(s): fabienfl (ANSSI)
//

#pragma once

#include <string_view>

#include <fmt/format.h>

namespace Orc {
namespace Traits {

template <typename T>
struct TimeUtc;

}  // namespace Traits
}  // namespace Orc

template <typename T>
struct fmt::formatter<Orc::Traits::TimeUtc<T>> : public fmt::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(const Orc::Traits::TimeUtc<T>& time, FormatContext& ctx) -> decltype(ctx.out());
};

template <typename T>
struct fmt::formatter<Orc::Traits::TimeUtc<T>, wchar_t> : public fmt::formatter<std::wstring_view, wchar_t>
{
    template <typename FormatContext>
    auto format(const Orc::Traits::TimeUtc<T>& time, FormatContext& ctx) -> decltype(ctx.out());
};
