public interface C1961ITF
{
    func toto(self);
}

public struct C1961AA
{

}

public impl C1961ITF for C1961AA
{
    func impl toto(self)
    {
    }
}

func(T) gen(x: T)
{
    let itf = @mkinterface(x, #typeof(T), C1961ITF)
    if itf != null:
        itf.toto()
}

#test
{
    var x: C1961AA
    gen'C1961AA(x)
}