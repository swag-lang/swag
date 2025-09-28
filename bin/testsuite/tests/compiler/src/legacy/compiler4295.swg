#global testerror "Err0561"
func toto(x: u32){}

#test
{
    toto(4294967295)
}