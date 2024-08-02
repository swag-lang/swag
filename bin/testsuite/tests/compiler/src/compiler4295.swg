#global testerror "Err0644"
func toto(x: u32){}

#test
{
    toto(4294967295)
}