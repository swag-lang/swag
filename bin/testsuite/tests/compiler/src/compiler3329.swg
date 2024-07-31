#global testerror "Err0631"
func toto()
{
    var values = [[1], [2]]
    var a = 0
    visit b, a: values
    {
        @print(a)
    }
}
