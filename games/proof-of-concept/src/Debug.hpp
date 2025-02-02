#pragma once

#include "Declare.hpp"

#if DEBUG
#define Error(...) kprintf(__VA_ARGS__)
#else
#define Error(...)
#endif