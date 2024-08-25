#global testerror "Err0001"
using Core

//#[Swag.PrintBc]
#test
{
    var x: BitArray = [true, false, true, false]
    foreach &ptr in x
    {
        @print("XX\n")
    }
}
