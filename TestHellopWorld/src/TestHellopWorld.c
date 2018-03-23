///*
// ============================================================================
// Name        : TestHellopWorld.c
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
// ============================================================================

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TestHellopWorld.h"

//全局变量
unsigned int zDlkTickCnt = 0;
StrCtrlContext_t zCtrlContext;
StrWorkOrder_t   zWorkOrder;
StrIntputFlow_t  zInputFlow;

//初始化各项参数
void global_init_coef(void)
{
	int i=0, j=0, tmp;

	//全局变量初始化
	memset(&zCtrlContext, 0, sizeof(StrCtrlContext_t));
	memset(&zWorkOrder, 0, sizeof(StrWorkOrder_t));
	memset(&zInputFlow, 0, sizeof(StrIntputFlow_t));
	srand((int)time(0));

	//Hopper链表初始化=>我们假设编号是按照从0到2000，从小到大排列，顺时针，但料斗的运动是逆时钟，这样从稳固不动的各个端口角度，运动其实还是从小到大跑动，依然是顺着NextHop方向走的。
	for (i=0; i<DLK_MAIN_BUF; i++){
		zCtrlContext.hop[i].index = i;
		zCtrlContext.hop[i].state = DLK_STATE_EMPTY;
		//zCtrlContext.hop[i].postInd = (i+1)%DLK_MAIN_BUF; //指向下一个
		//zCtrlContext.hop[i].preInd = (i+DLK_MAIN_BUF-1)%DLK_MAIN_BUF; //指向前一个
		zCtrlContext.hop[i].mat.productCat = 0xFFFFFFFF;
		zCtrlContext.hop[i].mat.productInd = 0xFFFFFFFF;
		zCtrlContext.hop[i].lifeRemain = 0;
		zCtrlContext.hop[i].outputPort = 0xFFFFFFFF;
		zCtrlContext.hop[i].woNbr = 0xFFFFFFFF;
		//printf("Hop/Post/PreInd=%d/%d/%d\n", i, zCtrlContext.hop[i].postInd, zCtrlContext.hop[i].preInd);
	}
	for (i=0; i<DLK_OUTPUT_PIPE_MAX; i++){
		zCtrlContext.woRecGlobalIndex[i] = 0xFFFFFFFF;
	}

	//挂载输入端口
	tmp = (int)(DLK_MAIN_BUF/(DLK_INPUT_PIPE_MAX + DLK_OUTPUT_PIPE_MAX + 1));
	//动态输入料斗方式：各个之间差距tmp=1/(4+5+1)=1/10
//	for(i=0; i<DLK_INPUT_PIPE_MAX; i++){
//		zCtrlContext.inputPort[i] = tmp*i + (rand()%tmp);  //前半段
//	}
	//静态输入料斗方式：各个之间差距1
	zCtrlContext.inputPort[0] = (rand()%tmp);
	for(i=1; i<DLK_INPUT_PIPE_MAX; i++){
		zCtrlContext.inputPort[i] = zCtrlContext.inputPort[0]+i;  //前半段
	}

	for(i=1; i<DLK_INPUT_PIPE_MAX; i++){
		for(j=0; j<i; j++){
			if(zCtrlContext.inputPort[i] == zCtrlContext.inputPort[j]){
				printf("Init inputPort error!\n");
				return;
			}
		}
	}
	printf("Input port = [%d/%d/%d/%d]\n", zCtrlContext.inputPort[0], zCtrlContext.inputPort[1], zCtrlContext.inputPort[2], zCtrlContext.inputPort[3]);

	//挂载输出端口
	for(i=0; i<DLK_OUTPUT_PIPE_MAX; i++){
		zCtrlContext.outputPort[i] = tmp*(DLK_INPUT_PIPE_MAX+i) + (rand()%tmp);  //后半段
	}
	for(i=1; i<DLK_OUTPUT_PIPE_MAX; i++){
		for(j=0; j<i; j++){
			if(zCtrlContext.outputPort[i] == zCtrlContext.outputPort[j]){
				printf("Init outputPort error!\n");
				return;
			}
		}
	}
	printf("Output port = [%d/%d/%d/%d]\n", zCtrlContext.outputPort[0], zCtrlContext.outputPort[1], zCtrlContext.outputPort[2], zCtrlContext.outputPort[3]);

	//垃圾出口
	zCtrlContext.rubbishPort = tmp*(DLK_INPUT_PIPE_MAX + DLK_OUTPUT_PIPE_MAX) + (rand()%tmp);  //最后一段段
	for(j=0; j<i; j++){
		if(zCtrlContext.rubbishPort == zCtrlContext.outputPort[j]){
			printf("Init rubbishPort error!\n");
			return;
		}
	}
	printf("rubbishPort = [%d]\n", zCtrlContext.rubbishPort);

	//输出目标
	if (DLK_INPUT_PIPE_MAX != 4){
		printf("Error -1!");
		return;
	}
	//2000目标包
	for (i=0; i<DLK_TARGET_WO_MAX; i++){
		//每一包必须包含输入类型ABCD中的各一个
		for (j=0; j<DLK_INPUT_PIPE_MAX; j++){
			zWorkOrder.wo[i].matEle[j].productCat = j;
			//具体到哪一种型号，由随机数产生，最大为INP_IND_MAX界定，这里是5种小型号
			tmp = rand()%(DLK_INP_IND_MAX);
			//printf("i/j/tmp=%d/%d/%d\n", i, j, tmp);
			zWorkOrder.wo[i].matEle[j].productInd = tmp; //某种具体型号的数量
			zWorkOrder.inputPkgNbrSta[j][tmp]++;
		}
	}
	printf("First WO=[%d-%d/%d-%d/%d-%d/%d-%d]\n", zWorkOrder.wo[0].matEle[0].productCat, zWorkOrder.wo[0].matEle[0].productInd,\
			zWorkOrder.wo[0].matEle[1].productCat, zWorkOrder.wo[0].matEle[1].productInd,\
			zWorkOrder.wo[0].matEle[2].productCat, zWorkOrder.wo[0].matEle[2].productInd,\
			zWorkOrder.wo[0].matEle[3].productCat, zWorkOrder.wo[0].matEle[3].productInd\
			);
	printf("WO Final Distribution, A[%d/%d/%d/%d/%d], B[%d/%d/%d/%d/%d], C[%d/%d/%d/%d/%d], D[%d/%d/%d/%d/%d]\n", \
			zWorkOrder.inputPkgNbrSta[0][0], zWorkOrder.inputPkgNbrSta[0][1], zWorkOrder.inputPkgNbrSta[0][2], zWorkOrder.inputPkgNbrSta[0][3], zWorkOrder.inputPkgNbrSta[0][4],\
			zWorkOrder.inputPkgNbrSta[1][0], zWorkOrder.inputPkgNbrSta[1][1], zWorkOrder.inputPkgNbrSta[1][2], zWorkOrder.inputPkgNbrSta[1][3], zWorkOrder.inputPkgNbrSta[0][4],\
			zWorkOrder.inputPkgNbrSta[2][0], zWorkOrder.inputPkgNbrSta[2][1], zWorkOrder.inputPkgNbrSta[2][2], zWorkOrder.inputPkgNbrSta[2][3], zWorkOrder.inputPkgNbrSta[0][4],\
			zWorkOrder.inputPkgNbrSta[3][0], zWorkOrder.inputPkgNbrSta[3][1], zWorkOrder.inputPkgNbrSta[3][2], zWorkOrder.inputPkgNbrSta[3][3], zWorkOrder.inputPkgNbrSta[0][4]\
			);

	//输出WO全体打印
//	for (i=0; i<DLK_TARGET_WO_MAX; i++){
//		printf("WO Index = %d, Content=[%d/%d/%d/%d]\n", i, zWorkOrder.wo[i].matEle[0].productInd, zWorkOrder.wo[i].matEle[1].productInd, zWorkOrder.wo[i].matEle[2].productInd, zWorkOrder.wo[i].matEle[3].productInd);
//	}

	//初始化输入模型的概率分布
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		for (j=0; j<DLK_INP_IND_MAX; j++){
			zCtrlContext.inputDis[i][j] = (double)zWorkOrder.inputPkgNbrSta[i][j]/(double)DLK_TARGET_WO_MAX;
		}
	}
	printf("InputDis=%5.3f/%5.3f/%5.3f/%5.3f/\n", zCtrlContext.inputDis[0][0], zCtrlContext.inputDis[0][1], zCtrlContext.inputDis[0][2], zCtrlContext.inputDis[0][3]);
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		zCtrlContext.inputDisRange[i][0] = zCtrlContext.inputDis[i][0];
		for (j=1; j<DLK_INP_IND_MAX; j++){
			zCtrlContext.inputDisRange[i][j] = zCtrlContext.inputDisRange[i][j-1] + zCtrlContext.inputDis[i][j];
		}
		printf("Accumulate Input Ratio = %f/%f/%f/%f/%f\n", zCtrlContext.inputDisRange[i][0], zCtrlContext.inputDisRange[i][1], zCtrlContext.inputDisRange[i][2], \
				zCtrlContext.inputDisRange[i][3], zCtrlContext.inputDisRange[i][4]);
	}

	//初始化各个工单的优先级
	unsigned int index = 0;
	unsigned int priority = 0;
	while(index < DLK_TARGET_WO_MAX)
	{
		tmp = 1+rand()%DLK_TARGET_PRI_LEN;
		zWorkOrder.priLenTable[priority] = tmp; //优先级长度
		for (j=0; j<tmp; j++){
			zWorkOrder.wo[index].pri = priority;
			//printf("Current Len = %d, Pri = %d\n", tmp, priority);
			index++;
			if (index >= DLK_TARGET_WO_MAX)
				break;
		}
		priority++;
	}
	zWorkOrder.priMax = priority;
	printf("Max Priority = %d, Index = %d\n", zWorkOrder.priMax, index);

	//输入数据
	//按照输出WO数量，准备输入物料的参照比例
	//这里先采取N-长度的等概率模型
	unsigned int change = 0; //看看改变过多少次
	unsigned int ProductInd = 0;
	double ratio = 0;
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		change = 0;
		index = 0;
		while(index < DLK_INPUT_PKG_MAX)
		{
			//随机模式
			//ProductInd += 1+rand()%(DLK_INP_IND_MAX-1);
			//ProductInd = ProductInd%DLK_INP_IND_MAX;
			//根据输出概率模式进行动态生成
			ratio = ((double)(rand()%1000))/1000.0;
			ProductInd = 0;
			for (j = 0; j<DLK_INP_IND_MAX-1; j++){
				if (ratio > zCtrlContext.inputDisRange[i][j])
					ProductInd = j+1;
			}
			tmp = 1+rand()%DLK_INPUT_DIS_LEN; //变化长度
			zInputFlow.chgLenTable[i][change] = tmp;//目标值
			//printf("Pipe = %d, Index = %d, ProductIndex = %d, SameItem Len = %d\n", i, index, ProductInd, tmp);
			for (j=0; j<tmp; j++){
				zInputFlow.fl[i][index] = ProductInd;
				index++;
				if (index >= DLK_INPUT_PKG_MAX)
					break;
			}
			change++;
		}
		zInputFlow.chgMax[i] = change;//改变产品次数
		printf("Input Pipe Nbr = %d, Max change Times = %d, Index = %d\n", i, zInputFlow.chgMax[i], index);
	}
	//输入数据全体打印
