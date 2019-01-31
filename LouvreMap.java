import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.File;
import java.lang.Math.*;
import java.util.concurrent.*;
import java.util.Stack;
import java.text.DecimalFormat;
import java.text.NumberFormat;
class Place{
	public int x,y;
	public Place(){}
	public Place(int x,int y){this.x=x;this.y=y;}
	public Place(Place C){x=C.x;y=C.y;}
    public void Set(int x,int y){
    	this.x=x;this.y=y;
    }
}
class People{
	public int x,y;//the location of people.
    public int dx,dy;
    public int id;//People's ID.
    public int chooseRoute;//Recording the destination of the people.
    public int steps;//Recording the distance in the path currently.
    public double schedule=16;
    public People( People p){
            x=p.x;y=p.y;
            dx=p.dx;dy=p.dy;
            id=p.id;
            chooseRoute=p.chooseRoute;
            steps=p.steps;
            schedule=p.schedule;
    }
    public People() {}
    public People(int x,int y){
    	this.x=x;
    	this.y=y;
    }
    public void update(int x,int y){
            this.x=x;
            this.y=y;
    }
    public void updateDestination(int dx,int dy){
            this.dx=dx;
            this.dy=dy;
    }
};
class Passage{
	public int people_count;//Number of people recorded at that point
    public Place []route;//Route to the exit
    public int []routeLength;
    public int routeNumber;//Recording the exit number is 'routeNumber'
    public Passage(){//Find the 3 fastest exits by default
    	people_count=0;
    	routeNumber=3;
        route=new Place[routeNumber];
        routeLength=new int[routeNumber];
        for(int i=0;i<routeNumber;i++) {
        	route[i]=new Place();
        }
    }
};
class Exit{
    public int maxCount;//The maximum number of people in the stair
    public int currentCount;//Current number of stair
    public int maxWindow;
    public int currentWindow;
    public int waittingTime;//Waiting time after an accident
    public double danger;//The facor of accident
    public int type;
    public LinkedBlockingQueue <People> enterQueue;//Line up
    public LinkedBlockingQueue <People> exitQueue;//People on the stairs
    public Exit(){
    	maxCount=256;
    	currentCount=0;
    	maxWindow=8;
    	currentWindow=8;
    	waittingTime=2;
    	danger=0.05;
    	type=1;
    	enterQueue=new LinkedBlockingQueue <People>();
    	exitQueue=new LinkedBlockingQueue <People>();
    }
    public double DownStairSpeed(){//Return to the downstairs speed
        int l=8,w=4;
        double densit=(double)(currentCount)/(l*w*2.0);
        if(densit<=1) return 1;
        else if(densit<=2) return ((0.35*(1.32-0.82*Math.log(densit))+0.01*(3.0-0.76*densit)+0.2)*1);
        else
            return 0.37;
    }
    public int ExitCount() {//throws Exception  Because the time interval is in one second, so the speed is the distance
        int num=currentWindow;
        int exitcount=0;
        LinkedBlockingQueue <People> exitQueue=new LinkedBlockingQueue <People>();
        if(!this.exitQueue.isEmpty()) {
        	for(People people:this.exitQueue) {
        		people.schedule=people.schedule-DownStairSpeed();
        		if(people.schedule<=0)
        			exitcount++;
        		else
        			exitQueue.offer(people);
        	}
        }
        while(!enterQueue.isEmpty()&&num-->0){
            exitQueue.offer(enterQueue.remove());
        }
        this.exitQueue=exitQueue;
        currentCount=currentCount-exitcount;
        return exitcount;
    }
    public void Danger(){//Determine whether to stop moving
        if(type==1){
            if(Math.random()*1.0<=danger){
                waittingTime=3;
                type=2;
                currentWindow=1;
            }
        }
        CongestionControl();
    }
    public void CongestionControl(){
        if(type==1){
            if(currentCount==maxCount){
                currentWindow=maxWindow/4;
            }
            else if(currentCount>maxCount) {
            	type=2; currentWindow=1;
            	waittingTime=3;
            }
            else if(currentCount<=maxCount/2)
                currentWindow=maxWindow;
        }
        else if(type==2&&waittingTime==0) {
        	if(currentWindow<maxWindow/2)
        		currentWindow=2*currentWindow;//Index increase.
        	else {
        		 if(currentWindow+2>maxWindow){
        			 currentWindow=maxWindow/2;
        			 type=1;
        		 }
                 else
                     currentWindow=currentWindow+2;//Evenly increase.
        	}
        }
        else if(type==2&&waittingTime>0){
            waittingTime--;
            currentWindow=1;
        }
       currentCount=currentCount+currentWindow;
    }
}

