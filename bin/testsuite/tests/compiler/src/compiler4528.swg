#global testerror "Err0550"

enum A
{
    R
}

#[Swag.Strict]
typealias AL = A

//#[Swag.PrintBc]
#test
{
    var x: AL
    var x1: A
    x = x1
}