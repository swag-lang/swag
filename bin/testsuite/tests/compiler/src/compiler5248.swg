#global testerror "Err0104"
#test
{
    var x = 5
    let y = ref &x
    y = ref &x
}