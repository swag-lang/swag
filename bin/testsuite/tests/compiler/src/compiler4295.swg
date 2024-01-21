#global testerror "Err0652"
func toto(x: u32){}

#test
{
    toto(4294967295)
}