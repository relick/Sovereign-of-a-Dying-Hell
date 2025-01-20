#pragma once

#include "Declare.hpp"

#include <tools.h>

#if DEBUG
#define Error(...) kprintf(__VA_ARGS__)
#else
#define Error(...)
#endif