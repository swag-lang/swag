struct A
{

}

impl A
{
    func opAffect(using self, t: [1] bool)
    {
        @assert(t[0] == true)
    }
}

//#[Swag.printbc]
#test
{
    var x: A = @[true]
    //var y: A
    //y = [true]
}