#global testerror "Err0433"
using Core

//#[Swag.PrintBc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit &ptr: x
    {
        @print("XX\n")
    }
}