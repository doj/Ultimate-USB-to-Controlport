EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "lun. 30 mars 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 4200 2550 1    60   ~ 0
IOREF
Text Label 3750 3600 0    60   ~ 0
A0
Text Label 3750 3700 0    60   ~ 0
A1
Text Label 3750 3800 0    60   ~ 0
A2
Text Label 3750 3900 0    60   ~ 0
A3
Text Label 3750 4100 0    60   ~ 0
A5(SCL)
Text Label 5400 4100 0    60   ~ 0
0(Rx)
Text Label 5400 3900 0    60   ~ 0
2
Text Label 5400 4000 0    60   ~ 0
1(Tx)
Text Label 5400 3800 0    60   ~ 0
3(**)
Text Label 5400 3700 0    60   ~ 0
4
Text Label 5400 3600 0    60   ~ 0
5(**)
Text Label 5400 3500 0    60   ~ 0
6(**)
Text Label 5400 3400 0    60   ~ 0
7
Text Label 5400 3200 0    60   ~ 0
8
Text Label 5400 3100 0    60   ~ 0
9(**)
Text Label 5400 3000 0    60   ~ 0
10(**/SS)
Text Label 5400 2900 0    60   ~ 0
11(**/MOSI)
Text Label 5400 2800 0    60   ~ 0
12(MISO)
Text Label 5400 2700 0    60   ~ 0
13(SCK)
Text Label 5400 2500 0    60   ~ 0
AREF
NoConn ~ 4250 2700
Text Label 5400 2400 0    60   ~ 0
A4(SDA)
Text Label 5400 2300 0    60   ~ 0
A5(SCL)
$Comp
L Connector_Generic:Conn_01x08 P1
U 1 1 56D70129
P 4450 3000
F 0 "P1" H 4450 3450 50  0000 C CNN
F 1 "Power" V 4550 3000 50  0000 C CNN
F 2 "Socket_Arduino_Uno:Socket_Strip_Arduino_1x08" V 4600 3000 20  0000 C CNN
F 3 "" H 4450 3000 50  0000 C CNN
	1    4450 3000
	1    0    0    -1  
$EndComp
Text Label 3500 2900 0    60   ~ 0
Reset
$Comp
L power:GND #PWR03
U 1 1 56D70CC2
P 4150 4750
F 0 "#PWR03" H 4150 4500 50  0001 C CNN
F 1 "GND" H 4150 4600 50  0000 C CNN
F 2 "" H 4150 4750 50  0000 C CNN
F 3 "" H 4150 4750 50  0000 C CNN
	1    4150 4750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 P2
U 1 1 56D70DD8
P 4450 3800
F 0 "P2" H 4450 3400 50  0000 C CNN
F 1 "Analog" V 4550 3800 50  0000 C CNN
F 2 "Socket_Arduino_Uno:Socket_Strip_Arduino_1x06" V 4600 3850 20  0000 C CNN
F 3 "" H 4450 3800 50  0000 C CNN
	1    4450 3800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 P4
U 1 1 56D7164F
P 4850 3700
F 0 "P4" H 4850 3200 50  0000 C CNN
F 1 "Digital" V 4950 3700 50  0000 C CNN
F 2 "Socket_Arduino_Uno:Socket_Strip_Arduino_1x08" V 5000 3650 20  0000 C CNN
F 3 "" H 4850 3700 50  0000 C CNN
	1    4850 3700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4200 2550 4200 2800
Wire Wire Line
	4200 2800 4250 2800
Wire Wire Line
	4250 3200 4150 3200
Wire Wire Line
	4250 3300 4150 3300
Connection ~ 4150 3300
Wire Wire Line
	4250 4100 3750 4100
$Comp
L Connector_Generic:Conn_01x10 P3
U 1 1 56D721E0
P 4850 2700
F 0 "P3" H 4850 3250 50  0000 C CNN
F 1 "Digital" V 4950 2700 50  0000 C CNN
F 2 "Socket_Arduino_Uno:Socket_Strip_Arduino_1x10" V 5000 2700 20  0000 C CNN
F 3 "" H 4850 2700 50  0000 C CNN
	1    4850 2700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5050 3100 5400 3100
Wire Wire Line
	5050 3000 5400 3000
