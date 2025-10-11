public interface C1961ITF
{
    func toto(me);
}

public struct C1961AA {}

public impl C1961ITF for C1961AA
{
    func impl toto(me) {}
}

func(T) gen(x: T)
{
    let itf = @mkinterface(x, #typeof(T), C1961ITF)
    if itf != null do
        itf.toto()
}

#test
{
    var x: C1961AA
    gen'C1961AA(x)
}
