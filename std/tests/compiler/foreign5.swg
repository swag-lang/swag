#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

func thisFunc1(v, w, x, y, z: s64)
{
    /*@print("thisFunc1: ")
    @print(cast(s64) v)
    @print(" ")
    @print(cast(s64) w)
    @print(" ")
    @print(cast(s64) x)
    @print(" ")
    @print(cast(s64) y)
    @print(" ")
    @print(cast(s64) z)
    @print("\n")*/

    @assert(v == 333)
    @assert(w == 444)
    @assert(x == 555)
    @assert(y == 666)
    @assert(z == 777)
}

//#[swag.printbc]
#test
{
    ptr3 := &tcf_toto19
    ptr3(333, 444, 555, 666, 777, &thisFunc1)
}