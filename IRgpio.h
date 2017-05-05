/************************************************************************
 *
 * Created by Manuel Rodriguez 04/04/2017
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
 * * Manuel Rodriguez 04/04/2017
 * Instalacion general: ZERO_RASPBIAN
 * Se han añadido las librerias wiringPi y lirc.
 * persianas funciona como un proceso arrancado en systemctl y
 * que espera señales IR que provienen de un mando EUSKALTEL/SANSUNG
 * para controlar por GPIO a la persiana del Dormitorio 
 * Se han añadido dos pulsadores para el control manual
 *
 *
********************* PINES USADOS EN ESTE PROYECTO	*********************
	3.3VDC	01  VDC IR					5VDC	02  VCC SALIDAS
	------  03  ---------				5VDC	04  VCC SALIDAS
	------  05  ---------				0.0VDC	06	GND SALIDAS	
	GPIO04 	07  ENTRADA_IR				------  08  ---------
	0.0VDC	06	GND IR					------  10  ---------
	GPIO17 	11 	SALIDA_BAJAR_FRONTAL	------  12  ---------
	GPIO27 	13 	SALIDA_SUBIR_FRONTAL	0.0VDC	14	GND SALIDAS	
	GPIO22 	15  SALIDA_BAJAR_LATERAL	GPIO23 	16	SALIDA_SUBIR_LATERAL	
	------  17	-----------				GPIO24  18  ENTRADA_SUBIR_FRONTAL
	------  19  -----------				0.0VDC	20	GND ENTRADAS
	------  21  -----------				GPIO25	22	ENTRADA_BAJAR_FRONTAL
	------  23  -----------			 	GPIO08	24	ENTRADA_SUBIR_LATERAL
	------  25  -----------				GPIO07	26  ENTRADA_BAJAR_LATERAL 
*************************************************************************/
#ifndef _IRgpio_h
#define _IRgpio_h
/*
* ***********************************************************************
*/
#include <wiringPi.h>
#include <lirc/lirc_client.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sched.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <fstream>
//Definicion de los pines GPIO de entradas (pulsadores) y salidas (Tricas)
#define	IN_PERSIANA_BAJAR 	17	//25
#define IN_PERSIANA_SUBIR 	27	//8
#define	PERSIANA_SUBIR		23
#define PERSIANA_BAJAR 		24

#define REBOTE 				100		//Retardo para estabilizar los pulsadores
#define SHORTDELAY			1000	//Pulsacion corta, activa todo el recoorido
#define TIMETRAVEL 			10000	//Tiempo de todo el recorrido
#define LOGFILE	"/var/log/persianas/IR_login.txt" 
#define LOG logger //cout

/*
* ***********************************************************************
*/
using namespace std;
/*
* ***********************************************************************
*/
void kb_handler(int s);
PI_THREAD (TimeTravel);
void PersianaBajar(void);
void PersianaSubir(void);
void proc_lirc(void);
string logTime(void);
void setupPins();
void cleanUp();
/*
* ***********************************************************************
*/
#endif
