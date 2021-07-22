#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "net/loramac.h"     
#include "semtech_loramac.h" 

#include "msg.h"
#include "periph/gpio.h"
#include "shell.h"
#include "thread.h"
#include "timex.h"
#include "xtimer.h"

#ifndef TTN_DEV_ID
#define TTN_DEV_ID ("safe_water")
#endif

#define DELAY 10000 

#define RECV_MSG_QUEUE (4U)

static msg_t _recv_queue[RECV_MSG_QUEUE];
static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

static semtech_loramac_t loramac; 


static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = {0x11, 0x12, 0x54, 0x33, 0x21, 0x11, 0x21, 0x22};
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01};
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = {0x55, 0x9C, 0x10, 0x94, 0x32, 0xC5, 0x66, 0x45, 0x3E, 0x8D, 0xA3, 0x2A, 0xB2, 0x24, 0x51, 0x33};


char stack_loop[THREAD_STACKSIZE_MAIN];

kernel_pid_t tmain;

int parse_command(char *command);


static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
								
static int mod_table[] = {0, 2, 1};


char *base64_encode(const char *data,size_t input_length,size_t *output_length) 
{
    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}



static void *_recv(void *arg) {
    msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
    (void)arg;
    while (1) {

        semtech_loramac_recv(&loramac);
        loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
        printf("Data received: %s, port: %d", (char *)loramac.rx_data.payload, loramac.rx_data.port);
        
        char * msg = (char *)loramac.rx_data.payload;
       
    
		if(strcmp(msg,"SERVOON")==0)			
					printf("GATE OPEN");
				
			
				
		if(strcmp(msg,"SERVOOFF")==0)
					printf("GATE CLOSE");

       
    }
    return NULL;
}


void send(char* message) {
  
    printf("Sending message '%s'\n", message);

    size_t inl = strlen(message);
    size_t output;

    char *encoded = base64_encode(message, inl, &output);
   
    uint8_t status = semtech_loramac_send(&loramac, (uint8_t *)message, strlen(message));
  
    if (status != SEMTECH_LORAMAC_TX_DONE) {
        printf("Cannot send message '%s'\n", message);
    } else {
        printf("Message '%s' sent\n", message);
    }
}

int lora_start(void) {
    dev_id = TTN_DEV_ID[strlen(TTN_DEV_ID)-1] - '0';
    printf("Dev id: %d\n", dev_id);
  
	semtech_loramac_init(&loramac);
	
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    semtech_loramac_set_dr(&loramac, 5);

    /* start the OTAA join procedure */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

    puts("Starting recv thread");
    thread_create(_recv_stack, sizeof(_recv_stack),
               THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread"); 


    return 0;
}

void createJSONwater(char *state,char*s)
{
	
		//define text to send
		char* prefix="{\"alarm\": \"";
		char* suffix="\"}";
		int i=0;
		
		for (i=0;i<(int)strlen(prefix);i++){
			s[i]=prefix[i];
		}
		
		
		for( int j=0;j<(int)strlen(state);j++){
			s[i++]=state[j];
		}
		
		for(int j=0;j<(int)strlen(suffix);j++){
			s[i++]=suffix[j];
		}
	
	
		s[i]='\0';
		
}


void createJSONtemperature(char* temp_s,char* s)
{
		//define text to send
		char* prefix="{\"temp\": \"";
		char* suffix="\"}";
		int i=0;
		
		for (i=0;i<(int)strlen(prefix);i++){
			s[i]=prefix[i];
		}
		
		
		for( int j=0;j<(int)strlen(temp_s);j++){
			s[i++]=temp_s[j];
		}
		
		for(int j=0;j<(int)strlen(suffix);j++){
			s[i++]=suffix[j];
		}
		s[i]='\0';
		
}


void *values_generation(void *arg) 
	{
    (void)arg;

    while (1) {
		
		
		char data[30];
		
		if(rand()%2==0)
			createJSONwater(ALARM,data);
		else
			createJSONwater(NOALARM,data);
		xtimer_sleep(60);
		
		send(data);
		int value=rand()%31+5;
		char temp_s[5];
		snprintf(temp_s,5,"%d",value);
		createJSONtemperature(temp_s,data);
		xtimer_sleep(60);
		
         send(data);
        }

    }




static const shell_command_t shell_commands[] = {    {NULL, NULL, NULL}};

int main(void) {
    srand(time(NULL));

    lora_start();

    printf("Sensors & Actuators will be simulated by  random values \n");

    thread_create(stack_loop, sizeof(stack_loop), THREAD_PRIORITY_MAIN, 0, values_generation, NULL, "values_generation");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}