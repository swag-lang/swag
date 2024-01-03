#global testerror "San0009"
#global testerror "Saf0011"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}