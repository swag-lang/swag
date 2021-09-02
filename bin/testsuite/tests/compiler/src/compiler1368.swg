interface IIncrementable
{
    decr: func(self)
    done: func(self)->bool
}

//#[Swag.PrintBc]
#test
{
    var x: IIncrementable
    @assert(x == null)
}