/*
 * aiqiyc.c
 *
 *  Created on: 2016年1月4日
 *      Author: test
 */

#include "aiqiyc.h"

/*
 *   爱启-扬尘项目：业务智能模块
 *
 *		业务智能的初步想法是，针对HCU工控机生成的诸多数据，在嵌入式前端系统内部，按照一定的规则和逻辑，进行数据挖掘和规整，从而产生有价值的信息，并上报后台
 *
 *		为什么需要前端的智能处理：考虑到中间传输数据数量巨大，不可能所有数据都会传送到后台，而且当前设置的采样频率是非常低下的，所以数据在前端做加工，具备很强
 *		的实用性和现实意义
 *
 *		上报方式：采用xml格式方式，增加#define CLOUDVELA_BH_MSG_TYPE_BIZ_ITG_STRING "hcu_biz_itg"  //业务智能 hcu_biz_inteligence
 *				该信息将由后台对应的L4BI模块直接处理
 *
 *		初步考虑的BI功能：
 *			1. 	参数配置过低，频繁汇报相同的数据内容，造成通信带宽消耗的效率低下。比如，平均xx秒的传感器汇报周期
 *				算法：低频分量频谱分析
 *			2. 	参数配置过大，造成汇报的数据不能完全反映外界的快速变化，比如，平均xx分钟的传感器汇报
 *				算法：高频分量频谱分析
 *			3.	外部测量量的周期性变化和预测，获得长周期（周-月）和短周期（小时-天）的预测模型及描述参量
 *				算法：基于最小二乘法的预测模型及训练
 *			4. 	基于1/2/3的模型，系统参数自学习，自配置和自优化。在模型统一的基础上，该参数是基于具体应用场景的最优配置，而非全局一致。
 *				算法：最优准则-通信量最低/信息量最大
 *			5. 	运动检测
 *			6. 	运动物体统计
 *			7.	基于运动物体的统计，做地段时段定价模型。该定价模型的现实意义是反应了该地段时段的人流物流通量（存量），流速（速度）以及速动率（加速度），
 *				从而为其它交易提供了基础成本信息。比如：
 *				通量低，流速高，速动率高，说明这里的潜力大
 *				通量大，流速高，速动率低，说明这里属于高速公路或者主干道
 *				通量低，流速高，速动率起伏不定，高价值区域
 *				流速低的区域，价值低，没有流动性的资产是没有价值的
 *				速动率变化极大的区域，属于敏感区域，价值需要极高的驾驭能力
 *			8.	多维采集量的平稳性和相关性挖掘
 *				算法：马尔科夫链，相关分析，贝叶斯法分析，高阶炬估计
 *			9. 	基于商用数据挖掘工具的分析，比如SPSS系统等
 *			10.	跟地理/人口等其它GIS系统的相关性分析
 *
 */

OPSTAT test(void)
{
	return SUCCESS;
}