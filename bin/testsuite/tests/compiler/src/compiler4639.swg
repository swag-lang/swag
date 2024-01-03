#global testerror "Saf0009"
#[Swag.Safety("", true)]
#test
{
    const month = 11'u8
    const year = 255'u8
    const day = month + year
}
