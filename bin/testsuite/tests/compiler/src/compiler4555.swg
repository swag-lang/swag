#global testerror "Saf0007"

typealias A = string

#[Swag.Optim("", false), Swag.Safety("", true)]
#test
{
    var x: A
    var y  = x[1..2]
}
