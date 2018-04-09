We can it seems, reasonably rely on 

dead zone + 400us is the maximum.

So if we assume:

Centre of the dead zone, is "centre stick" or about 1450us

Edge of the dead zone + 400us is the limit,

So if we want to drive at N%, then we either drive at 1450us (if abs(N) < ~ 2.5)
OR

dead_zone + (N / 100 * 400)


