#global testerror "Err0299"

#test
{
    var str   = "string"
    var tuple = {@dataof(str), 2}
    var ptr   = @dataof(tuple)
}
