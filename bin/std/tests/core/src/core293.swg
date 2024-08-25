using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        let res = r.grep("ggggabc")
        @assert(res == "abc")
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        let res = r.grep("XXXaaaaXXX")
        @assert(res == "aaaa")
    }

    {
        var r: Parser.RegExp
        assume r.compile("(ab)*")

        var cpt = 0
        var res = "XXXabababXXXaabXXXabbb"
        let end = @dataof(res) + @countof(res)
        for
        {
            let val = r.grep(res)
            if !val:
                break
            let newStart = @dataof(val) + @countof(val)
            res = @mkstring(newStart, end - newStart)
            cpt += 1
        }

        @assert(cpt == 3)
    }
}
