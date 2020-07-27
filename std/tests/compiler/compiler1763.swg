//#[swag.printbc]
func toto()
{
    a := 0xBB
}

//#[swag.printbc]
#test
{
    a := 0xAA
    b := 1.5'f32
    c := true
    d := "toto也toto"
    toto()
    a = 5
    c = false
}