struct AA
{

}

var G = false
func toto(x: AA)
{
    G = true
}

//#[Swag.PrintBc]
#test
{
    var x = &toto
    var tt: AA
    tt.x()
}