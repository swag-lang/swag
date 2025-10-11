const X1 = """\
A\
    B\
  C\
"""

const X2 = """
A
    B
  C
"""

const X3 = """ABC"""
const X4 = """ABC("A")"""

const X5 = #"\
  A
  B
  C
  "#

const X6 = #"\
A\
B\
C\
"#

const X7 = """this is
              a
              string
              """

#test
{
    @assert(X1 == "A    B  C")
    @assert(X2[0] != 'A''u8)
    @assert(X3 == "ABC")
    @assert(X4 == "ABC(\"A\")")
    @assert(X5[0] == 'A''u8)
    @assert(X6 == """ABC""")
    @assert(X7[0 to 6] == "this is")
}