//	for(i=0; i<DLK_INPUT_PKG_MAX; i++){
//		printf("INPUT Index = %d, A[%d] B[%d] C[%d] D[%d]\n", i, zInputFlow.fl[0][i], zInputFlow.fl[1][i], zInputFlow.fl[2][i], zInputFlow.fl[3][i], zInputFlow.fl[4][i]);
//	}
	//重新验证总输入长度
	//重新验证的结果，发现概率上还是有差异，问题不大，从仿真角度可以的。
	//如果是更为精确，则需要改为控制概率，从目标数量里面控制数字，一个一个的往下减，那个方式可以保证数量上严格相等。这里生成的输入物料流，只是在大致概率上保持均等。
	unsigned int recheck[DLK_INPUT_PIPE_MAX][DLK_INP_IND_MAX];
	double zRecheckDouble[DLK_INPUT_PIPE_MAX][DLK_INP_IND_MAX];
	memset(recheck, 0, sizeof(unsigned int)*DLK_INPUT_PIPE_MAX*DLK_INP_IND_MAX);
	memset(zRecheckDouble, 0, sizeof(unsigned int)*DLK_INPUT_PIPE_MAX*DLK_INP_IND_MAX);
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		for (j=0; j<DLK_INPUT_PKG_MAX; j++){
			tmp = zInputFlow.fl[i][j];
			//printf("tmp=%d\n", tmp);
			recheck[i][tmp]++;
		}
	}
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		for (j=0; j<DLK_INP_IND_MAX; j++){
			zRecheckDouble[i][j] = (double)(recheck[i][j]*1.0)/(double)(DLK_INPUT_PKG_MAX*1.0);
			printf("Input Re-check ratio, i/j=%d/%d, result = %5.3f/%5.3f\n", i, j, zCtrlContext.inputDis[i][j], zRecheckDouble[i][j]);
		}
	}
}

