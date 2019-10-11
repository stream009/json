//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

// Test that header file is self-contained.
#include <boost/json/array.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>

#include "test_storage.hpp"

namespace boost {
namespace json {

class array_test : public beast::unit_test::suite
{
public:
    using init_list = std::initializer_list<value>;

    void
    check(array const& a)
    {
        BEAST_EXPECT(a.size() == 3);
        BEAST_EXPECT(a[0].is_number());
        BEAST_EXPECT(a[1].is_bool());
        BEAST_EXPECT(a[2].is_string());
    }

    void
    check(
        array const& a,
        storage_ptr const& sp)
    {
        check(a);
        check_storage(a, sp);
    }

    void
    testCtors()
    {
        // ~array()
        {
            // implied
        }

        // array()
        {
            scoped_fail_storage fs;
            array a;
            BEAST_EXPECT(a.empty());
            BEAST_EXPECT(a.size() == 0);
        }

        // array(storage_ptr)
        {
            scoped_fail_storage fs;
            array a(default_storage());
            check_storage(a, default_storage());
        }

        // array(size_type, value, storage)
        {
            // default storage
            {
                array a(3, true);
                BEAST_EXPECT(a.size() == 3);
                for(auto const& v : a)
                    BEAST_EXPECT(v.is_bool());
                check_storage(a, default_storage());
            }

            // construct with zero `true` values
            {
                array(0, true);
            }

            // construct with three `true` values
            fail_loop([](storage_ptr const& sp)
            {
                array a(3, true, sp);
                BEAST_EXPECT(a.size() == 3);
                check_storage(a, sp);
            });
        }

        // array(size_type, storage)
        {
            // default storage
            fail_loop([]
            {
                array a(3);
                BEAST_EXPECT(a.size() == 3);
                for(auto const& v : a)
                    BEAST_EXPECT(v.is_null());
                check_storage(a, default_storage());
            });

            fail_loop([](storage_ptr const& sp)
            {
                array a(3, sp);
                BEAST_EXPECT(a.size() == 3);
                check_storage(a, sp);
            });
        }

        // array(InputIt, InputIt, storage)
        {
            // default storage
            {
                init_list init{ 1, true, "hello" };
                array a(init.begin(), init.end());
                check(a);
                check_storage(a, default_storage());
            }

            // forward iterator
            fail_loop([this](storage_ptr const& sp)
            {
                init_list init{ 1, true, "hello" };
                array a(init.begin(), init.end(), sp);
                check(a);
                check_storage(a, sp);
            });

            // input iterator
            fail_loop([this](storage_ptr const& sp)
            {
                init_list init{ 1, true, "hello" };
                array a(
                    make_input_iterator(init.begin()),
                    make_input_iterator(init.end()), sp);
                check(a);
                check_storage(a, sp);
            });
        }

        // array(array const&)
        {
            {
                array a1;
                array a2(a1);
            }

            {
                array a1;
                array a2({ 1, true, "hello" });
                a2 = a1;
            }

            {
                init_list init{ 1, true, "hello" };
                array a1(init.begin(), init.end());
                array a2(a1);
                check(a2);
                check_storage(a2, default_storage());
            }
        }

        // array(array const&, storage)
        fail_loop([](storage_ptr const& sp)
        {
            init_list init{ 1, true, "hello" };
            array a1(init.begin(), init.end());
            array a2(a1, sp);
            BEAST_EXPECT(a2.size() == 3);
            check_storage(a2, sp);
        });

        // array(pilfered<array>)
        {
            init_list init{ 1, true, "hello" };
            array a1(init.begin(), init.end());
            array a2(pilfer(a1));
            BEAST_EXPECT(a1.empty());
            BEAST_EXPECT(! a1.get_storage());
            check(a2);
            check_storage(a2, default_storage());
        }

        // array(array&&)
        {
            init_list init{ 1, true, "hello" };
            array a1(init.begin(), init.end());
            array a2 = std::move(a1);
            BEAST_EXPECT(a1.empty());
            check(a2);
            check_storage(a2, default_storage());
        }

        // array(array&&, storage)
        {
            {
                init_list init{ 1, true, "hello" };
                array a1(init.begin(), init.end());
                array a2(
                    std::move(a1), default_storage());
                BEAST_EXPECT(a1.empty());
                check(a2);
                check_storage(a1, default_storage());
                check_storage(a2, default_storage());
            }

            fail_loop([this](storage_ptr const& sp)
            {
                init_list init{ 1, true, "hello" };
                array a1(init.begin(), init.end());
                array a2(std::move(a1), sp);
                BEAST_EXPECT(! a1.empty());
                check(a2);
                check_storage(a1, default_storage());
                check_storage(a2, sp);
            });
        }

        // array(init_list, storage)
        {
            // default storage
            {
                array a({1, true, "hello"});
                check(a);
                check_storage(a, default_storage());
            }

            fail_loop([this](storage_ptr const& sp)
            {
                array a({1, true, "hello"}, sp);
                check(a, sp);
                check_storage(a, sp);
            });
        }
    }

