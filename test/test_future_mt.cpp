//  (C) Copyright 2008-10 Anthony Williams 
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/fiber/all.hpp>
#include <boost/test/unit_test.hpp>

int fn( int i) {
    return i;
}

void test_async() {
    for ( int i = 0; i < 100; ++i) {
        int n = 3;
        boost::fibers::packaged_task< int() > pt( std::bind( fn, n) );
        boost::fibers::future< int > f( pt.get_future() );
        std::thread t([pt_=std::move( pt)] () mutable -> void {
                boost::fibers::fiber f( std::move( pt_) );
                boost::this_fiber::yield();
                f.join();
                });
        int result = f.get();
        BOOST_CHECK_EQUAL( n, result);
        t.join();
    }
}

void test_dummy() {}

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[]) {
    boost::unit_test_framework::test_suite* test =
        BOOST_TEST_SUITE("Boost.Fiber: futures-mt test suite");

#if ! defined(BOOST_FIBERS_NO_ATOMICS)
    test->add(BOOST_TEST_CASE(test_async));
#else
    test->add(BOOST_TEST_CASE(test_dummy));
#endif

    return test;
}
