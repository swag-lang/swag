#global testerror "Err0584"

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
    x = !x
}