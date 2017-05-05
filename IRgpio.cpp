/*************************************************************************************
 *
 * Created by Manuel Rodriguez 29/04/2017
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Manuel Rodriguez 24/04/2017
 * Instalacion general: ZERO_RASPBIAN
 * Se han añadido las librerias wiringPi y lirc.
 * persianas funciona como un proceso arrancado en systemctl y
 * que espera señales IR que provienen de un mando EUSKALTEL/SANSUNG
 * para controlar por GPIO a la persiana del Dormitorio 
 * Se han añadido dos pulsadores para el control manual
 * 
 *
********************* PINES USADOS EN ESTE PROYECTO	**********************************
	3.3VDC	01  VDC IR					5VDC	02  VCC SALIDAS
	------  03  ---------				5VDC	04  VCC SALIDAS
	------  05  ---------				0.0VDC	06	GND SALIDAS	
	GPIO04 	07  ENTRADA_IR				------  08  ---------
	0.0VDC	06	GND IR					------  10  ---------
	GPIO17 	11 	-----------				------  12  ---------
	GPIO27 	13 	----------- 			0.0VDC	14	GND SALIDAS	
	GPIO22 	15  -----------				GPIO23 	16	PERSIANA_SUBIR	
	------  17	-----------				GPIO24  18  PERSIANA_BAJAR
	------  19  -----------				0.0VDC	20	GND ENTRADAS
	------  21  -----------				GPIO25	22	IN_PERSIANA_SUBIR
	------  23  -----------			 	GPIO08	24	IN_PERSIANA_BAJAR
	------  25  -----------				GPIO07	26   
**************************************************************************************/
# include "IRgpio.h"

 using namespace std;
 
 ofstream logger;
 int timeTravel;
/*
* ************************************************************************************
*/ 
int main(int argc, char *argv[])
{
	logger.open (LOGFILE);
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = kb_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	struct lirc_config *config;	
	if (wiringPiSetupGpio () == -1){
		exit (EXIT_FAILURE) ;
		} else {
				setupPins();	
				cleanUp();
		}
	 if (wiringPiISR (IN_PERSIANA_SUBIR, INT_EDGE_BOTH, &PersianaSubir) < 0){
		LOG<<logTime()<<"Fallo al Planificar interrupcion"<<endl;
		exit (EXIT_FAILURE) ;
		}
	 if (wiringPiISR (IN_PERSIANA_BAJAR, INT_EDGE_BOTH, &PersianaBajar) < 0){
		LOG<<logTime()<<"Fallo al Planificar interrupcion"<<endl;
		exit (EXIT_FAILURE) ;
		}		
// Para matar los zombies del forck
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) 
		{
			LOG<<logTime()<<"error" << SIG_ERR <<endl;
			exit(EXIT_FAILURE);
		}
//Initiate LIRC. Exit on failure
    if(lirc_init((char*)"lirc",1)==-1)
		{
		LOG<<logTime()<<logTime()<<"Fallo inicializando lirc"<<endl;
		exit(EXIT_FAILURE);
		}
//Read the default LIRC config at /etc/lirc/lircd.conf  This is the config for your remote.
	bool lircdresult = lirc_readconfig(NULL,&config,NULL);
	if(lircdresult==0)		
		LOG<<logTime()<<"Lectura de lircd.conf cargada y OK:"<<lircdresult<<endl;		
 	else LOG<<logTime()<<"fallo en la lectura de lircd.conf"<<lircdresult<<endl;
			proc_lirc();
