#global testerror "San0010"
#global testerror "Saf0027"

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 0
    x += 100
    x += 200
}