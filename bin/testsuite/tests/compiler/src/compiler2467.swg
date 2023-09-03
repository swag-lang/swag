#global testerror "Saf0014"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: s64 = Swag.S64.Min - 1
}