const ALPHA = true

struct A
{

}

impl A
{
    func(T) opCast(using self)->T
    {
        #if @typeof(T) == @typeof(string)
            return #"AA = 10"#
        #else
            #assert(false)
    }

    func opDrop(using self)
    {
        @print("opDrop\n")
    }
}

enum Color
{
    Red
    Green
    Blue

    #[swag.printbc]
    #ast
    {
        var x: A
        return cast(string) x
    }
}

//#[swag.printbc]
#test
{
    @assert(cast(s64) Color.AA == 10)
}