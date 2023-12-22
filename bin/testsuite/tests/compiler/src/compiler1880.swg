#global testerror "invalid type s32"
func(T) AA()
{
    #if @typeof(T) != f32 #do
        #error "invalid type " ++ @typeof(T).fullname ++ "$ hint"
}

#test
{
    AA's32()
}