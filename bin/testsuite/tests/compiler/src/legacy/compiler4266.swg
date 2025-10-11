func isThisRelease()->bool
{
    return true
}

#if #run isThisRelease() == false
{
    #warning("this should not be called !")
}
