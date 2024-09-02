#global testerror "Err0493"

enum A : string
{
    R = ""
}

#[Swag.Strict]
alias AL = A

//#[Swag.PrintBc]
#test
{
    var x: AL
}