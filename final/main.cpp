#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include "stdlib.h"
#include "string.h"


PwmOut pin5(D5), pin6(D6);

BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(A1,A0); //tx,rx
DigitalInOut pin10(D10);
BufferedSerial xbee(D1, D0);
Ticker servo_ticker;
BBCar car(pin5, pin6, servo_ticker);

Thread t;
EventQueue queue(64 * EVENTS_EVENT_SIZE);

char recvall[50];
char recvall2[50];
char tmp[50];
int now;

int re;

void linego(){
    char nx1[4], nx2[4], ny1[4], ny2[4];
    int x1, x2, y1, y2;
    int tz;
    int i = 0;
    int len;
    char buff[25];
    int task = 0;
    int dx, dy;
    float r;
    int temp;

    while(1){

        re = 0;
        i = 0;
        len = strlen(recvall);
       
        tz = 0;

        if (recvall[0] == '('){
            i = 1;
            while(recvall[i] != ')') {
                nx1[i-1] = recvall[i];
                i++;
            }
            tz = atoi(nx1);
        }
       
            x1 = atoi(nx1);
            y1 = atoi(ny1);
            x2 = atoi(nx2);
            y2 = atoi(ny2);
        
        
        for (i = 0; i < 4; i++) {
            nx1[i] = '\0';
            ny1[i] = '\0';
            nx2[i] = '\0';
            ny2[i] = '\0';
        }
        for (i = 0; i < 25; i++) {
            recvall[i] = '\0';
        
        }

        if (y1 > y2) {
            
            temp = x1;
            x1 = x2;
            x2 = temp;
            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        sprintf(buff, "start1\r\n");
        xbee.write(buff, 9);

        re = 1;
        dx = x1 - x2;
        dy = y1 - y2;

        if (abs(dx) + abs(dy) > 0) {

            if (x1 < 65 + task * 5 && task < 2) {
                car.turn(20, 1);
                ThisThread::sleep_for(100ms);
                
            }

            else if (x1 > 95 - task * 5 && task < 2) {
                car.turn(-20, 1);
                ThisThread::sleep_for(100ms);
                
            }
            

            if (tz >= -5 && tz != 0) {
                if (task == 0) {
                    sprintf(buff, "start2\r\n");
                    xbee.write(buff, 7);

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
                    task = 1;
                    sprintf(buff, "end 2\r\n");
                    xbee.write(buff, 7);

                } 

                else if (task == 1){
                    car.stop();
                    sprintf(buff, "end all\r\n");
                    xbee.write(buff, 9);
                    task++;
                }
            }
            
        }

        else{
            car.stop();
        }
        ThisThread::sleep_for(100ms);
    }
}


int main(){

   uart.set_baud(9600);
   xbee.set_baud(9600);
   char recv[1];

   
   t.start(callback(&queue, &EventQueue::dispatch_forever));

   queue.call(linego);

   while(1){

      if(uart.readable()){

            uart.read(recv, sizeof(recv));

            if(recv[0] == '(') {
                now = 0;
            }

            else if(recv[0] == ')') {
                tmp[now] = recv[0];

                if(re) strcpy(recvall, tmp);
                strcpy(tmp, "  ");
            }

            else if (recv[0] != ',') {
                tmp[now] = recv[0];
                now ++;
            }
      } 
   }
}
