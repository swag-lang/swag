//#global testerror "Err0530"

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
