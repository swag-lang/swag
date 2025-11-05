enum A { Z }
alias B = A

#assert(#typeof(A).rawType == s32)
#assert(#typeof(B).rawType == s32)
