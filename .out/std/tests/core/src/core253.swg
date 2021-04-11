using core, strconv

struct atofTest
{
	in:     string
	out:    string
	error:  bool
}

var atoftests: [] atofTest = @[
	{"1", "1", false},
	{"1e23", "1e+23", false},
	{"1E23", "1e+23", false},
    //{"100000000000000000000000", "1e+23", false},
]

#test
{
    var cb: ConcatBuffer
    visit t: atoftests
    {
        v := catch atod(t.in)
        @assert(!@err || t.error)
        cb.clear()
        dtoa(&cb, v.value, "g"'u8, -1, false)

        console.print("% %\n", t.out, cb.toString())
    }
}