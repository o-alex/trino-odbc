#pragma once

#include <string>

std::string fromBase64url(const std::string& encodedString);

// Standard base64 (RFC 4648 section 4) with padding and no line breaks.
std::string stringToBase64(const std::string& data);
