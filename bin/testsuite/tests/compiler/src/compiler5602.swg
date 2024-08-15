//#global testerror "Err0551"

func toto() => 1

#test
{
    var g: bool
    if var x = toto() where x > 2
    {
        g = true
    }

    @assert(!g)
}
