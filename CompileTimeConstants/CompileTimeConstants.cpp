#include "compile.h"
#include <cstdio>

int main()
{
    auto E1 = v2::obfuscate::String(__DATE__);
    auto E2 = v2::obfuscate::String(__TIME__);

	printf("funko [%s][%s]\n", E1.Decrypt(), E2.Decrypt());
	printf("funko [%s][%s]\n", E1.Decrypt(), E2.Decrypt());
	printf("funko [%s][%s]\n", E1.Decrypt(), E2.Decrypt());
	printf("funko [%s][%s]\n", E1.Decrypt(), E2.Decrypt());
	printf("funko [%s][%s]\n", E1.Decrypt(), E2.Decrypt());

    return 0;
}