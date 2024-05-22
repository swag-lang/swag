interface IIncrementable
{
    func decr(self);
    func done(self)->bool;
}

//#[Swag.PrintBc]
#test
{
    var x: IIncrementable
    @assert(x == null)
}