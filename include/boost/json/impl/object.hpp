//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/json
//

#ifndef BOOST_JSON_IMPL_OBJECT_HPP
#define BOOST_JSON_IMPL_OBJECT_HPP

#include <boost/json/value.hpp>
#include <boost/json/detail/varint.hpp>
#include <boost/core/exchange.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/utility/string_view.hpp>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace boost {
namespace json {

//------------------------------------------------------------------------------

struct object::list_hook
{
    element* prev;
    element* next;
};

//------------------------------------------------------------------------------

struct object::element : list_hook
{
    value v;
    element* local_next;

    BOOST_JSON_DECL
    string_view
    key() const noexcept;

    BOOST_JSON_DECL
    static
    void
    destroy(
        element const* e,
        storage_ptr const& sp) noexcept;

    template<class Arg>
    element(
        Arg&& arg,
        storage_ptr sp)
        : v(std::forward<Arg>(arg),
            std::move(sp))
    {
    }
};

//------------------------------------------------------------------------------

class object::undo_range
{
    object& self_;
    element* head_ = nullptr;
    element* tail_ = nullptr;
    size_type n_ = 0;

public:
    BOOST_JSON_DECL
    explicit
    undo_range(object& self) noexcept;

    BOOST_JSON_DECL
    ~undo_range();

    BOOST_JSON_DECL
    void
    insert(element* e) noexcept;

    BOOST_JSON_DECL
    void
    commit(
        const_iterator pos,
        size_type min_buckets);
};

//------------------------------------------------------------------------------

class object::hasher
{
    BOOST_JSON_DECL
    static
    std::pair<
        std::uint64_t, std::uint64_t>
    init(std::true_type) noexcept;

    BOOST_JSON_DECL
    static
    std::pair<
        std::uint32_t, std::uint32_t>
    init(std::false_type) noexcept;

public:
    BOOST_JSON_DECL
    std::size_t
    operator()(key_type key) const noexcept;
};

//------------------------------------------------------------------------------

class object::key_equal
{
public:
    bool
    operator()(
        string_view lhs,
        string_view rhs) const noexcept
    {
        return lhs == rhs;
    }
};

//------------------------------------------------------------------------------

class object::pointer
{
    reference t_;

public:
    pointer(reference t)
        : t_(t)
    {
    }

    reference*
    operator->() noexcept
    {
        return &t_;
    }
};

//------------------------------------------------------------------------------

class object::const_pointer
{
    const_reference t_;

public:
    const_pointer(
        const_reference t)
        : t_(t)
    {
    }

    const_reference*
    operator->() noexcept
    {
        return &t_;
    }
};

//------------------------------------------------------------------------------

class object::const_iterator
{
    element* e_ = nullptr;

    friend class object;

    const_iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::const_pointer;
    using reference = object::const_reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    const_iterator() = default;
    const_iterator(const_iterator const&) = default;
    const_iterator& operator=(const_iterator const&) = default;

    const_iterator(iterator it) noexcept;
    const_iterator(local_iterator it) noexcept;
    const_iterator(const_local_iterator it) noexcept;

    const_iterator&
    operator++() noexcept
    {
        e_ = e_->next;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_iterator&
    operator--() noexcept
    {
        e_ = e_->prev;
        return *this;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v };
    }

    reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v };
    }

    bool
    operator==(const_iterator other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(const_iterator other) const noexcept
    {
        return e_ != other.e_;
    }
};

//------------------------------------------------------------------------------

class object::iterator
{
    element* e_ = nullptr;

    friend class object;

    iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::pointer;
    using reference = object::reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() = default;
    iterator(iterator const&) = default;
    iterator& operator=(iterator const&) = default;

    iterator(local_iterator it) noexcept;

    iterator&
    operator++() noexcept
    {
        e_ = e_->next;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator&
    operator--() noexcept
    {
        e_ = e_->prev;
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    pointer
    operator->() const noexcept
    {
        return reference{
            e_->key(), e_->v };
    }

    reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v };
    }

