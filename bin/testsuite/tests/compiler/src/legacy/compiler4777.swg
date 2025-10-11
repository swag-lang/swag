#global testerror "Saf0027"

#[Swag.Safety("", true)]
#test
{
    var b: u8 = 255'u8 + 1'u8
}

#assert(1.0 / 2.0 == 0.5)
