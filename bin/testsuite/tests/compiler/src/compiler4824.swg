#global testerror "San0010"
#global testerror "Saf0026"
#[Swag.Safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x *= 2
}
