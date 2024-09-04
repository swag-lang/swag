#global testerror "Err0090"
#test
{
    var x = 5
    let y = #ref &x
    y = #ref &x
}