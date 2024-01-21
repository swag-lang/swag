#global testerror "Saf0026"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = Swag.U64.Max * 2
}