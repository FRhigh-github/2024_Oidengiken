#include "mbed.h"
#include "FastPWM.h"


Serial Line(PC_12,PD_2);//Lineのシリアルピン
Serial pc(USBTX,USBRX);
Serial IR(PA_9,PA_10); //IRのシリアルピン
Serial IMU(PC_10,PC_11); //Gyroのシリアルピン
Serial OpenMV(PA_0,PA_1);

void getOpenMVdata();//シリアル通信でopenmvの値を取得する関数
struct OpenMVdata{
    char angle;
    char distance;
    short critical;
 };
int attack_critical_angle;
int attack_goal_distance;//攻撃する側の距離
int attack_OpenMVAngle;//攻撃する側のゴールの角度
int pre_attack_OpenMVAngle;//ひとつ前の攻撃する側の距離
int defence_goal_distance;//守る側のゴールの距離
int defence_OpenMVAngle;//守る側のゴールの角度
int pre_defence_OpenMVAngle;//一つ前の守り側のゴールの距離
int midpower;//機体が後ろに下がるとき、すぐに早くならずにだんだん早くすることで機体が効率的に攻撃できるようにする
void getIRdata();//ボールの角度と位置を取得する関数
struct{
    char highbyte;
    char lowbyte;
    short origin;
}IRdata;
int allpower;//機体を徐々に加速させるために必要な変数
int IRAngle;//ボールの角度を格納する変数
int pre_IRdr=0; //ひとつ前のボールとの距離(ぐちゃぐちゃになるのを抑制)
int IRdr;//ボールまでの距離を格納する変数

int sub[3];//3つ外側についているラインセンサの値を格納する配列

void getLinedata();//エンジェルラインの読み取り関数
struct{
    char highbyte;
    char lowbyte;
    short origin;
}Linedata;
int LineAngle;//エンジェルで読み取ったラインセンサの角度
int LinePowor;//ラインセンサ内での独自の力
void LineMove();//ラインセンサの角度もとにしてオムニの4輪のパワー比率を出す
double Line_rate[4];//ラインセンサから読み取った角度をもとに算出したパワー比率
double Linekesu=3;//ライン反応時の戻る力

FastPWM MotorPWM1(PB_0); //PA_10         デフェンスPB_6
FastPWM MotorPWM2(PB_1); //PB_3                   PB_3
FastPWM MotorPWM3(PB_2); // PB_5                 PA_10
FastPWM MotorPWM4(PB_10); // PB_6                  PB_5

void getIMU();//ジャイロの値を取得する関数
struct{
    char highbyte;
    char lowbyte;
    short origin;
    }IMUdata;
int Gyrodegree;//機体の角度を格納する関数
float Kp = 1.1; //3.06 2.26   //PID設定
float Kd = 8.5; //0.58
float Ki = 0.000002; //5.03
float Kp_Ac;//機体の速さが変わると向きがずれずれになるのを防ぐ
int Gyro_power = 0;//機体の向きを修正するときのモーターの力
int GyroE;
int GyroE_1;
int GyroD;

double V[4];//行列
double kesu[4][3] = {1, 1, 0.00,
                     1, -1, 0.00,
                     -1, -1, 0.00,
                     -1, 1, 0.00,
                    };
double yoso[3] ;
float pi = 3.14;//3.141592
float rad;
double Vx, Vy, L;
void Angle_Move(int degree,int power);//角度と力を入力したらその方向に進む(gyropowerの初期化が必要)
void Angle_Movesub(int degree,int power);//ラインセンサとIRの方向を合成した方向に進む
void Improveattitude(int Angle);//機体の向きをジャイロセンサをもとに修正するときのモーターのパワーを計算する
void Improveattitude_for_target(int Angle,int basic);//ゴールの角度をもとに算出するときに使用する
int derectionpower1,derectionpower2,derectionpower3,derectionpower4;
int Allp1,Allp2,Allp3,Allp4;
void Motor_Speed (int Port,int Power);//モーター関数、ポートとモータのパワーを入力する
void Motor_Stop ();//モーターのパワーに0を入力

DigitalIn ST_SW(PC_0);
DigitalIn HoldIR1(PA_7);
DigitalIn HoldIR2(PA_9);//キッカーのスペル間違えるなよ