//所有HOP移动一格
void global_move_input_output_port(void)
{
	int i=0, j=0;
	for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
		zCtrlContext.inputPort[i]++;
		zCtrlContext.inputPort[i] = zCtrlContext.inputPort[i]%DLK_MAIN_BUF;
	}
	for (i=0; i<DLK_OUTPUT_PIPE_MAX; i++){
		zCtrlContext.outputPort[i]++;
		zCtrlContext.outputPort[i] = zCtrlContext.outputPort[i]%DLK_MAIN_BUF;
	}
	zCtrlContext.rubbishPort++;
	zCtrlContext.rubbishPort = zCtrlContext.rubbishPort%DLK_MAIN_BUF;
	//所有物料的生命周期减少一
	for (i=0; i<DLK_MAIN_BUF; i++){
		if (zCtrlContext.hop[i].lifeRemain > 100000){
			printf("Error 1!\n\n");
			exit(1);
		}
		if(zCtrlContext.hop[i].lifeRemain > 0)
			zCtrlContext.hop[i].lifeRemain--;
	}

	//物料出料指示器的状态更新：不用来控制物料，只是指示和辅助，因为它很难对物料做互斥。互斥依赖物料状态进行控制，这样更加方便。
	for (i=0; i<DLK_OUTPUT_PIPE_MAX; i++){
		for (j=0; j<DLK_INPUT_PIPE_MAX; j++){
			if (zWorkOrder.woOutHopRemSteps[i][j] > 100000){
				printf("Error 2!\n\n");
				exit(1);
			}
			if (zWorkOrder.woOutHopRemSteps[i][j] == 0)
				continue;
			if (zWorkOrder.woOutHopRemSteps[i][j] > 0)
				zWorkOrder.woOutHopRemSteps[i][j]--;
		}
	}

}

//刷新缺料状态，为入料做准备
void global_mat_lack_refresh(void)
{
	int prdCatIndex=0, prdTypeIndex=0;
	int i=0, j=0;
	int tmp=0;

	memset(zCtrlContext.lackMat, 0, sizeof(unsigned int)*DLK_INPUT_PIPE_MAX*DLK_INP_IND_MAX);

	//统计Main Buffer中的空斗
	tmp=0;
	for (i=0; i<DLK_MAIN_BUF; i++){
		if (zCtrlContext.hop[i].state == DLK_STATE_EMPTY) tmp++;
	}
	zCtrlContext.staMbEmpty = tmp;

	//扫描需求量：一包4料，所以WO深度=200/4=50个包，但存入时需要做N次
	for (i=zCtrlContext.woExeGlobalIndex; i<(zCtrlContext.woExeGlobalIndex + DLK_ALGO_SEG_PD_TYPE); i++)
	{
		//j就是物料类型
		for (j=0; j<DLK_INPUT_PIPE_MAX; j++){
			prdTypeIndex = zWorkOrder.wo[i].matEle[j].productInd;
			zCtrlContext.lackMat[j][prdTypeIndex]++;
		}
	}

	//主BUF中有的，降低需求量
	for (i=0; i<DLK_MAIN_BUF; i++){
		if (zCtrlContext.hop[i].state == DLK_STATE_OCCPUY){
			prdCatIndex = zCtrlContext.hop[i].mat.productCat;
			prdTypeIndex = zCtrlContext.hop[i].mat.productInd;
			if (zCtrlContext.lackMat[prdCatIndex][prdTypeIndex] > 0)
				zCtrlContext.lackMat[prdCatIndex][prdTypeIndex]--;
		}
	}
}

