#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
PwmOut pin5(D5), pin6(D6);
Ticker servo_ticker;
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(A1,A0); //tx,rx
DigitalInOut pin10(D10);
BufferedSerial xbee(D1, D0);

BBCar car(pin5, pin6, servo_ticker);

Thread t1;
EventQueue queue(64 * EVENTS_EVENT_SIZE);

char recvall[50], recvall2[50], tmp[50];
int now;
int re = 1;

void Follow();

int main(){
    xbee.set_baud(9600);
   char recv[1];
   uart.set_baud(9600);
   t1.start(callback(&queue, &EventQueue::dispatch_forever));
   queue.call(Follow);
    int a = 0;
   while(1){
      if(uart.readable()){
            uart.read(recv, sizeof(recv));
            //if (recv[0] == 'a') a = 1;
            //pc.write(recv, sizeof(recv));
            if(recv[0] == '(') now = 0;
            else if(recv[0] == ')') {
                tmp[now] = recv[0];
                if(re) strcpy(recvall, tmp);
                //if(re && a) strcpy(recvall2, tmp);
                //a = 0;
                strcpy(tmp, "");
            }
            else if (recv[0] != ',') {
                tmp[now++] = recv[0];
            }
      } 
   }
}

void Follow(){
    char n[4][4];
    int x1, x2, y1, y2;
    int tz;
    int i = 0;
    int j = 0;
    int count = 0;
    int dx, dy;
    int deg;
    int len;
    float a = 2.3;
    int turn = 0;
    float r;
    bool stop = false;
    char buff[25];
    int task = 0;
    sprintf(buff, "start 1\r\n");
    xbee.write(buff, 9);
    
    while(1){
        re = 0;
        //printf("%s\n", recvall);
        i = 0;
        tz = 0;
        len = strlen(recvall);
        if (recvall[0] == 'a'){
            i = 1;
            while(recvall[i] != ')') {
                n[0][i-1] = recvall[i];
                i++;
            }
            tz = atoi(n[0]);
            printf("tz = %d\n", tz);
        }
        else {
            i = 0;
            j = 0;
            count = 0;
            while(recvall[i] != ')') {
                if (recvall[i] == ' ') {
                    j++;
                    count = 0;
                    i++;
                }
                n[j][count] = recvall[i];
                count++;
                i++;
            }
            if (len != 0) {
                x1 = atoi(n[0]);
                y1 = atoi(n[1]);
                x2 = atoi(n[2]);
                y2 = atoi(n[3]);
                printf("%d %d %d %d\n", x1, y1, x2 ,y2);
            }
        }
        len = strlen(recvall);
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                n[i][j] = '\0';
            }
        }
        for (i = 0; i < 50; i++) {
            recvall[i] = '\0';
        
        }
        re = 1;
        dx = x1 - x2;
        dy = y1 - y2;

        if (abs(dx) + abs(dy) > 0) {
            if (x1 < 70) {
                car.turn(25, 1);
                printf("Right\n");
                ThisThread::sleep_for(50ms);
                car.stop();
            }
            else if (x1 > 90) {
                car.turn(-25, 1);
                printf("LEFT\n");
                ThisThread::sleep_for(50ms);
                car.stop();
            }
            else {
                car.goStraight(35);
            }

            if (tz >= -4 && tz != 0) {
                if (task == 0) {
                    sprintf(buff, "start2\r\n");
                    xbee.write(buff, 9);
                    car.goStraight(100);
                    ThisThread::sleep_for(1500ms);
                
                    car.stop();
                    ThisThread::sleep_for(1000ms);

                    car.turn(-100, 1);
                    ThisThread::sleep_for(630ms);

                    car.goStraight(100);
                    ThisThread::sleep_for(2500ms);

                    car.stop();
                    ThisThread::sleep_for(1000ms);

                    car.turn(100, 1);
                    ThisThread::sleep_for(680ms);

                    car.goStraight(100);
                    ThisThread::sleep_for(3500ms);

                    car.stop();
                    ThisThread::sleep_for(1000ms);

                    car.turn(100, 1);
                    ThisThread::sleep_for(1000ms);

                    car.goStraight(1000);
                    ThisThread::sleep_for(1300ms);

                    car.stop();
                    ThisThread::sleep_for(1000ms);

                    car.turn(-100, 1);
                    ThisThread::sleep_for(450ms);
                    printf("detect line\n");
                    task = 1;
                    sprintf(buff, "end1\r\n");
                    xbee.write(buff, 6);
                } else if (task == 1){
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    sprintf(buff, "end all\r\n");
                    xbee.write(buff, 9);
                    task++;
                }
            }
            
        }
        else{
            car.stop();
        }
        ThisThread::sleep_for(50ms);
    }
}