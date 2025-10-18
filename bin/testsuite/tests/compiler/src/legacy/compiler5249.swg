#global testerror "Err0017"
#test
{
    var x = 5
    let y = #ref &x
    y = #ref &x
}
