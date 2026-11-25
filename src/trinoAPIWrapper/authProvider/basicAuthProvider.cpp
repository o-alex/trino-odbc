#include "basicAuthProvider.hpp"

#include <map>
#include <stdexcept>
#include <string>

#include "../../util/b64decoder.hpp"
#include "../../util/delimKvpHelper.hpp"


std::string buildBasicAuthHeaderValue(const std::string& username,
                                      const std::string& password) {
  return "Basic " + stringToBase64(username + ":" + password);
}

class BasicAuthConfig : public AuthConfig {
  public:
    BasicAuthConfig(std::string hostname,
                    unsigned short port,
                    std::string connectionName,
                    std::string username,
                    std::string password)
        : AuthConfig(hostname, port, connectionName) {
      this->headers.insert({"X-Trino-User", username});
      this->headers.insert(
          {"Authorization", buildBasicAuthHeaderValue(username, password)});
    }

    bool const isExpired() override {
      return false;
    }
    void
    refresh(CURL* curl,
            std::string* responseData,
            std::map<std::string, std::string>* responseHeaderData) override {
      // Static credentials have nothing to refresh.
    }
    ~BasicAuthConfig() override = default;
};

std::unique_ptr<AuthConfig> getBasicAuthConfigPtr(std::string hostname,
                                                  unsigned short port,
                                                  std::string connectionName,
                                                  std::string username,
                                                  std::string password) {
  if (!toLowercase(hostname).starts_with("https://")) {
    throw std::invalid_argument(
        "Basic Auth requires an https:// hostname; Trino refuses passwords "
        "over plain HTTP");
  }
  if (username.empty()) {
    throw std::invalid_argument("Basic Auth requires a username");
  }
  return std::make_unique<BasicAuthConfig>(
      hostname, port, connectionName, username, password);
}
