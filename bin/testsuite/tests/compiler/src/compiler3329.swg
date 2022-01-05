#global skip
func toto(values: s32...)
{
    v := 0
    visit v: values
        @print(v)
}