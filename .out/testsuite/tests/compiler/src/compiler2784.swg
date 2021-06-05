struct AA
{

}

var G = false
func toto(x: AA)
{
    G = true
}

//#[Swag.printbc]
#test
{
    x := &toto
    var tt: AA
    tt.x()
}