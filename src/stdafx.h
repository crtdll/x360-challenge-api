#pragma once
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <functional>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <inttypes.h>

#define UNUSED_RETURN(x) { auto v = x; v; };

template<typename T, int N>
constexpr int NUMOF(T(&)[N]) { return N; }