    void
    testAssignment()
    {
        // operator=(array const&)
        {
            {
                array a1({1, true, "hello"});
                array a2({nullptr, value(kind::object), 1.f});
                a2 = a1;
                check(a1);
                check(a2);
                check_storage(a1, default_storage());
                check_storage(a2, default_storage());
            }

            fail_loop([this](storage_ptr const& sp)
            {
                array a1({1, true, "hello"});
                array a2({nullptr, value(kind::object), 1.f}, sp);
                a2 = a1;
                check(a1);
                check(a2);
                check_storage(a1, default_storage());
                check_storage(a2, sp);
            });
        }

        // operator=(array&&)
        {
            {
                array a1({1, true, "hello"});
                array a2({nullptr, object{}, 1.f});
                a2 = std::move(a1);
                BEAST_EXPECT(a1.empty());
                check(a2);
            }

            fail_loop([this](storage_ptr const& sp)
            {
                array a1({1, true, "hello"});
                array a2({nullptr, value(kind::object), 1.f}, sp);
                a2 = std::move(a1);
                check(a1);
                check(a2);
                check_storage(a1, default_storage());
                check_storage(a2, sp);
            });
        }

        // operator=(init_list)
        {
            {
                array a;
                a = {};
            }

            {
                array a({ 1, true, "hello" });
                a = {};
            }

            {
                init_list init{ 1, true, "hello" };
                array a({nullptr, value(kind::object), 1.f});
                a = init;
                check(a);
                check_storage(a, default_storage());
            }

            fail_loop([this](storage_ptr const& sp)
            {
                init_list init{ 1, true, "hello" };
                array a({nullptr, value(kind::object), 1.f}, sp);
                a = init;
                check(a);
                check_storage(a, sp);
            });
        }
    }

    void
    testGetStorage()
    {
        // get_storage()
        {
            // implied
        }
    }

    void
    testAccess()
    {
        // at(pos)
        {
            array a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.at(0).is_number());
            BEAST_EXPECT(a.at(1).is_bool());
            BEAST_EXPECT(a.at(2).is_string());
            try
            {
                a.at(3);
                BEAST_FAIL();
            }
            catch(std::out_of_range const&)
            {
                BEAST_PASS();
            }
        }

        // at(pos) const
        {
            array const a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.at(0).is_number());
            BEAST_EXPECT(a.at(1).is_bool());
            BEAST_EXPECT(a.at(2).is_string());
            try
            {
                a.at(3);
                BEAST_FAIL();
            }
            catch(std::out_of_range const&)
            {
                BEAST_PASS();
            }
        }

        // operator[](size_type)
        {
            array a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a[0].is_number());
            BEAST_EXPECT(a[1].is_bool());
            BEAST_EXPECT(a[2].is_string());
        }

