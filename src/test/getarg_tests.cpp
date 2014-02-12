#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-Flap");
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", false));
    BOOST_CHECK(GetBoolArg("-Flap", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-Flapo"));
    BOOST_CHECK(!GetBoolArg("-Flapo", false));
    BOOST_CHECK(GetBoolArg("-Flapo", true));

    ResetArgs("-Flap=0");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", false));
    BOOST_CHECK(!GetBoolArg("-Flap", true));

    ResetArgs("-Flap=1");
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", false));
    BOOST_CHECK(GetBoolArg("-Flap", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noFlap");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", false));
    BOOST_CHECK(!GetBoolArg("-Flap", true));

    ResetArgs("-noFlap=1");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", false));
    BOOST_CHECK(!GetBoolArg("-Flap", true));

    ResetArgs("-Flap -noFlap");  // -Flap should win
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", false));
    BOOST_CHECK(GetBoolArg("-Flap", true));

    ResetArgs("-Flap=1 -noFlap=1");  // -Flap should win
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", false));
    BOOST_CHECK(GetBoolArg("-Flap", true));

    ResetArgs("-Flap=0 -noFlap=0");  // -Flap should win
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", false));
    BOOST_CHECK(!GetBoolArg("-Flap", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--Flap=1");
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", false));
    BOOST_CHECK(GetBoolArg("-Flap", true));

    ResetArgs("--noFlap=1");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", false));
    BOOST_CHECK(!GetBoolArg("-Flap", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Flap", "eleven"), "eleven");

    ResetArgs("-Flap -bar");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Flap", "eleven"), "");

    ResetArgs("-Flap=");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Flap", "eleven"), "");

    ResetArgs("-Flap=11");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-Flap", "eleven"), "11");

    ResetArgs("-Flap=eleven");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-Flap", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Flap", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-Flap", 0), 0);

    ResetArgs("-Flap -bar");
    BOOST_CHECK_EQUAL(GetArg("-Flap", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-Flap=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-Flap", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-Flap=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-Flap", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--Flap");
    BOOST_CHECK_EQUAL(GetBoolArg("-Flap"), true);

    ResetArgs("--Flap=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-Flap", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noFlap");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", true));
    BOOST_CHECK(!GetBoolArg("-Flap", false));

    ResetArgs("-noFlap=1");
    BOOST_CHECK(!GetBoolArg("-Flap"));
    BOOST_CHECK(!GetBoolArg("-Flap", true));
    BOOST_CHECK(!GetBoolArg("-Flap", false));

    ResetArgs("-noFlap=0");
    BOOST_CHECK(GetBoolArg("-Flap"));
    BOOST_CHECK(GetBoolArg("-Flap", true));
    BOOST_CHECK(GetBoolArg("-Flap", false));

    ResetArgs("-Flap --noFlap");
    BOOST_CHECK(GetBoolArg("-Flap"));

    ResetArgs("-noFlap -Flap"); // Flap always wins:
    BOOST_CHECK(GetBoolArg("-Flap"));
}

BOOST_AUTO_TEST_SUITE_END()
