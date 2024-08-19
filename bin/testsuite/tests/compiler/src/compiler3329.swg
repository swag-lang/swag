#global testerror "Err0519"
func toto()
{
    var values = [[1], [2]]
    var a = 0
    visit b, a in values
    {
        @print(a)
    }
}
