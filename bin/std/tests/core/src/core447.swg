using Core
#test
{
    {
         var r = assume Parser.RegExp.create(#".*XY"#)
         //@assert(r.match("tXY"))
    }
}