#
# here are some shell commands for general testing,
# neutral point is about 1450. Left motor is 24, right is 23.
# left motor is reversed, right is not.
#
alias zeroff='pigs s 23 0 s 24 0'
alias off='pigs s 23 1450 s 24 1450'
alias fwd='echo "s 23 1600 s 24 1350" > /dev/pigpio'
alias rev='echo "s 23 1350 s 24 1550" > /dev/pigpio'
alias tleft='echo "s 23 1600 s 24 1550" > /dev/pigpio'
alias fast='echo "s 23 2000 s 24 1000" > /dev/pigpio'
alias fastrev='echo "s 23 1300 s 24 1700" > /dev/pigpio'
alias



