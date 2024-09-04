//#global testerror "Err0518"

func toto() => 1

#test
{
    var g: bool
    if let x = toto() where x > 2
    {
        g = true
    }

    @assert(!g)
}
