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
    ResetArgs("-CHR");
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", false));
    BOOST_CHECK(GetBoolArg("-CHR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CHRo"));
    BOOST_CHECK(!GetBoolArg("-CHRo", false));
    BOOST_CHECK(GetBoolArg("-CHRo", true));

    ResetArgs("-CHR=0");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", false));
    BOOST_CHECK(!GetBoolArg("-CHR", true));

    ResetArgs("-CHR=1");
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", false));
    BOOST_CHECK(GetBoolArg("-CHR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCHR");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", false));
    BOOST_CHECK(!GetBoolArg("-CHR", true));

    ResetArgs("-noCHR=1");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", false));
    BOOST_CHECK(!GetBoolArg("-CHR", true));

    ResetArgs("-CHR -noCHR");  // -CHR should win
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", false));
    BOOST_CHECK(GetBoolArg("-CHR", true));

    ResetArgs("-CHR=1 -noCHR=1");  // -CHR should win
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", false));
    BOOST_CHECK(GetBoolArg("-CHR", true));

    ResetArgs("-CHR=0 -noCHR=0");  // -CHR should win
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", false));
    BOOST_CHECK(!GetBoolArg("-CHR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CHR=1");
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", false));
    BOOST_CHECK(GetBoolArg("-CHR", true));

    ResetArgs("--noCHR=1");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", false));
    BOOST_CHECK(!GetBoolArg("-CHR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CHR", "eleven"), "eleven");

    ResetArgs("-CHR -bar");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CHR", "eleven"), "");

    ResetArgs("-CHR=");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CHR", "eleven"), "");

    ResetArgs("-CHR=11");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CHR", "eleven"), "11");

    ResetArgs("-CHR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CHR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CHR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CHR", 0), 0);

    ResetArgs("-CHR -bar");
    BOOST_CHECK_EQUAL(GetArg("-CHR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CHR=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CHR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CHR=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CHR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CHR");
    BOOST_CHECK_EQUAL(GetBoolArg("-CHR"), true);

    ResetArgs("--CHR=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CHR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCHR");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", true));
    BOOST_CHECK(!GetBoolArg("-CHR", false));

    ResetArgs("-noCHR=1");
    BOOST_CHECK(!GetBoolArg("-CHR"));
    BOOST_CHECK(!GetBoolArg("-CHR", true));
    BOOST_CHECK(!GetBoolArg("-CHR", false));

    ResetArgs("-noCHR=0");
    BOOST_CHECK(GetBoolArg("-CHR"));
    BOOST_CHECK(GetBoolArg("-CHR", true));
    BOOST_CHECK(GetBoolArg("-CHR", false));

    ResetArgs("-CHR --noCHR");
    BOOST_CHECK(GetBoolArg("-CHR"));

    ResetArgs("-noCHR -CHR"); // CHR always wins:
    BOOST_CHECK(GetBoolArg("-CHR"));
}

BOOST_AUTO_TEST_SUITE_END()
