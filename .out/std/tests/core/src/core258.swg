using core, Drawing

struct Test
{
    color:  Color
    color1: Color
}

#test
{
    var tw: File.TweakFile
    var t1: Test
    tw.registerFolder(&t1)

    const BUF = @"
    /Test
        color  127 0 0 255
        color1 Red
    "@

    arr := tokenize.splitLines(BUF)
    assume tw.parse(arr)
    @assert(t1.color.a == 127)
    @assert(t1.color.r == 0)
    @assert(t1.color.g == 0)
    @assert(t1.color.b == 255)
    @assert(t1.color1.r == 255)

    //Console.println(t1)
}