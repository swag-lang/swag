using Core
#test
{
    {
         var r = assume Parser.RegExp.create(#".*XY"#)
         @assert(r.match("tXY"))
    }

    {
         var r = assume Parser.RegExp.create(#"<(\w+)>.*<<\1>>"#)
         @assert(r.match("<aa><<aa>>"))
    }
}