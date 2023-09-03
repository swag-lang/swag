#global testerror "Saf0011"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = 0
    x += 100
    x += 100
}