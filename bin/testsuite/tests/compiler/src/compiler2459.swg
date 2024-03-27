#global testerror "San0010"
#global testerror "Saf0026"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = 0
    x += 100
    x += 100
}
