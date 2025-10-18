#global testerror "Err0737"

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
