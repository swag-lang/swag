#global testerror "Err0647"
func toto(x: u32){}

#test
{
    toto(4294967295)
}