int Hold;
bool hold_judg;//ボールをホールドしているかを判断する真偽を入れる変数
int pre_Hold;
bool ball_judg(int judg_value);//ボールを持っているかを判断する関数
void getHolddata();//ボールの値を読み取る関数


struct OpenMVdata attack;//int mainの中に入れるとグローバル変数じゃないから関数のスコープ内で使えなくなる(関数内で定義すると呼び出すときに毎回定義して時間かかる)
struct OpenMVdata defence;
int main(){
    MotorPWM1.period_us(5);//PWMの周波数を指定する
    MotorPWM2.period_us(5);
    MotorPWM3.period_us(5);
    MotorPWM4.period_us(5);
    pc.baud(115200);
    Line.baud(115200);
    IR.baud(115200);
    IMU.baud(115200);
    OpenMV.baud(115200);
    getIMU();
    getIRdata();
    getLinedata();
    Motor_Stop();
    // while(ST_SW.read()==0){
    //     //getIRdata();
    //     //printf("%d\r\n",IRdr);
    // }
    while(1) {
        getIRdata();
    
}
}

void getLinedata(){
    while(Line.getc()!=255);
    Linedata.lowbyte=Line.getc();
    Linedata.highbyte=Line.getc();
    Linedata.origin=Linedata.highbyte*256+Linedata.lowbyte;
    LinePowor=Linedata.origin%100;
    LinePowor=abs(LinePowor);
    LineAngle=Linedata.origin*0.01;
    LinePowor=LinePowor*Linekesu;
}

void getIMU(){
    while(IMU.getc()!='H');
    IMUdata.lowbyte=IMU.getc();
    IMUdata.highbyte=IMU.getc();
    IMUdata.origin=IMUdata.highbyte*256+IMUdata.lowbyte;
    //degree=IMUdata.origin-10000;
    if(IMUdata.origin>180){
        IMUdata.origin = IMUdata.origin-360;
    }
    Gyrodegree = IMUdata.origin;
    //printf("%d\r\n",IMUdata.origin);//"%d\r\n",degree
}

void getIRdata(){
    while(IR.getc()!=255);
    IRdata.lowbyte=IR.getc();
    IRdata.highbyte=IR.getc();
    IRdata.origin=IRdata.highbyte*256+IRdata.lowbyte;
    IRAngle=IRdata.origin*0.01; // /10
    IRdr=IRdata.origin%100;
    IRdr=abs(IRdr);//近いほうが大きい値が返ってくる
    
    /*if(abs(pre_IRdr-IRdr)>15){ //ローパスかけてみたけどどうすればいいかは判断してほしい!!!!!!!!!!
        IRdr=pre_IRdr*0.01+IRdr*0.99;
    }else{
        IRdr=pre_IRdr*0.8+IRdr*0.2;
    }
    pre_IRdr=IRdr;*/
    printf("%d\r\n",IRdata.origin);
}

void getOpenMVdata(){
    while(OpenMV.getc()!=255);
        attack.angle=OpenMV.getc();//角度
        attack.distance=OpenMV.getc();//距離
        defence.angle=OpenMV.getc();//角度
        defence.distance=OpenMV.getc();//距離
        attack_goal_distance=attack.distance;
        defence_goal_distance=defence.distance;
        attack_OpenMVAngle=attack.angle;
        defence_OpenMVAngle=defence.angle;
        if((attack_OpenMVAngle!=200)&&(attack_OpenMVAngle>90)){
            attack_OpenMVAngle=(attack_OpenMVAngle-90)*-1;
        }else if(attack_OpenMVAngle==200){
            attack_OpenMVAngle=0;
        }
        if((defence_OpenMVAngle!=200)&&(defence_OpenMVAngle<90)){
            defence_OpenMVAngle=(defence_OpenMVAngle+90)*-1;
        }else if(defence_OpenMVAngle==200){
            defence_OpenMVAngle=0;
        }
        if(attack_goal_distance==0){
            attack_goal_distance=200;
        }
        if(defence_goal_distance==0){
            defence_goal_distance=200;
        }
        /*
        ゴールが見えないときはgoal_distanceが200として帰ってくる
        */
}

