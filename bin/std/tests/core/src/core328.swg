using Core

alias CB = closure()

//#[Swag.PrintBc]
#test
{
    var arr: Array'CB
    x := @() {
    }

    arr.add(x)
}