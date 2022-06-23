#include <Windows.h>
#include "compile.h"
#include <cstdio>
#include <type_traits>

#define STANDARD_CRYPT(x) obfuscate::String(x).Decrypt()
#define FORCE_VOLATILE_STRING(x) ([]() { volatile auto a = obfuscate::String(x); std::remove_volatile<decltype(a)>::type* b = (std::remove_volatile<decltype(a)>::type*)&a; return *b; })().Decrypt()

__declspec(noinline) void print_fn()
{
    printf(
        FORCE_VOLATILE_STRING("funko [%s][%s][%s]\n"),
        FORCE_VOLATILE_STRING(__DATE__),
        FORCE_VOLATILE_STRING(__TIME__),
        FORCE_VOLATILE_STRING("Big hiding"));
}

int main()
{
    print_fn();
    print_fn();
    print_fn();
    return 0;
}