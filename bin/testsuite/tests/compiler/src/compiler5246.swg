#global testerror "Err0101"
#test
{
    var x = 5
    let y =,constref &x
    y =,ref &x
}