//Frees the data structures associated with config.
		lirc_freeconfig(config);		
		exit(EXIT_FAILURE);
}
/*
* ************************************************************************************
*/
PI_THREAD (TimeTravel)
{
	LOG<<"TIEMPO DE CARRERA"<<endl;
	while(millis()-timeTravel <= TIMETRAVEL){
		LOG<<"CARRERA : "<< millis()-timeTravel<<endl;
		delay(1000);
		}
	LOG<<"FINAL DE TIEMPO DE CARRERA"<<endl;
	cleanUp();
	bool ret = true;
	pthread_exit(&ret);
}
/*
* ************************************************************************************
*/
void PersianaSubir(void){
	int shortPush;
	LOG<<logTime()<<"PULSADO EL BOTON DE SUBIR"<<endl;
	if(digitalRead(PERSIANA_BAJAR)==1) {
		digitalWrite(PERSIANA_BAJAR,0);	
		LOG<<logTime()<<"VA A SUBIR...PARADA LA BAJADA"<<endl;
		} 
	bool ESTADO = digitalRead (IN_PERSIANA_SUBIR);	
	delay(REBOTE);	
	LOG<<logTime()<<"SUBIENDO DESPUES DE REBOTES"<<endl;
	if(ESTADO==0){
		shortPush = millis();
		if(digitalRead(PERSIANA_BAJAR)==1) {
			digitalWrite(PERSIANA_BAJAR,0);	
			LOG<<logTime()<<"VA A SUBIR...PARADA LA BAJADA"<<endl;
			} 		
		digitalWrite(PERSIANA_SUBIR,1);		
		LOG<<logTime()<<"SUBIENDO LA PERSIANA"<<endl;
	}else if(ESTADO==1) {
		if(millis()-shortPush < SHORTDELAY){
//			LOG<<millis()<<":"<<shortPush<<":"<<SHORTDELAY<<endl;
			/*****************************************************************/	
			timeTravel=millis();
			int x = piThreadCreate (TimeTravel) ;
			if (x != 0)
			LOG<<"No arranca el proceso de retardo"<<endl;	
			/*****************************************************************/
		}else{
			digitalWrite(PERSIANA_SUBIR,0);	
			LOG<<logTime()<<"PARADA LA SUBIDA"<<endl;		
			}			
		}	
}
/*
* ************************************************************************************
*/
void PersianaBajar(void){
	int shortPush ;
	LOG<<logTime()<<"PULSADO EL BOTON DE BAJAR"<<endl;
	if(digitalRead(PERSIANA_SUBIR)==1){
		digitalWrite(PERSIANA_SUBIR,0);
		LOG<<logTime()<<"VA A BAJAR...PARADA LA SUBIDA"<<endl;
	}
	
	bool ESTADO = digitalRead (IN_PERSIANA_BAJAR);
	delay(REBOTE);
	LOG<<logTime()<<"BAJANDO DESPUES DE REBOTES"<<endl;
	if(ESTADO==0){
		shortPush = millis();
		if(digitalRead(PERSIANA_SUBIR)==1){
			digitalWrite(PERSIANA_SUBIR,0);
			LOG<<logTime()<<"VA A BAJAR...PARADA LA SUBIDA"<<endl;
		}
		digitalWrite(PERSIANA_BAJAR,1);
		LOG<<logTime()<<"BAJANDO LA PERSIANA"<<endl;
	}else if(ESTADO==1){
		if(millis()-shortPush < SHORTDELAY){
//			LOG<<millis()<<":"<<shortPush<<":"<<SHORTDELAY<<endl;
			/*****************************************************************/	
			timeTravel=millis();
			int x = piThreadCreate (TimeTravel) ;
			if (x != 0)
			LOG<<"No arranca el proceso de retardo"<<endl;	
			/*****************************************************************/
		}else{
		digitalWrite(PERSIANA_BAJAR,0);	
		LOG<<logTime()<<"PARADA LA BAJADA"<<endl;				
		}
	}	
}
/*
* ************************************************************************************
*/
void proc_lirc(void)
{
	int buttonTimer = millis();
	char *code;
	while(lirc_nextcode(&code)==0){
//If code = NULL, meaning nothing was returned from LIRC socket,
//then skip lines below and start while loop again.		
        if(code==NULL) continue;
		{		
//Make sure there is a 400ms gap before detecting button presses.
        if (millis() - buttonTimer  > 400){
			LOG<<logTime()<<"Econtrado  KEY : "<<code<<endl;			
			if(strstr (code,"KEY_YELLOW")){
					LOG<<logTime()<<"Econtrado  KEY_YELLOW: SUBE PERSIANA"<<endl;
					if(digitalRead(PERSIANA_BAJAR) == 1){
						digitalWrite(PERSIANA_BAJAR,0);
						delay(500);
					}else {
						digitalWrite(PERSIANA_SUBIR,1);
					/*****************************************************************/	
						timeTravel=millis();
						int x = piThreadCreate (TimeTravel) ;
						if (x != 0)
							LOG<<"No arranca el proceso de retardo"<<endl;	
					/*****************************************************************/
					}digitalWrite(PERSIANA_SUBIR,1);
					buttonTimer = millis();
				}				
            else if(strstr (code,"KEY_BLUE")){
					LOG<<logTime()<<"Econtrado  KEY_GREEN: BAJA PERSIANA"<<endl;
					if(digitalRead(PERSIANA_SUBIR) == 1){
						digitalWrite(PERSIANA_SUBIR,0);
						delay(500);						
					}else{
						digitalWrite(PERSIANA_BAJAR,1);	
						timeTravel=millis();
					/*****************************************************************/	
						int x = piThreadCreate (TimeTravel) ;
						if (x != 0)
							LOG<<"No arranca el proceso de retardo"<<endl;	
					/*****************************************************************/
					} digitalWrite(PERSIANA_BAJAR,1);
					buttonTimer = millis();
				}

			else if (strstr (code,"KEY_INFO")){
					LOG<<logTime()<<"Encontrado key KEY_INFO: PARA PERSIANA"<<endl;
					cleanUp();
					buttonTimer = millis();
				}

			}
        free(code);		
        } 					
	}
}
/*
* ************************************************************************************
*/
string logTime(){
	char TimeString[128];
	timeval curTime;
	gettimeofday(&curTime, NULL);
	strftime(TimeString, 80, "%Y-%m-%d %H:%M:%S -> ", localtime(&curTime.tv_sec));
	return(TimeString);
}
/*
* ************************************************************************************
*/
void kb_handler(int s){
    LOG<<logTime()<<"Interrupcion de teclado"<<s<<endl;
	logger.close();
    exit(1); 
}
/*
* ************************************************************************************
*/
void setupPins(){
	pinMode (IN_PERSIANA_BAJAR, INPUT) ;
	pinMode (IN_PERSIANA_SUBIR, INPUT) ;
	pullUpDnControl (IN_PERSIANA_BAJAR, PUD_UP) ;
	pullUpDnControl (IN_PERSIANA_SUBIR, PUD_UP) ;
	pinMode (PERSIANA_SUBIR, 	OUTPUT) ;
	pinMode (PERSIANA_BAJAR,	OUTPUT) ;	
}
/*
* ************************************************************************************
*/
void cleanUp(){
	digitalWrite(PERSIANA_BAJAR , 0);
	digitalWrite(PERSIANA_SUBIR , 0);	
}
/*
* ************************************************************************************
*/
