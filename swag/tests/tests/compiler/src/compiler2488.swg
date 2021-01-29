using swag

#[safety("of", true)]
//#[printbc]
#test
{
    #semerror var x0: u8 = cast(u8) 256.0
    #semerror var x1: u8 = cast(u8) -1.0

    x := 0'u32
    x -= 1
}