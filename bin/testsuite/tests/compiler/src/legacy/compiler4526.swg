#global testerror "Err0579"

enum A: string
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
