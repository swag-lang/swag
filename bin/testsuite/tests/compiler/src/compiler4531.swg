#global testerror "Err0647"

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
    x |= .R
}