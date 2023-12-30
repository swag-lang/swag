using Core
#test
{
     var r = assume Parser.RegExp.create(#"([a-z])\1"#)
     @assert(r.match("aa"))
     @assert(!r.match("ab"))
}