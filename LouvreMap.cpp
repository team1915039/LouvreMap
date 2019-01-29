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

class People{
public: int x;int y;//the location of people.
        int dx;int dy;
        int number;//People's ID.
        int destination;//Recording the destination of the people.
        int steps;//Recording the distance in the path currently.
        double time;
public: People(const People &p){
            x=p.x;y=p.y;
            dx=p.dx;dy=p.dy;
            number=p.number;
            destination=p.destination;
            steps=p.steps;
            time=p.time;
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
class Point{
public:
    int x;
    int y;
    int flag;//flag has many values: 0-----No passage.
             //1-----The passage in the building.
             //2-----Stairs in the building.
             //3-----Elevator in the building.
    float* parameter;
    int people_count=-1;//The current number of people at that point
    Point** route;//Recording route to stair,route[i][j] on behalf of the point to i stair of j step.
    int WaittingTime;
    double dangerous;//Is there a danger in the stair?
    int type;//Type of congestion control
    Point(){flag=0;x=-1;y=-1;}
    Point(int x,int y){flag=0;this->x=x;this->y=y;}
    Point(const Point &ppp ){//只需要拷贝这三个就足够了.
        x=ppp.x;
        y=ppp.y;
        flag=ppp.flag;
    }
    queue<People>EnterQueue;
    queue<People>ExitQueue;
    Point &operator=(const Point &ppp){
        x=ppp.x;
        y=ppp.y;
        flag=ppp.flag;
    }
    void setup(int flag){
        this->flag=flag;
        if(flag>=1){
            route=new Point*[3];
            people_count=0;
        }
        if(flag>1){
            dangerous=0.05;
            WaittingTime=2;//The waiting time for congestion control is two seconds.
            parameter=new float[5];//parameter[0] Is the maximum number of people the elevator can be accepted.
                                   //parameter[1] Is the number of people in the current stair.
                                   //parameter[2] Is the maximum number of visitors' window.
                                   //parameter[3] Is the current number of visitors' window after control.
                                   //parameter[4] Is Downstairs time under this current number of people.
            parameter[0]=256;
            parameter[1]=0;
            parameter[2]=16;
            parameter[3]=16;
            parameter[4]=(int)DownstairsTime();//-The unit is seconds
        }
    }
    double DownstairsTime(){
        int l=8,w=4;
        double densit=parameter[1]/(l*w*2);
        if(densit<=1) return l*2/1;
        else if(densit<=2) return l*2/((0.35*(1.32-0.82*log(densit))+0.01*(3.0-0.76*densit)+0.2)*1);
        else
            return 0.37;
    }
    void Danger(){//Determine whether to stop moving
        if(type==1){
            srand(time(0));
            if(rand()*1.0/(RAND_MAX*1.0)<=dangerous){
                WaittingTime=2;
                type=2;
            }
        }
        CongestionControl();
    }
    void CongestionControl(){
        if(parameter[1]>=parameter[0]){
            parameter[3]=parameter[3]/2;
            type=1;
        }
        else if(type==1){
            if(parameter[1]<parameter[0]&&parameter[1]>parameter[0]/2&&parameter[3]<parameter[2]/2){
                if(parameter[3]+2>parameter[2]/2)
                    parameter[3]=parameter[2]/2;
                else parameter[3]=parameter[3]+2;//Evenly increase.
            }
            else if(parameter[1]<=parameter[0]/2)
                parameter[3]=parameter[2];
        }
        else if(type==2&&WaittingTime==0)
            parameter[3]=2*parameter[3];//Index increase.
        else if(type==2&&WaittingTime>0)
            WaittingTime--;
        parameter[1]+=parameter[3];
    }

};
class Louver{
public:
    int counts;//Number of people at a moment.
    int v=1;//The speed of people when the Louver has the count people.
    Point** p;
    People* peo;
    int row=310;
    int col=700;
    int building[8][4]={
                {0,14,0,279}, //Create passage where flags are 1;
                {0,74,280,479},
                {295,309,0,279},
                {235,309,280,479},
                {55,74,480,699},
                {235,254,480,699},
                {75,234,500,519},
                {75,234,680,699}
                };
    int stairOneToZero[13][3]={  //Has three values: the stair num and location
                      {1,5,90},
                      {2,60,298},
                      {9,60,458},
                      {12,130,512},
                      {13,160,512},
                      {14,245,567},
                      {15,245,608},
                      {19,70,667},
                      {22,245,440},
                      {23,245,392},
                      {25,288,321},
                      {30,245,321},
                      {31,241,693}
                      };
    int elevator[11][3]={  //Has three values: the elevator num and location
                {3,66,528},
                {7,175,512},
                {11,50,361},
                {12,50,387},
                {16,247,378},
                {18,278,310},
                {19,45,285},
                {20,246,678},
                {21,240,620},
                {22,245,567},
                {23,5,120}
                };
public: Louver(Louver &l){
            row=l.row;
            col=l.col;
            p=new Point* [row];// l is Louver's map
            for(int i=0;i<row;i++){
                p[i]=new Point[col];
            }//Create a two-dimensional array
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    p[i][j]=l.p[i][j];
                }
            }
        }
public: Louver &operator=(const Louver &l){
            row=l.row;
            col=l.col;
            p=new Point* [row];// l is Louver's map
            for(int i=0;i<row;i++){
                p[i]=new Point[col];
            }//Create a two-dimensional array
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    p[i][j]=l.p[i][j];
                }
            }
        }
