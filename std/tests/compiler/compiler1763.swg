enum TITI: f32
{
    AA = 1.0
    BB = 1.5
}

#[swag.printbc]
#test
{
     pt0 := TITI.AA
     pt1 := TITI.BB
}