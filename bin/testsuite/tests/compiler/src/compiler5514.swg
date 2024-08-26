#global testerror "Err0251"

#test
{
    for i in 1'u64 to 0'u64 
    {       
        @print(@typeof(i).name) 
    }
}