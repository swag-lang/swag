#import "core"
using core

#test
{
    v := String.from("this is a string")
    v.replace("string", "big string")
    @assert(v == "this is a big string")
    v.replace("big string", "string")
    @assert(v == "this is a string")
    v.replace("this", "it")
    @assert(v == "it is a string")
    v.replace("IT IS", "IT IS", utf8.ComparisonType.Latin1NoCase)
    @assert(v == "IT IS a string")
}