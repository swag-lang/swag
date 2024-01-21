#global testerror "Saf0026"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: s64 = Swag.S64.Min * 2
}