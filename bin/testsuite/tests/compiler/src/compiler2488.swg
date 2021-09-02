using Swag

#[Safety("overflow", true)]
//#[PrintBc]
#test
{
    #semerror var x0: u8 = cast(u8) 256.0
    #semerror var x1: u8 = cast(u8) -1.0
}