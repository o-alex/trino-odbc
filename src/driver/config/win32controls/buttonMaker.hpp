#pragma once

#include "../../../util/windowsLean.hpp"
#include <string>

HWND buttonMaker(HWND form,
                 int horizOffset,
                 int vertOffset,
                 int buttonId,
                 std::string buttonText);
