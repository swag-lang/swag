#global testerror "Saf0027"

#test
{
    #[Swag.Safety(.All, "overflow", true)]
    var x: s64 = Swag.S64.Min - 1
}