void Improveattitude(int Angle){
    Gyro_power = 0;
    getIMU();
    //pc.printf("%d\r\n",Jyrodata.Degree);// 角度表示
    //GyroE_1 = GyroE;
    GyroE = Angle - Gyrodegree;
    //GyroI += (GyroE_1 + GyroE) / 2.0 / timer;
    GyroD=GyroE;
    GyroE_1=GyroE+GyroE_1;
    Gyro_power=(GyroE*Kp)+(GyroE_1*Ki)+((GyroD-GyroE)*Kd);
}

void Improveattitude_for_target(int Angle,int basic){//ゴールに対して姿勢制御する際に必要
    Gyro_power = 0;
    getIMU();
    //pc.printf("%d\r\n",Jyrodata.Degree);// 角度表示
    //GyroE_1 = GyroE;
    GyroE = Angle - basic;
    //GyroI += (GyroE_1 + GyroE) / 2.0 / timer;
    GyroD=GyroE;
    GyroE_1=GyroE+GyroE_1;
    Gyro_power=(GyroE*Kp)+(GyroE_1*Ki)+((GyroD-GyroE)*Kd);
}

void Motor_Speed (int Port,int Power){ //　モータ速度関数（モーター番号、速度｛-100~100｝）
        if(Power>95){//上限決める必要がある(まだ上限上げれるかもしれない)!!!!!!!!!!!!!!!!
            Power=95;
        }else if(Power<-95){
            Power=-95;
        }
        float MotorPWM=Power*0.005+0.5; //PWM 0.0~0.5~1.0   回転～停止～回転　ここ変えるなよ
        if(Port==1){
             MotorPWM1.write(MotorPWM);//マイナスはダメ
        }else if(Port==2){
             MotorPWM2.write(MotorPWM);
        }else if(Port==3){
             MotorPWM3.write(MotorPWM);
        }else if(Port==4){
             MotorPWM4.write(MotorPWM);
        }
    }
void Motor_Stop(){
        Motor_Speed(1,Gyro_power);
        Motor_Speed(2,Gyro_power);
        Motor_Speed(3,Gyro_power);
        Motor_Speed(4,Gyro_power);
    }

void LineMove(){
    getLinedata();
    Line_rate[0] = 0.00;
    Line_rate[1] = 0.00;
    Line_rate[2] = 0.00;
    Line_rate[3] = 0.00;
    rad = LineAngle * 0.017;// π/180の計算を1つにまとめた
    Vx = sin(rad);
    Vy = cos(rad);
    L = 0;
    yoso[0] = Vx;
    yoso[1] = Vy;
    yoso[2] = L;
    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j <= 2; j++) {
            Line_rate[i] = Line_rate[i] + (yoso[j] * kesu[i][j]);
        }
    }
}

bool ball_judg(int judg_value){ //関数としてボールセンサーの値をカッコに入れればあっているかあってないかだけ帰ってくる、正確な数値は使ってない
    Hold=0;
    for(int i=0;i<40000;i++){
        if(HoldIR1.read()==0){
            Hold++;
        }
        if(HoldIR2.read()==0){
            Hold++; 
        }
    }
    /*if(abs(Hold-pre_Hold)>6000){ //ローパス値が大変なことになってるから入れてみたほうがいいかも試してみて
        Hold=pre_Hold*0.1+Hold*0.9;
    }else{
        Hold=pre_Hold*0.3+Hold*0.7;
    }*/
    if(Hold>judg_value){
        hold_judg = true;
    }else{
        hold_judg = false;
    }
    //pre_Hold=Hold;
    return hold_judg;
}

