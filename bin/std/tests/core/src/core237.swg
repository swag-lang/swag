#global testerror "Err0085"


using Core

#[Swag.NoCopy]
struct AA
{
}

//#[Swag.PrintBc]
#test
{
    var arr: Array'AA
    var y: AA
    arr[0] = y
}