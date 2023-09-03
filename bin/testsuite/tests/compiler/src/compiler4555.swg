#global testerror "Saf0005"

typealias A = string

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var x: A
    var y = x[1..2]
}