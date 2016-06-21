#pragma once

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

/// assertion in tomo library
#define TOMO_ASSERT(x) BOOST_ASSERT(x)
/// static assertion in tomo library
#define TOMO_STATIC_ASSERT(x) BOOST_STATIC_ASSERT(x)
/// assertion specific to mark unimplemented code sections
#define TOMO_NOT_IMPLEMENTED() TOMO_ASSERT(BOOST_CURRENT_FUNCTION == "method not implemented yet")
/// assertion specfitc to mark untested code sections
#define TOMO_NOT_TESTED() TOMO_ASSERT(BOOST_CURRENT_FUNCTION == "method never called before")

