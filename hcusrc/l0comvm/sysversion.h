/*
 * version.h
 *
 *  Created on: 2016年1月11日
 *      Author: test
 */

#ifndef L0COMVM_SYSVERSION_H_
#define L0COMVM_SYSVERSION_H_

//硬件版本定义及状态
#define CURRENT_HW_TYPE 2 // 1 = WEMC, 2 = HCU, 3 = WIFI_EMC
#define CURRENT_HW_MODULE 3 // 0 = NULL, 1 = Samsung 6410, 2 = FSL, 3 = Feilinx 335D, 4 = RaspberryPi 3B

//SW version control, for internal usage
#define CURRENT_SW_RELEASE 1 //R01 = 0x01 (XQ.HCU.SW.R01.456)


#define CURRENT_SW_DELIVERY 128 //001 = 0x01 (XQ.HCU.SW.R01.456), starting from 100 as 2015/11/02


//= ZJL, 2015 Nov.1, CURRENT_SW_DELIVERY 1
//= ZJL, 2015 Nov.3, CURRENT_SW_DELIVERY 2
//= ZJL, 2015 Nov.4, CURRENT_SW_DELIVERY 3
//= ZJL, 2015 Nov.9, CURRENT_SW_DELIVERY 4
//= ZJL, 2015 Nov.10, CURRENT_SW_DELIVERY 5
//= ZJL, 2015 Nov.12, CURRENT_SW_DELIVERY 6
//= ZJL, 2015 Nov.13, CURRENT_SW_DELIVERY 7
//= ZJL, 2015 Nov.15, CURRENT_SW_DELIVERY 8
//= ZJL, 2015 Nov.16, CURRENT_SW_DELIVERY 9
//= ZJL, 2015 Nov.17, CURRENT_SW_DELIVERY 10
//= ZJL, 2015 Nov.18, CURRENT_SW_DELIVERY 11
//= ZJL, 2015 Nov.19, CURRENT_SW_DELIVERY 12
//= ZJL, 2015 Nov.20, CURRENT_SW_DELIVERY 13
//= ZJL, 2015 Nov.21, CURRENT_SW_DELIVERY 14
//= MYC, 2015 Nov.22, CURRENT_SW_DELIVERY 15
//= ZJL, 2015 Nov.23, CURRENT_SW_DELIVERY 16
//= ZJL, 2015 Nov.24, CURRENT_SW_DELIVERY 17
//= ZJL, 2015 Nov.25, CURRENT_SW_DELIVERY 18
//= ZJL, 2015 Dec.1, CURRENT_SW_DELIVERY 23
//= ZJL, 2015 Dec.2, CURRENT_SW_DELIVERY 24
//= MYC, 2015 Dec.5, CURRENT_SW_DELIVERY 25
//= ZJL, 2015 Dec.7, CURRENT_SW_DELIVERY 26
//= MYC, 2015 Dec.7, CURRENT_SW_DELIVERY 27
//= ZJL, 2015 Dec.8, CURRENT_SW_DELIVERY 28
//= ZJL, 2015 Dec.9, CURRENT_SW_DELIVERY 29
//= ZJL, 2015 Dec.11, CURRENT_SW_DELIVERY 30
//= ZJL, 2015 Dec.15, CURRENT_SW_DELIVERY 31
//= ZJL, 2015 Dec.18, CURRENT_SW_DELIVERY 32
//= ZSC, 2015 Dec.19, CURRENT_SW_DELIVERY 33
//= ZJL, 2015 Dec.20, CURRENT_SW_DELIVERY 34
//= ZJL, 2015 Dec.21, CURRENT_SW_DELIVERY 35
//= ZJL, 2015 Dec.23, CURRENT_SW_DELIVERY 36
//= ZJL, 2015 Dec.24, CURRENT_SW_DELIVERY 37
//= ZJL, 2015 Dec.25, CURRENT_SW_DELIVERY 38
//= ZJL, 2015 Dec.27, CURRENT_SW_DELIVERY 39
//= ZJL, 2015 Dec.28, CURRENT_SW_DELIVERY 40
//= ZJL, 2015 Dec.28, CURRENT_SW_DELIVERY 41
//= ZJL, 2015 Dec.29, CURRENT_SW_DELIVERY 42
//= ZJL, 2015 Dec.30, CURRENT_SW_DELIVERY 43
//= ZSC, 2015 Dec.30, CURRENT_SW_DELIVERY 44
//= ZJL, 2015 Dec.31, CURRENT_SW_DELIVERY 45
//= ZJL, 2016 Jan.1, CURRENT_SW_DELIVERY 46
//= ZJL, 2016 Jan.3, CURRENT_SW_DELIVERY 47
//= ZJL, 2016 Jan.4, CURRENT_SW_DELIVERY 48
//= ZJL, 2016 Jan.5, CURRENT_SW_DELIVERY 49
//= ZJL, 2016 Jan.6, CURRENT_SW_DELIVERY 50
//= ZJL, 2016 Jan.7, CURRENT_SW_DELIVERY 51
//= ZJL, 2016 Jan.11, CURRENT_SW_DELIVERY 52
//= ZJL, 2016 Jan.12, CURRENT_SW_DELIVERY 53
//= ZJL, 2016 Jan.13, CURRENT_SW_DELIVERY 54
//= ZJL, 2016 Jan.14, CURRENT_SW_DELIVERY 55
//= ZJL, 2016 Jan.15, CURRENT_SW_DELIVERY 56
//= ZJL, 2016 Jan.16, CURRENT_SW_DELIVERY 57
//= ZJL, 2016 Jan.16, CURRENT_SW_DELIVERY 58
//= ZJL, 2016 Jan.19, CURRENT_SW_DELIVERY 59
//= ZSC, 2016 Jan.22, CURRENT_SW_DELIVERY 60
//= ZSC, 2016 Feb.6, CURRENT_SW_DELIVERY 61
//= ZJL, 2016 Feb.7, CURRENT_SW_DELIVERY 62
//= QL, 2016 Feb.17, CURRENT_SW_DELIVERY 63
//= ZSC, 2016 Feb.22, CURRENT_SW_DELIVERY 64
//= MYC, 2016 Mar.01, CURRENT_SW_DELIVERY 66
//= ZSC, 2016 Mar.8, CURRENT_SW_DELIVERY 67
//= ZSC, 2016 Mar.13, CURRENT_SW_DELIVERY 68
//= ZSC, 2016 Mar.16, CURRENT_SW_DELIVERY 69
//= ZSC, 2016 Mar.18 CURRENT_SW_DELIVERY 70
//= ZJL, 2016 Mar.21, CURRENT_SW_DELIVERY 71
//= ZJL, 2016 Mar.23, CURRENT_SW_DELIVERY 72
//= ZJL, 2016 Mar.24, CURRENT_SW_DELIVERY 73
//= ZSC, 2016 Apr.4, CURRENT_SW_DELIVERY 74
//= ZSC, 2016 Apr.15, CURRENT_SW_DELIVERY 75
//= ZSC, 2016 Apr.19, CURRENT_SW_DELIVERY 77
//= MYC, 2016 Apr.25, CURRENT_SW_DELIVERY 78 //No any source code change, only add RasberryPi project config (for Ubuntu MATE 15.x)
//= MYC, 2016 May.01, CURRENT_SW_DELIVERY 79 //No any source code change, only add RasberryPi project config (for Raspberry Jessie)
//= JT/ZSC/QL, 2016 May.07, CURRENT_SW_DELIVERY 79DEMO // In Pi: Connect to hcudb in mysql OK, add read PM25 from Serial UART0 for Raspberry Pi)
//= ZJL, 2016 May.10, CURRENT_SW_DELIVERY 80
//= ZJL, 2016 May.10, CURRENT_SW_DELIVERY 81
//= ZJL, 2016 May.11, CURRENT_SW_DELIVERY 82
//= ZSC, 2016 May.12, CURRENT_SW_DELIVERY 83
//= ZJL, 2016 May.14, CURRENT_SW_DELIVERY 84
//= ZJL, 2016 May.15, CURRENT_SW_DELIVERY 85
//= ZJL, 2016 May.17, CURRENT_SW_DELIVERY 86
//= ZSC, 2016 May.18, CURRENT_SW_DELIVERY 87
//= ZJL, 2016 May.18, CURRENT_SW_DELIVERY 88
//= ZSC, 2016 May.20, CURRENT_SW_DELIVERY 89
//= ZSC, 2016 May.23, CURRENT_SW_DELIVERY 90
//= ZJL, 2016 May.31, CURRENT_SW_DELIVERY 91
//= ZSC, 2016 May.31, CURRENT_SW_DELIVERY 92
//= ZJL, 2016 June.2, CURRENT_SW_DELIVERY 93
//= Multi People, 2016 June.11, CURRENT_SW_DELIVERY 94
//= ZJL, 2016 June.11, CURRENT_SW_DELIVERY 95
//= ZJL, 2016 June.15, CURRENT_SW_DELIVERY 96
//= ZJL, 2016 June.16, CURRENT_SW_DELIVERY 97
//= ZJL, 2016 June.16, CURRENT_SW_DELIVERY 98
//= ZJL, 2016 June.16, CURRENT_SW_DELIVERY 99
//= ZSC, 2016 July.15, CURRENT_SW_DELIVERY 100
//= ZJL, 2016 July.18, CURRENT_SW_DELIVERY 101
//= ZJL, 2016 July.21, CURRENT_SW_DELIVERY 102
//= ZJL, 2016 July.22, CURRENT_SW_DELIVERY 103
//= ZJL, 2016 July.23, CURRENT_SW_DELIVERY 104
//= ZJL, 2016 July.24, CURRENT_SW_DELIVERY 105
//= ZJL, 2016 July.24, CURRENT_SW_DELIVERY 106
//= ZJL, 2016 July.25, CURRENT_SW_DELIVERY 107
//= ZSC, 2016 July.30, CURRENT_SW_DELIVERY 108
//= ZSC, 2016 Aug.26, CURRENT_SW_DELIVERY 109
//= ZSC, 2016 SEP.20, CURRENT_SW_DELIVERY 110
//= ZSC, 2016 OCT.20, CURRENT_SW_DELIVERY 111
//= ZSC, 2016 OCT.28, CURRENT_SW_DELIVERY 112
//= ZSC, 2016 NOV.4, CURRENT_SW_DELIVERY 113
//= ZSC, 2016 NOV.6, CURRENT_SW_DELIVERY 114 =>AQYC项目
//= ZJL, 2016 NOV.13, CURRENT_SW_DELIVERY 115 =>BFSC项目
//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 116 =>BFSC项目
//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 117 =>BFSC项目
//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 118 =>BFSC项目
//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 119 =>BFSC项目
//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 120 =>BFSC项目
//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 121 =>BFSC项目
//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 122 =>BFSC项目
//= ZJL, 2016 NOV.16, CURRENT_SW_DELIVERY 123 =>BFSC项目
//= ZJL, 2016 NOV.16, CURRENT_SW_DELIVERY 124 =>BFSC项目
//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 125 =>BFSC项目
//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 126 =>BFSC项目
//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 127 =>BFSC项目
//= ZSC/ZJL, 2016 NOV.30, CURRENT_SW_DELIVERY 128 =>BFSC项目/G2项目









#endif /* L0COMVM_SYSVERSION_H_ */
