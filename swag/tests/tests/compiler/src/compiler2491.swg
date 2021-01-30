#[swag.printbc]
#test
{
    var x: *u32
    if !x return

    a := 0
    switch(a)
    {
        case 0:
            if !a break
            @print("0")

        case 1:
            @print("1")
    }
}