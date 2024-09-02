#global testerror "Err0573"

enum A
{
    R
}

#[Swag.Strict]
alias AL = A

//#[Swag.PrintBc]
#test
{
    var x: AL
    var x1: A
    x = x1
}