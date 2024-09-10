#global testerror "Err0498"
alias A = {item1, y: s32}

#test
{
    var a, b: s32
    var c: A = {1, 2}

    (a, b) = c
}