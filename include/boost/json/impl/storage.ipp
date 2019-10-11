//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/json
//

#ifndef BOOST_JSON_IMPL_STORAGE_IPP
#define BOOST_JSON_IMPL_STORAGE_IPP

#include <boost/json/storage.hpp>
#include <boost/json/detail/storage_adaptor.hpp>
#include <memory>

namespace boost {
namespace json {

namespace detail {

inline
storage_ptr&
raw_default_storage() noexcept
{
    static storage_ptr sp =
        make_storage_adaptor(
            std::allocator<void>());
    return sp;
}

} // detail

storage_ptr
default_storage() noexcept
{
    return detail::raw_default_storage();
}

void
default_storage(storage_ptr sp) noexcept
{
    detail::raw_default_storage() = std::move(sp);
}

} // json
} // boost

#endif