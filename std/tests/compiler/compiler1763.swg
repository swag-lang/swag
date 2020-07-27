#[swag.printbc]
func toto()
{
    a := 0xBB
}

#[swag.printbc]
#test
{
    a := 0xAA
    b := 1.5'f32
    toto()
    a = 5
}