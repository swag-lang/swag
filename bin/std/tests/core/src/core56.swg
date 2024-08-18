#global testerror "Err0001"
using Core

//#[Swag.PrintBc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit &ptr in x
    {
        @print("XX\n")
    }
}
