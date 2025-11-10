func sendMessage(text: string) {}

func indexOf(a: string, b: rune, c: u64)->u64
{
    return 2
}

func sendWords(text: string)
{
    var start = 0'u64
    while start < @countof(text)
    {
        //@print("start:", start, "\n")
        let stop = indexOf(text, ' ''rune, start)
        //@print("stop:", stop, "\n")
        if stop == Swag.U64.Max do
            break
        let word = text[start to stop]
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
    let text = "hi there you"
    sendWords(text)
}
