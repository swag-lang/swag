#global testerror "Err0020"
#test
{
    var x = 5
    let y = #ref &x
    y = #ref &x
}
