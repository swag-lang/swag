const A = #"\titi"# ++
          #"\tata"# ++
          "toto" ++
          """tonton"""

#test
{
    @assert(A == #"\titi\tatatototonton"#)
}
