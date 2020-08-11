//var g = "toto"
var g = 5's8

#[swag.printbc]
#test
{
    {
        x := @[1's64, 2, 3]
        @print(x[0])
    }

    if false
    {
        {
            a := 5
            if a != 5
                @print("a")
            else
                @print("b")
        }

        {
            a := 5
            if a == 5
                @print("a")
            else
                @print("b")
        }

        {
            a := 5
            if a == 4
                @print("a")
            else
                @print("b")
        }

        loop 5 @print("g")

        {
            x := -6's8
            @print(cast(s64) x)
        }

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
}