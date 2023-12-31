using Core
#test
{
    {
         var r = assume Parser.RegExp.create(#".*XY"#)
         @assert(r.match("tXY"))
    }

    {
         var r = assume Parser.RegExp.create(#"<(\w+)>\s*.*\s*<\/aa>"#)
         @assert(r.match("<aa>toto</aa>"))
    }
}