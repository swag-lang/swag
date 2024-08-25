#global testerror "Saf0007"

typealias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#test
{
    var x: A
    var y = x[1..2]
}