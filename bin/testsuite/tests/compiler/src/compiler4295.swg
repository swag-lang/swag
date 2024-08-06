#global testerror "Err0608"
func toto(x: u32){}

#test
{
    toto(4294967295)
}