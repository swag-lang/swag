#global testerror "San0006"

typealias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Safety("sanity", true)]
#test
{
    var x: A
    var y = x[1..2]
}