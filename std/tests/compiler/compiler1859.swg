//var g = "toto"
var g = -6's8

#[swag.printbc]
#test
{
    {
        x := -6's64
        @print(cast(s64) x)
        @print("\n")
        @print(cast(s64) g)
        @print("\n")
        @print("toto la fripouille\n")
        @print(666's64)
        @print("\n")
        @print(7666'f64)
    }
}