//入料
//方式一：入料改造为100%匹配
//方式二：入料增加BUF，然后在BUF中搜索，BUF深度可控。有料时直接送入，无料时按照概率往MAIN_BUF中放料
void global_mat_inflow(void)
{
	int iPipId=0;
	int tmp=0;
	int distance=0;
	float emptyRatio = 0;
	int inPort;

	/*
	 *
	 * [SOLUTION 3]
	 *
	 */
	//先使用全匹配方案，使用精确过程，而且输入物料之间的料斗间距必须为1，不然没法使用
	if (zCtrlContext.lackFullMatCtrlProg >=DLK_TARGET_WO_MAX)
		return;
	for (iPipId=0; iPipId<DLK_INPUT_PIPE_MAX; iPipId++)
	{
		inPort = zCtrlContext.inputPort[iPipId];
		if (zCtrlContext.hop[inPort].state != DLK_STATE_EMPTY)
			return;
	}

	for (iPipId=0; iPipId<DLK_INPUT_PIPE_MAX; iPipId++)
	{
		inPort = zCtrlContext.inputPort[iPipId];
		zCtrlContext.hop[inPort].state = DLK_STATE_OCCPUY;
		zCtrlContext.hop[inPort].lifeRemain = DLK_LIFE_CYCLE_MAX;
		zCtrlContext.hop[inPort].mat.productCat = iPipId;
		zCtrlContext.hop[inPort].mat.productInd = zWorkOrder.wo[zCtrlContext.lackFullMatCtrlProg].matEle[iPipId].productInd;
		zCtrlContext.inputFlCur[iPipId]++;
		zCtrlContext.staMatInc++;
		zCtrlContext.inputCtrlCnt[iPipId]++;
		//printf("TEST, Input mat progress = %d!\n", zCtrlContext.lackFullMatCtrlProg);
	}
	zCtrlContext.lackFullMatCtrlProg++;




	/*
	 *
	 * [SOLUTION 2]
	 *
	 */
	//先使用全匹配方案，使用统计方式
//	for(iPipId=1; iPipId<DLK_INPUT_PIPE_MAX; iPipId++){
//		distance += zCtrlContext.inputCtrlCnt[iPipId];
//	}
//	emptyRatio = (float)zCtrlContext.staMbEmpty / (float)DLK_MAIN_BUF;
//	distance = zCtrlContext.inputCtrlCnt[0] - distance/3;
//	//控制4个输入线相互之间的差距不能太大
//	if ((distance < 5) && (emptyRatio > DLK_ALGO_HOP_EMPTY_RATIO)){
//		if(zCtrlContext.hop[zCtrlContext.inputPort[0]].state == DLK_STATE_EMPTY)
//		{
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].state = DLK_STATE_OCCPUY;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].lifeRemain = DLK_LIFE_CYCLE_MAX;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].mat.productCat = 0;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].mat.productInd = zWorkOrder.wo[zCtrlContext.inputFlCur[0]].matEle[0].productInd;
//			zCtrlContext.inputFlCur[0]++;
//			zCtrlContext.staMatInc++;
//			zCtrlContext.inputCtrlCnt[0]++;
//		}
//	}
//	for(iPipId=1; iPipId<DLK_INPUT_PIPE_MAX; iPipId++){
//		distance = zCtrlContext.inputCtrlCnt[iPipId] - zCtrlContext.inputCtrlCnt[0];
//		if ((distance < 5))
//		{
//			if(zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].state == DLK_STATE_EMPTY)
//			{
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].state = DLK_STATE_OCCPUY;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].lifeRemain = DLK_LIFE_CYCLE_MAX;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].mat.productCat = iPipId;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].mat.productInd = zWorkOrder.wo[zCtrlContext.inputFlCur[iPipId]].matEle[iPipId].productInd;
//				zCtrlContext.inputFlCur[iPipId]++;
//				zCtrlContext.staMatInc++;
//				zCtrlContext.inputCtrlCnt[iPipId]++;
//			}
//		}
//	}



	/*
	 *
	 * [SOLUTION 1]
	 *
	 */
	//完全随机输入算法
	//第A个输入
//	tmp = rand()%100;
//	for(iPipId=1; iPipId<DLK_INPUT_PIPE_MAX; iPipId++){
//		distance += zCtrlContext.inputCtrlCnt[iPipId];
//	}
//	distance = zCtrlContext.inputCtrlCnt[0] - distance/3;
//	//printf("Distance=%d\n", distance);
//	if ((tmp>5) && (distance < 20))
//	{
//		if(zCtrlContext.hop[zCtrlContext.inputPort[0]].state == DLK_STATE_EMPTY)
//		{
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].state = DLK_STATE_OCCPUY;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].lifeRemain = DLK_LIFE_CYCLE_MAX;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].mat.productCat = 0;
//			zCtrlContext.hop[zCtrlContext.inputPort[0]].mat.productInd = zInputFlow.fl[0][zCtrlContext.inputFlCur[0]];
//			zCtrlContext.inputFlCur[0]++;
//			zCtrlContext.staMatInc++;
//			zCtrlContext.inputCtrlCnt[0]++;
//		}
//	}
//
//	//后续几个
//	for(iPipId=1; iPipId<DLK_INPUT_PIPE_MAX; iPipId++){
//		tmp = rand()%100;
//		distance = zCtrlContext.inputCtrlCnt[iPipId] - zCtrlContext.inputCtrlCnt[0];
//		//if ((tmp>25) && (distance<3))
//		if ((distance < 20))
//		{
//			if(zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].state == DLK_STATE_EMPTY)
//			{
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].state = DLK_STATE_OCCPUY;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].lifeRemain = DLK_LIFE_CYCLE_MAX;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].mat.productCat = iPipId;
//				zCtrlContext.hop[zCtrlContext.inputPort[iPipId]].mat.productInd = zInputFlow.fl[iPipId][zCtrlContext.inputFlCur[iPipId]];
//				zCtrlContext.inputFlCur[iPipId]++;
//				zCtrlContext.staMatInc++;
//				zCtrlContext.inputCtrlCnt[iPipId]++;
//			}
//		}
//	}
	//printf("Inflow MAT index: Inflow=%d/%d/%d/%d\n", zCtrlContext.inputFlCur[0], zCtrlContext.inputFlCur[1], zCtrlContext.inputFlCur[2], zCtrlContext.inputFlCur[3]);
}