void Angle_Movesub(int degree,int power){//進みたい角度とモーターの出力値を入力すればその角度に進む
    V[0] = 0.00;
    V[1] = 0.00;
    V[2] = 0.00;
    V[3] = 0.00;
    rad = degree * 0.017;// π/180の計算を1つにまとめた
    Vx = sin(rad);
    Vy = cos(rad);
    L = 0.00;
    if((sub[0]!=0)&&(Vx<0)){
        Vx=0;
    }else if((sub[1]!=0)&&(Vy<0)){
        Vy=0;
    }else if((sub[2]!=0)&&(Vx>0)){
        Vx=0;
    }
    yoso[0] = Vx;
    yoso[1] = Vy;
    yoso[2] = L;
    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j <= 2; j++) {
            V[i] = V[i] + (yoso[j] * kesu[i][j]);
        }
    }
    derectionpower1=-V[0]*power-Line_rate[0]*LinePowor;//powerの値を0にするとラインセンサの力だけで動くのラインセンサからよけようとする
    derectionpower2=-V[1]*power-Line_rate[1]*LinePowor;
    derectionpower3=-V[2]*power-Line_rate[2]*LinePowor;
    derectionpower4=-V[3]*power-Line_rate[3]*LinePowor;
    Allp1=derectionpower1+Gyro_power;
    Allp2=derectionpower2+Gyro_power;
    Allp3=derectionpower3+Gyro_power;
    Allp4=derectionpower4+Gyro_power;
    Motor_Speed(1,Allp1);
    Motor_Speed(2,-Allp2);
    Motor_Speed(3,-Allp3);
    Motor_Speed(4,Allp4);
}


void getHolddata(){
    Hold=0;
    for(int i=0;i<40000;i++){
        if(HoldIR1.read()==0){
            Hold++;
        }
        if(HoldIR2.read()==0){
            Hold++; 
        }
    }
}

void Angle_Move(int degree,int power){//進みたい角度とモーターの出力値を入力すればその角度に進む(使ってないこの関数は)
    V[0] = 0.00;
    V[1] = 0.00;
    V[2] = 0.00;
    V[3] = 0.00;
    rad = degree * pi / 180;
    Vx = sin(rad);
    Vy = cos(rad);
    L = 0.00;
    yoso[0] = Vx;
    yoso[1] = Vy;
    yoso[2] = L;
    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j <= 2; j++) {
            V[i] = V[i] + (yoso[j] * kesu[i][j]);
        }
    }
    derectionpower1=-V[0]*power;//-Li[0]*LinePowor;
    derectionpower2=-V[1]*power;//-Li[1]*LinePowor;
    derectionpower3=-V[2]*power;//-Li[2]*LinePowor;
    derectionpower4=-V[3]*power;//-Li[3]*LinePowor;
    Allp1=derectionpower1+Gyro_power;
    Allp2=derectionpower2+Gyro_power;
    Allp3=derectionpower3+Gyro_power;
    Allp4=derectionpower4+Gyro_power;
    Motor_Speed(1,Allp1);
    Motor_Speed(2,-Allp2);
    Motor_Speed(3,-Allp3);
    Motor_Speed(4,Allp4);
}
    /*
    使えるセンサ
    IR
    ライン
    ジャイロ
    カメラ
    ボールセンサ

    次の活動でやること
    ボールがないときの処理を詰める
    ・どこに移動するか(真ん中or中立点)
    姿勢制御を詰める
    白線にサイドラインセンサが反応したとき機体の動きがすべて止まる理由がわからない!!!!!!!!!!!!!!!
    デフェンスとの干渉を避ける
    ・機体を後ろまで下げすぎないようにする
    ・前もdistanceを使って進みすぎないようにする(ゴール前で機体が出ることがなくなる)
    ・四つ角に行ったボールは見逃して中立点の少し手前に移動してチャンスをうかがう


    プログラム上で意識している点
    センサデータの取得を最小限に抑えている、時間がかかってしまう要因だから
    
    問題点
    ・ゴールの端にはまったときに抜け出せなくなってボールを追わない
    ・回り込みがうまくいかない(特に180度に近いところだとボールと一緒に移動してしまい一緒に自陣のゴールに突き進む、やばい)
    ・ラインセンサの閾値の調整しないとラインから出る
    ・キッカーのシュートタイミングは(カメラの角度が-10から10度の時&&機体のホールドセンサが反応しているとき)
    打った後にホールドセンサが反応しているときは目の前に敵がいると考えて機体を傾かせてボールをルーズにする
    ・姿勢制御まだまだ詰めれる気がするからそこをやってみる

    檜垣への質問
    IRにローパスをかける必要があるか？
    ホールドセンサにローパスかける？
    姿勢制御はどう改善する？
    回り込み改善はどうすればいい？
    */

