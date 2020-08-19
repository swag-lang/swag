#import "tests_compiler_foreign"

using tests_compiler_foreign
#test
{
    @print(toto9(1,2,3,4,5,6))
    @print(cast(s64) toto8(1,2,3,4,5,6))
    @print(toto7(1,2,3,4))
    @print(toto6("666"))
    @print(toto5(666))
    @print(toto4())
    toto3(1,2,3,4,5,6)
    toto()
    toto1(64)
    toto2(1,2,3,4)
    @print("\n")
}