Wire Wire Line
	5050 2900 5400 2900
Wire Wire Line
	5050 2800 5400 2800
Wire Wire Line
	5050 2700 5400 2700
Wire Wire Line
	5050 2500 5400 2500
Wire Wire Line
	5050 2400 5400 2400
Wire Wire Line
	5050 2300 5400 2300
Wire Wire Line
	5050 4100 5400 4100
Wire Wire Line
	5050 2600 5150 2600
Wire Wire Line
	5150 2600 5150 3250
Wire Wire Line
	4150 3200 4150 3300
Wire Wire Line
	4150 3300 4150 4200
Wire Wire Line
	4250 2900 3500 2900
Text Notes 4550 2700 0    60   ~ 0
1
$Comp
L control-port:Commodore_Control_Port port1
U 1 1 5F5BE940
P 2200 3950
F 0 "port1" H 2285 3258 50  0000 C CNN
F 1 "Commodore_Control_Port" H 2285 3349 50  0000 C CNN
F 2 "Connector_Dsub:DSUB-9_Female_Horizontal_P2.77x2.84mm_EdgePinOffset4.94mm_Housed_MountingHolesOffset7.48mm" H 2200 3950 50  0001 C CNN
F 3 "" H 2200 3950 50  0001 C CNN
	1    2200 3950
	-1   0    0    1   
$EndComp
$Comp
L control-port:Commodore_Control_Port port2
U 1 1 5F5BF6ED
P 7300 3300
F 0 "port2" H 7814 3346 50  0000 L CNN
F 1 "Commodore_Control_Port" H 7814 3255 50  0000 L CNN
F 2 "Connector_Dsub:DSUB-9_Female_Horizontal_P2.77x2.84mm_EdgePinOffset4.94mm_Housed_MountingHolesOffset7.48mm" H 7300 3300 50  0001 C CNN
F 3 "" H 7300 3300 50  0001 C CNN
	1    7300 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 3600 6200 3500
Wire Wire Line
	5050 3600 6200 3600
Wire Wire Line
	6100 3500 6100 3300
Wire Wire Line
	5050 3500 6100 3500
Wire Wire Line
	6000 3400 6000 3100
Wire Wire Line
	5050 3400 6000 3400
Wire Wire Line
	5900 3200 5900 3650
Wire Wire Line
	5900 3650 6350 3650
Wire Wire Line
	6350 3650 6350 3600
Wire Wire Line
	5050 3200 5900 3200
Wire Wire Line
	5850 3800 5850 3000
Wire Wire Line
	5050 3800 5850 3800
Wire Wire Line
	5950 4000 5950 2900
Wire Wire Line
	5050 4000 5950 4000
Wire Wire Line
	6050 3200 6050 3250
Wire Wire Line
	6050 3250 5150 3250
Connection ~ 5150 3250
$Comp
L Device:LED D1
U 1 1 5F5F196B
P 6250 3950
F 0 "D1" V 6289 3832 50  0000 R CNN
F 1 "LED" V 6198 3832 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm" H 6250 3950 50  0001 C CNN
F 3 "~" H 6250 3950 50  0001 C CNN
	1    6250 3950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 3400 6250 3800
$Comp
L Device:R R1
U 1 1 5F5F4CBB
P 5900 4300
F 0 "R1" V 6107 4300 50  0000 C CNN
F 1 "470" V 6016 4300 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5830 4300 50  0001 C CNN
F 3 "~" H 5900 4300 50  0001 C CNN
	1    5900 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 4100 6250 4300
Wire Wire Line
	6250 4300 6050 4300
Wire Wire Line
	5750 4300 5150 4300
Wire Wire Line
	5150 3250 5150 4300
Wire Wire Line
	3700 3550 3700 3600
Wire Wire Line
	3700 3600 4250 3600
Wire Wire Line
	3450 3750 3450 3700
Wire Wire Line
	3450 3700 4250 3700
Wire Wire Line
	3500 3950 3500 3800
Wire Wire Line
	3500 3800 4250 3800
Wire Wire Line
	3550 4150 3550 3900
Wire Wire Line
	3550 3900 4250 3900
Wire Wire Line
	3600 3650 3600 4000
