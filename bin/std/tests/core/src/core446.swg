using Core
#test
{
    {
         var r = assume Parser.RegExp.create(#"X.X"#)
         @assert(r.match("X.X"))
    }

    {
         var r = assume Parser.RegExp.create(#"X\s*.*\s*X"#)
         @assert(r.match("X011X"))
         @assert(r.match("X  011  X"))
    }

    {
         var r = assume Parser.RegExp.create(#"<(\w+)>\s*.*\s*<\/aa>"#)
         @assert(r.match("<aa>toto</aa>"))
    }
}