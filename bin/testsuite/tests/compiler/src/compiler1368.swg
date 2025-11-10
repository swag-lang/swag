interface IIncrementable
{
    func decr(me);
    func done(me)->bool;
}

//#[Swag.PrintBc]
#test
{
    var x: IIncrementable
    @assert(x == null)
}