Wire Wire Line
	3600 4000 4250 4000
Wire Wire Line
	3750 4350 3750 4100
Wire Wire Line
	3500 4050 3500 4200
Wire Wire Line
	3500 4200 4150 4200
Wire Wire Line
	5700 4250 5700 3900
Wire Wire Line
	5050 3900 5700 3900
$Comp
L Device:LED D2
U 1 1 5F624294
P 3350 4650
F 0 "D2" H 3343 4867 50  0000 C CNN
F 1 "LED" H 3343 4776 50  0000 C CNN
F 2 "LED_THT:LED_D5.0mm" H 3350 4650 50  0001 C CNN
F 3 "~" H 3350 4650 50  0001 C CNN
	1    3350 4650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3150 3850 3150 4650
Wire Wire Line
	3150 4650 3200 4650
$Comp
L Device:R R2
U 1 1 5F62D7DA
P 3800 4650
F 0 "R2" V 3593 4650 50  0000 C CNN
F 1 "470" V 3684 4650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3730 4650 50  0001 C CNN
F 3 "~" H 3800 4650 50  0001 C CNN
	1    3800 4650
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 4650 3650 4650
Wire Wire Line
	3950 4650 4150 4650
Wire Wire Line
	4150 4650 4150 4750
$Comp
L Device:R R9
U 1 1 5F638691
P 2850 3550
F 0 "R9" V 2643 3550 50  0000 C CNN
F 1 "220" V 2734 3550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 3550 50  0001 C CNN
F 3 "~" H 2850 3550 50  0001 C CNN
	1    2850 3550
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5F63982E
P 2850 3650
F 0 "R8" V 2643 3650 50  0000 C CNN
F 1 "220" V 2734 3650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 3650 50  0001 C CNN
F 3 "~" H 2850 3650 50  0001 C CNN
	1    2850 3650
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 5F639EC4
P 2850 3750
F 0 "R7" V 2643 3750 50  0000 C CNN
F 1 "220" V 2734 3750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 3750 50  0001 C CNN
F 3 "~" H 2850 3750 50  0001 C CNN
	1    2850 3750
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 5F63A540
P 2850 3950
F 0 "R6" V 2643 3950 50  0000 C CNN
F 1 "220" V 2734 3950 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 3950 50  0001 C CNN
F 3 "~" H 2850 3950 50  0001 C CNN
	1    2850 3950
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5F63A86F
P 2850 4150
F 0 "R5" V 2643 4150 50  0000 C CNN
F 1 "220" V 2734 4150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 4150 50  0001 C CNN
F 3 "~" H 2850 4150 50  0001 C CNN
	1    2850 4150
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5F63AB6F
P 2850 4250
F 0 "R4" V 2643 4250 50  0000 C CNN
F 1 "220" V 2734 4250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 4250 50  0001 C CNN
F 3 "~" H 2850 4250 50  0001 C CNN
	1    2850 4250
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 5F63AE7F
P 2850 4350
F 0 "R3" V 2643 4350 50  0000 C CNN
F 1 "220" V 2734 4350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2780 4350 50  0001 C CNN
F 3 "~" H 2850 4350 50  0001 C CNN
	1    2850 4350
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 3550 2700 3550
Wire Wire Line
	2500 3650 2700 3650
Wire Wire Line
	2500 3750 2700 3750
Wire Wire Line
	2500 3950 2700 3950
Wire Wire Line
	2500 4150 2700 4150
Wire Wire Line
	2500 4250 2700 4250
Wire Wire Line
	2500 4350 2700 4350
Wire Wire Line
	3000 4350 3750 4350
Wire Wire Line
	3000 4250 5700 4250
Wire Wire Line
	3000 4150 3550 4150
Wire Wire Line
	3000 3950 3500 3950
Wire Wire Line
	3000 3750 3450 3750
Wire Wire Line
	3000 3650 3600 3650
Wire Wire Line
	3000 3550 3700 3550
Wire Wire Line
	2500 3850 3150 3850
Wire Wire Line
	2500 4050 3500 4050
$Comp
L Device:R R10
U 1 1 5F6746ED
P 6700 3700
F 0 "R10" V 6493 3700 50  0000 C CNN
F 1 "220" V 6584 3700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3700 50  0001 C CNN
F 3 "~" H 6700 3700 50  0001 C CNN
	1    6700 3700
	0    1    1    0   
