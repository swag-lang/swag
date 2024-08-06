#global testerror "Err0610"
func toto(x: u32){}

#test
{
    toto(4294967295)
}