//出料
void global_mat_outflow(void)
{
	int oPipId=0, i=0;
	int tmp=0;
	int res=0;

	//剔除料
	if ((zCtrlContext.hop[zCtrlContext.rubbishPort].lifeRemain == 0) && ((zCtrlContext.hop[zCtrlContext.rubbishPort].state == DLK_STATE_OCCPUY))){
		zCtrlContext.hop[zCtrlContext.rubbishPort].state = DLK_STATE_EMPTY;
		zCtrlContext.staMatTgv++;
	}

	//成功出料
	for (oPipId=0; oPipId<DLK_OUTPUT_PIPE_MAX; oPipId++){
		//的确是本物料出口，出料没有考虑多BUFFER的情况
		//输出后才重新扫描组合，这本身就强行控制了，两个大包之间不会出现混淆的情况出现
//		printf("OUTPUT: OutPipeId=%d, State=%d, Global WO Index = %d, Pointer HOP Wo=%d, RecWoIndex=%d\n", \
//				oPipId, zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].state, zCtrlContext.woExeGlobalIndex, zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].woNbr,\
//				zCtrlContext.woRecGlobalIndex[oPipId]);
		if ((zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].state == DLK_STATE_ALLOC) && \
				(zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].woNbr == zCtrlContext.woRecGlobalIndex[oPipId]) &&\
				(zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].outputPort == oPipId))
		{
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].state = DLK_STATE_EMPTY;
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].lifeRemain = 0;
			zWorkOrder.oPkgEleProgCtrl[oPipId] = zWorkOrder.oPkgEleProgCtrl[oPipId] - 1;
			//完整一包(4个物料)后，算是完整的出料了

			//更新出料指示器
			tmp = zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].mat.productCat;
			zWorkOrder.woOutHopRemSteps[oPipId][tmp] = 0;

			//更新物料状态
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].mat.productCat = 0xFFFFFFFF;
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].mat.productInd = 0xFFFFFFFF;
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].woNbr = 0xFFFFFFFF;
			zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].outputPort = 0xFFFFFFFF;

			//完成一次完整的PKG出料，更新WO存储状态
			if (zWorkOrder.oPkgEleProgCtrl[oPipId] == 0){
				zCtrlContext.staCombPkgTimes++;
				tmp = zCtrlContext.outputPort[oPipId];
				printf("OUTPUT Pkg Finished: WoExeRec=%d, oPipeId=%d, outputId=%d, Next 4 materials status: Hid/Cat/Type=[%d/%d/%d][%d/%d/%d][%d/%d/%d][%d/%d/%d]\n", \
						zCtrlContext.woRecGlobalIndex[oPipId], oPipId, tmp,\
						(tmp+1)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+1)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+1)%DLK_MAIN_BUF].mat.productInd, \
						(tmp+2)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+2)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+2)%DLK_MAIN_BUF].mat.productInd, \
						(tmp+3)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+3)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+3)%DLK_MAIN_BUF].mat.productInd, \
						(tmp+4)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+4)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+4)%DLK_MAIN_BUF].mat.productInd\
						);
				tmp = zCtrlContext.woExeGlobalIndex;
				printf("OUTPUT WO next Snapshot request: woExe=%d, Request Cat/Type=[0/%d][1/%d][2/%d][3/%d]\n", tmp, zWorkOrder.wo[tmp].matEle[0].productInd, zWorkOrder.wo[tmp].matEle[1].productInd, zWorkOrder.wo[tmp].matEle[2].productInd, zWorkOrder.wo[tmp].matEle[3].productInd);
				//任务目标终结
				if (zCtrlContext.woRecGlobalIndex[oPipId] == DLK_TARGET_WO_CTRL){
					printf("Target accomplished!\n");
					exit(1);
				}
				zCtrlContext.woRecGlobalIndex[oPipId] = 0xFFFFFFFF;
			}
		}

		//复核一下剩余数量
		res = 0;
		for (i=0; i<DLK_INPUT_PIPE_MAX; i++){
			if (zWorkOrder.woOutHopRemSteps[oPipId][i] != 0)
				res++;
		}
		if (res != zWorkOrder.oPkgEleProgCtrl[oPipId]){
			printf("Audit error! oPipId=%d, StepsInd=%d/%d/%d/%d, Progress=%d\n", \
					oPipId, zWorkOrder.woOutHopRemSteps[oPipId][0], zWorkOrder.woOutHopRemSteps[oPipId][1], zWorkOrder.woOutHopRemSteps[oPipId][2], zWorkOrder.woOutHopRemSteps[oPipId][3],\
					zWorkOrder.oPkgEleProgCtrl[oPipId]);
			printf("Current CASE: State=%d, WoNbr=%d, RecWoInd=%d\n", zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].state, zCtrlContext.hop[zCtrlContext.outputPort[oPipId]].woNbr,\
					zCtrlContext.woRecGlobalIndex[oPipId]);
			exit(1);
		}
	}
}


