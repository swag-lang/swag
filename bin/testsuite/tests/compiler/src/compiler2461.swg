#global testerror "San0009"
#global testerror "Saf0009"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s16 = Swag.S16.Max
    x += 1
}