    bool
    operator==(const_iterator other) const noexcept
    {
        return const_iterator(*this) == other;
    }

    bool
    operator!=(const_iterator other) const noexcept
    {
        return const_iterator(*this) != other;
    }
};

//------------------------------------------------------------------------------

class object::local_iterator
{
    element* e_ = nullptr;

    friend class object;

    local_iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::pointer;
    using reference = object::reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    local_iterator() = default;
    local_iterator(
        local_iterator const&) = default;
    local_iterator&
    operator=(local_iterator const&) = default;

    local_iterator&
    operator++() noexcept
    {
        e_ = e_->local_next;
        return *this;
    }

    local_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v };
    }

    const_reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v };
    }

    bool
    operator==(const_iterator other) const noexcept
    {
        return const_iterator(*this) == other;
    }

    bool
    operator!=(const_iterator other) const noexcept
    {
        return const_iterator(*this) != other;
    }
};

//------------------------------------------------------------------------------

class object::const_local_iterator
{
    element* e_ = nullptr;

    friend class object;

    const_local_iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::const_pointer;
    using reference = object::const_reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    const_local_iterator() = default;
    const_local_iterator(
        const_local_iterator const&) = default;
    const_local_iterator&
    operator=(
        const_local_iterator const&) = default;

    const_local_iterator(local_iterator it) noexcept;

    const_local_iterator&
    operator++() noexcept
    {
        e_ = e_->local_next;
        return *this;
    }

    const_local_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v };
    }

    const_reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v };
    }

    bool
    operator==(const_iterator other) const noexcept
    {
        return const_iterator(*this) == other;
    }

    bool
    operator!=(const_iterator other) const noexcept
    {
        return const_iterator(*this) != other;
    }
};

//------------------------------------------------------------------------------

inline
object::
const_iterator::
const_iterator(
    iterator it) noexcept
    : e_(it.e_)
{
}

inline
object::
const_iterator::
const_iterator(
    local_iterator it) noexcept
    : e_(it.e_)
{
}

inline
object::
const_iterator::
const_iterator(
    const_local_iterator it) noexcept
    : e_(it.e_)
{
}

inline
object::
iterator::
iterator(
    local_iterator it) noexcept
    : e_(it.e_)
{
}

inline
object::
const_local_iterator::
const_local_iterator(
    local_iterator it) noexcept
    : e_(it.e_)
{
}

//------------------------------------------------------------------------------

class object::node_type
{
    element* e_ = nullptr;
    storage_ptr sp_;

    friend class object;

    BOOST_JSON_DECL
    node_type(
        element* e,
        storage_ptr sp) noexcept;

public:
    using key_type = string_view;
    using mapped_type = json::value;

    node_type() = default;
    node_type(node_type const&) = delete;

    BOOST_JSON_DECL
    ~node_type();

    BOOST_JSON_DECL
    node_type(
        node_type&& other) noexcept;

    BOOST_JSON_DECL
    node_type&
    operator=(node_type&& other) noexcept;

    storage_ptr const&
    get_storage() const noexcept
    {
        return sp_;
    }

    bool
    empty() const noexcept
    {
        return e_ == nullptr;
    }

    explicit
    operator bool() const noexcept
    {
        return ! empty();
    }

    key_type const
    key() const
    {
        return e_->key();
    }

    mapped_type&
    mapped()
    {
        return e_->v;
    }

    mapped_type const&
    mapped() const
    {
        return e_->v;
    }

    void
    swap(node_type& other) noexcept
    {
        std::swap(e_, other.e_);
        std::swap(sp_, other.sp_);
    }
};

inline
void
swap(
    object::node_type& lhs,
    object::node_type& rhs) noexcept
{
    lhs.swap(rhs);
}

//------------------------------------------------------------------------------

struct object::insert_return_type
{
    iterator position;
    node_type node;
    bool inserted;
};

//------------------------------------------------------------------------------

template<class InputIt, class>
object::
object(
    InputIt first,
    InputIt last)
    : object(
        first,
        last,
        0,
        default_storage())
{
}