//算法更新
void global_algo_search(void)
{
	unsigned int oPipId=0, iFlIndex=0, tmp=0;
	unsigned int woIndex = 0;
	unsigned int startSearch = 0, searchIndex=0;
	unsigned int result[DLK_INPUT_PIPE_MAX];
	unsigned int resOk = 0;
	unsigned int findFlag = 0;
	//unsigned int noNeedSearchFlag = 0; //探索是否有空搜索
	for(oPipId=0; oPipId<DLK_OUTPUT_PIPE_MAX; oPipId++){
		//先明确，是否处于出料状态
		if (zWorkOrder.oPkgEleProgCtrl[oPipId] != 0)
			continue;
		//没有使用链表，而是直接使用了顺序号。这是第二个没有使用链表的地方
		startSearch = (zCtrlContext.outputPort[oPipId] + 1) % DLK_MAIN_BUF;
		//寻找目标WO的产品包组合, woIndex = zCtrlContext.woAlgoScanIndex[i], 目标=zWorkOrder.wo[woIndex]
		woIndex = zCtrlContext.woExeGlobalIndex;
		//结果清零
		for (iFlIndex=0; iFlIndex<DLK_INPUT_PIPE_MAX; iFlIndex++){
			result[iFlIndex] = 0;
		}

		//往前看4个物料状态
		tmp = zCtrlContext.outputPort[oPipId];
		printf("ALGO Previous 4 materials snapshot: oPipeId=%d, woExe=%d, HopId=%d, Hid/State/Cat/Type=[%d/%d/%d/%d][%d/%d/%d/%d][%d/%d/%d/%d][%d/%d/%d/%d]\n", oPipId, zCtrlContext.woExeGlobalIndex, tmp,\
				(tmp+1)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+1)%DLK_MAIN_BUF].state, zCtrlContext.hop[(tmp+1)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+1)%DLK_MAIN_BUF].mat.productInd, \
				(tmp+2)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+2)%DLK_MAIN_BUF].state, zCtrlContext.hop[(tmp+2)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+2)%DLK_MAIN_BUF].mat.productInd, \
				(tmp+3)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+3)%DLK_MAIN_BUF].state, zCtrlContext.hop[(tmp+3)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+3)%DLK_MAIN_BUF].mat.productInd, \
				(tmp+4)%DLK_MAIN_BUF, zCtrlContext.hop[(tmp+4)%DLK_MAIN_BUF].state, zCtrlContext.hop[(tmp+4)%DLK_MAIN_BUF].mat.productCat, zCtrlContext.hop[(tmp+4)%DLK_MAIN_BUF].mat.productInd\
				);

		//开始搜索：从之前单个搜索，改为连续搜索
		resOk=0;
		for (iFlIndex=0; iFlIndex<DLK_INPUT_PIPE_MAX; iFlIndex++){
			//4种物料必须全部找到，才能成功
			searchIndex = 0;
			findFlag = 0;
			while ((searchIndex < DLK_SEARCH_WINDOW_MAX) && (findFlag == 0))
			{
				//往左边搜索！
				tmp = (startSearch + searchIndex)%DLK_MAIN_BUF;
				if ((zCtrlContext.hop[tmp].state == DLK_STATE_OCCPUY) && (zCtrlContext.hop[tmp].mat.productCat == iFlIndex) && \
						(zCtrlContext.hop[tmp].mat.productInd==zWorkOrder.wo[woIndex].matEle[iFlIndex].productInd))
				{
					resOk++;
					result[iFlIndex]=tmp;
					findFlag = 1;
					//printf("TEMP Search: oPipId = %d, resOk=%d, sStart=%d, sInd=%d, sTmp=%d, State=%d, pdCat=%d\n", oPipId, resOk, startSearch, searchIndex, tmp, zCtrlContext.hop[tmp].state, zCtrlContext.hop[tmp].mat.productCat);
				}
				searchIndex++;
			}
			//如果上面没有被找到，直接退出
			if (findFlag == 0)
				break;
		}
		//是否找全
		if (resOk==DLK_INPUT_PIPE_MAX)
		{
			//更新物料指示器
			for(iFlIndex=0; iFlIndex<DLK_INPUT_PIPE_MAX; iFlIndex++)
			{
				zWorkOrder.woOutHopRemSteps[oPipId][iFlIndex] = (result[iFlIndex]  + DLK_MAIN_BUF - zCtrlContext.outputPort[oPipId])%DLK_MAIN_BUF;
			}
			//printf("Searched Result Steps: %d/%d/%d/%d\n", result[0], result[1], result[2], result[3]);
			//printf("Searched Result Wo Info: %d/%d/%d/%d\n", zWorkOrder.wo[woIndex].matEle[0].productInd, zWorkOrder.wo[woIndex].matEle[1].productInd, zWorkOrder.wo[woIndex].matEle[2].productInd, zWorkOrder.wo[woIndex].matEle[3].productInd);
			//更新HOP状态
			for(iFlIndex=0; iFlIndex<DLK_INPUT_PIPE_MAX; iFlIndex++)
			{
				zCtrlContext.hop[result[iFlIndex]].state = DLK_STATE_ALLOC;
				zCtrlContext.hop[result[iFlIndex]].woNbr = zCtrlContext.woExeGlobalIndex;
				zCtrlContext.hop[result[iFlIndex]].outputPort = oPipId;
			}
			//同时赋给本接口
			zCtrlContext.woRecGlobalIndex[oPipId] = zCtrlContext.woExeGlobalIndex;
			//全部赋值
			zWorkOrder.oPkgEleProgCtrl[oPipId] = DLK_INPUT_PIPE_MAX;
			//工单往下执行
			zCtrlContext.woExeGlobalIndex++;
			zCtrlContext.staMatTtt += DLK_INPUT_PIPE_MAX;
			printf("ALGO One package searched! Progress = %d, OutputPipId=%d, WoIndex = %d, Steps remaining=%d/%d/%d/%d, Current outputPort=%d\n", zDlkTickCnt, oPipId, zCtrlContext.woExeGlobalIndex-1,\
					result[0], result[1], result[2], result[3], zCtrlContext.outputPort[oPipId]);
		}
	}//所有的输出通道均搜索一遍
}

