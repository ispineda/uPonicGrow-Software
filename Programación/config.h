#ifndef config_h_
#define config_h_
#include "uPonicGrow2.h"
#	if defined(_EXTERN_LIBRARIES_UPG_)
// Pins <U8glib.h> u8g

#		define UPG_LCD_D1 13
#		define UPG_LCD_D2 11
#		define UPG_LCD_D3 16	// 12
#		define UPG_PIN_NONE_U8G U8G_PIN_NONE
// Pins <ErriezRotaryFullStep.h> Rotary
#		define UPG_ROTARY_PIN1 18	//2
#		define UPG_ROTARY_PIN2 19	//3
#		define UPG_ROTARY_BUTTON_PIN A8	//4
#		define UPG_ROTARY_ENABLED true
#		define UPG_SENSIBILITY 100

// ================ MEGA ==================== 
#		define UPG_FLOW_PIN 3		
//#		define UPG_CROSS_ZERO	2	
#		define UPG_WATER_PUMP 	22
#		define UPG_SPEED_WATER_PUMP	46
#		define UPG_PHASE_CONTROL_ALT 4
#		define UPG_DHTTYPE DHT11
#		define UPG_DHT1_PIN 42				//Sensores
#		define UPG_DHT2_PIN	40
#		define UPG_SPEED_FAN1 4
#		define UPG_SPEED_FAN2 12
#		define UPG_PUMP1 32
#		define UPG_PUMP2 31

#		define UPG_PH1 A1
#		define UPG_EC1 A2
#		define UPG_PH2 A3
#		define UPG_EC2 A4
#		define UPG_PR1 A5
#		define UPG_PR2 A6

#		define UPG_R1 10	
#		define UPG_G1 9
#		define UPG_B1 8

#		define UPG_R2 7	
#		define UPG_G2 6
#		define UPG_B2 5

#		define UPG_CONNECTION_TX 14
#		define UPG_CONNECTION_RX 15
#		define UPG_DIGITAL_TEST1 41	 
#		define UPG_DIGITAL_TEST2 43

#		define UPG_SD 33
#		define UPG_BUZZER 44

// Designed Screen
#		define UPG_SCROLL
#		define UPG_SCROLL_WIDTH     2
#		define UPG_SCROLL_PADDING_MARGIN 1
#		define UPG_LIST_MENU_TOP_MARGIN 22
#		define UPG_FONT_FORMAT u8g_font_6x12
#		define UPG_FONT_FORMAT2 u8g_font_baby
#		define UPG_FONT_FORMAT3 u8g_font_courB10

#		define UPG_SECTIONS 2
#		define UPG_TEMPLATES_CULTURE 5
#		define UPG_ITEM true
#		define UPG_ITEM_NOT false

// overload Typedef variables
		typedef u8g_pgm_uint8_t UPG_pgm_uint8;
		typedef const u8g_fntpgm_uint8_t* UPG_font;

#	endif //_EXTERN_LIBRARIES_UPG_


#	ifndef _MACROS_UPG_
#	define _MACROS_UPG_
#		define UPG_SIZE(a) sizeof(a)/sizeof(*a)
#		define UPG_ERROR(a, b) while(!a){ b; }
// Logic gates
#		define UPG_OR(ptr, val) (*(ptr) |= (val))
#		define UPG_AND(ptr, val) (*(ptr) &=(val))
#		define UPG_NOT(ptr) (~(*(ptr)))
// Typedef variables
		typedef unsigned char uint8_t;
		typedef signed char int8_t;
		typedef uint8_t upg_uint8;
		typedef uint16_t upg_uint16;
		typedef char* upg_v_char;
		typedef int* upg_v_int;
		typedef double*	upg_v_double;
		typedef const char* upg_vc_char;
		typedef int upg_int;
		typedef float upg_float;
		typedef double upg_double;
		typedef byte upg_byte;
		typedef char upg_char;
		typedef union upg_union;
		typedef volatile int upg_vol_int;
		typedef volatile bool upg_vol_bool;
		typedef bool upg_bool;
		typedef volatile upg_uint8 upg_vol_uint8;
		typedef unsigned long upg_un_long;
		typedef unsigned upg_un;
		typedef const int upg_c_int;
		typedef const long upg_c_long;

		typedef struct UPG_menu{

		  upg_vc_char nameId;
		  upg_int id;
		  upg_vc_char name;
		  upg_int idJump;

		};

		typedef struct UPG_GeneralSystem{

		upg_int Sup_Humd;
		upg_int Sub_Humd;

		upg_int Sup_Temp;
		upg_int Sub_Temp;

		upg_bool autom;
		upg_bool breakSystem;
		upg_bool connectWifi;
		upg_bool connectBluetooth;
		upg_bool sincro;
		upg_bool format24;

		upg_int varIncrea;
		upg_bool status;
		  
		};

		typedef struct UPG_crops{ 

		upg_uint8 hours_alarm;
		upg_uint8 day_alarm;

		upg_vc_char NameCrop;
		upg_uint8 Temp;
		upg_uint8 Humd;
		upg_uint8 Ph;
		upg_uint8 Ec;
		upg_uint8 PR;

		upg_uint8 SpeedFan;
		upg_uint8 Light;
		upg_uint8 irrigation;
		
		upg_uint8 lapseHFan;
		upg_uint8 lapseHLight;
		upg_uint8 lapseHIrrigation;

		upg_bool lapseFanHability;
		upg_bool lapseLightHability;
		upg_bool lapseIrrigationtHability;

		upg_bool maintenance;
		upg_bool alarm;

		upg_bool light_activate;
		upg_bool fan_activate;
		upg_bool irrigation_activate;

		upg_uint8 R;
		upg_uint8 G;
		upg_uint8 B;

		};
		
		typedef struct upg_rgb{
			upg_uint8 R;
			upg_uint8 G;
			upg_uint8 B;

			void begin(){
				pinMode(R, OUTPUT);
      			pinMode(G, OUTPUT);
      			pinMode(B, OUTPUT);
			}
		};

		typedef const PROGMEM upg_uint8 UPG_PROGMEM_uint8;


#	endif	//_MACROS_UPG_

#endif