$EndComp
$Comp
L Device:R R11
U 1 1 5F67CE5C
P 6700 3600
F 0 "R11" V 6493 3600 50  0000 C CNN
F 1 "220" V 6584 3600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3600 50  0001 C CNN
F 3 "~" H 6700 3600 50  0001 C CNN
	1    6700 3600
	0    1    1    0   
$EndComp
$Comp
L Device:R R12
U 1 1 5F67FDE4
P 6700 3500
F 0 "R12" V 6493 3500 50  0000 C CNN
F 1 "220" V 6584 3500 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3500 50  0001 C CNN
F 3 "~" H 6700 3500 50  0001 C CNN
	1    6700 3500
	0    1    1    0   
$EndComp
$Comp
L Device:R R13
U 1 1 5F682D6C
P 6700 3300
F 0 "R13" V 6493 3300 50  0000 C CNN
F 1 "220" V 6584 3300 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3300 50  0001 C CNN
F 3 "~" H 6700 3300 50  0001 C CNN
	1    6700 3300
	0    1    1    0   
$EndComp
$Comp
L Device:R R14
U 1 1 5F686130
P 6700 3100
F 0 "R14" V 6493 3100 50  0000 C CNN
F 1 "220" V 6584 3100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3100 50  0001 C CNN
F 3 "~" H 6700 3100 50  0001 C CNN
	1    6700 3100
	0    1    1    0   
$EndComp
$Comp
L Device:R R15
U 1 1 5F6890B8
P 6700 3000
F 0 "R15" V 6493 3000 50  0000 C CNN
F 1 "220" V 6584 3000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 3000 50  0001 C CNN
F 3 "~" H 6700 3000 50  0001 C CNN
	1    6700 3000
	0    1    1    0   
$EndComp
$Comp
L Device:R R16
U 1 1 5F68C040
P 6700 2900
F 0 "R16" V 6493 2900 50  0000 C CNN
F 1 "220" V 6584 2900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6630 2900 50  0001 C CNN
F 3 "~" H 6700 2900 50  0001 C CNN
	1    6700 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 3700 6550 3700
Wire Wire Line
	6350 3600 6550 3600
Wire Wire Line
	6200 3500 6550 3500
Wire Wire Line
	6100 3300 6550 3300
Wire Wire Line
	6000 3100 6550 3100
Wire Wire Line
	5850 3000 6550 3000
Wire Wire Line
	5950 2900 6550 2900
Wire Wire Line
	6050 3200 7000 3200
Wire Wire Line
	6250 3400 7000 3400
Wire Wire Line
	6850 3700 7000 3700
Wire Wire Line
	6850 3600 7000 3600
Wire Wire Line
	6850 3500 7000 3500
Wire Wire Line
	6850 3300 7000 3300
Wire Wire Line
	6850 3100 7000 3100
Wire Wire Line
	6850 3000 7000 3000
Wire Wire Line
	6850 2900 7000 2900
NoConn ~ 5400 2400
NoConn ~ 5400 2500
NoConn ~ 5400 2700
NoConn ~ 5400 2800
NoConn ~ 5400 2900
NoConn ~ 5400 3000
NoConn ~ 5400 3100
NoConn ~ 5400 4100
NoConn ~ 4200 2550
NoConn ~ 4250 3000
NoConn ~ 4250 3100
Wire Wire Line
	4150 4650 4150 4300
Connection ~ 4150 4650
Connection ~ 4150 4200
Wire Wire Line
	5150 4300 4150 4300
Connection ~ 5150 4300
Connection ~ 4150 4300
Wire Wire Line
	4150 4300 4150 4200
NoConn ~ 4250 3400
$Comp
L Switch:SW_Push reset
U 1 1 5F75A243
P 3500 3100
F 0 "reset" V 3546 3052 50  0000 R CNN
F 1 "SW_Push" V 3455 3052 50  0000 R CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 3500 3300 50  0001 C CNN
F 3 "~" H 3500 3300 50  0001 C CNN
	1    3500 3100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3500 3300 4150 3300
$EndSCHEMATC
