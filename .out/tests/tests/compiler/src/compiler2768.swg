const AA = @[[[1],  [2]],
            [[30],  [40]],
            [[300], [400]]]

#assert AA[2,1,0] == 400
#assert AA[1,1,0] == 40
#assert AA[0,0,0] == 1
#assert AA[0,1,0] == 2