template<class InputIt, class>
object::
object(
    InputIt first,
    InputIt last,
    size_type bucket_count)
    : object(
        first,
        last,
        bucket_count,
        default_storage())
{
}

template<class InputIt, class>
object::
object(
    InputIt first,
    InputIt last,
    storage_ptr sp)
    : object(
        first,
        last,
        0,
        sp)
{
}

template<class InputIt, class>
object::
object(
    InputIt first,
    InputIt last,
    size_type bucket_count,
    storage_ptr sp)
    : sp_(std::move(sp))
{
    insert_range(end(),
        first, last, bucket_count);
}

//------------------------------------------------------------------------------

template<class P, class>
auto
object::
insert(P&& p)->
    std::pair<iterator, bool>
{
    return insert(
        end(), std::forward<P>(p));
}

template<class P, class>
auto
object::
insert(
    const_iterator pos, P&& p) ->
        std::pair<iterator, bool>
{
    value_type v(std::forward<P>(p));
    return emplace(pos, v.first,
        std::move(v.second));
}

template<class InputIt, class>
void
object::
insert(InputIt first, InputIt last)
{
    insert_range(end(), first, last, 0);
}

template<class InputIt, class>
void
object::
insert(
    const_iterator pos,
    InputIt first,
    InputIt last)
{
    insert_range(pos, first, last, 0);
}

template<class M>
auto
object::
insert_or_assign(
    key_type key, M&& obj) ->
        std::pair<iterator, bool>
{
    return insert_or_assign(end(), key,
        std::forward<M>(obj));
}

template<class M>
auto
object::
insert_or_assign(
    const_iterator pos,
    key_type key,
    M&& obj) ->
        std::pair<iterator, bool>
{
    auto const result =
        find_impl(key);
    if(result.first)
    {
        result.first->v = std::forward<M>(obj);
        return { iterator(result.first), false };
    }
    auto const e = allocate(
        key, std::forward<M>(obj));
    insert(pos, result.second, e);
    return { iterator(e), true };
}

template<class Arg>
auto
object::
emplace(
    key_type key,
    Arg&& arg) ->
        std::pair<iterator, bool>
{
    return emplace(
        end(),
        key,
        std::forward<Arg>(arg));
}

template<class Arg>
auto
object::
emplace(
    const_iterator pos,
    key_type key,
    Arg&& arg) ->
        std::pair<iterator, bool>
{
    auto const result =
        find_impl(key);
    if(result.first)
        return { iterator(
            result.first), false };
    auto const e = allocate(
        key, std::forward<Arg>(arg));
    insert(pos, result.second, e);
    return { iterator(e), true };
}

inline
auto
object::
hash_function() const noexcept ->
    hasher
{
    return hasher{};
}

inline
auto
object::
key_eq() const noexcept ->
    key_equal
{
    return key_equal{};
}

//------------------------------------------------------------------------------

// type-erased constructor to
// reduce template instantiations.
struct object::construct_base
{
    virtual
    ~construct_base() = default;

    virtual
    void
    operator()(void* p) const = 0;
};

template<class Arg>
auto
object::
allocate(key_type key, Arg&& arg) ->
    element*
{
    struct place : construct_base
    {
        Arg&& arg;
        storage_ptr const& sp;
        
        place(
            Arg&& arg_,
            storage_ptr const& sp_) noexcept
            : arg(std::forward<Arg>(arg_))
            , sp(sp_)
        {
        }

        void
        operator()(void* p) const override
        {
            ::new(p) element(
                std::forward<Arg>(arg), sp);
        }
    };

    return allocate_impl(key, place(
        std::forward<Arg>(arg), sp_));
}

template<class InputIt>
void
object::
insert_range(
    const_iterator pos,
    InputIt first,
    InputIt last,
    size_type bucket_count)
{
    undo_range u(*this);
    while(first != last)
        u.insert(allocate(*first++));
    u.commit(pos, bucket_count);
}

} // json
} // boost

#endif