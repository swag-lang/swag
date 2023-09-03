#global testerror "invalid type s32"
func(T) AA()
{
    #if @typeof(T) != f32 #error "invalid type " ++ @typeof(T).fullname
}

#test
{
    AA's32()
}