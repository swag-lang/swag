#global testerror "Saf0027"
#global testerror "Saf0027"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s16 = Swag.S16.Max
    x += 1
}