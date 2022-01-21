#ifndef UPONICGROW_H_
#define UPONICGROW_H_

#	define VERSION "bv3.1"
#	define BUILDED "00012"

#	if defined(__AVR__)
#		include <avr/io.h>
#		include <avr/pgmspace.h>
#		include <avr/interrupt.h>
#		include <avr/wdt.h>
#		include <util/delay.h>
#		include <math.h>
#		include <inttypes.h>    
#		include <string.h>
#		include <stdlib.h>
#		include <stdint.h>
#		include <stdbool.h>
#	endif	//__AVR__
#	include "Arduino.h"
// Extern Libraries
#	ifndef _EXTERN_LIBRARIES_UPG_
#	define _EXTERN_LIBRARIES_UPG_
#		include <YetAnotherPcInt.h>
#		include <ErriezRotaryFullStep.h>
#		include <U8glib.h>
#		include <Wire.h>
#		include <RTClib.h>
#		include <DHT.h>
#		include <ArxContainer.h>
#		include <SD.h>
#		include <SPI.h>
#	endif	//_EXTERN_LIBRARIES_UPG_

// Internal Libraries
#	include "config.h"
#	include "sdprotocol.h"
#	include "UPG_Icons.h"

void InitUponicGrow();
upg_bool DefinePorts();
upg_bool BeingPorts();
void SystemUponicGrow();
void SystemLoaderUPG();	
void ManagerMainUPG();	
void TemplateMainUPG( upg_int n, DHT dht, UPG_crops crop);
void MenuDynamicUPG( upg_int n, upg_bool itemMode );
void ScrollVUPG( upg_uint8 n , upg_int posX );
void IncUPG  ( upg_uint8 &var);
void BoolUPG ( upg_bool &stateBoolean );
void ErrorsSystem( upg_int error );
upg_bool SystemDefault();	
void ReadSensors();
void UpdateIcons();
void UpdateDatetime();
void Format24hours( DateTime format24 );
void ActivateActuadors();
void DesactivateActuadors();
void RotaryEncoder();
void EncoderActionInt(upg_int encoder, upg_int &var, upg_int min, upg_int max);
void PinCenterChanged();
upg_uint8  SizeIndMenu();
upg_v_char GetTextId( upg_int position );
upg_uint8  GetIdJump( upg_uint8 position );
upg_v_char GetTextIdData( upg_int position );

#endif	//UPONICGROW_H_