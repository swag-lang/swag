#global testerror "Saf0027"

#test
{
    #[Swag.Safety(.All, "overflow", true)]
    var x: u64 = 0'u64 - 1
}
