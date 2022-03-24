#global public
using Core, Pixel

struct RichEditLexerSwag
{
    #[Swag.Incomplete]
    enum SwagState: s32
    {
        Default
        LineComment
        Comment
        Keyword
        Declaration
        Control
    }

    currentState:   s32
    mapStyles:      [256] RichEditStyleRef
    mapWords:       HashTable'(string, s32)
}

impl IRichEditLexer for RichEditLexerSwag
{
    mtd setup(ed: *RichEdit)
    {
        mapStyles[SwagState.LineComment] = 1
        mapStyles[SwagState.Comment] = 1
        mapStyles[SwagState.Keyword] = 2
        mapStyles[SwagState.Control] = 3
        mapStyles[SwagState.Declaration] = 4

        ed.styles[1].colFg = 0xFF6A9955
        ed.styles[2].colFg = 0xFF569cd6
        ed.styles[3].colFg = 0xFFD8A0DF
        ed.styles[4].colFg = 0xFF569cd6

        mapWords.add("using", SwagState.Keyword)
        mapWords.add("with", SwagState.Keyword)
        mapWords.add("cast", SwagState.Keyword)
        mapWords.add("acast", SwagState.Keyword)
        mapWords.add("bitcast", SwagState.Keyword)
        mapWords.add("dref", SwagState.Keyword)
        mapWords.add("retval", SwagState.Keyword)
        mapWords.add("try", SwagState.Keyword)
        mapWords.add("catch", SwagState.Keyword)
        mapWords.add("assume", SwagState.Keyword)
        mapWords.add("throw", SwagState.Keyword)
        mapWords.add("discard", SwagState.Keyword)

        mapWords.add("enum", SwagState.Declaration)
        mapWords.add("struct", SwagState.Declaration)
        mapWords.add("union", SwagState.Declaration)
        mapWords.add("impl", SwagState.Declaration)
        mapWords.add("interface", SwagState.Declaration)
        mapWords.add("func", SwagState.Declaration)
        mapWords.add("closure", SwagState.Declaration)
        mapWords.add("mtd", SwagState.Declaration)
        mapWords.add("mtdc", SwagState.Declaration)
        mapWords.add("namespace", SwagState.Declaration)
        mapWords.add("alias", SwagState.Declaration)
        mapWords.add("attr", SwagState.Declaration)

        mapWords.add("if", SwagState.Control)
        mapWords.add("else", SwagState.Control)
        mapWords.add("elif", SwagState.Control)
        mapWords.add("for", SwagState.Control)
        mapWords.add("while", SwagState.Control)
        mapWords.add("switch", SwagState.Control)
        mapWords.add("defer", SwagState.Control)
        mapWords.add("errdefer", SwagState.Control)
        mapWords.add("loop", SwagState.Control)
        mapWords.add("visit", SwagState.Control)
        mapWords.add("break", SwagState.Control)
        mapWords.add("fallthrough", SwagState.Control)
        mapWords.add("return", SwagState.Control)
        mapWords.add("case", SwagState.Control)
        mapWords.add("continue", SwagState.Control)
        mapWords.add("default", SwagState.Control)
        mapWords.add("scope", SwagState.Control)
        mapWords.add("and", SwagState.Control)
        mapWords.add("or", SwagState.Control)
        mapWords.add("orelse", SwagState.Control)
    }

    mtd setState(state: s32)
    {
        currentState = state
    }

    mtd getState()->s32
    {
        return currentState
    }

    mtd compute(line: *RichEditLine)
    {
        seek := 0'uint

        #[Swag.Mixin]
        func getCur()->rune
        {
            if seek >= line.chars.count return 0
            return line.chars[seek]
        }

        #[Swag.Mixin]
        func getNext()->rune
        {
            if seek >= line.chars.count - 1 return 0
            return line.chars[seek + 1]
        }

        #[Swag.Mixin]
        func seekNext()
        {
            line.styles[seek] = mapStyles[currentState]
            seek += 1
        }

        loop
        {
            c  := getCur()
            cn := getNext()
            if !c break

            switch currentState
            {
            case SwagState.Comment:
                if c == "*"'rune and cn == "/"'rune
                {
                    seekNext()
                    seekNext()
                    currentState = SwagState.Default
                    continue
                }

            case SwagState.Default:
                if Unicode.isLetter(c) or c == "_"'rune
                {
                    var word: String
                    saveSeek := seek
                    while Unicode.isLetterOrDigit(c) or c == "_"'rune
                    {
                        word += c
                        seek += 1
                        c = getCur()
                    }

                    seekEnd := seek
                    seek = saveSeek

                    res := mapWords.find(word.toString())
                    if res
                        currentState = res.value
                    else
                        currentState = SwagState.Default
                    while seek != seekEnd
                        seekNext()
                    currentState = SwagState.Default
                    continue
                }

                if c == "/"'rune and cn == "*"'rune
                {
                    currentState = SwagState.Comment
                    seekNext()
                    seekNext()
                    continue
                }

                if c == "/"'rune and cn == c
                {
                    currentState = SwagState.LineComment
                    seekNext()
                    seekNext()
                    continue
                }
            }

            seekNext()
        }

        if currentState == SwagState.LineComment
            currentState = SwagState.Default
    }
}