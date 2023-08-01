#global testerror

enum A : string
{
    R = ""
}

#[Swag.Strict]
typealias AL = A

//#[Swag.PrintBc]
#test
{
    var x: AL
}