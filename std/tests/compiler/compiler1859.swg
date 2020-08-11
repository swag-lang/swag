//var g = "toto"
var g = 5's8

#[swag.printbc]
#test
{
    if true
    {
        {
            x := @[1's64, 2, 3]
            ptr := &x[0]
            @print(ptr[0])
            @print(ptr[1])
        }

        {
            x := @[1's64, 2, 3]
            @print(x[0])
            @print(x[1])
            @print(x[2])
        }

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
            @print(cast(s64) g)
            @print("fripouille")
            @print(666's64)
            @print(7666'f64)
        }
    }
}