EESchema Schematic File Version 4
LIBS:betsieboost-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Betsie Boost"
Date "2019-07-16"
Rev "1"
Comp "Mark Robson"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR01
U 1 1 5D273AA1
P 5150 4400
F 0 "#PWR01" H 5150 4150 50  0001 C CNN
F 1 "GND" H 5155 4227 50  0000 C CNN
F 2 "" H 5150 4400 50  0001 C CNN
F 3 "" H 5150 4400 50  0001 C CNN
	1    5150 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 3850 3000 3850
Wire Wire Line
	3750 3850 3750 4400
Wire Wire Line
	3750 4400 4450 4400
Connection ~ 5150 4400
$Comp
L Device:CP C1
U 1 1 5D273FB3
P 4450 4050
F 0 "C1" H 4565 4096 50  0000 L CNN
F 1 "47uF" H 4565 4005 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-7343-30_AVX-N" H 4488 3900 50  0001 C CNN
F 3 "~" H 4450 4050 50  0001 C CNN
	1    4450 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3750 4450 3900
Wire Wire Line
	4450 3750 3900 3750
Wire Wire Line
	4450 4200 4450 4400
Connection ~ 4450 4400
Wire Wire Line
	4450 4400 4800 4400
Connection ~ 4450 3750
$Comp
L Device:R R3
U 1 1 5D274F6F
P 4800 3750
F 0 "R3" V 4593 3750 50  0000 C CNN
F 1 "10k" V 4684 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4730 3750 50  0001 C CNN
F 3 "~" H 4800 3750 50  0001 C CNN
	1    4800 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 3750 4650 3750
$Comp
L Device:R R2
U 1 1 5D2777F6
P 6500 3650
F 0 "R2" H 6570 3696 50  0000 L CNN
F 1 "12k" H 6570 3605 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 6430 3650 50  0001 C CNN
F 3 "~" H 6500 3650 50  0001 C CNN
	1    6500 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5D2779BD
P 6500 4250
F 0 "R1" H 6570 4296 50  0000 L CNN
F 1 "1k" H 6570 4205 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 6430 4250 50  0001 C CNN
F 3 "~" H 6500 4250 50  0001 C CNN
	1    6500 4250
	1    0    0    -1  
$EndComp
Connection ~ 6500 4400
Connection ~ 5700 4400
Wire Wire Line
	6500 4400 5700 4400
Wire Wire Line
	5700 4400 5150 4400
$Comp
L Device:L L1
U 1 1 5D2727D5
P 6050 3150
F 0 "L1" V 6240 3150 50  0000 C CNN
F 1 "47uH" V 6149 3150 50  0000 C CNN
F 2 "Inductor_SMD:L_10.4x10.4_H4.8" H 6050 3150 50  0001 C CNN
F 3 "~" H 6050 3150 50  0001 C CNN
	1    6050 3150
	0    -1   -1   0   
$EndComp
$Comp
L betsieboost:XL6019E1 U1
U 1 1 5D270AFE
P 5700 3950
F 0 "U1" H 5700 4631 50  0000 C CNN
F 1 "XL6019E1" H 5700 4540 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TO-263-5_TabPin3" H 5700 3950 50  0001 C CNN
F 3 "" H 5700 3950 50  0001 C CNN
	1    5700 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3450 5700 3450
Wire Wire Line
	4450 3450 4450 3750
Wire Wire Line
	5900 3150 4450 3150
Wire Wire Line
	4450 3150 4450 3450
Connection ~ 4450 3450
Wire Wire Line
	6200 3150 6400 3150
Wire Wire Line
	6200 4100 6400 4100
Wire Wire Line
	6400 4100 6400 3150
Connection ~ 6400 3150
Wire Wire Line
	6400 3150 6650 3150
Wire Wire Line
	7250 3450 6500 3450
Wire Wire Line
	6500 3450 6500 3500
Wire Wire Line
	6500 3800 6500 3850
Wire Wire Line
	6200 3850 6500 3850
Connection ~ 6500 3850
Wire Wire Line
	6500 3850 6500 4100
Wire Wire Line
	7250 3150 7250 3450
