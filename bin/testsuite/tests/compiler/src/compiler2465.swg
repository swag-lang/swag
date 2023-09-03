#global testerror "Saf0012"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: s64 = Swag.S64.Max + 1
}