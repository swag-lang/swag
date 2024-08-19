#global testerror "Err0631"

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
    x = !x
}