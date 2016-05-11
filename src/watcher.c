#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <mcp3004.h>    //MCP3004 and MCP3008 are the same..
#include <config.h>
#include <fftw3.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define REAL 0
#define IMAG 1

//Typedefs
typedef enum {    // measured and calibrated, but can be calculated
	START,
	L0,
	L1,
	L2,
	L3,
	R0,
	R1,
	R2,
	R3
} STATE_t;

// Function declarations
void term(int);
void acquire_signal(double*, int, int, int);
void analyze_spectrum(fftw_complex*, int*, int*);
void transition_fsm(STATE_t, STATE_t*, int, int);

volatile sig_atomic_t done = 0;

int main(){
	//SETUP
    struct sigaction action;
    int a = 0;	// 0 when the 125 Hz source is visible, 1 when not visible
    int b = 0;  // 0 when the 250 Hz source is visible, 1 when not visible
    int chickens_inside = 0;
    char buffer[50];
    /*Set up DAQ*/
    if(wiringPiSetup() == -1){
        exit(EXIT_FAILURE);
    }    
    mcp3004Setup(BASE, SPI_CHAN);   //MCP3004 and MCP3008 are the same..
    
    /*Set up fft plan*/
    fftw_complex* fft_result =(fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);
    assert(fft_result != NULL);
	double* fft_signal = (double*) fftw_malloc(sizeof(double)*FRAME_SIZE);
	assert(fft_signal != NULL);
	fftw_plan fft_plan = fftw_plan_dft_r2c_1d(FRAME_SIZE, fft_signal, fft_result, FFTW_MEASURE);
	assert(fft_plan != NULL);
	STATE_t new_state;
	STATE_t old_state = START;
	
	
    /* Set up a handler to break the loop when we want to shutdown. */
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    
	//LOOP
	while(!done){
		/*Acquire signal*/
		acquire_signal(fft_signal, 16, 1000, 0);
	
		/*Execute_FFT*/
		fftw_execute(fft_plan);
	
		/*Extract information from spectrum*/
		analyze_spectrum(fft_result, &a, &b);
		/*FSM transitions*/
		transition_fsm(old_state, &new_state, a, b);
	
		old_state = new_state;
		/*FSM outputs*/
		if(old_state == L3){
			chickens_inside++;
			printf("new chick, %d chicks in tha house\n", chickens_inside);
		}
		if(old_state == R3){
			if(chickens_inside == 0){
				printf("Ghost chicken left \n")
			}else{
				chickens_inside--;
				printf("Chick out to tha mall, %d chicks in da house\n", chickens_inside);
			}
		}
		snprintf(buffer, 50, "echo %d > chickens_inside",chickens_inside);
		system(buffer);
		usleep(1000);
	}
	
	//CLEAN UP
	fftw_free(fft_signal);
	fftw_free(fft_result);
    fftw_destroy_plan(fft_plan);
    fft_signal = NULL;
    fft_result = NULL;
    fft_plan = NULL;
    fftw_cleanup();
    exit(EXIT_SUCCESS);    
}

void term(int signum){
	done = 1;
}

void acquire_signal(double* signal, int length, int usleep_time, int channel){
	int i = 0;
	for(i=0; i < length; i++){
        signal[i] = analogRead(BASE + channel) * 3.3 / 1024;
        usleep(usleep_time);
	}
}

void analyze_spectrum(fftw_complex* result, int* a, int* b){
	double Pr = (M_PI * sqrt(result[3][REAL] * result[3][REAL] +
                          result[3][IMAG] * result[3][IMAG])) / (FRAME_SIZE);
	if(Pr > POWER_THRESHHOLD){
		*a = 0;
	}else{
		*a = 1;
	}
	Pr = (M_PI * sqrt(result[5][REAL] * result[5][REAL] +
                          result[5][IMAG] * result[5][IMAG])) / (FRAME_SIZE);
	if(Pr > POWER_THRESHHOLD){
		*b = 0;
	}else{
		*b = 1;
	}
}

void transition_fsm(STATE_t old_state, STATE_t* new_state, int a, int b){
	switch (old_state) {
		case START://####################################################
				if((a == 1) && (b == 0)){
					*new_state = L0;
					break;
				}
				if((a == 0) && (b == 1)){
					*new_state = R0;
					break;
				}
				*new_state = START;
				break;
		case L0://#######################################################
				if(a == 0){
					*new_state = START;
					break;
				}
				if((a == 1) && (b == 1)){
					*new_state = L1;
					break;
				}
				*new_state = L0;
				break;
		case L1://#######################################################
				if((a == 1) && (b == 0)){
					*new_state = L0;
					break;
				}
				if((a == 0) && (b == 1)){
					*new_state = L2;
					break;
				}
				*new_state = L1;
				break;
		case L2://#######################################################
				if((a == 1) && (b == 1)){
					*new_state = L1;
					break;
				}
				if((a == 0) && (b == 0)){
					*new_state = L3;
					break;
				}
				*new_state = L2;
				break;
		case L3://#######################################################
				*new_state = START;
				break;
		case R0://#######################################################
				if(b == 0){
					*new_state = START;
					break;
				}
				if((a == 1) && (b == 1)){
					*new_state = R1;
					break;
				}
				*new_state = R0;
				break;
		case R1://#######################################################
				if((a == 0) && (b == 1)){
					*new_state = R0;
					break;
				}
				if((a == 1) && (b == 1)){
					*new_state = R2;
					break;
				}
				*new_state = R1;
				break;
		case R2://#######################################################
				if((a == 1) && (b == 1)){
					*new_state = R1;
					break;
				}
				if((a == 0) && (b == 0)){
					*new_state = R3;
					break;
				}
				*new_state = R2;
				break;
		case R3://#######################################################
				*new_state = START;
				break;
		default://#######################################################
				*new_state = START;
				break;
	}
}
