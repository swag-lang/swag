//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var t = 0

    var x = 3
    x += 1

    /*let y = @dataof([x, x+1])
@print(y[0])
@print(y[1])*/

    visit i: [x, x + 1]
    {
        t += i
    }

    //@print(t)
    @assert(t == 9)
}
