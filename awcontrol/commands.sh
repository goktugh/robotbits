# GPIO18 - PWM LEFT
# GPIO23, 24 - Direction left

# GPIO 7 - PWM RIGHT
# GPIO 8,25 = Direction Right

L_PWM=18
L_0=24
L_1=23
R_PWM=7
R_0=8
R_1=25

function off {
    pigs w $L_PWM 0 w $R_PWM 0
    pigs w $L_0 1 w $L_1 0
    pigs w $R_0 1 w $R_1 0
    sleep 0.25
    pigs w $L_0 0 w $R_0 0

}

function pwms_on {
    pigs w $L_PWM 1 w $R_PWM 1

}

function fwd {
    pwms_on
    pigs w $L_0 0 w $L_1 1
    pigs w $R_0 0 w $R_1 1
}

function fwd_slow {
    pwms_on
    pigs w $L_0 0 p $L_1 92
    pigs w $R_0 0 p $R_1 92

}
function rev_slow {
    pwms_on
    pigs p $L_0 92 w $L_1 0
    pigs p $R_0 92 w $R_1 0

}

function spin_left {
    pwms_on
    pigs p $L_0 56 w $L_1 0
    pigs w $R_0 0 p $R_1 56


}
function off2 {
    pwms_on
    pigs w $L_0 0 w $L_1 0
    pigs w $R_0 0 w $R_1 0
}

