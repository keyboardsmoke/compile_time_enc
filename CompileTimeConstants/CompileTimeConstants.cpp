#include <Windows.h>
#include "compile.h"
#include <cstdio>
#include <type_traits>

#define SELECT_IMPL() static_cast<std::size_t>(__LINE__ % static_cast<unsigned>(obfuscate::StringCryptMode::Last))
#define STANDARD_CRYPT(x) obfuscate::StringModeImpl<SELECT_IMPL()>::Generate(x).Decrypt()
#define FORCE_VOLATILE_STRING(x) ([]() { volatile auto a = obfuscate::StringModeImpl<SELECT_IMPL()>::Generate(x); std::remove_volatile<decltype(a)>::type* b = (std::remove_volatile<decltype(a)>::type*)&a; return *b; })().Decrypt()

__declspec(noinline) void print_fn()
{
    printf(
        STANDARD_CRYPT("funko [%s][%s][%s]\n"),
        STANDARD_CRYPT(__DATE__),
        STANDARD_CRYPT(__TIME__),
        STANDARD_CRYPT("Big hiding"));
}

int main()
{
    print_fn();
    print_fn();
    print_fn();
    return 0;
}