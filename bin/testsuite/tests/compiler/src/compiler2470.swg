#global testerror "Saf0027"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: s64 = Swag.S64.Min * 2
}