//打印
void global_state_update(void)
{
	int i=0, tmp=0;

	printf("-------------------------------\n");

#if 0
	//打印各物料的最新进料状态
	printf("PROG1: Tick=%d, Input Material in PortA=%d, PortB=%d, PortC=%d, PortD=%d\n", zDlkTickCnt, zCtrlContext.inputFlCur[0], zCtrlContext.inputFlCur[1], zCtrlContext.inputFlCur[2], zCtrlContext.inputFlCur[3]);
	//打印料斗状态
	printf("PROG2: MAIN BUF Index=%d, BUF=[%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/]\n", zDlkTickCnt, \
				zCtrlContext.hop[0].state, zCtrlContext.hop[1].state, zCtrlContext.hop[2].state, zCtrlContext.hop[3].state,\
				zCtrlContext.hop[4].state, zCtrlContext.hop[5].state, zCtrlContext.hop[6].state, zCtrlContext.hop[7].state, \
				zCtrlContext.hop[8].state, zCtrlContext.hop[9].state, zCtrlContext.hop[10].state, zCtrlContext.hop[11].state, \
				zCtrlContext.hop[12].state, zCtrlContext.hop[13].state, zCtrlContext.hop[14].state, zCtrlContext.hop[15].state, \
				zCtrlContext.hop[16].state, zCtrlContext.hop[17].state, zCtrlContext.hop[18].state, zCtrlContext.hop[19].state);
	//打印各个输入输出端口的位置信息
	printf("PROG3: PORT PROGRESS: Index=%d, 4Input=%d/%d/%d/%d, 6Output=%d/%d/%d/%d/%d/%d, Rubbish=%d\n", zDlkTickCnt, zCtrlContext.inputPort[0], zCtrlContext.inputPort[1], \
				zCtrlContext.inputPort[2], zCtrlContext.inputPort[3], zCtrlContext.outputPort[0], zCtrlContext.outputPort[1], zCtrlContext.outputPort[2], zCtrlContext.outputPort[3],\
				zCtrlContext.outputPort[4], zCtrlContext.outputPort[5], zCtrlContext.rubbishPort);

	//打印各个输出料斗的PkgNbr情况
	printf("PROG4: WO PkgCtrl Remaining = %d/%d/%d/%d/%d/%d\n", zWorkOrder.oPkgEleProgCtrl[0], zWorkOrder.oPkgEleProgCtrl[1], zWorkOrder.oPkgEleProgCtrl[2], zWorkOrder.oPkgEleProgCtrl[3], zWorkOrder.oPkgEleProgCtrl[4], zWorkOrder.oPkgEleProgCtrl[6]);
#endif

	//打印物料指示器
	printf("PROG5: Output Remaining steps control, woOutExcuting=[%d/%d/%d/%d/%d/%d], O0[%d/%d/%d/%d-%d-%d-%d], O1[%d/%d/%d/%d-%d-%d-%d], O2[%d/%d/%d/%d-%d-%d-%d], O3[%d/%d/%d/%d-%d-%d-%d], O4[%d/%d/%d/%d-%d-%d-%d], O5[%d/%d/%d/%d-%d-%d-%d]\n", \
		zCtrlContext.woRecGlobalIndex[0], zCtrlContext.woRecGlobalIndex[1], zCtrlContext.woRecGlobalIndex[2], zCtrlContext.woRecGlobalIndex[3], zCtrlContext.woRecGlobalIndex[4], zCtrlContext.woRecGlobalIndex[5],\
		zWorkOrder.woOutHopRemSteps[0][0], zWorkOrder.woOutHopRemSteps[0][1], zWorkOrder.woOutHopRemSteps[0][2], zWorkOrder.woOutHopRemSteps[0][3], zWorkOrder.oPkgEleProgCtrl[0], zCtrlContext.outputPort[0], zCtrlContext.woRecGlobalIndex[0], \
		zWorkOrder.woOutHopRemSteps[1][0], zWorkOrder.woOutHopRemSteps[1][1], zWorkOrder.woOutHopRemSteps[1][2], zWorkOrder.woOutHopRemSteps[1][3], zWorkOrder.oPkgEleProgCtrl[1], zCtrlContext.outputPort[1], zCtrlContext.woRecGlobalIndex[1], \
		zWorkOrder.woOutHopRemSteps[2][0], zWorkOrder.woOutHopRemSteps[2][1], zWorkOrder.woOutHopRemSteps[2][2], zWorkOrder.woOutHopRemSteps[2][3], zWorkOrder.oPkgEleProgCtrl[2], zCtrlContext.outputPort[2], zCtrlContext.woRecGlobalIndex[2], \
		zWorkOrder.woOutHopRemSteps[3][0], zWorkOrder.woOutHopRemSteps[3][1], zWorkOrder.woOutHopRemSteps[3][2], zWorkOrder.woOutHopRemSteps[3][3], zWorkOrder.oPkgEleProgCtrl[3], zCtrlContext.outputPort[3], zCtrlContext.woRecGlobalIndex[3], \
		zWorkOrder.woOutHopRemSteps[4][0], zWorkOrder.woOutHopRemSteps[4][1], zWorkOrder.woOutHopRemSteps[4][2], zWorkOrder.woOutHopRemSteps[4][3], zWorkOrder.oPkgEleProgCtrl[4], zCtrlContext.outputPort[4], zCtrlContext.woRecGlobalIndex[4], \
		zWorkOrder.woOutHopRemSteps[5][0], zWorkOrder.woOutHopRemSteps[5][1], zWorkOrder.woOutHopRemSteps[5][2], zWorkOrder.woOutHopRemSteps[5][3], zWorkOrder.oPkgEleProgCtrl[5], zCtrlContext.outputPort[5], zCtrlContext.woRecGlobalIndex[5]\
		);
#if 0
	//打印物料状态
	char stmp[40], sOutput[1000];
	memset(sOutput, 0, sizeof(sOutput));
	strcpy(sOutput, "PROG6: BUF ALLOCATE Status, Index-Mat-LiefRem/oPort/woNbr, ");
	int i=0;
	for (i=0; i<DLK_MAIN_BUF; i++){
		if (zCtrlContext.hop[i].state == DLK_STATE_ALLOC){
			memset(stmp, 0, sizeof(stmp));
			sprintf(stmp, "[%d-%d/%d-%d/%d/%d]", i, zCtrlContext.hop[i].mat.productCat, zCtrlContext.hop[i].mat.productInd, \
					zCtrlContext.hop[i].lifeRemain,zCtrlContext.hop[i].outputPort, zCtrlContext.hop[i].woNbr);
			strcat(sOutput, stmp);
		}
	}
	strcat(sOutput, " \n");
	printf("%s", sOutput);
#endif

	//打印统计信息，包括BUF满率
	printf("PROG7: Tick=%d, Sta MatIncoming=%d, Ttt=%d, Tgv=%d, CombPkgTimes=%d, Main Buffer Empty ratio = %6.2f%%\n", \
			zDlkTickCnt, zCtrlContext.staMatInc, zCtrlContext.staMatTtt, zCtrlContext.staMatTgv, zCtrlContext.staCombPkgTimes, ((float)zCtrlContext.staMbEmpty*100/200.0));

	//缺料信息
#if 0
	printf("PROG8: Lacking Mat status, prdCat/prdType= I0[%d/%d/%d/%d/%d] I1[%d/%d/%d/%d/%d] I2[%d/%d/%d/%d/%d] I3[%d/%d/%d/%d/%d].\n", \
		zCtrlContext.lackMat[0][0], zCtrlContext.lackMat[0][1], zCtrlContext.lackMat[0][2], zCtrlContext.lackMat[0][3], zCtrlContext.lackMat[0][4], \
		zCtrlContext.lackMat[1][0], zCtrlContext.lackMat[1][1], zCtrlContext.lackMat[1][2], zCtrlContext.lackMat[1][3], zCtrlContext.lackMat[1][4], \
		zCtrlContext.lackMat[2][0], zCtrlContext.lackMat[2][1], zCtrlContext.lackMat[2][2], zCtrlContext.lackMat[2][3], zCtrlContext.lackMat[2][4], \
		zCtrlContext.lackMat[3][0], zCtrlContext.lackMat[3][1], zCtrlContext.lackMat[3][2], zCtrlContext.lackMat[3][3], zCtrlContext.lackMat[3][4]\
		);
#endif

	//EXIT CONTROL
	if (zDlkTickCnt >= DLK_INPUT_PKG_MAX)
	exit(1);
}