        // operator[](size_type) const
        {
            array const a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a[0].is_number());
            BEAST_EXPECT(a[1].is_bool());
            BEAST_EXPECT(a[2].is_string());
        }

        // front()
        {
            array a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.front().is_number());
        }

        // front() const
        {
            array const a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.front().is_number());
        }

        // back()
        {
            array a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.back().is_string());
        }

        // back() const
        {
            array const a({1, true, "hello"});
            scoped_fail_storage fs;
            BEAST_EXPECT(a.back().is_string());
        }

        // data()
        {
            {
                array a({1, true, "hello"});
                scoped_fail_storage fs;
                BEAST_EXPECT(a.data() == &a[0]);
            }
            {
                BEAST_EXPECT(array{}.data() == nullptr);
            }
        }

        // data() const
        {
            {
                array const a({1, true, "hello"});
                scoped_fail_storage fs;
                BEAST_EXPECT(a.data() == &a[0]);
            }
            {
                array const a;
                BEAST_EXPECT(a.data() == nullptr);
            }
        }
    }

    void
    testIterators()
    {
        array a({1, true, "hello"});
        auto const& ac(a);

        {
            auto it = a.begin();
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it == a.end());
        }
        {
            auto it = a.cbegin();
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it == a.cend());
        }
        {
            auto it = ac.begin();
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it == ac.end());
        }
        {
            auto it = a.end();
            --it; BEAST_EXPECT(it->is_string());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_number());
            BEAST_EXPECT(it == a.begin());
        }
        {
            auto it = a.cend();
            --it; BEAST_EXPECT(it->is_string());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_number());
            BEAST_EXPECT(it == a.cbegin());
        }
        {
            auto it = ac.end();
            --it; BEAST_EXPECT(it->is_string());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_number());
            BEAST_EXPECT(it == ac.begin());
        }

        {
            auto it = a.rbegin();
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it == a.rend());
        }
        {
            auto it = a.crbegin();
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it == a.crend());
        }
        {
            auto it = ac.rbegin();
            BEAST_EXPECT(it->is_string()); ++it;
            BEAST_EXPECT(it->is_bool());   it++;
            BEAST_EXPECT(it->is_number()); ++it;
            BEAST_EXPECT(it == ac.rend());
        }
        {
            auto it = a.rend();
            --it; BEAST_EXPECT(it->is_number());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_string());
            BEAST_EXPECT(it == a.rbegin());
        }
        {
            auto it = a.crend();
            --it; BEAST_EXPECT(it->is_number());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_string());
            BEAST_EXPECT(it == a.crbegin());
        }
        {
            auto it = ac.rend();
            --it; BEAST_EXPECT(it->is_number());
            it--; BEAST_EXPECT(it->is_bool());
            --it; BEAST_EXPECT(it->is_string());
            BEAST_EXPECT(it == ac.rbegin());
        }

        {
            array a2;
            array const& ca2(a2);
            BEAST_EXPECT(std::distance(
                a2.begin(), a2.end()) == 0);
            BEAST_EXPECT(std::distance(
                ca2.begin(), ca2.end()) == 0);
            BEAST_EXPECT(std::distance(
                a2.rbegin(), a2.rend()) == 0);
            BEAST_EXPECT(std::distance(
                ca2.rbegin(), ca2.rend()) == 0);
        }
    }

    void
    testCapacity()
    {
        // empty()
        {
            array a;
            BEAST_EXPECT(a.empty());
            a.emplace_back(1);
            BEAST_EXPECT(! a.empty());
        }

        // size()
        {
            array a;
            BEAST_EXPECT(a.size() == 0);
            a.emplace_back(1);
            BEAST_EXPECT(a.size() == 1);
        }

        // max_size()
        {
            array a;
            BEAST_EXPECT(a.max_size() > 0);
        }

        // reserve()
        {
            {
                array a;
                a.reserve(0);
            }

            {
                array a(3);
                a.reserve(1);
            }

            {
                array a(3);
                a.reserve(0);
            }

            {
                array a;
                a.reserve(50);
                BEAST_EXPECT(a.capacity() >= 50);
            }
        }

        // capacity()
        {
            array a;
            BEAST_EXPECT(a.capacity() == 0);
        }

        // shrink_to_fit()
        {
            {
                array a(1);
                a.shrink_to_fit();
                BEAST_EXPECT(a.size() == 1);
                BEAST_EXPECT(a.capacity() >= 1);
            }

            fail_loop([](storage_ptr const& sp)
            {
                array a(1, sp);
                a.resize(a.capacity());
                a.shrink_to_fit();
                BEAST_EXPECT(a.size() == a.capacity());
            });

            fail_loop([](storage_ptr const& sp)
            {
                array a(sp);
                a.reserve(10);
                BEAST_EXPECT(a.capacity() >= 10);
                a.shrink_to_fit();
                BEAST_EXPECT(a.capacity() == 0);
            });

            fail_loop([](storage_ptr const& sp)
            {
                array a(3, sp);
                a.reserve(10);
                BEAST_EXPECT(a.capacity() >= 10);
                a.shrink_to_fit();
                if(a.capacity() > 3)
                    throw test_failure{};
            });
        }
    }

    void
    testModifiers()
    {
        // clear
        {
            {
                array a;
                BEAST_EXPECT(a.size() == 0);
                BEAST_EXPECT(a.capacity() == 0);
                a.clear();
                BEAST_EXPECT(a.size() == 0);
                BEAST_EXPECT(a.capacity() == 0);
            }
            {
                array a({1, true, "hello"});
                a.clear();
                BEAST_EXPECT(a.size() == 0);
                BEAST_EXPECT(a.capacity() > 0);
            }
        }

        // insert(const_iterator, value_type const&)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, "hello"}, sp);
            value v(true);
            a.insert(a.begin() + 1, v);
            check(a);
            check_storage(a, sp);
        });

        // insert(const_iterator, value_type&&)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, "hello"}, sp);
            value v(true);
            a.insert(
                a.begin() + 1, std::move(v));
            check(a);
            check_storage(a, sp);
        });

        // insert(const_iterator, size_type, value_type const&)
        fail_loop([](storage_ptr const& sp)
        {
            value v({1,2,3});
            array a({1, "hello"}, sp);
            a.insert(a.begin() + 1, 3, v);
            BEAST_EXPECT(a[0].is_number());
            BEAST_EXPECT(a[1].size() == 3);
            BEAST_EXPECT(a[2].size() == 3);
            BEAST_EXPECT(a[3].size() == 3);
            BEAST_EXPECT(a[4].is_string());
        });

        // insert(const_iterator, InputIt, InputIt)
        {
            // forward iterator
            fail_loop([this](storage_ptr const& sp)
            {
                std::initializer_list<
                    value> init = {1, true};
                array a({"hello"}, sp);
                a.insert(a.begin(),
                    init.begin(), init.end());
                check(a);
            });

            // forward iterator (multiple growth)
            fail_loop([this](storage_ptr const& sp)
            {
                std::initializer_list<
                    value> init = {1, "hello", true, 1, 2, 3, 4, 5, 6, 7};
                array a(sp);
                a.insert(a.begin(),
                    init.begin(), init.end());
            });

            // input iterator (empty range)
            {
                scoped_fail_storage fs;
                std::initializer_list<value> init;
                array a;
                a.insert(a.begin(),
                    make_input_iterator(init.begin()),
                    make_input_iterator(init.end()));
                BEAST_EXPECT(a.empty());
            }

            // input iterator
            fail_loop([this](storage_ptr const& sp)
            {
                std::initializer_list<
                    value> init = {1, true};
                array a({"hello"}, sp);
                a.insert(a.begin(),
                    make_input_iterator(init.begin()),
                    make_input_iterator(init.end()));
                check(a);
            });

            // input iterator (multiple growth)
            fail_loop([this](storage_ptr const& sp)
            {
                std::initializer_list<
                    value> init = {1, true, 1, 2, 3, 4, 5, 6, 7};
                array a({"hello"}, sp);
                a.insert(a.begin(),
                    make_input_iterator(init.begin()),
                    make_input_iterator(init.end()));
                BEAST_EXPECT(a.size() == init.size() + 1);
            });

            // backward relocate
            fail_loop([this](storage_ptr const& sp)
            {
                std::initializer_list<
                    value> init = {1, 2};
                array a({"a", "b", "c", "d", "e"}, sp);
                a.insert(
                    a.begin() + 1,
                    init.begin(), init.end());
            });
        }

        // insert(const_iterator, init_list)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({"hello"}, sp);
            a.insert(a.begin(), {1, true});
            check(a);
        });

        // emplace(const_iterator, arg)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, "hello"}, sp);
            auto it = a.emplace(
                a.begin() + 1, true);
            BEAST_EXPECT(it == a.begin() + 1);
            check(a);
        });

        // erase(pos)
        {
            array a({1, true, nullptr, "hello"});
            a.erase(a.begin() + 2);
            check(a);
        }

        // erase(first, last)
        {
            array a({1, true, nullptr, 1.f, "hello"});
            a.erase(
                a.begin() + 2,
                a.begin() + 4);
            check(a);
        }

        // push_back(value const&)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, true}, sp);
            value v("hello");
            a.push_back(v);
            BEAST_EXPECT(
                v.as_string() == "hello");
            check(a);
            check_storage(a, sp);
        });

        // push_back(value&&)
        {
            fail_loop([this](storage_ptr const& sp)
            {
                array a({1, true}, sp);
                value v("hello");
                a.push_back(std::move(v));
                check(a);
                check_storage(a, sp);
            });
        }

        // emplace_back(arg)
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, true}, sp);
            a.emplace_back("hello");
            check(a);
            check_storage(a, sp);
        });

        // pop_back()
        fail_loop([this](storage_ptr const& sp)
        {
            array a({1, true, "hello", nullptr}, sp);
            a.pop_back();
            check(a);
            check_storage(a, sp);
        });

        // resize(size_type)
        {
            value v(array{});
            v.emplace_back(1);
            v.emplace_back(true);
            v.emplace_back("hello");

            fail_loop([&](storage_ptr const& sp)
            {
                array a(5, sp);
                a.resize(3);
                BEAST_EXPECT(a.size() == 3);
                check_storage(a, sp);
            });

            fail_loop([&](storage_ptr const& sp)
            {
                array a(sp);
                a.resize(3);
                BEAST_EXPECT(a.size() == 3);
                check_storage(a, sp);
            });
        }

        // resize(size_type, value_type const&)
        {
            value v(array{});
            v.emplace_back(1);
            v.emplace_back(true);
            v.emplace_back("hello");

            fail_loop([&](storage_ptr const& sp)
            {
                array a(5, v, sp);
                a.resize(3, v);
                BEAST_EXPECT(a.size() == 3);
                check_storage(a, sp);
            });

            fail_loop([&](storage_ptr const& sp)
            {
                array a(3, v, sp);
                a.resize(5, v);
                BEAST_EXPECT(a.size() == 5);
                check_storage(a, sp);
            });
        }

        // swap
        {
            // same storage
            {
                array a1({1, true, "hello"});
                array a2 = {1.};
                scoped_fail_storage fs;
                a1.swap(a2);
                check(a2);
                BEAST_EXPECT(a1.size() == 1);
                BEAST_EXPECT(a1.front().is_number());
                BEAST_EXPECT(a1.front().as_number().get_double() == 1.);
            }

            // different storage
            fail_loop([&](storage_ptr const& sp)
            {
                array a1({1, true, "hello"}, sp);
                array a2 = {1.};
                a1.swap(a2);
                check(a2);
                BEAST_EXPECT(a1.size() == 1);
            });
        }
    }

    void
    testExceptions()
    {
        // operator=(array const&)
        fail_loop([this](storage_ptr const& sp)
        {
            array a0({1, true, "hello"});
            array a1;
            array a(sp);
            a.emplace_back(nullptr);
            a = a0;
            a1 = a;
            check(a1);
        });

        // operator=(init_list)
        fail_loop([this](storage_ptr const& sp)
        {
            init_list init{ 1, true, "hello" };
            array a1;
            array a(sp);
            a.emplace_back(nullptr);
            a = init;
            a1 = a;
            check(a1);
        });

        // insert(const_iterator, count, value_type const&)
        fail_loop([](storage_ptr const& sp)
        {
            array a1;
            array a({1, true}, sp);
            a.insert(a.begin() + 1,
                3, value(kind::null));
            a1 = a;
            BEAST_EXPECT(a1.size() == 5);
            BEAST_EXPECT(a1[0].is_number());
            BEAST_EXPECT(a1[1].is_null());
            BEAST_EXPECT(a1[2].is_null());
            BEAST_EXPECT(a1[3].is_null());
            BEAST_EXPECT(a1[4].is_bool());
        });

        // insert(const_iterator, InputIt, InputIt)
        fail_loop([this](storage_ptr const& sp)
        {
            init_list init{ 1, true, "hello" };
            array a1;
            array a(sp);
            a.insert(a.end(),
                init.begin(), init.end());
            a1 = a;
            check(a1);
        });

        // emplace(const_iterator, arg)
        fail_loop([](storage_ptr const& sp)
        {
            array a1;
            array a({1, nullptr}, sp);
            a.emplace(a.begin() + 1, true);
            a1 = a;
            BEAST_EXPECT(a1.size() == 3);
            BEAST_EXPECT(a1[0].is_number());
            BEAST_EXPECT(a1[1].is_bool());
            BEAST_EXPECT(a1[2].is_null());
        });

        // emplace(const_iterator, arg)
        fail_loop([this](storage_ptr const& sp)
        {
            array a1;
            array a({1, "hello"}, sp);
            a.emplace(a.begin() + 1, true);
            a1 = a;
            check(a1);
            BEAST_EXPECT(a1.size() == 3);
            BEAST_EXPECT(a1[0].is_number());
            BEAST_EXPECT(a1[1].is_bool());
            BEAST_EXPECT(a1[2].is_string());
        });
    }

    void
    run() override
    {
        testCtors();
        testAssignment();
        testGetStorage();
        testAccess();
        testIterators();
        testCapacity();
        testModifiers();
        testExceptions();
    }
};

BEAST_DEFINE_TESTSUITE(boost,json,array);

} // json
} // boost