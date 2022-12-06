using Core

alias CB = #type closure()

//#[Swag.PrintBc]
#test
{
    var arr: Array'CB
    x := func() {
    }

    arr.add(x)
}