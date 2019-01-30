#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <queue>
#include <stack>
#include <windows.h>
#include <time.h>

using namespace std;
class Place{
public:
    int x,y;
    Place(){}
    Place(int x,int y){this->x=x;this->y=y;}
    Place(const Place& C){x=C.x;y=C.y;}
    Place&operator=(const Place& C){
        x=C.x;
        y=C.y;
        return *this;
    }
    void Set(int x,int y){
        this->x=x;this->y=y;
    }
};

class People{
public: int x;int y;//the location of people.
        int dx;int dy;
        int id;//People's ID.
        int chooseRoute;//Recording the destination of the people.
        int steps;//Recording the distance in the path currently.
        double schedule;
public: People(const People &p){
            x=p.x;y=p.y;
            dx=p.dx;dy=p.dy;
            id=p.id;
            chooseRoute=p.chooseRoute;
            steps=p.steps;
            schedule=p.schedule;
        }
public: People(){}
public: void update(int x,int y){
            this->x=x;
            this->y=y;
        }
public: void updateDestination(int dx,int dy){
            this->dx=dx;
            this->dy=dy;
        }
};

class Passage{
public:
    int people_count=0;//记录在该点的人数
    Place route[3];//到出口的路线
    int routeLength[3];
    int routeNumber;//记录有routeNumber个出口
    Passage(int routeNumber=3){//默认寻找3条最快出口
        this->routeNumber=routeNumber;
        //route=new Place[routeNumber];
        //routeLength=new int[routeNumber];
    }
    ~Passage(){}//delete route;delete routeLength;}
};

class Exit{
public:
    int maxCount;
    int currentCount;
    int maxWindow;
    int currentWindow;
    int waittingTime;
    double danger;
    int type;
    queue <People> enterQueue;
    queue <People> exitQueue;
    Exit(int maxCount=256,int currentCount=0,int maxWindow=16,int currentWindow=16,int waittingTime=2,double danger=0.05,int type=1 ){
        this->maxCount=maxCount;
        this->currentCount=currentCount;
        this->maxWindow=maxWindow;
        this->currentWindow=currentWindow;
        this->waittingTime=waittingTime;
        this->danger=danger;
        this->type=type;
    }//带默认参数的构造函数
    double DownStairSpeed(){//返回下楼速度
        int l=8,w=4;
        double densit=currentCount/(l*w*2);
        if(densit<=1) return 1;
        else if(densit<=2) return ((0.35*(1.32-0.82*log(densit))+0.01*(3.0-0.76*densit)+0.2)*1);
        else
            return 0.37;
    }
    int ExitCount(){//因为时间间隔是以一秒为单位的所以速度即路程
        int num=currentWindow;
        int exitcount=0;
        queue <People> exitQueue;
        while(!this->exitQueue.empty()){//先将所有人在exitQueue中的人的进度增加,即出入时间减少.再进入.
            People p1=this->exitQueue.front();
            this->exitQueue.pop();
            p1.schedule-=DownStairSpeed();
            if(p1.schedule<=0)
                exitcount++;
            else
                exitQueue.push(p1);
        }
        while(!enterQueue.empty()&&num-->0){
            exitQueue.push(enterQueue.front());
            enterQueue.pop();
        }
        this->exitQueue=exitQueue;
        currentCount-=exitcount;
        return exitcount;
    }
    void Danger(){//Determine whether to stop moving
        if(type==1){
            srand(time(0));
            if(rand()*1.0/(RAND_MAX*1.0)<=danger){
                waittingTime=2;
                type=2;
            }
        }
        CongestionControl();
    }
    void CongestionControl(){
        if(currentCount>=maxCount){
            currentWindow=currentWindow/2;
            type=1;
        }
        else if(type==1){
            if(currentCount<maxCount&&currentCount>maxCount/2&&currentWindow<maxWindow/2){
                if(currentWindow+2>maxWindow/2)
                   currentWindow=maxWindow/2;
                else
                    currentWindow=currentWindow+2;//Evenly increase.
            }
            else if(currentCount<=maxCount/2)
                currentWindow=maxWindow;
        }
        else if(type==2&&waittingTime==0)
            currentWindow=2*currentWindow;//Index increase.
        else if(type==2&&waittingTime>0)
            waittingTime--;
       currentCount+=currentWindow;
    }
};

