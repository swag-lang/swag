#global testerror "Err0579"
func toto(x: u32){}

#test
{
    toto(4294967295)
}