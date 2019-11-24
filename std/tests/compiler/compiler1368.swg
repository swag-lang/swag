interface IIncrementable
{
    decr: func(self)
    done: func(self)->bool
}

//#[swag.printbc]
#test
{   
    var x: IIncrementable
    @assert(x == null)
}