class Point{
public:
    Place *c;
    Passage* passage;
    Exit* exit;
    int flag;
    Point(){c=new Place();flag=0;}
    Point(int x,int y){c=new Place(x,y);flag=0;}
    Point(int x,int y,int flag){c=new Place(x,y);this->flag=flag;Create();}
    Point(const Point& P){c=new Place(*P.c);flag=P.flag;}//浅拷贝,只拷贝点的位置以及点的类型。
    Point&operator=(const Point& P){c=new Place(*P.c);flag=P.flag;return *this;}//浅赋值,只给点的位置以及点的类型赋值。
    ~Point(){delete c; }
    void Set(int flag){this->flag=flag; Create();}
    void Create(){
        if(flag==0)  return ;
        else if(flag==1)
            passage=new Passage();
        else if(flag==2)
            exit=new Exit();
    }
};

class Louvre{
public:
    int counts;//Number of people at a moment.
    int v=1;//The speed of people when the Louver has the count people.
    People* peo;
    Point** p;
    Point** pp;
    Place **last;
    int row=310;
    int col=700;
    //Create passage where flags are 1;
    int building[8][4]={ {0,14,0,279}, {0,74,280,479}, {295,309,0,279}, {235,309,280,479},
                         {55,74,480,699}, {235,254,480,699}, {75,234,500,519},{75,234,680,699} };
    //Has three values: the stair num and location
    int stairOneToZero[13][3]={ {1,5,90}, {2,60,298}, {9,60,458}, {12,130,512},
                                {13,160,512}, {14,245,567}, {15,245,608}, {19,70,667},
                                {22,245,440}, {23,245,392}, {25,288,321}, {30,245,321},
                                {31,241,693} };
    //Has three values: the elevator num and location
    int elevator[11][3]={ {3,66,528}, {7,175,512}, {11,50,361},{12,50,387},
                          {16,247,378}, {18,278,310}, {19,45,285}, {20,246,678},
                          {21,240,620}, {22,245,567}, {23,5,120} };
    Louvre(int counts=6000){
        //cout<<"please set a count"<<endl;
        //cin>>counts;// Input a count to counts.
        this->counts=counts;
        peo=new People[counts];
        p=new Point* [row];
        pp=new Point* [row];
        last=new Place*[310];
        //Create a two-dimensional array
        for(int i=0;i<row;i++){
            last[i]=new Place[col];
            p[i]=new Point[col];
             pp[i]=new Point[col];
            for(int j=0;j<col;j++){
                p[i][j].c->Set(i,j);
            }
        }
        for(int i=0;i<8;i++){
            for(int j=building[i][0];j<=building[i][1];j++)
                for(int k=building[i][2];k<=building[i][3];k++)
                    p[j][k].Set(1);
        }
        for(int i=0;i<13;i++){
            p[stairOneToZero[i][1]][stairOneToZero[i][2]].Set(2);
        }
        /*
        for(int i=0;i<11;i++){
            p[elevator[i][1]][elevator[i][2]].Set(3);
        }
        */
        ofstream out("out.txt");//open file to write the louver in out.txt
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                pp[i][j]=p[i][j];
                out<<p[i][j].flag<<" ";
            }
            out<<endl;
        }
        //Distributed();//Create an even distribution in the map
    }
    void Distributed(){//Create an even distribution in the map
        srand(time(0));
        int x,y;
        for(int i=0;i<counts;i++){
            do{
                peo[i].id=i;
                x=rand()*1.0/RAND_MAX*row;
                y=rand()*1.0/RAND_MAX*col;
                peo[i].update(x,y);
            }while(p[x][y].flag!=1);//Continue loop if the flag is zero.
            p[x][y].passage->people_count++;//The number of plus one at this point
        }
    }
    void Finding(){
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
                cout<<i<<" "<<j<<endl;
                if(p[i][j].flag==1){
                    //复制地图
                    for(int a=0;a<row;a++){
                        for(int b=0;b<col;b++)
                            pp[a][b]=p[a][b];
                    }
                    for(int t=0;t<3;t++)
                        bfs(i,j,t);//Find out 3 exits or stairs to choose.
                   // for(int a=0;a<row;a++)
                        //delete []pp[a];
                    //delete []pp;
                }
            }
        }
        ChoseRoute();
    }
    void bfs(int x,int y,int t){
        queue<Place> Q; while(!Q.empty()){Q.pop();}
        stack<Place> S; while(!S.empty()){S.pop();}
        int direction[4][2]={{1,0},{0,1},{0,-1},{-1,0}};
        int visited[310][700];//Is this point visited.
        //Place last[310][700];
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
                visited[i][j]=0;
            }
        }
        Q.push(Place(x,y));
        while(!Q.empty()){
            bool fl=false;
            Place k=Q.front();Q.pop();
            int num=0;
            for(int i=0; i<4; i++) {
                int next_x = k.x + direction[i][0];
                int next_y = k.y + direction[i][1];
                if(next_x>=0&&next_x<row&&next_y>=0&&next_y<col){
                    if(pp[next_x][next_y].flag!=0&&visited[next_x][next_y]==0){
                         visited[next_x][next_y]=1;
                         last[next_x][next_y]=k;
                         Q.push(Place(next_x, next_y));
                         if(pp[next_x][next_y].flag>1){//Finding the exit
                            pp[next_x][next_y].flag=1;
                            fl=true;
                            int cur_x=next_x,cur_y=next_y;
                            while(true){
                                S.push(Place(cur_x,cur_y));
                                if(cur_x==x&&cur_y==y) {break;}
                                cur_x=last[cur_x][cur_y].x;
                                cur_y=last[cur_x][cur_y].y;
                            }
                            p[x][y].passage->routeLength[t]=S.size();
                            p[x][y].passage->route[t]=Place(next_x,next_y);
                            break;
                         }
                    }
                }
            }
            if(fl==true)
                break;
        }
       // for(int i=0;i<310;i++)
         //   delete []last[i];
        //delete []last;
        int k=S.size();

    }
    void ChoseRoute(){
        for(int i=0;i<counts;i++){
            int x=peo[i].x;
            int y=peo[i].y;
            if(p[x][y].passage->people_count<=16){
                peo[i].chooseRoute=0;
                peo[i].steps=p[x][y].passage->routeLength[0];
                peo[i].updateDestination(p[x][y].passage->route[0].x,p[x][y].passage->route[0].y);
            }
            else if(p[x][y].passage->people_count>16&&p[x][y].passage->people_count<=32){
                    if( (rand()*1.0/(RAND_MAX*1.0) < p[x][y].passage->routeLength[1])/
                                                    double(p[x][y].passage->routeLength[0]+p[x][y].passage->routeLength[1]))
                    {
                        peo[i].chooseRoute=0;
                        peo[i].steps=p[x][y].passage->routeLength[0];
                        peo[i].updateDestination(p[x][y].passage->route[0].x , p[x][y].passage->route[0].y);
                    }
                    else{
                        peo[i].chooseRoute=1;
                        peo[i].steps=p[x][y].passage->routeLength[1];
                        peo[i].updateDestination(p[x][y].passage->route[1].x , p[x][y].passage->route[1].y);
                    }
            }
            else{
                double pr1,pr2,pr3;
                pr1=double(p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[2])/
                double(p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[0]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[0]);
                pr2=double(p[x][y].passage->routeLength[0]*p[x][y].passage->routeLength[2])/
                double(p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[0]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[0]);
                pr3=double(p[x][y].passage->routeLength[0]*p[x][y].passage->routeLength[1])/
                double(p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[0]*p[x][y].passage->routeLength[2]+
                        p[x][y].passage->routeLength[1]*p[x][y].passage->routeLength[0]);
                double k=rand()*1.0/(RAND_MAX*1.0);
                if(k<=pr1){
                    peo[i].chooseRoute=0;
                    peo[i].steps=p[x][y].passage->routeLength[0];
                    peo[i].updateDestination(p[x][y].passage->route[0].x , p[x][y].passage->route[0].y);
                }
                else if(k<=pr2+pr1){
                    peo[i].chooseRoute=1;
                    peo[i].steps=p[x][y].passage->routeLength[1];
                    peo[i].updateDestination(p[x][y].passage->route[1].x , p[x][y].passage->route[1].y);
                }
                else{
                    peo[i].chooseRoute=2;
                    peo[i].steps=p[x][y].passage->routeLength[2];
                    peo[i].updateDestination(p[x][y].passage->route[2].x , p[x][y].passage->route[2].y);
                }
            }
        }
    }
};
int main(){
    Louvre l;
    l.Finding();
    int time=0;
    int out_people=0;
    while(out_people<l.counts){//The cycle will be stop if the number of people going out is equal to the total number.
        /**
        我们最开始要遍历每一个人，如果该人到达目的出口则将进入出口队列等待出去。
        We start by traversing everyone.
        If the person arrives at the destination exit,
        they will enter the exit queue and wait for it.
        */
        for(int i=0;i<l.counts;i++){
            if(l.peo[i].steps==time)
                l.p[l.peo[i].dx][l.peo[i].dy].exit->enterQueue.push(l.peo[i]);//Put the i-th person in the queue.
        }
        for(int i=0;i<13;i++){
            int x=l.stairOneToZero[i][1];int y=l.stairOneToZero[i][2];
            l.p[x][y].exit->Danger();
            out_people+=l.p[x][y].exit->ExitCount();
        }
        time++;
    }
    cout<<time<<endl;
    cout<<"找好路线了"<<endl;

    return 0;
}