Text Notes 5500 4850 0    50   ~ 0
This chip tries to make VFB 1.25\nVOUT=1.25 * (1+R2/R1)
Text Label 2850 3750 0    50   ~ 0
VIN
Text Label 7100 3150 0    50   ~ 0
VOUT
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5D27E645
P 4800 4400
F 0 "#FLG0101" H 4800 4475 50  0001 C CNN
F 1 "PWR_FLAG" H 4800 4573 50  0000 C CNN
F 2 "" H 4800 4400 50  0001 C CNN
F 3 "~" H 4800 4400 50  0001 C CNN
	1    4800 4400
	-1   0    0    1   
$EndComp
Connection ~ 4800 4400
Wire Wire Line
	4800 4400 5150 4400
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5D27EDC1
P 3900 3750
F 0 "#FLG0102" H 3900 3825 50  0001 C CNN
F 1 "PWR_FLAG" H 3900 3923 50  0000 C CNN
F 2 "" H 3900 3750 50  0001 C CNN
F 3 "~" H 3900 3750 50  0001 C CNN
	1    3900 3750
	1    0    0    -1  
$EndComp
Connection ~ 3900 3750
Wire Wire Line
	3900 3750 3050 3750
$Comp
L Connector:Conn_01x05_Male J1
U 1 1 5D27F080
P 2300 3850
F 0 "J1" H 2300 4150 50  0000 C CNN
F 1 "Conn_01x03_Male" H 1900 3900 50  0000 C CNN
F 2 "betsieboost:PinHeader_1x05_P2.54mm_BIG1" H 2300 3850 50  0001 C CNN
F 3 "~" H 2300 3850 50  0001 C CNN
	1    2300 3850
	1    0    0    -1  
$EndComp
Text Label 3000 4050 0    50   ~ 0
VOUT
$Comp
L Device:CP C2
U 1 1 5D2812F2
P 7250 3850
F 0 "C2" H 7365 3896 50  0000 L CNN
F 1 "47uF" H 7365 3805 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-7343-30_AVX-N" H 7288 3700 50  0001 C CNN
F 3 "~" H 7250 3850 50  0001 C CNN
	1    7250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 3450 7250 3700
Connection ~ 7250 3450
Wire Wire Line
	7250 4000 7250 4400
Wire Wire Line
	7250 4400 6500 4400
Text Label 6400 3350 0    50   ~ 0
SW
Text Label 6200 3850 0    50   ~ 0
FB
Wire Wire Line
	5200 3750 4950 3750
Text Label 5050 3750 0    50   ~ 0
EN
$Comp
L Device:D_Schottky D1
U 1 1 5D290329
P 6800 3300
F 0 "D1" H 6800 3084 50  0000 C CNN
F 1 "SS420C" H 6800 3175 50  0000 C CNN
F 2 "Diode_SMD:D_2816_7142Metric" H 6800 3300 50  0001 C CNN
F 3 "~" H 6800 3300 50  0001 C CNN
	1    6800 3300
	-1   0    0    1   
$EndComp
Wire Wire Line
	7050 3150 7250 3150
Wire Wire Line
	2500 4050 3400 4050
Wire Wire Line
	2500 3950 3000 3950
Wire Wire Line
	3000 3950 3000 3850
Connection ~ 3000 3850
Wire Wire Line
	3000 3850 3750 3850
Wire Wire Line
	2500 3650 3050 3650
Wire Wire Line
	3050 3650 3050 3750
Connection ~ 3050 3750
Wire Wire Line
	3050 3750 2500 3750
$Comp
L Device:D_Schottky D2
U 1 1 5D2C77C8
P 6800 2950
F 0 "D2" H 6800 2734 50  0000 C CNN
F 1 "SS420C" H 6800 2825 50  0000 C CNN
F 2 "Diode_SMD:D_2816_7142Metric" H 6800 2950 50  0001 C CNN
F 3 "~" H 6800 2950 50  0001 C CNN
	1    6800 2950
	-1   0    0    1   
$EndComp
Wire Wire Line
	6650 2950 6650 3150
Wire Wire Line
	6650 3150 6650 3300
Connection ~ 6650 3150
Wire Wire Line
	6950 2950 7050 2950
Wire Wire Line
	7050 2950 7050 3150
Wire Wire Line
	6950 3300 7050 3300
Wire Wire Line
	7050 3300 7050 3150
Connection ~ 7050 3150
$EndSCHEMATC
