func sendMessage(text: string)
{
}

func indexOf(a: string, b: rune, c: uint)->uint
{
    return 2
}

func sendWords(text: string)
{
    start := 0'uint
    while start < @countof(text)
    {
        //@print("start:", start, "\n")
        stop := indexOf(text, " "'rune, start)
        //@print("stop:", stop, "\n")
        if stop == Swag.UInt.Max
            break
        word := text[start..stop]
        sendMessage(word)
        //@breakpoint()
        start = stop + 1
        //@print("start:", start, "\n")
        @assert(start == 3)
        break
        //break
    }
}

#test
{
    text := "hi there you"
    sendWords(text)
}