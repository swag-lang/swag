
#global testerror


using Core

//#[Swag.printbc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit *ptr: x
    {
        @print("XX\n")
    }
}