//int ML=16;
//int SN=16;
//int NBR=10;
//void global_search_max_split_len(void)
//{
//	int i=0, j=0, k=0, l=0;
//	unsigned char S[ML][ML];
//	unsigned char search[NBR][SN];
//
//	memset(search, 0, sizeof(unsigned char)*NBR*SN);
//
//	for (i=0; i<ML; i++){
//		for (j=0; j<ML; j++){
//			S[i] =
//		}
//	}
//	printf("Test: %x/%x/%x\n", S[0], S[11], S[33]);
//
//	//搜索长度N=12的方案
//	for (i=0; i<NBR; i++){
//		for (j=0; j<SN; j++){
//
//		}
//	}
//
//
//}




//主函数
int main(int argc, char* argv[])
{
	//搜索4x4x4x2的全排列，最长段数量
	//global_search_max_split_len();


	//第一步：初始化所有的参数和全局表
	global_init_coef();
	printf("START to run!\n");

	//第二步：进入正式循环
	zDlkTickCnt = 0;

	while(1){
		usleep(DLK_TICK);
		zDlkTickCnt++;
		//端口对其
		global_move_input_output_port();
		//出料
		global_mat_outflow();
		//缺料状态刷新
		global_mat_lack_refresh();
		//进料
		global_mat_inflow();
		//算法更新
		global_algo_search();
		//打印状态
		global_state_update();
	}
}














