using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        res := r.grep("ggggabc")
        @assert(res == "abc")
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        res := r.grep("XXXaaaaXXX")
        @assert(res == "aaaa")
    }

    {
        var r: Parser.RegExp
        assume r.compile("(ab)*")

        cpt := 0
        res := "XXXabababXXXaabXXXabbb"
        end := @dataof(res) + @countof(res)
        loop
        {
            res = r.grep(res)
            if !res
                break
            //@print(res)
            newStart := @dataof(res) + @countof(res)
            res = @mkstring(newStart, end - newStart)
            cpt += 1
        }

        @assert(cpt == 3)
    }
}