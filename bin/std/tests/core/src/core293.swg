using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        let res = r.grep("ggggabc")
        @assert(res.value == "abc")
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        let res = r.grep("XXXaaaaXXX")
        @assert(res.value == "aaaa")
    }

    {
        var r: Parser.RegExp
        assume r.compile("(ab)*")

        var cpt = 0
        var res = "XXXabababXXXaabXXXabbb"
        let end = @dataof(res) + @countof(res)
        loop
        {
            let val = r.grep(res)
            if !val.value do
                break
            let newStart = @dataof(val.value) + @countof(val.value)
            res = @mkstring(newStart, end - newStart)
            cpt += 1
        }

        @assert(cpt == 3)
    }
}