//#global testerror

func toto() {}
func titi() {}

#test
{
    let x = &toto
    var r = 0
    switch x
    {
    case &titi: break
    case &toto: r = 1; break
    }

    @assert(r == 1)
}