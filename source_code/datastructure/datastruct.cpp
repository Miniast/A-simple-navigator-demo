#include "datastruct.h"

using namespace std;

int nNode, nEdge;
vector<Node> pNode;
vector<vector<PointTable>> pTable;
vector<vector<Edge>> mymap;
vector<Bus> busTable;

static bool *myVis;
static Edge *pre;
static double *myDis;
static int *pathMethod,pathCnt;

struct myNode{
	int p;
	double w;
	myNode(int a, double b):p(a), w(b){}
	bool operator< (const myNode& b) const
	{
		return w>b.w;
	}
};
static priority_queue<myNode> sup;

void dijkstra(Person *person)
{
	myVis=new bool[nNode+10];
	//printf("nNode=%d\n", nNode);
	//cout << (long long)(myVis) << endl;
	//cout << (long long)&mymap[0][0] << endl;
	//cout << "passcnt: " << (long long)&pathCnt << endl;
	pre=new Edge[nNode+10];
	pathMethod=new int[nNode+10];
	myDis=new double[nNode+10];

	if (person->pStCamp==person->pEnCamp)
	{
		for (int i=0;i<nNode;i++) myDis[i]=1e12;
		for (int i=0;i<nNode;i++) pre[i].eStart=-1;
		//memset(myVis,0,sizeof(myVis));
		//printf("sizeof vis=%d\n", sizeof(myVis));
		for (int i=0;i<nNode;i++) myVis[i]=0;
		//for (int i=0;i < nNode; i++) printf("%d ", myVis[i]); printf("\n");

		myDis[person->pStId]=0;
		sup.push(myNode(person->pStId,0));
		while (!sup.empty())
		{
			myNode front=sup.top();
			sup.pop();
			int tempv=front.p;
			//printf("tempv %d myVis=%d\n", tempv,myVis[tempv]);
			if (myVis[tempv]) continue;
			myVis[tempv]=1;
			
			for (int i=0;i<mymap[tempv].size();i++)
			{
				Edge e=mymap[tempv][i];
				double v=e.dis;
				if (person->pStrategy==1)
				{
					if (person->pType==0) v/=SPEEDFOOT;
					else if (person->pType==1) v/=SPEEDBICYCLE;
					v/=1-e.eCrowd;
				}
				int p=e.eEnd;
				//printf("to=%d v=%lf\n", p, v);
				//printf("myVis=%d distemp=%lf disp=%lf\n", myVis[p], myDis[tempv], myDis[p]);
				if ((!myVis[p])&&myDis[tempv]+v<myDis[p]&&e.eType>=person->pType)
				{
					myDis[p]=myDis[tempv]+v;
					pre[p]=e;
					sup.push(myNode(p,myDis[p]));
					//printf("p in! %d\n", p);
				}
			}
		}
		pathCnt=0;
		int ntmp=person->pEnId;
		while (ntmp!=person->pStId)
		{
			//printf("%d\n",ntmp) ;
			pathMethod[pathCnt++]=ntmp;
			ntmp=pre[ntmp].eStart;
			if (ntmp==-1) break;
		}
		//printf("ntmp=%d\n",ntmp);
		if (ntmp!=-1&&pathCnt!=0)
		{
			person->pLen=pathCnt;
			//printf("pathCnt=%d\n",pathCnt);
			person->phead=new Line;
			Line* tmp=person->phead;
			tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
			tmp->lStId=person->pStId;
			tmp->lEnId=pathMethod[pathCnt-1];
			tmp->nowTime=tmp->nowDis=myDis[tmp->lEnId];
			
			if (person->pStrategy==0)
			{
				if (person->pType==0) tmp->nowTime/=SPEEDFOOT;
				else if (person->pType==1) tmp->nowTime/=SPEEDBICYCLE;
				tmp->nowTime/=1-pre[tmp->lEnId].eCrowd;
			}
			else
			{
				if (person->pType==0) tmp->nowDis*=SPEEDFOOT;
				else if (person->pType==1) tmp->nowDis*=SPEEDBICYCLE;
				tmp->nowDis*=1-pre[tmp->lEnId].eCrowd;
			}
			tmp->lType=pre[tmp->lEnId].eType;
			
			double lastTime=tmp->nowTime;
			double lastDis=tmp->nowDis;
			for (int i=pathCnt-1;i>0;i--)
			{
				tmp->lNext=new Line;
				tmp=tmp->lNext;
				tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
				tmp->lStId=pathMethod[i];
				tmp->lEnId=pathMethod[i-1];
				if (person->pStrategy==1)
				{
					tmp->nowTime=(lastTime+=pre[tmp->lEnId].dis);
					double tmpDis;
					if (person->pType==0) tmpDis=pre[tmp->lEnId].dis*SPEEDFOOT;
					else if (person->pType==1) tmpDis=pre[tmp->lEnId].dis*SPEEDBICYCLE;
					tmpDis*=1-pre[tmp->lEnId].eCrowd;
					tmp->nowDis=(lastDis+=tmpDis);
				}
				else
				{
					tmp->nowDis=(lastDis+=pre[tmp->lEnId].dis);
					double tmpTime;
					if (person->pType==0) tmpTime=pre[tmp->lEnId].dis/SPEEDFOOT;
					else if (person->pType==1) tmpTime=pre[tmp->lEnId].dis/SPEEDBICYCLE;
					tmpTime/=1-pre[tmp->lEnId].eCrowd;
					tmp->nowTime=(lastTime+=tmpTime);
				}
				tmp->lType=pre[tmp->lEnId].eType;
			}
			
			//tmp=person->phead;
			//while (tmp->lEnId!=person->pEnId) printf("end=%d\n",tmp->lEnId),tmp=tmp->lNext;
		}
		else
		{
			person->pLen=-1;
			person->phead=NULL;
		}
	}
	else
	{
		for (int i=0;i<nNode;i++) myDis[i]=1e12;
		//memset(myVis,0,sizeof(myVis));
		for (int i=0;i<nNode;i++) myVis[i]=0;
		for (int i=0;i<nNode;i++) pre[i].eStart=-1; 
		pre[person->pStId].eStart=person->pStId;
		
		myDis[person->pStId]=0;
		sup.push(myNode(person->pStId,0));
		while (!sup.empty())
		{
			myNode front=sup.top();
			sup.pop();
			int tempv=front.p;
			if (myVis[tempv]) continue;
			myVis[tempv]=1;
			for (int i=0;i<mymap[tempv].size();i++)
			{
				Edge e=mymap[tempv][i];
				double v=e.dis;
				if (person->pStrategy==1)
				{
					if (person->pType==0) v/=SPEEDFOOT;
					else if (person->pType==1) v/=SPEEDBICYCLE;
					v/=1-e.eCrowd;
				}
				int p=e.eEnd;
				if (!myVis[p]&&myDis[tempv]+v<myDis[p]&&e.eType>=person->pType)
				{
					myDis[p]=myDis[tempv]+v;
					pre[p]=e;
					sup.push(myNode(p,myDis[p]));
				}
			}
		}
		
		pathCnt=0;
		int ntmp;
		if (person->pStCamp==0) ntmp=DOOR0; else ntmp=DOOR1;
		while (ntmp!=person->pStId)
		{
			//printf("ntmp=%d\n",ntmp); 
			pathMethod[pathCnt++]=ntmp;
			ntmp=pre[ntmp].eStart;
			if (ntmp==-1) break;
		}
		//printf("ntmp=%d\n",ntmp);
		if (ntmp==-1)
		{
			person->pLen=-1;
			person->phead=NULL;
			delete[] myVis;
			delete[] pre;
			delete[] pathMethod;
			delete[] myDis;
			return;
		}
		person->pLen=pathCnt;
		person->phead=new Line;
		Line* tmp=person->phead;
		if (pathCnt==0)
		{
			person->pLen=1;
			tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
			tmp->lStId=tmp->lEnId=person->pStId;
			tmp->nowTime=tmp->nowDis=0;
			tmp->lType=1;
		}
		else
		{
			tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
			tmp->lStId=person->pStId;
			tmp->lEnId=pathMethod[pathCnt-1];
			tmp->nowTime=tmp->nowDis=myDis[tmp->lEnId];
			if (person->pStrategy==0)
			{
				if (person->pType==0) tmp->nowTime/=SPEEDFOOT;
				else if (person->pType==1) tmp->nowTime/=SPEEDBICYCLE;
				tmp->nowTime/=1-pre[tmp->lEnId].eCrowd;
			}
			else
			{
				if (person->pType==0) tmp->nowDis*=SPEEDFOOT;
				else if (person->pType==1) tmp->nowDis*=SPEEDBICYCLE;
				tmp->nowDis*=1-pre[tmp->lEnId].eCrowd;
			}
			tmp->lType=pre[tmp->lEnId].eType;
		}
		double lastTime=tmp->nowTime;
		double lastDis=tmp->nowDis;
		for (int i=pathCnt-1;i>0;i--)
		{
			tmp->lNext=new Line;
			tmp=tmp->lNext;
			tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
			tmp->lStId=pathMethod[i];
			tmp->lEnId=pathMethod[i-1];
			if (person->pStrategy==1)
			{
				tmp->nowTime=(lastTime+=pre[tmp->lEnId].dis);
				double tmpDis;
				if (person->pType==0) tmpDis=pre[tmp->lEnId].dis*SPEEDFOOT;
				else if (person->pType==1) tmpDis=pre[tmp->lEnId].dis*SPEEDBICYCLE;
				tmpDis*=1-pre[tmp->lEnId].eCrowd;
				tmp->nowDis=(lastDis+=tmpDis);
			}
			else
			{
				tmp->nowDis=(lastDis+=pre[tmp->lEnId].dis);
				double tmpTime;
				if (person->pType==0) tmpTime=pre[tmp->lEnId].dis/SPEEDFOOT;
				else if (person->pType==1) tmpTime=pre[tmp->lEnId].dis/SPEEDBICYCLE;
				tmpTime/=1-pre[tmp->lEnId].eCrowd;
				tmp->nowTime=(lastTime+=tmpTime);
			}
			tmp->lType=pre[tmp->lEnId].eType;
		}
		Bus goodBus;
		goodBus.bStartTime=99999999;
		for (int i=0;i<busTable.size();i++)
		{
			if (busTable[i].bStCamp==person->pStCamp&&busTable[i].bStartTime>=person->pStartTime+tmp->nowTime)
			{
				if(busTable[i].bType==0&&busTable[i].bStartTime<=goodBus.bStartTime)
					goodBus=busTable[i];
				else if(busTable[i].bType==1&&busTable[i].bStartTime<goodBus.bStartTime)
					goodBus=busTable[i];
				
			}
		}
		//printf("bustime %lf\n",goodBus.bStartTime);
		//printf("ok\n");
		if (goodBus.bStartTime==99999999)
		{
			person->pLen=-1;
			person->phead=NULL;
			delete[] myVis;
			delete[] pre;
			delete[] pathMethod;
			delete[] myDis;
			return;
		}
		tmp->lNext=new Line;
		tmp=tmp->lNext;
		tmp->lType=FOOT;
		if (person->pStCamp==0) tmp->lStId=DOOR0; else tmp->lStId=DOOR1;
		if (person->pStCamp==0) tmp->lEnId=DOOR0; else tmp->lEnId=DOOR1;
		tmp->nowTime=(lastTime=goodBus.bStartTime-person->pStartTime);
		//printf("tmp->nowTime=%lf\n",tmp->nowTime); 
		tmp->nowDis=lastDis;
		tmp->lStCamp=tmp->lEnCamp=person->pStCamp;
		tmp->lType=FOOT;
		
		tmp->lNext=new Line;
		tmp=tmp->lNext;
		tmp->lStCamp=person->pStCamp;
		tmp->lEnCamp=person->pEnCamp;
		if (person->pStCamp==0) tmp->lStId=DOOR0,tmp->lEnId=DOOR1;
		else tmp->lStId=DOOR1,tmp->lEnId=DOOR0;
		if (goodBus.bType==0) tmp->nowTime=(lastTime+=goodBus.dis/SPEEDSCHOOLBUS);
		else tmp->nowTime=(lastTime+=goodBus.dis/SPEEDBUS);
		tmp->nowDis=(lastDis+=goodBus.dis);
		tmp->lType=BUS;
		person->pLen+=2;
		
		for (int i=0;i<nNode;i++) myDis[i]=1e12;
		//memset(myVis,0,sizeof(myVis));
		for (int i=0;i<nNode;i++) myVis[i]=0;
		for (int i=0;i<nNode;i++) pre[i].eStart=-1;
		
		if (person->pStCamp==0) myDis[DOOR1]=0; else myDis[DOOR0]=0;
		if (person->pStCamp==0) sup.push(myNode(DOOR1,0)); else sup.push(myNode(DOOR0,0));
		while (!sup.empty())
		{
			myNode front=sup.top();
			sup.pop();
			int tempv=front.p;
			if (myVis[tempv]) continue;
			myVis[tempv]=1;
			for (int i=0;i<mymap[tempv].size();i++)
			{
				Edge e=mymap[tempv][i];
				double v=e.dis;
				if (person->pStrategy==1)
				{
					if (person->pType==0) v/=SPEEDFOOT;
					else if (person->pType==1) v/=SPEEDBICYCLE;
					v/=1-e.eCrowd;
				}
				int p=e.eEnd;
				if (!myVis[p]&&myDis[tempv]+v<myDis[p]&&e.eType>=person->pType)
				{
					myDis[p]=myDis[tempv]+v;
					pre[p]=e;
					sup.push(myNode(p,myDis[p]));
				}
			}
		}
		
		pathCnt=0;
		ntmp=person->pEnId;
		while (ntmp!=DOOR0&&ntmp!=DOOR1)
		{
			pathMethod[pathCnt++]=ntmp;
			ntmp=pre[ntmp].eStart;
			if (ntmp==-1) break;
		}
		//tmp=person->phead;
		//while (tmp->lEnId!=person->pEnId) printf("end=%d\n",tmp->lEnId),tmp=tmp->lNext;

		if (ntmp==-1||pathCnt==0)
		{
			if (ntmp==-1)
			{
				person->pLen=-1;
				person->phead=NULL;
			}
			delete[] myVis;
			delete[] pre;
			delete[] pathMethod;
			delete[] myDis;
			return;
		}
		
		person->pLen+=pathCnt;
		tmp->lNext=new Line;
		tmp=tmp->lNext;
		tmp->lStCamp=tmp->lEnCamp=person->pEnCamp;
		if (person->pStCamp==0) tmp->lStId=DOOR1; else tmp->lStId=DOOR0;
		tmp->lEnId=pathMethod[pathCnt-1];
		if (person->pStrategy==1)
		{
			tmp->nowTime=(lastTime+=pre[tmp->lEnId].dis);
			double tmpDis;
			if (person->pType==0) tmpDis=pre[tmp->lEnId].dis*SPEEDFOOT;
			else if (person->pType==1) tmpDis=pre[tmp->lEnId].dis*SPEEDBICYCLE;
			tmpDis*=1-pre[tmp->lEnId].eCrowd;
			tmp->nowDis=(lastDis+=tmpDis);
		}
		else
		{
			tmp->nowDis=(lastDis+=pre[tmp->lEnId].dis);
			double tmpTime;
			if (person->pType==0) tmpTime=pre[tmp->lEnId].dis/SPEEDFOOT;
			else if (person->pType==1) tmpTime=pre[tmp->lEnId].dis/SPEEDBICYCLE;
			tmpTime/=1-pre[tmp->lEnId].eCrowd;
			tmp->nowTime=(lastTime+=tmpTime);
		}
		tmp->lType=pre[tmp->lEnId].eType;
		
		for (int i=pathCnt-1;i>0;i--)
		{
			tmp->lNext=new Line;
			tmp=tmp->lNext;
			tmp->lStCamp=tmp->lEnCamp=person->pEnCamp;
			tmp->lStId=pathMethod[i];
			tmp->lEnId=pathMethod[i-1];
			if (person->pStrategy==1)
			{
				tmp->nowTime=(lastTime+=pre[tmp->lEnId].dis);
				double tmpDis;
				if (person->pType==0) tmpDis=pre[tmp->lEnId].dis*SPEEDFOOT;
				else if (person->pType==1) tmpDis=pre[tmp->lEnId].dis*SPEEDBICYCLE;
				tmpDis*=1-pre[tmp->lEnId].eCrowd;
				tmp->nowDis=(lastDis+=tmpDis);
			}
			else
			{
				tmp->nowDis=(lastDis+=pre[tmp->lEnId].dis);
				double tmpTime;
				if (person->pType==0) tmpTime=pre[tmp->lEnId].dis/SPEEDFOOT;
				else if (person->pType==1) tmpTime=pre[tmp->lEnId].dis/SPEEDBICYCLE;
				tmpTime/=1-pre[tmp->lEnId].eCrowd;
				tmp->nowTime=(lastTime+=tmpTime);
			}
			tmp->lType=pre[tmp->lEnId].eType;
		}
		//tmp=person->phead;
		//while (tmp->lEnId!=person->pEnId) printf("end=%d\n",tmp->lEnId),tmp=tmp->lNext;

	}
	
	delete[] myVis;
	delete[] pre;
	delete[] pathMethod;
	delete[] myDis;
}