public: Louver(){
        cout<<"please set a count"<<endl;
        cin>>counts;// Input a count to counts.
        peo=new People[counts];
        p=new Point* [row];// l is Louver's map
        for(int i=0;i<row;i++){
            p[i]=new Point[col];
        }//Create a two-dimensional array
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                p[i][j].x=i;
                p[i][j].y=j;
            }
        }
        for(int i=0;i<8;i++){
            for(int j=building[i][0];j<=building[i][1];j++)
                for(int k=building[i][2];k<=building[i][3];k++)
                    p[j][k].setup(1);
        }
        for(int i=0;i<13;i++){
            p[stairOneToZero[i][1]][stairOneToZero[i][2]].setup(2);
        }
        /*
        for(int i=0;i<11;i++){
            p[elevator[i][1]][elevator[i][2]].setup(3);
        }
        */
        ofstream out("out.txt");//open file to write the louver in out.txt
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++)
                out<<p[i][j].flag<<" ";
            out<<endl;
        }
        //Distributed();//Create an even distribution in the map
    }
public: void Distributed(){//Create an even distribution in the map
            srand((unsigned)time(0));
            double x,y;
            for(int i=0;i<counts;i++){
                do{
                    peo[i].number=i;
                    x=(double)rand()/RAND_MAX *row;
                    y=(double)rand()/RAND_MAX *col;
                    peo[i].update((int)x,(int)y);
                }while(p[(int)x][(int)y].flag==0);//Continue loop if the flag is zero.
                p[(int)x][(int)y].people_count++;//The number of plus one at this point
            }
        }
public: int Row(){return row;}
public: int Col(){return col;}
};

