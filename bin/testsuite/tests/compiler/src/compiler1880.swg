func(T) AA()
{
    #assert @typeof(T) != s32, "invalid type " ++ @typeof(T).fullname
}

#test
{
    AA'f32()
}