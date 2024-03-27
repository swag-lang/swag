using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String

    arr.add("0")
    arr.add("1")

    var x = String.from("A")
    arr.insertAt(0, moveref &x)
}