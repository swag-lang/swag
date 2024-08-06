#global testerror "Err0637"
func toto(x: u32){}

#test
{
    toto(4294967295)
}