struct AA
{

}

var G = false
func toto(x: AA)
{
    G = true
}

//#[swag.printbc]
#test
{
    @dbgbreak()

    x := &toto
    var tt: AA
    tt.x()
}