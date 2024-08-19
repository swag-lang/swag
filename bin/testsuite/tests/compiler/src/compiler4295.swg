#global testerror "Err0591"
func toto(x: u32){}

#test
{
    toto(4294967295)
}