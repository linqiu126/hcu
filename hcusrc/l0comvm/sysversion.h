/*
 * version.h
 *
 *  Created on: 2016年1月11日
 *      Author: test
 */

#ifndef L0COMVM_SYSVERSION_H_
#define L0COMVM_SYSVERSION_H_

//硬件版本定义及状态
#define HCU_CURRENT_HW_PEM 3

//SW version control, for internal usage
#define HCU_CURRENT_SW_RELEASE 3 //R01 = 0x01 (XQ.HCU.SW.R01.456)
#define HCU_CURRENT_SW_DELIVERY 296   //001 = 0x01 (XQ.HCU.SW.R01.456), starting from 100 as 2015/11/02
#define HCU_CURRENT_DB_DELIVERY 200 //UPDATE once modified.
#define HCU_CURRENT_VM_DELIVERY 4  //表达LIB-HCUVM


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
//= ZJL, 2016 DEC.1, CURRENT_SW_DELIVERY 129 =>BFSC项目
//= ZJL, 2016 DEC.3, CURRENT_SW_DELIVERY 130 =>BFSC项目
//= ZSC, 2016 DEC.4, CURRENT_SW_DELIVERY 131 =>G20项目
//= ZSC, 2016 DEC.7, CURRENT_SW_DELIVERY 132 =>G20项目
//= ZSC, 2016 DEC.27, CURRENT_SW_DELIVERY 133 =>G20项目
//= ZSC, 2016 DEC.29, CURRENT_SW_DELIVERY 134 =>G20项目
//= ZSC, 2017 JAN.3, CURRENT_SW_DELIVERY 135 =>G20项目
//= ZSC, 2017 JAN.5, CURRENT_SW_DELIVERY 136 =>G20项目
//= ZSC, 2017 JAN.13, CURRENT_SW_DELIVERY 137 =>G20项目
//= ZSC, 2017 JAN.16, CURRENT_SW_DELIVERY 138 =>G20项目
//= ZSC, 2017 JAN.18, CURRENT_SW_DELIVERY 139 =>G20项目
//= ZJL, 2017 Feb.1, CURRENT_SW_DELIVERY 140 =>BFSC项目
//= ZJL, 2017 Feb.2, CURRENT_SW_DELIVERY 141 =>BFSC项目
//= ZJL, 2017 Feb.2, CURRENT_SW_DELIVERY 142 =>BFSC项目
//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 143 =>BFSC项目
//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 144 =>BFSC项目
//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 145 =>BFSC项目
//= ZJL, 2017 Feb.4, CURRENT_SW_DELIVERY 146 =>BFSC项目
//= ZJL, 2017 Feb.4, CURRENT_SW_DELIVERY 147 =>BFSC项目
//= ZJL, 2017 Feb.5, CURRENT_SW_DELIVERY 148=>BFSC项目
//= ZJL, 2017 Feb.5, CURRENT_SW_DELIVERY 149=>BFSC项目
//= ZJL, 2017 Feb.6, CURRENT_SW_DELIVERY 150=>BFSC项目
//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 151=>BFSC项目
//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 152=>BFSC项目
//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 153=>BFSC项目
//= ZJL, 2017 Feb.8, CURRENT_SW_DELIVERY 154=>BFSC项目
//= ZJL, 2017 Feb.9, CURRENT_SW_DELIVERY 155=>BFSC项目
//= ZJL, 2017 Feb.9, CURRENT_SW_DELIVERY 156=>BFSC项目
//= ZJL, 2017 Feb.10, CURRENT_SW_DELIVERY 157=>BFSC项目
//= ZJL, 2017 Feb.11, CURRENT_SW_DELIVERY 158=>BFSC项目
//= ZJL, 2017 Feb.11, CURRENT_SW_DELIVERY 159=>BFSC项目
//= ZSC, 2017 Feb.12, CURRENT_SW_DELIVERY 160=>AQYC项目
//= ZJL, 2017 Feb.12, CURRENT_SW_DELIVERY 161=>BFSC项目
//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 162=>BFSC项目
//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 163=>BFSC项目
//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 165=>BFSC项目
//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 166=>BFSC项目
//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 167=>BFSC项目
//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 168=>BFSC项目
//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 169=>BFSC项目
//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 170=>BFSC项目
//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 171=>BFSC项目
//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 172=>BFSC项目
//= ZJL, 2017 Feb.16, CURRENT_SW_DELIVERY 173=>BFSC项目
//= ZSC, 2017 Feb.16, CURRENT_SW_DELIVERY 174=>AQYC项目
//= ZJL, 2017 Feb.17, CURRENT_SW_DELIVERY 175=>BFSC项目
//= ZJL, 2017 Feb.17, CURRENT_SW_DELIVERY 176=>BFSC项目
//= ZJL, 2017 Feb.18, CURRENT_SW_DELIVERY 177=>AQYC项目
//= ZJL, 2017 Feb.19, CURRENT_SW_DELIVERY 178=>AQYC项目
//= ZJL, 2017 Feb.20, CURRENT_SW_DELIVERY 179=>AQYC项目
//= ZJL, 2017 Feb.21, CURRENT_SW_DELIVERY 180=>AQYC项目
//= ZJL, 2017 Feb.22, CURRENT_SW_DELIVERY 181=>AQYC项目
//= ZJL, 2017 Feb.23, CURRENT_SW_DELIVERY 182=>BFSC项目
//= ZSC, 2017 Feb.24, CURRENT_SW_DELIVERY 183=>AQYC项目
//= ZJL, 2017 Feb.24, CURRENT_SW_DELIVERY 184=>BFSC项目
//= ZSC, 2017 Feb.28, CURRENT_SW_DELIVERY 185=>AQYC项目
//= ZSC, 2017 Mar.3, CURRENT_SW_DELIVERY 186=>AQYC项目
//= ZSC, 2017 Mar.4, CURRENT_SW_DELIVERY 187=>AQYC项目
//= ZSC, 2017 Mar.6, CURRENT_SW_DELIVERY 188=>AQYC项目
//= ZSC, 2017 Mar.8, CURRENT_SW_DELIVERY 189=>AQYC项目
//= ZSC, 2017 Mar.8, CURRENT_SW_DELIVERY 190=>AQYC项目
//= ZSC, 2017 Mar.10, CURRENT_SW_DELIVERY 191=>AQYC项目
//= ZSC, 2017 Mar.13, CURRENT_SW_DELIVERY 192=>AQYC项目
//= ZSC, 2017 Mar.16, CURRENT_SW_DELIVERY 193=>AQYC项目
//= ZSC, 2017 Apr.16, CURRENT_SW_DELIVERY 194=>AQYC项目
//= ZSC, 2017 May.6, CURRENT_SW_DELIVERY 195=>AQYC项目
//= ZSC, 2017 May.16, CURRENT_SW_DELIVERY 196=>AQYC项目
//= ZJL/MYC, 2017 May.25, CURRENT_SW_DELIVERY 197=>BFSC项目
//= ZSC, 2017 May.18, CURRENT_SW_DELIVERY 198=>AQYC项目
//= ZSC, 2017 May.20, CURRENT_SW_DELIVERY 199=>AQYC项目
//= ZSC, 2017 May.20, CURRENT_SW_DELIVERY 200=>AQYC项目
//= ZJL, 2017 May.27, CURRENT_SW_DELIVERY 201=>BFSC项目
//= ZJL, 2017 May.29, CURRENT_SW_DELIVERY 202=>BFSC项目
//= ZJL, 2017 May.31, CURRENT_SW_DELIVERY 203=>BFSC项目
//= ZJL, 2017 June.1 CURRENT_SW_DELIVERY 204=>BFSC项目
//= ZJL, 2017 June.4 CURRENT_SW_DELIVERY 205=>BFSC项目
//= ZSC, 2017 June.4, CURRENT_SW_DELIVERY 206=>AQYC项目
//= ZJL, 2017 June.10 CURRENT_SW_DELIVERY 207=>BFSC项目
//= ZJL, 2017 June.12 CURRENT_SW_DELIVERY 208=>BFSC项目
//= ZSC, 2017 June.17 CURRENT_SW_DELIVERY 209=>AQYC项目
//= ZJL, 2017 June.18 CURRENT_SW_DELIVERY 210=>BFSC项目
//= ZJL, 2017 June.21 CURRENT_SW_DELIVERY 211=>BFSC项目
//= LZH, 2017 June.26 CURRENT_SW_DELIVERY 212=>BFSC项目
//= ZJL, 2017 June.28 CURRENT_SW_DELIVERY 213=>BFSC项目
//= ZJL, 2017 June.29 CURRENT_SW_DELIVERY 214=>BFSC项目
//= ZJL, 2017 June.30 CURRENT_SW_DELIVERY 215=>BFSC项目
//= ZJL, 2017 June.30 CURRENT_SW_DELIVERY 216=>BFSC项目
//= ZJL, 2017 July.2 CURRENT_SW_DELIVERY 217=>BFSC项目
//= ZJL, 2017 July.3 CURRENT_SW_DELIVERY 218=>BFSC项目
//= ZJL, 2017 July.3 CURRENT_SW_DELIVERY 219=>BFSC项目
//= ZJL/LZH, 2017 July.7 CURRENT_SW_DELIVERY 220=>BFSC项目
//= ZJL/LZH, 2017 July.15 CURRENT_SW_DELIVERY 221=>BFSC项目
//= ZSC, 2017 July.15 CURRENT_SW_DELIVERY 222=>AQYC项目
//= ZJL/LZH, 2017 July.19 CURRENT_SW_DELIVERY 223=>BFSC项目
//= ZSC, 2017 July.19 CURRENT_SW_DELIVERY 224=>AQYC项目
//= ZJL, 2017 July.22 CURRENT_SW_DELIVERY 225=>BFSC项目
//= ZSC, 2017 July.23 CURRENT_SW_DELIVERY 226=>AQYC项目
//= ZJL, 2017 July.24 CURRENT_SW_DELIVERY 227=>BFSC项目
//= ZJL, 2017 July.27 CURRENT_SW_DELIVERY 228=>BFSC项目
//= ZJL, 2017 July.28 CURRENT_SW_DELIVERY 229=>BFSC项目
//= ZJL, 2017 July.29 CURRENT_SW_DELIVERY 229=>BFSC项目
//= ZJL,, 2017 July.30 CURRENT_SW_DELIVERY 231=>BFSC项目
//= ZJL,, 2017 July.30 CURRENT_SW_DELIVERY 232=>BFSC项目
//= ZJL, 2017 July.31 CURRENT_SW_DELIVERY 233=>BFSC项目
//= ZJL, 2017 Aug.1 CURRENT_SW_DELIVERY 234=>BFSC项目
//= ZJL, 2017 Aug.2 CURRENT_SW_DELIVERY 235=>BFSC项目
//= ZJL, 2017 Aug.3 CURRENT_SW_DELIVERY 236=>BFSC项目
//= ZJL, 2017 Aug.5 CURRENT_SW_DELIVERY 237=>BFSC项目
//= ZJL, 2017 Aug.10 CURRENT_SW_DELIVERY 238=>BFSC项目
//= ZJL, 2017 Aug.1５ CURRENT_SW_DELIVERY 239=>BFSC项目
//= ZSC, 2017 Aug.1５ CURRENT_SW_DELIVERY 240=>AQYC项目
//= ZJL, 2017 Aug.16 CURRENT_SW_DELIVERY 241=>BFSC项目
//= ZJL, 2017 Aug.18 CURRENT_SW_DELIVERY 242=>BFSC项目
//= ZJL, 2017 Aug.19 CURRENT_SW_DELIVERY 243=>BFSC项目
//= ZJL, 2017 Aug.22 CURRENT_SW_DELIVERY 244=>BFSC项目
//= ZJL, 2017 Aug.23 CURRENT_SW_DELIVERY 245=>BFSC项目
//= ZJL, 2017 Aug.25 CURRENT_SW_DELIVERY 246=>BFSC项目
//= ZJL, 2017 Aug.28 CURRENT_SW_DELIVERY 247=>BFSC项目
//= ZJL, 2017 Aug.29 CURRENT_SW_DELIVERY 248=>BFSC项目
//= ZJL, 2017 Aug.30 CURRENT_SW_DELIVERY 249=>BFSC项目
//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 250=>BFSC项目
//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 251=>BFSC项目
//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 252=>BFSC项目
//= ZJL, 2017 Sep.5 CURRENT_SW_DELIVERY 253=>BFDF项目
//= ZJL, 2017 Sep.5 CURRENT_SW_DELIVERY 254=>BFDF项目
//= ZJL, 2017 Sep.6 CURRENT_SW_DELIVERY 255=>BFDF项目
//= ZSC, 2017 Sep.6 CURRENT_SW_DELIVERY 256=>AQYC项目
//= ZJL, 2017 Sep.7 CURRENT_SW_DELIVERY 256=>BFDF项目
//= ZJL, 2017 Sep.7 CURRENT_SW_DELIVERY 258=>BFHS项目
//= ZJL, 2017 Sep.8 CURRENT_SW_DELIVERY 259=>BFDF项目
//= ZJL, 2017 Sep.12 CURRENT_SW_DELIVERY 260=>AQYC项目
//= ZJL, 2017 Sep.12 CURRENT_SW_DELIVERY 261=>AQYC项目
//= ZJL, 2017 Sep.13 CURRENT_SW_DELIVERY 262=>AQYC项目
//= ZJL, 2017 Sep.14 CURRENT_SW_DELIVERY 263=>AQYC项目
//= ZJL, 2017 Sep.15 CURRENT_SW_DELIVERY 264=>BFSC项目
//= ZJL, 2017 Sep.16 CURRENT_SW_DELIVERY 265=>BFSC项目
//= ZJL, 2017 Sep.18 CURRENT_SW_DELIVERY 266=>BFDF项目
//= ZJL, 2017 Sep.19 CURRENT_SW_DELIVERY 267=>BFDF项目
//= ZJL, 2017 Sep.20 CURRENT_SW_DELIVERY 268=>BFDF项目
//= ZJL, 2017 Sep.20 CURRENT_SW_DELIVERY 269=>BFDF项目
//= ZJL, 2017 Sep.22 CURRENT_SW_DELIVERY 270=>BFDF项目
//= ZJL, 2017 Sep.25 CURRENT_SW_DELIVERY 271=>BFDF项目
//= ZJL, 2017 Sep.27 CURRENT_SW_DELIVERY 272=>BFDF项目
//= ZSC, 2017 Oct.1 CURRENT_SW_DELIVERY 273=>AQYC项目
//= ZJL, 2017 Oct.9 CURRENT_SW_DELIVERY 274=>BFDF项目
//= ZJL, 2017 Oct.11 CURRENT_SW_DELIVERY 275=>BFSC项目
//= ZJL, 2017 Oct.13 CURRENT_SW_DELIVERY 276=>BFDF项目
//= ZJL, 2017 Oct.15 CURRENT_SW_DELIVERY 277=>AQYC项目
//= ZJL, 2017 Oct.16 CURRENT_SW_DELIVERY 278=>AQYC项目
//= ZJL, 2017 Oct.16 CURRENT_SW_DELIVERY 279=>BFSC项目
//= ZJL, 2017 Oct.16 CURRENT_SW_DELIVERY 280=>BFHS项目
//= ZJL, 2017 Oct.17 CURRENT_SW_DELIVERY 281=>BFHS项目
//= ZJL, 2017 Oct.18 CURRENT_SW_DELIVERY 282=>BFSC项目
//= ZJL, 2017 Oct.19 CURRENT_SW_DELIVERY 283=>BFHS项目
//= ZJL, 2017 Oct.19 CURRENT_SW_DELIVERY 284=>BFDF项目
//= ZJL, 2017 Oct.20 CURRENT_SW_DELIVERY 285=>BFDF项目
//= ZJL, 2017 Oct.21 CURRENT_SW_DELIVERY 286=>BFHS项目
//= ZSC, 2017 Nov.04 CURRENT_SW_DELIVERY 287=>AQYC项目
//= ZJL, 2017 Nov.16 CURRENT_SW_DELIVERY 288=>BFSC项目
//= ZJL, 2017 Nov.24 CURRENT_SW_DELIVERY 289=>BFHS项目
//= ZJL, 2017 Nov.25 CURRENT_SW_DELIVERY 290=>BFHS项目
//= ZJL, 2017 Nov.27 CURRENT_SW_DELIVERY 291=>BFHS项目
//= ZJL, 2017 Nov.28 CURRENT_SW_DELIVERY 292=>BFHS项目
//= ZJL, 2017 Nov.30 CURRENT_SW_DELIVERY 293=>BFHS项目
//= ZJL, 2017 Dec.4 CURRENT_SW_DELIVERY 294=>BFHS项目
//= ZSC, 2017 Dec.5 CURRENT_SW_DELIVERY 295=>AQYC项目
//= ZSC, 2017 Dec.9 CURRENT_SW_DELIVERY 296=>AQYC项目









#endif /* L0COMVM_SYSVERSION_H_ */
