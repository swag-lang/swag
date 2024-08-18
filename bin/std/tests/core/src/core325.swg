using Core

func sendMessage(text: string)
{
    //Console.print(text, "\n")
}

func sendWords(text: string)
{
    var start = 0'u64
    while start < @countof(text)
    {
        var stop = Utf8.indexOf(text, ` `'rune, start)
        if stop == Swag.U64.Max:
            stop = @countof(text) - 1
        let word = text[start..stop]
        sendMessage(word)
        start = stop + 1
    }
}

#test
{
    let text = "aa bbbb c ddddd "
    sendWords(text)
}
