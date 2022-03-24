#global public
using Core, Pixel

struct RichEditLexerSwag
{
    #[Swag.Incomplete]
    enum Style: u8
    {
        Default
        Comment
        Keyword
        Control
    }

    #[Swag.Incomplete]
    enum State: s32
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
        mapStyles[State.LineComment] = RichEditLexerSwag.Style.Comment
        mapStyles[State.Comment]     = RichEditLexerSwag.Style.Comment
        mapStyles[State.Keyword]     = RichEditLexerSwag.Style.Keyword
        mapStyles[State.Control]     = RichEditLexerSwag.Style.Control
        mapStyles[State.Declaration] = RichEditLexerSwag.Style.Keyword

        ed.styles[RichEditLexerSwag.Style.Comment].colFg = 0xFF6A9955
        ed.styles[RichEditLexerSwag.Style.Keyword].colFg = 0xFF569cd6
        ed.styles[RichEditLexerSwag.Style.Control].colFg = 0xFFD8A0DF

        mapWords.add("using", State.Keyword)
        mapWords.add("with", State.Keyword)
        mapWords.add("cast", State.Keyword)
        mapWords.add("acast", State.Keyword)
        mapWords.add("bitcast", State.Keyword)
        mapWords.add("dref", State.Keyword)
        mapWords.add("retval", State.Keyword)
        mapWords.add("try", State.Keyword)
        mapWords.add("catch", State.Keyword)
        mapWords.add("assume", State.Keyword)
        mapWords.add("throw", State.Keyword)
        mapWords.add("discard", State.Keyword)

        mapWords.add("enum", State.Declaration)
        mapWords.add("struct", State.Declaration)
        mapWords.add("union", State.Declaration)
        mapWords.add("impl", State.Declaration)
        mapWords.add("interface", State.Declaration)
        mapWords.add("func", State.Declaration)
        mapWords.add("closure", State.Declaration)
        mapWords.add("mtd", State.Declaration)
        mapWords.add("mtdc", State.Declaration)
        mapWords.add("namespace", State.Declaration)
        mapWords.add("alias", State.Declaration)
        mapWords.add("attr", State.Declaration)

        mapWords.add("if", State.Control)
        mapWords.add("else", State.Control)
        mapWords.add("elif", State.Control)
        mapWords.add("for", State.Control)
        mapWords.add("while", State.Control)
        mapWords.add("switch", State.Control)
        mapWords.add("defer", State.Control)
        mapWords.add("errdefer", State.Control)
        mapWords.add("loop", State.Control)
        mapWords.add("visit", State.Control)
        mapWords.add("break", State.Control)
        mapWords.add("fallthrough", State.Control)
        mapWords.add("return", State.Control)
        mapWords.add("case", State.Control)
        mapWords.add("continue", State.Control)
        mapWords.add("default", State.Control)
        mapWords.add("scope", State.Control)
        mapWords.add("and", State.Control)
        mapWords.add("or", State.Control)
        mapWords.add("orelse", State.Control)
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
            case State.Comment:
                if c == "*"'rune and cn == "/"'rune
                {
                    seekNext()
                    seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Default:
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
                        currentState = State.Default
                    while seek != seekEnd
                        seekNext()
                    currentState = State.Default
                    continue
                }

                if c == "/"'rune and cn == "*"'rune
                {
                    currentState = State.Comment
                    seekNext()
                    seekNext()
                    continue
                }

                if c == "/"'rune and cn == c
                {
                    currentState = State.LineComment
                    seekNext()
                    seekNext()
                    continue
                }
            }

            seekNext()
        }

        if currentState == State.LineComment
            currentState = State.Default
    }
}