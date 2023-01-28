func toto(x: u8){}
func toto(x: u16){}
func toto(x: u32){}

#test
{
    toto(0xFFFFFFFF)
}