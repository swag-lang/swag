#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

func thisFunc(x: s64)
{
    //@print(x)
    @assert(x == 555)
}

func thisFunc1(x, y: s64)
{
    /*@print("thisFunc1: ")
    @print(cast(s64) x)
    @print(" ")
    @print(cast(s64) y)
    @print("\n")*/

    @assert(x == 555)
    @assert(y == 666)
}

//#[swag.printbc]
#test
{
    ptr3 := &tcf_toto18
    ptr3(555, 666, &thisFunc1)

    ptr := &tcf_toto16
    ptr(456,3.14)

    ptr1 := &tcf_toto5
    @assert(ptr1(10) == 10)

    ptr2 := &tcf_toto17
    ptr2(555, &thisFunc)
}