class BFS{
public:
    Louver l;
    int visited[310][700];//Is this point visited.
    int direction[8][2]={{1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}};
    bool Valid(int x, int y) {//To judge the point
        if(x<0||x>=l.Row()||y<0||y>=l.Col()) return false;
        if(l.p[x][y].flag==0||visited[x][y]==true) return false;
        return true;
    }
    void three_bfs_for_map(){  //Traverse the entire map
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
                if(l.p[i][j].flag>=1){
                    Louver *ll=new Louver(l);
                    for(int t=0;t<3;t++){
                        bfs(i,j,ll,t);//Find out 3 exits or stairs to choose.
                    }
                }
            }
        }

    }
    void bfs(int xx,int yy,Louver *ll,int t) {//t is a t-times near exit.
        queue<Point> Q;
        stack<Point> S;
        while(!Q.empty()){Q.pop();}
        while(!S.empty()){S.pop();}
        Point** last;  last=new Point*[310];
        for(int i=0;last[i]=new Point[700],i<310;i++);
        memset(visited,0,sizeof(visited));
        Q.push(Point(xx,yy));
        while(!Q.empty()) {
            bool fl=false;
            Point k=Q.front();
            Q.pop();
            for(int i=0; i<8; i++) {
                int next_x = k.x + direction[i][0];
                int next_y = k.y + direction[i][1];
                if(Valid(next_x, next_y)) {
                    visited[next_x][next_y]=1;
                    last[next_x][next_y]=k;
                    Q.push(Point(next_x, next_y));
                    if(l.p[next_x][next_y].flag>1){//Finding the exit
                        l.p[next_x][next_y].flag=1;
                        fl=true;
                        int cur_x=next_x,cur_y=next_y;
                        while(true){
                            S.push(Point(cur_x,cur_y));
                            if(cur_x==xx&&cur_y==yy) break;
                            cur_x=last[cur_x][cur_y].x;
                            cur_y=last[cur_x][cur_y].y;
                        }
                        break;
                    }
                }
            }
            if(fl==true)
                break;
        }
        cout<<S.size()<<endl;
        l.p[xx][yy].route[t]=new Point[S.size()];

        for(int i=0;i<S.size();i++){
            l.p[xx][yy].route[t][i]=S.top(); S.pop();
            cout<<l.p[xx][yy].route[t][i].x<<" "<<l.p[xx][yy].route[t][i].y<<endl;
        }
        return ;
    }

};

