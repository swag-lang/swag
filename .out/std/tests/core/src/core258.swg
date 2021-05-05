using core, drawing

struct Test
{
    color: Color
}

#test
{
    var tw: file.TweakFile
    var t1: Test
    tw.registerFolder(&t1)

    const BUF = @"
    /Test
        color  127 0 0 255
    "@

    arr := tokenize.splitLines(BUF)
    assume tw.parse(arr)
    console.println(t1)
}