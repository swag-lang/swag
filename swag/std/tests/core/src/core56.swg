
#global testerror


using core

//#[swag.printbc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit *ptr: x
    {
        @print("XX\n")
    }
}