class Point{
    public Place c;
    public Passage passage;
    public Exit exit;
    public int flag;
    public Point(){c=new Place();flag=0;}
    public Point(int x,int y){c=new Place(x,y);flag=0;}
    public Point(int x,int y,int flag){c=new Place(x,y);this.flag=flag;Create();}
    public Point(Point P){c=new Place(P.c);flag=P.flag;}//Shallow copy, copy only the position of the point and the type of point.
    //public void finalize() {c=null;passage=null;exit=null;}	
    public void Set(int flag){this.flag=flag; Create();} 
    public void Create(){
        if(flag==0)  return ;
        else if(flag==1)
            passage=new Passage();
        else if(flag==2)
            exit=new Exit();
    }
}

class Louvre{
	public int counts;//Number of people at a moment.
    public int v=1;//The speed of people when the Louver has the count people.
    public People []peo;
    public Point [][]p;
    Point [][]pp;
    Place [][]last;
    public int row=310;
    public int col=700;
    //Create passage where flags are 1;
    public int [][]building={ {0,14,0,279}, {0,74,280,479}, {295,309,0,279}, {235,309,280,479},
                         {55,74,480,699}, {235,254,480,699}, {75,234,500,519},{75,234,680,699} };
    //Has three values: the stair num and location
    public int [][]stairOneToZero={ {1,5,90}, {2,60,298}, {9,60,458}, {12,130,512},
                                {13,160,512}, {14,245,567}, {15,245,608}, {19,70,667},
                                {22,245,440}, {23,245,392}, {25,288,321}, {30,245,321},
                                {31,241,693} };
    //Has three values: the elevator num and location
    public int [][]elevator={ {3,66,528}, {7,175,512}, {11,50,361},{12,50,387},
                          {16,247,378}, {18,278,310}, {19,45,285}, {20,246,678},
                          {21,240,620}, {22,245,567}, {23,5,120} };
    public Louvre(){
        counts=6000;
        peo=new People[counts];
        p=new Point[row][col];
        //Create a two-dimensional array
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                p[i][j]=new Point(i,j);
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
        }*/
        //open file to write the louver in out.txt
        int cc=counts;
        while(--cc>=0) {
        	do {
        		peo[cc]=new People((int)(Math.random()*row),(int)(Math.random()*col));
        	}while(p[peo[cc].x][peo[cc].y].flag!=1);
        	p[peo[cc].x][peo[cc].y].passage.people_count++;
        }
        for(int i=0;i<row;i++){
        	for(int j=0;j<col;j++){
        		if(p[i][j].flag==1) {
        			p[i][j].passage.people_count=p[i][j].passage.people_count*20;
        			//System.out.print(p[i][j].passage.people_count+" ");
                 }
            }
        }
        //Distributed();//Create an even distribution in the map
    }
    public void Distributed(){//Create an even distribution in the map
        int x,y;
        for(int i=0;i<counts;i++){
            do{
                peo[i].id=i;
                x=(int)(Math.random()*1.0*row);
                y=(int)(Math.random()*1.0*col);
                peo[i].update(x,y);
            }while(p[x][y].flag!=1);//Continue loop if the flag is zero.
            p[x][y].passage.people_count++;//The number of plus one at this point
        }
    }
    public void Finding(int i,int j){
                if(p[i][j].flag==1){
                    Point [][]pp=new Point[row][col];//Copying map
                    for(int a=0;a<row;a++){
                        for(int b=0;b<col;b++)
                            pp[a][b]=new Point(p[a][b]);
                    }
                    for(int t=0;t<3;t++)
                        bfs(i,j,pp,t);//Find out 3 exits or stairs to choose.
                    for(int a=0;a<row;a++){
                        for(int b=0;b<col;b++)
                            pp[a][b]=null;
                    }
                    pp=null;
                    //System.gc();
            }
    }
    public void bfs(int x,int y,Point [][]pp,int t){
    	LinkedBlockingQueue <Place> Q=new LinkedBlockingQueue <Place>();
        Stack<Place> S=new Stack<Place>(); 
        int [][]direction={{1,0},{0,1},{0,-1},{-1,0}};
        int [][]visited=new int[row][col];//Is this point visited.
        Place [][]last=new Place[row][col];
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
                visited[i][j]=0;
                last[i][j]=new Place();
            }
        }
        Q.offer(new Place(x,y));
        while(!Q.isEmpty()){
            boolean fl=false;
            Place k=new Place(Q.remove());
            int num=0;
            for(int i=0; i<4; i++) {
                int next_x = k.x + direction[i][0];
                int next_y = k.y + direction[i][1];
                if(next_x>=0&&next_x<row&&next_y>=0&&next_y<col){
                    if(pp[next_x][next_y].flag!=0&&visited[next_x][next_y]==0){
                         visited[next_x][next_y]=1;
                         last[next_x][next_y]=new Place(k);
                         Q.add(new Place(next_x, next_y));
                         if(pp[next_x][next_y].flag>1){//Finding the exit
                            pp[next_x][next_y].flag=1;
                            fl=true;
                            int cur_x=next_x;int cur_y=next_y;  
                            while(true){
                                S.push(new Place(cur_x,cur_y));
          
                                if((x==cur_x)&&(cur_y==y)) {break;}
                                int xx=cur_x;
                                cur_x=last[cur_x][cur_y].x;
                                cur_y=last[xx][cur_y].y;
                            }
                            p[x][y].passage.routeLength[t]=S.size();
                            p[x][y].passage.route[t]=new Place(next_x,next_y);
                            //System.out.println(S.size()+" "+next_x+" "+next_y);
                            break;
                         }
                    }
                }
            }
            if(fl==true) break;
        }
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
                last[i][j]=null;
            }
        }
        Q.clear();S.clear();
        visited=null; last=null;Q=null;S=null;
    }
    public void ChoseRoute(){
        for(int i=0;i<counts;i++){
            int x=peo[i].x;
            int y=peo[i].y;
            if(p[x][y].passage.people_count<=16){
                peo[i].chooseRoute=0;
                peo[i].steps=p[x][y].passage.routeLength[0];
                peo[i].updateDestination(p[x][y].passage.route[0].x,p[x][y].passage.route[0].y);
            }
            else if(p[x][y].passage.people_count>16&&p[x][y].passage.people_count<=32){
                    if( Math.random()*1.0 < (p[x][y].passage.routeLength[1]/(double)(p[x][y].passage.routeLength[0]+p[x][y].passage.routeLength[1])))
                    {
                        peo[i].chooseRoute=0;
                        peo[i].steps=p[x][y].passage.routeLength[0];
                        peo[i].updateDestination(p[x][y].passage.route[0].x , p[x][y].passage.route[0].y);
                    }
                    else{
                        peo[i].chooseRoute=1;
                        peo[i].steps=p[x][y].passage.routeLength[1];
                        peo[i].updateDestination(p[x][y].passage.route[1].x , p[x][y].passage.route[1].y);
                    }
            }
            else{
                double pr1,pr2,pr3;
                pr1=(double)(p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[2])/
                (double)(p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[0]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[0]);
                pr2=(double)(p[x][y].passage.routeLength[0]*p[x][y].passage.routeLength[2])/
                (double)(p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[0]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[0]);
                pr3=(double)(p[x][y].passage.routeLength[0]*p[x][y].passage.routeLength[1])/
                (double)(p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[0]*p[x][y].passage.routeLength[2]+
                        p[x][y].passage.routeLength[1]*p[x][y].passage.routeLength[0]);
                double k=Math.random()*1.0;
                if(k<=pr1){
                    peo[i].chooseRoute=0;
                    peo[i].steps=p[x][y].passage.routeLength[0];
                    peo[i].updateDestination(p[x][y].passage.route[0].x , p[x][y].passage.route[0].y);
                }
                else if(k<=pr2+pr1){
                    peo[i].chooseRoute=1;
                    peo[i].steps=p[x][y].passage.routeLength[1];
                    peo[i].updateDestination(p[x][y].passage.route[1].x , p[x][y].passage.route[1].y);
                }
                else{
                    peo[i].chooseRoute=2;
                    peo[i].steps=p[x][y].passage.routeLength[2];
                    peo[i].updateDestination(p[x][y].passage.route[2].x , p[x][y].passage.route[2].y);
                }
            }
        }
    }
}
public class LouvreMap{
	public static void main(String []args){
		System.out.println("This program is slow, please be patient!!!");
		Louvre l=new Louvre();
        for(int i=0;i<310;i++){
            for(int j=0;j<700;j++){
            	l.Finding(i,j);
            }
    		DecimalFormat df = new DecimalFormat("0.00%");
    		String r = df.format((i+1)*1.0/(310*1.0));
    		System.out.println(r);//great
        }
        l.ChoseRoute();
        int time=0;
        int out_people=0;
        while(out_people<l.counts&&time<1200){//The cycle will be stop if the number of people going out is equal to the total number.
        	/**
                        我们最开始要遍历每一个人，如果该人到达目的出口则将进入出口队列等待出去。
            We start by traversing everyone.
            If the person arrives at the destination exit,
            they will enter the exit queue and wait for it.
            */
            for(int i=0;i<l.counts;i++){
                if(l.peo[i].steps==time)
                    l.p[l.peo[i].dx][l.peo[i].dy].exit.enterQueue.offer(l.peo[i]);//Put the i-th person in the queue.
            }
            for(int i=0;i<13;i++){
                int x=l.stairOneToZero[i][1];int y=l.stairOneToZero[i][2];
                l.p[x][y].exit.Danger();
                out_people=out_people+l.p[x][y].exit.ExitCount();
            }
            System.out.println(time++);
        }
        System.out.println("Everyone has escaped,and the escape time is "+time+" seconds.");
        System.out.println("So we predict that the escape time when everyone escapes will be "+ time*2.5+ "seconds");
	}
}
