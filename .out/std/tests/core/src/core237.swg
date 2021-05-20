#global testerror


using Core

#[Swag.nocopy]
struct AA
{
}

//#[Swag.printbc]
#test
{
    var arr: Array'AA
    var y: AA
    arr[0] = y
}