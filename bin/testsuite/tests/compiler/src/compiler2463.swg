#global testerror "San0009"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}