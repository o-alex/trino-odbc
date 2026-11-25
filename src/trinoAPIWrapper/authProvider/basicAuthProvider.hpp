#pragma once

#include <memory>
#include <string>

#include "authConfig.hpp"

std::string buildBasicAuthHeaderValue(const std::string& username,
                                      const std::string& password);

// Throws std::invalid_argument when hostname is not https:// or the username
// is empty: Trino rejects passwords sent over plain HTTP.
std::unique_ptr<AuthConfig> getBasicAuthConfigPtr(std::string hostname,
                                                  unsigned short port,
                                                  std::string connectionName,
                                                  std::string username,
                                                  std::string password);
