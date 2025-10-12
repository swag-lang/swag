#global testerror "Saf0027"
#[Swag.Safety("", "", true)]
#test
{
    const month = 11'u8
    const year  = 255'u8
    const day   = month + year
}
