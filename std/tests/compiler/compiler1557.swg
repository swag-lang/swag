//using swag

struct(T) A
{
}

impl A
{
    func fct(using self)
    {
        var ptr : T
    }

    /*func opDrop(using self)
    {
        var ptr : T
    }*/
}

//#[swag.printbc]
#test
{
    {
        var x: A's32
        //x.fct()
    }
}