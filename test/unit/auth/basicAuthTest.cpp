#include <gtest/gtest.h>
#include <map>
#include <stdexcept>
#include <string>

#include "../../../src/driver/config/driverConfig.hpp"
#include "../../../src/trinoAPIWrapper/authProvider/basicAuthProvider.hpp"
#include "../../../src/util/b64decoder.hpp"
#include "../../../src/util/delimKvpHelper.hpp"

TEST(StringToBase64Test, Empty) {
  EXPECT_EQ(stringToBase64(""), "");
}

TEST(StringToBase64Test, Padding) {
  EXPECT_EQ(stringToBase64("A"), "QQ==");
  EXPECT_EQ(stringToBase64("AA"), "QUE=");
  EXPECT_EQ(stringToBase64("AAA"), "QUFB");
}

TEST(StringToBase64Test, LongInputHasNoLineBreaks) {
  std::string encoded = stringToBase64(std::string(200, 'x'));
  EXPECT_EQ(encoded.find('\r'), std::string::npos);
  EXPECT_EQ(encoded.find('\n'), std::string::npos);
  EXPECT_EQ(encoded.find('\0'), std::string::npos);
  EXPECT_EQ(encoded.size(), 268u);
}

TEST(BasicAuthTest, HeaderValueMatchesRfc7617Example) {
  EXPECT_EQ(buildBasicAuthHeaderValue("Aladdin", "open sesame"),
            "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
}

TEST(BasicAuthTest, HeadersCarryUserAndAuthorization) {
  auto config = getBasicAuthConfigPtr(
      "https://trino.example.com", 443, "dsn", "proj__alice", "s3cret");
  EXPECT_EQ(config->headers.at("X-Trino-User"), "proj__alice");
  EXPECT_EQ(config->headers.at("Authorization"),
            buildBasicAuthHeaderValue("proj__alice", "s3cret"));
  EXPECT_FALSE(config->isExpired());
}

TEST(BasicAuthTest, HttpsSchemeIsCaseInsensitive) {
  EXPECT_NO_THROW(
      getBasicAuthConfigPtr("HTTPS://trino.example.com", 443, "dsn", "u", "p"));
}

TEST(BasicAuthTest, RejectsPlainHttp) {
  EXPECT_THROW(
      getBasicAuthConfigPtr("http://trino.example.com", 8080, "dsn", "u", "p"),
      std::invalid_argument);
}

TEST(BasicAuthTest, RejectsHostnameWithoutScheme) {
  EXPECT_THROW(getBasicAuthConfigPtr("trino.example.com", 443, "dsn", "u", "p"),
               std::invalid_argument);
}

TEST(BasicAuthTest, RejectsEmptyUsername) {
  EXPECT_THROW(
      getBasicAuthConfigPtr("https://trino.example.com", 443, "dsn", "", "p"),
      std::invalid_argument);
}

TEST(BasicAuthConfigTest, ConnectionStringKeysOverrideDsnKeys) {
  // Connection string keys arrive lowercased; DSN keys keep their case.
  std::map<std::string, std::string> kvps = {
      {"authmethod", "basic auth"},
      {"username", "dsnUser"},
      {"password", "dsnPassword"},
      {"uid", "connUser"},
      {"pwd", "connPassword"},
  };
  DriverConfig config = driverConfigFromKVPs(kvps);
  EXPECT_EQ(config.getAuthMethodEnum(), AM_BASIC_AUTH);
  EXPECT_EQ(config.getUsername(), "connUser");
  EXPECT_EQ(config.getPassword(), "connPassword");
}

TEST(BasicAuthConfigTest, EmptyConnectionStringKeysKeepDsnValues) {
  std::map<std::string, std::string> kvps = {
      {"username", "dsnUser"},
      {"password", "dsnPassword"},
      {"uid", ""},
      {"pwd", ""},
  };
  DriverConfig config = driverConfigFromKVPs(kvps);
  EXPECT_EQ(config.getUsername(), "dsnUser");
  EXPECT_EQ(config.getPassword(), "dsnPassword");
}

TEST(BasicAuthConfigTest, RoundTripThroughKvps) {
  DriverConfig config;
  config.setHostname("https://trino.example.com");
  config.setPort(static_cast<uint16_t>(443));
  config.setAuthMethod(AM_BASIC_AUTH);
  config.setUsername("proj__alice");
  config.setPassword("s3cret");

  std::map<std::string, std::string> kvps = driverConfigToKVPs(config);
  EXPECT_EQ(kvps.at("authmethod"), "Basic Auth");

  DriverConfig roundTripped = driverConfigFromKVPs(kvps);
  EXPECT_EQ(roundTripped.getAuthMethodEnum(), AM_BASIC_AUTH);
  EXPECT_EQ(roundTripped.getUsername(), "proj__alice");
  EXPECT_EQ(roundTripped.getPassword(), "s3cret");
}

TEST(RedactSecretValuesTest, RedactsSecretKeysInAnyCase) {
  std::map<std::string, std::string> kvps = {
      {"pwd", "a"},
      {"PWD", "b"},
      {"password", "c"},
      {"clientSecret", "d"},
      {"uid", "alice"},
      {"hostname", "https://trino.example.com"},
  };
  std::map<std::string, std::string> redacted = redactSecretValues(kvps);
  EXPECT_EQ(redacted.at("pwd"), "***");
  EXPECT_EQ(redacted.at("PWD"), "***");
  EXPECT_EQ(redacted.at("password"), "***");
  EXPECT_EQ(redacted.at("clientSecret"), "***");
  EXPECT_EQ(redacted.at("uid"), "alice");
  EXPECT_EQ(redacted.at("hostname"), "https://trino.example.com");
}