int main()
{
    //Louver l;
    srand(time(NULL));
    BFS b;
    b.three_bfs_for_map();
    //for(int i=0;i<sizeof(b.l.p[1][1].route[0])/sizeof(int);i++)
      //  cout<<b.l.p[1][1].route[0][i].x<<" "<<b.l.p[1][1].route[0][i].y<<endl;
    /*
    /**
    首先遍历每一个人，每个人的点可以找到对应的该点有多少人 ，然后进行分配路线。
    First traverse each person,
    each person's point can find the corresponding number of people at that point,
    and then assign the route.
    */
    /*
    for(int i=0;i<b.l.counts;i++){
        int x=b.l.peo[i].x;
        int y=b.l.peo[i].y;
        if(b.l.p[x][y].people_count<=16){
            b.l.peo[i].destination=0;
            b.l.peo[i].steps=sizeof(b.l.p[x][y].route[0])/sizeof(int)-1;
            b.l.peo[i].updateDestination(b.l.p[x][y].route[0][b.l.peo[i].steps].x,b.l.p[x][y].route[0][b.l.peo[i].steps].y);
        }
        else if(b.l.p[x][y].people_count>16&&b.l.p[x][y].people_count<=16){
                if( rand()*1.0/(RAND_MAX*1.0) < sizeof(b.l.p[x][y].route[1])/double(sizeof(b.l.p[x][y].route[0])+sizeof(b.l.p[x][y].route[1]))){
                    b.l.peo[i].destination=0;
                    b.l.peo[i].steps=sizeof(b.l.p[x][y].route[0])/sizeof(int)-1;
                    b.l.peo[i].updateDestination(b.l.p[x][y].route[0][b.l.peo[i].steps].x,b.l.p[x][y].route[0][b.l.peo[i].steps].y);
                }
                else{
                    b.l.peo[i].destination=1;
                    b.l.peo[i].steps=sizeof(b.l.p[x][y].route[1])/sizeof(int)-1;
                    b.l.peo[i].updateDestination(b.l.p[x][y].route[1][b.l.peo[i].steps].x,b.l.p[x][y].route[1][b.l.peo[i].steps].y);
                }
        }
        else{
                double pr1,pr2,pr3;
                pr1=double(sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[2]))
                /double(sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[0])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[0]));
                pr2=double(sizeof(b.l.p[x][y].route[0])*sizeof(b.l.p[x][y].route[2]))
                /double(sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[0])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[0]));
                pr3=double(sizeof(b.l.p[x][y].route[0])*sizeof(b.l.p[x][y].route[1]))
                /double(sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[0])*sizeof(b.l.p[x][y].route[2])+
                        sizeof(b.l.p[x][y].route[1])*sizeof(b.l.p[x][y].route[0]));
                double k=rand()*1.0/(RAND_MAX*1.0);
                if(k<=pr1){
                    b.l.peo[i].destination=0;
                    b.l.peo[i].steps=sizeof(b.l.p[x][y].route[0])/sizeof(int)-1;
                    b.l.peo[i].updateDestination(b.l.p[x][y].route[0][b.l.peo[i].steps].x,b.l.p[x][y].route[0][b.l.peo[i].steps].y);
                }
                else if(k<=pr2+pr1){
                    b.l.peo[i].destination=1;
                    b.l.peo[i].steps=sizeof(b.l.p[x][y].route[1])/sizeof(int)-1;
                    b.l.peo[i].updateDestination(b.l.p[x][y].route[1][b.l.peo[i].steps].x,b.l.p[x][y].route[1][b.l.peo[i].steps].y);
                }
                else{
                    b.l.peo[i].destination=2;
                    b.l.peo[i].steps=sizeof(b.l.p[x][y].route[2])/sizeof(int)-1;
                    b.l.peo[i].updateDestination(b.l.p[x][y].route[2][b.l.peo[i].steps].x,b.l.p[x][y].route[2][b.l.peo[i].steps].y);
                }
        }

    }
    int time=0;
    int out_people=0;
    while(out_people<b.l.counts){//The cycle will be stop if the number of people going out is equal to the total number.
        /**
        我们最开始要遍历每一个人，如果该人到达目的出口则将进入出口队列等待出去。
        We start by traversing everyone.
        If the person arrives at the destination exit,
        they will enter the exit queue and wait for it.
        */
        /*
        for(int i=0;i<b.l.counts;i++){
            if(b.l.peo[i].steps==time)
                b.l.p[b.l.peo[i].dx][b.l.peo[i].dy].EnterQueue.push(b.l.peo[i]);//Put the i-th person in the queue.
        }
        /**
        然后对每一个出入口遍历。
        1.更新拥塞程度
        2.更新行走时间
        3.更新楼道情况
        对每一个出口进入口和出口更新
        Then update the congestion control to update the window and other parameters.
        */
        /*
        for(int i=0;i<13;i++){
            queue<People> ExitQueue;
            int x=b.l.stairOneToZero[i][1];int y=b.l.stairOneToZero[i][2];
            b.l.p[x][y].Danger();
            double DownstairsTime=b.l.p[x][y].DownstairsTime();
            while(!b.l.p[x][y].ExitQueue.empty()){
                    b.l.p[x][y].ExitQueue.front().time-=DownstairsTime;
                    if(b.l.p[x][y].ExitQueue.front().time<=0){
                        b.l.p[x][y].ExitQueue.pop();
                        out_people++;
                    }
                    else{
                        ExitQueue.push(b.l.p[x][y].ExitQueue.front());
                        b.l.p[x][y].ExitQueue.pop();
                    }
            }
            b.l.p[x][y].ExitQueue=ExitQueue;//更新容器
            for(int j=0;j<b.l.p[x][y].parameter[2]&&(!b.l.p[x][y].EnterQueue.empty());j++){
                b.l.p[x][y].EnterQueue.front().time=time;
                b.l.p[x][y].ExitQueue.push(b.l.p[x][y].EnterQueue.front());
                b.l.p[x][y].EnterQueue.pop();
            }
        }
        time++;
    }
    cout<<--time<<endl;
    */
    return 0;
}
