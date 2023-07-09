using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        var res = r.grep("ggggabc")
        @assert(res == "abc")
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        var res = r.grep("XXXaaaaXXX")
        @assert(res == "aaaa")
    }

    {
        var r: Parser.RegExp
        assume r.compile("(ab)*")

        var cpt = 0
        var res = "XXXabababXXXaabXXXabbb"
        var end = @dataof(res) + @countof(res)
        loop
        {
            res = r.grep(res)
            if !res
                break
            //@print(res)
            var newStart = @dataof(res) + @countof(res)
            res = @mkstring(newStart, end - newStart)
            cpt += 1
        }

        @assert(cpt == 3)
    }
}