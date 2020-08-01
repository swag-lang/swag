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

//#[swag.printbc]
#test
{
    var x: A
    x = @[true]
}