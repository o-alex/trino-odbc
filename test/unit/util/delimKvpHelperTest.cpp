#include <gtest/gtest.h>
#include <map>
#include <string>

#include "../../../src/util/delimKvpHelper.hpp"

using KVPs = std::map<std::string, std::string>;

TEST(ConnStrParseTest, Empty) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr(""), KVPs{});
}

TEST(ConnStrParseTest, PlainValuesWithAndWithoutTrailingSemicolon) {
  KVPs expected = {{"dsn", "Trino"}, {"uid", "alice"}};
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("DSN=Trino;UID=alice"), expected);
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("DSN=Trino;UID=alice;"), expected);
}

TEST(ConnStrParseTest, KeysAreTrimmedAndLowercasedValuesAreNot) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("  UiD = Alice "),
            (KVPs{{"uid", " Alice "}}));
}

TEST(ConnStrParseTest, EmptyValue) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("UID=;PWD="),
            (KVPs{{"uid", ""}, {"pwd", ""}}));
}

TEST(ConnStrParseTest, UnbracedValueKeepsEqualsSigns) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD=a=b"), (KVPs{{"pwd", "a=b"}}));
}

TEST(ConnStrParseTest, BracedValueKeepsSemicolonsAndEquals) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("UID=alice;PWD={pa;ss=word};X=1"),
            (KVPs{{"uid", "alice"}, {"pwd", "pa;ss=word"}, {"x", "1"}}));
}

TEST(ConnStrParseTest, BracedValueAtEndOfString) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={a;b}"),
            (KVPs{{"pwd", "a;b"}}));
}

TEST(ConnStrParseTest, DoubledClosingBraceIsLiteral) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={a}}b;c}}};UID=u"),
            (KVPs{{"pwd", "a}b;c}"}, {"uid", "u"}}));
}

TEST(ConnStrParseTest, OpeningBraceInsideBracesIsLiteral) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={a{b}"),
            (KVPs{{"pwd", "a{b"}}));
}

TEST(ConnStrParseTest, EmptyBracedValue) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={};UID=u"),
            (KVPs{{"pwd", ""}, {"uid", "u"}}));
}

TEST(ConnStrParseTest, DriverNameInBraces) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("Driver={Trino ODBC Driver};DSN=t"),
            (KVPs{{"driver", "Trino ODBC Driver"}, {"dsn", "t"}}));
}

TEST(ConnStrParseTest, TextAfterClosingBraceIsIgnored) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={ab}cd;UID=u"),
            (KVPs{{"pwd", "ab"}, {"uid", "u"}}));
}

TEST(ConnStrParseTest, UnterminatedBraceIsLiteralToNextSemicolon) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD={abc;UID=u"),
            (KVPs{{"pwd", "{abc"}, {"uid", "u"}}));
}

TEST(ConnStrParseTest, BraceNotAtValueStartIsLiteral) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("PWD=a{b}c"),
            (KVPs{{"pwd", "a{b}c"}}));
}

TEST(ConnStrParseTest, SegmentsWithoutEqualsAreSkipped) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr(";;junk;UID=u;;PWD=p"),
            (KVPs{{"uid", "u"}, {"pwd", "p"}}));
}

TEST(ConnStrParseTest, EmptyKeyIsDropped) {
  EXPECT_EQ(parseKVPsFromSemicolonDelimStr("=x;UID=u"), (KVPs{{"uid", "u"}}));
}
