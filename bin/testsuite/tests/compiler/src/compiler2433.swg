func append()
    #selectif
    {
        @assert(@stringof(#self) == "__compiler2433.append")
        return true
    }
{
}

#run
{
    append()
}