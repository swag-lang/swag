#import "std"

//#[swag.printbc]
#test
{
    @print(cast(s64) std.toto8(1,2,3,4,5,6))

    if true
    {
        @print(std.toto7(1,2,3,4))
        @print(std.toto6("666"))
        @print(std.toto5(666))
        @print(std.toto4())
        std.toto3(1,2,3,4,5,6)
        std.toto()
        std.toto1(64)
        std.toto2(1,2,3,4)
    }
}