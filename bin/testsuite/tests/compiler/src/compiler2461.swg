#global testerror "San0010"
#global testerror "Saf0026"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s16 = Swag.S16.Max
    x += 1
}
