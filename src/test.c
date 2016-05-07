#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <mcp3004.h>    //MCP3004 and MCP3008 are the same..
#include <config.h>


int main(){
    int chan;
    int x;
    printf("Raspberry Pi wiringPi test program\n");
    if(wiringPiSetup() == -1){
        exit(EXIT_FAILURE);
    }
    
    mcp3004Setup(BASE, SPI_CHAN);   //MCP3004 and MCP3008 are the same..
    
    for (chan = 0; chan < 8; ++chan){
        x = analogRead(BASE + chan);
        printf("Channel %d : %lf V\n", chan, x*3.3/1024);
    }
        exit(EXIT_SUCCESS);
}
