interface IIncrementable
{
    decr: func(self)
    done: func(self)->bool
}

//#[Swag.printbc]
#test
{
    var x: IIncrementable
    @assert(x == null)
}