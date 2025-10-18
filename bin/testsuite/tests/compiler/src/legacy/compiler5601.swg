//#global testerror "Err0023"

func toto() => 4

#test
{
    var g: bool
    if let x = toto() where x > 2
    {
        g = true
    }

    @assert(g)
}
