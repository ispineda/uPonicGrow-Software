#include "uPonicGrow2.h"

// Screen and Encoder
U8GLIB_ST7920_128X64 ST7920_UPG ( UPG_LCD_D1, UPG_LCD_D2, UPG_LCD_D3, UPG_PIN_NONE_U8G ); //Control de pantalla
RotaryFullStep ROTARY_UPG ( UPG_ROTARY_PIN1, UPG_ROTARY_PIN2, UPG_ROTARY_ENABLED, UPG_SENSIBILITY );  //Control de encoder
sdprotocol protSD; 

// Clock
RTC_DS3231 RTC_UPG; // Sensor RTC
DateTime NOW_UPG;   // Constrol Datetime

// I/O Ports Vectors
arx::vector<DHT>       dhtCom;    //Vector DHT 
arx::vector<upg_uint8> pinsPH;    //Vector PH
arx::vector<upg_uint8> pinsEC;    //Vector Electroconductivity
arx::vector<upg_uint8> pinsPR;    //Vector Photoresistor
arx::vector<upg_rgb>   pinsRGB;   //Vector RGB
arx::vector<upg_uint8> pinsFan;   //Vector Fan
arx::vector<upg_uint8> pinsPump;  //Vector Pump

//Dynamic Menu List
enum upg_ml{
  screens = 0,
  menu_config,
  particular_settings,
  activate_cycle,
  current_screen_config,
  crop_definition,
  modify_definition,
  save_definition,
  change_settings,
  general_settings,
  control_datetime,
  alarms,
  lapsesTimeAlarm,
  about_system,
  maintenance_settings,
  connectivity,
  wifi,
  bluetooth,
  light_control,
  fan_control,
  pump_control
};

UPG_menu M[]{
//==================================================================================================================
//|        Parent      ||            id             ||        option name         ||          Jump id             ||
//==================================================================================================================
  {  NULL,                screens,                      NULL,                     menu_config                },
  
  { "ACCESO",             menu_config,                  "VOLVER",                 screens                    },
  { "ACCESO",             menu_config,                  "MI CULTIVO",             particular_settings        },
  { "ACCESO",             menu_config,                  "AJUSTES",                general_settings           },

  { "AJUSTES",            general_settings,             "VOLVER",                 menu_config                },
  { "AJUSTES",            general_settings,             "MANTENIMIENTO",          maintenance_settings       },
  { "AJUSTES",            general_settings,             "CONECTAR",               connectivity               },
  { "AJUSTES",            general_settings,             "SISTEMA",                about_system               },

  { "CONECTIVIDAD",       connectivity,                 "VOLVER",                 general_settings           },
  { "CONECTIVIDAD",       connectivity,                 "WIFI",                   wifi                       },
  { "CONECTIVIDAD",       connectivity,                 "BLUETOOTH",              bluetooth                  },

  { "WI-FI",              wifi,                         "VOLVER",                 connectivity               },
  { "WI-FI",              wifi,                         NULL,                     wifi                       },

  { "BLUETOOTH",          bluetooth,                    "VOLVER",                 connectivity               },
  { "BLUETOOTH",          bluetooth,                    NULL,                     bluetooth                  },

  { "MANTENIMIENTO",      maintenance_settings,         "VOLVER",                 general_settings           },
  { "MANTENIMIENTO",      maintenance_settings,         NULL,                     maintenance_settings       },

  {  NULL,                about_system,                 NULL,                     general_settings           },

  { "CONFIGURAR",         particular_settings,          "VOLVER",                 menu_config                },
  { "CONFIGURAR",         particular_settings,          "VER ACTUAL",             current_screen_config      },
  { "CONFIGURAR",         particular_settings,          "SELECCIONAR",            crop_definition            },
  { "CONFIGURAR",         particular_settings,          "MODIFICAR",              modify_definition          },
  { "CONFIGURAR",         particular_settings,          "VER ALARMAS",            alarms                     },

  { NULL,                 current_screen_config,         NULL,                    particular_settings        },
  { NULL,                 crop_definition,               NULL,                    crop_definition            },

  { "MODIFICAR",          modify_definition,             "VOLVER",                particular_settings        },
  { "MODIFICAR",          modify_definition,             "LUZ",                   light_control              },
  { "MODIFICAR",          modify_definition,             "VENTILADOR",            fan_control                },
  { "MODIFICAR",          modify_definition,             "BOMBA",                 pump_control               },

  { "LUZ",                light_control,                 NULL,                    modify_definition          },
  { "VENTILADOR",         fan_control,                   NULL,                    modify_definition          },
  { "BOMBA",              pump_control,                  NULL,                    modify_definition          },

};

// Date information
upg_v_char dayOfTheWeek[]   ={ "Dom", "Lun" ,"Mar", "Mie", "Jue", "Vier", "Sab" };
upg_v_char monthOfTheYear[] ={ "Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic" };

// Menu Variables
upg_vol_int indMenu = 0;               // Variable defined to advance between menu IDs
upg_c_int   sizeMenu  = UPG_SIZE( M ); // Defines the size of the structure
upg_int     posMenu[ sizeMenu ] = {0}; // Initialize the position vector to zero
upg_bool    buttonCenter = false;      // Variable defined for the central encoder button
upg_bool    stateVar = true;           // Support variable in changing the state of Boolean variables
upg_int     numScreen = 0;             // Variable for scrolling between main screens

// General Variables
upg_uint8  width, height;    // Dimensions the display
upg_uint8  c, cc, cp, d, t;  // Menu size handles
upg_v_char ttx;              // Simple text arrangement
upg_uint8  htop;             // Top screen margin
upg_uint8  hf;               // Font height
upg_uint8  hmax;             // Maximum screen height
upg_uint8  x0, y0, x, y;     // Shape Chart Variables



// Support variables to reference
upg_int  varChanged = 0;       // Supporting integer variable
upg_bool varState   = false;   // Supporting boolean variable

// Date Time variables
upg_int  upgHour, upgMinutes, upgSeconds;
upg_int  upgDay,  upgMonth,   upgYear;
upg_int  upgDayOfTheWeek, upgSecondsBefore;

// BLink variables
upg_bool  blinkSeconds;
upg_bool  lightBlink, fanBlink, irrigationBlink, humdBlink, upgTemBlink;   

// Global configuration structures
UPG_crops UPG_section [ UPG_SECTIONS ];           // Sections data structures
UPG_GeneralSystem confine;                        // Limits variables
upg_int posSection = 0;

/// ===============================================================================================================================================================
/// Main
/// ===============================================================================================================================================================

int main(void){

  init();
  #if defined(USBCON)
    USBDevice.attach();
  #endif

  InitUponicGrow();
  for(;;){
    SystemUponicGrow();  
  }
  return 0;
}

/// ===============================================================================================================================================================
/// Initialize the system
/// ===============================================================================================================================================================
void InitUponicGrow(){
  
  // Encoder Setup
  attachInterrupt(digitalPinToInterrupt(UPG_ROTARY_PIN1), RotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(UPG_ROTARY_PIN2), RotaryEncoder, CHANGE);
  pinMode(UPG_ROTARY_BUTTON_PIN, INPUT_PULLUP);
  PcInt::attachInterrupt(UPG_ROTARY_BUTTON_PIN, PinCenterChanged, FALLING);

  // Clock Setup
  if (!RTC_UPG.begin() ){ abort(); }
  if ( RTC_UPG.lostPower() ){  RTC_UPG.adjust(DateTime(F(__DATE__), F(__TIME__))); }

  ST7920_UPG.begin();
  protSD.begin(UPG_SD);

  width =  ST7920_UPG.getWidth ();
  height = ST7920_UPG.getHeight();

  UPG_ERROR(BeingPorts(),        ErrorsSystem(1))       // Initialize preloaded ports
  UPG_ERROR(SystemDefault(),     ErrorsSystem(2))       // Initialize system variables
  
  ST7920_UPG.firstPage();  
  do {
        ST7920_UPG.drawXBMP( 0, 0, uPonicGrow_Loader_width, uPonicGrow_Loader_height, uPonicGrow_Loader_bits);
  } while( ST7920_UPG.nextPage() );
  delay(1600);
}

upg_bool BeingPorts(){

  if(DefinePorts()){
    for(upg_int i = 0; i < UPG_SECTIONS; i++){
      dhtCom[i].begin();
      pinMode(pinsPH[i],  INPUT);
      pinMode(pinsEC[i],  INPUT);
      pinMode(pinsPR[i],  INPUT);
      pinMode(pinsFan[i], OUTPUT);
      pinsRGB[i].begin();
    }
  }else{ return false; }
  return true;
}

upg_bool DefinePorts(){
  
  // Sensors DHT
  DHT dht1(UPG_DHT1_PIN, UPG_DHTTYPE);
  DHT dht2(UPG_DHT2_PIN, UPG_DHTTYPE);
  dhtCom.push_back(dht1);
  dhtCom.push_back(dht2);
  
  // Sensors Ph
  pinsPH.push_back(UPG_PH1);
  pinsPH.push_back(UPG_PH2);

  // Sensors Ec
  pinsEC.push_back(UPG_EC1);
  pinsEC.push_back(UPG_EC2);

  // Sensors PR
  pinsPR.push_back(UPG_PR1);
  pinsPR.push_back(UPG_PR2);

  if( (dhtCom.size() != UPG_SECTIONS) || (pinsPH.size() != UPG_SECTIONS) || (pinsEC.size() != UPG_SECTIONS) || (pinsPR.size() != UPG_SECTIONS ))
  { return false; }

  // Fans
  pinsFan.push_back(UPG_SPEED_FAN1);
  pinsFan.push_back(UPG_SPEED_FAN2);

  // Lights
  upg_rgb RGB1, RGB2;
  RGB1.R = UPG_R1; RGB1.G = UPG_G1; RGB1.B = UPG_B1;
  RGB2.R = UPG_R2; RGB2.G = UPG_G2; RGB2.B = UPG_B2;
  pinsRGB.push_back(RGB1);
  pinsRGB.push_back(RGB2);

  // Pump
  pinsPump.push_back(UPG_PUMP1);
  pinsPump.push_back(UPG_PUMP2);

  if( (pinsFan.size() != UPG_SECTIONS) || (pinsRGB.size() != UPG_SECTIONS) || (pinsPump.size() != UPG_SECTIONS))
  { return false; }

  return true;
}


upg_bool SystemDefault(){
  
  confine.Sup_Humd = 80;
  confine.Sub_Humd = 20;
  
  confine.Sup_Temp = 60;
  confine.Sub_Temp = -20;

  confine.format24 = true;
  confine.autom = true;
  confine.breakSystem = false;
  confine.connectWifi = false;
  confine.sincro = false;
  upgTemBlink = false;

  for(upg_int i = 0; i < UPG_SECTIONS; i++){

      
      UPG_section[i].hours_alarm = 0;
      UPG_section[i].day_alarm = 0;

      UPG_section[i].NameCrop = "";
      UPG_section[i].Temp = 0;
      UPG_section[i].Humd = 0;
      UPG_section[i].Ph = 0;
      UPG_section[i].Ec = 0;
      UPG_section[i].PR = 0;

      UPG_section[i].SpeedFan = 255;
      UPG_section[i].Light = 255;
      UPG_section[i].irrigation = 255;
      
      UPG_section[i].lapseHFan = 0;
      UPG_section[i].lapseHLight = 0;
      UPG_section[i].lapseHIrrigation = 0;

      UPG_section[i].maintenance = false;
      UPG_section[i].alarm = false;

      UPG_section[i].light_activate = true;    
      UPG_section[i].fan_activate   = true;
      UPG_section[i].irrigation_activate = true;
      UPG_section[i].R = 255;
      UPG_section[i].G = 255;
      UPG_section[i].B = 255;
    }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!       Prototype      !!!!!
  if(protSD.available()){
    for(upg_int i = 0; i < UPG_SECTIONS; i++){
      protSD.assignedCrops(UPG_section[i], i);
    }
  }
  return true;
}

//========================================================================================================================
// Control main
//========================================================================================================================

void SystemUponicGrow(){
    UpdateDatetime();
    UpdateIcons();
    ReadSensors();

    ST7920_UPG.firstPage();
    do {
       switch (indMenu) {       
          case  screens:               
            ManagerMainUPG();                              
            break;
          case  menu_config:           
          case  general_settings:
          case  connectivity:
            MenuDynamicUPG( posSection, UPG_ITEM_NOT );    
            break;
          case  light_control:         
            IncUPG ( UPG_section[posSection].Light );      
            break;
          case  fan_control:           
            IncUPG ( UPG_section[posSection].SpeedFan );   
            break;
          case  pump_control:          
            IncUPG ( UPG_section[posSection].irrigation ); 
            break;
          default:                     
            MenuDynamicUPG( posSection, UPG_ITEM );        
            break;
       }
    } while( ST7920_UPG.nextPage() );
}


//========================================================================================================================
// Layouts
//=========================================================================================================================
void ManagerMainUPG(){
  // manager screens
  for(upg_int u = 0; u < UPG_SECTIONS; u++){
    if( u == posMenu[indMenu]){
      TemplateMainUPG(u, dhtCom[u], UPG_section[u]);
    }
  }    
}

void TemplateMainUPG(upg_int n, DHT dht, UPG_crops cropSection){
    
    /// Cabecera
    //ST7920_UPG.drawXBMP ( 0, 1, uPonicGrow_BlackMini_width, uPonicGrow_BlackMini_height, uPonicGrow_BlackMini_bits );

    ST7920_UPG.drawXBMP ( 15, 1, uPonicGrow_width, uPonicGrow_height, uPonicGrow_bits );
    
    ST7920_UPG.setFont( UPG_FONT_FORMAT3 );  
    ST7920_UPG.setFontRefHeightText( ); 
    ST7920_UPG.setFontPosTop( );   

    ST7920_UPG.setDefaultForegroundColor();
    ST7920_UPG.drawFrame(0,0, width, height );
    ST7920_UPG.drawBox(0,1,15,20);
    
    ST7920_UPG.setDefaultBackgroundColor();
    ST7920_UPG.setPrintPos(6,6);
    ST7920_UPG.print(n + 1);
    ST7920_UPG.setDefaultForegroundColor();

    ST7920_UPG.drawHLine( uPonicGrow_BlackMini_width, uPonicGrow_BlackMini_height, width );
    ST7920_UPG.drawVLine( 65, 1, uPonicGrow_BlackMini_height );
    if(cropSection.alarm){
      ST7920_UPG.drawXBMP ( 70, 1, Clock_width, Clock_height, Clock_bits );
    }
    // Iconos en cuadros de cabecera
    if ( confine.connectWifi ){ ST7920_UPG.drawXBMP(52, 2, Wi_Fi_width, Wi_Fi_height, Wi_Fi_bits);
    } else{         ST7920_UPG.drawXBMP(52, 2, Wi_Fi_width, Wi_Fi_height, Wi_Fi_bits); }

    if ( confine.sincro ){  ST7920_UPG.drawXBMP(51, 11, Upload_width, Upload_height, Upload_bits);}
  

    ST7920_UPG.setFont( UPG_FONT_FORMAT );  
    ST7920_UPG.setFontRefHeightText( ); 
    ST7920_UPG.setFontPosTop( );     

    // Visualización de hora
    if ( confine.format24 ){ Format24hours( NOW_UPG ); }
    ST7920_UPG.setPrintPos( 80, 1 );
    if ( upgHour < 10 ){ ST7920_UPG.print('0'); }
    ST7920_UPG.print(upgHour);
    ST7920_UPG.setPrintPos(92, 1);
    if ( blinkSeconds ){ ST7920_UPG.print(':'); }
    ST7920_UPG.setPrintPos(97, 1);
    if ( upgMinutes < 10 ){ ST7920_UPG.print('0'); }
    ST7920_UPG.print(upgMinutes);
    
    // Visualización de fecha
    ST7920_UPG.setPrintPos(69,  10);
    if ( upgDay < 10 ){ ST7920_UPG.print('0'); }
    ST7920_UPG.print(upgDay, DEC);
    ST7920_UPG.print('-');
    ST7920_UPG.print(monthOfTheYear[upgMonth-1]);
    ST7920_UPG.print('-');
    ST7920_UPG.print(upgYear-2000, DEC);

    //Datos de sensor dht temperatua
    ST7920_UPG.setPrintPos(18,  33);
    ST7920_UPG.print(cropSection.Temp);
    ST7920_UPG.setPrintPos(31,  33);
    ST7920_UPG.print("C");
    ST7920_UPG.disableCursor();

    //Datos de sensor dht humedad
    if ( cropSection.Temp < 50 ) { ST7920_UPG.setPrintPos(70, 33);   }
    else{ ST7920_UPG.setPrintPos(64, 33); }
    ST7920_UPG.print(cropSection.Humd);
    ST7920_UPG.setPrintPos(85, 33);
    ST7920_UPG.print('%');

    ST7920_UPG.setFont(UPG_FONT_FORMAT2);  // Establece fuente
    ST7920_UPG.setFontRefHeightText();     // Obtiene datos de la fuente
    ST7920_UPG.setFontPosTop();            // Establece posición de impresión de fuente
    hf = ST7920_UPG.getFontAscent()-ST7920_UPG.getFontDescent()+2; 

    // Visualización de texto
    ST7920_UPG.drawXBMP(  4,  24, Temp_Min_width, Temp_Min_height, Temp_Min_bits );
    if(upgTemBlink){
      upg_int porcentTemp = 36 - cropSection.Temp*10/confine.Sup_Temp; 
      ST7920_UPG.drawLine(8, 36, 8, porcentTemp );
    }
    ST7920_UPG.drawStr ( 18,  25, "Temp" );
    ST7920_UPG.drawStr ( 68,  25, "Humd" );

    //Barra termometro
    

    if ( humdBlink ){  ST7920_UPG.drawXBMP(43, 24, Hum_ON_width,  Hum_ON_height, Hum_ON_bits);
    } else{            ST7920_UPG.drawXBMP(43, 24, Hum_Off_width, Hum_Off_height, Hum_Off_bits);}

    ST7920_UPG.setPrintPos( 98, 25 );
    ST7920_UPG.print( "Ph: " );
    ST7920_UPG.print(cropSection.Ph);
    ST7920_UPG.setPrintPos( 96, 35 );
    ST7920_UPG.print( "Ec: ");
    ST7920_UPG.print(cropSection.Ec);

    if ( cropSection.light_activate ) {
      if ( lightBlink ){    
        ST7920_UPG.drawXBMP( 3, 46, light_ON_width, light_ON_height, light_ON_bits );
      }else{        
        ST7920_UPG.drawXBMP( 3, 46, light_Off_width, light_Off_height, light_Off_bits );
      }
      
      ST7920_UPG.setDefaultForegroundColor();
      ST7920_UPG.drawRBox(15, 50, ST7920_UPG.getStrWidth("Luz") + 1, hf, 2);
      ST7920_UPG.setDefaultBackgroundColor();
      ST7920_UPG.drawStr(16,  51, "Luz");
      ST7920_UPG.setDefaultForegroundColor();

    }else{
      ST7920_UPG.drawXBMP(3, 46, light_Off_width, light_Off_height, light_Off_bits);
      ST7920_UPG.drawStr (16,  51, "Luz");
    }
    
    if ( cropSection.fan_activate ) {
      if ( fanBlink ) {    
        ST7920_UPG.drawXBMP(35, 48, Fan_ON_width, Fan_ON_height, Fan_ON_bits);
      }else{        
        ST7920_UPG.drawXBMP(35, 48, Fan_Off_width, Fan_Off_height, Fan_Off_bits);
      }

      ST7920_UPG.setDefaultForegroundColor();
      ST7920_UPG.drawRBox(49, 50, ST7920_UPG.getStrWidth("Aire") + 1, hf, 2);
      ST7920_UPG.setDefaultBackgroundColor();
      ST7920_UPG.drawStr(50,  51, "Aire");
      ST7920_UPG.setDefaultForegroundColor();
    }else{
      ST7920_UPG.drawXBMP(35, 48, Fan_Off_width, Fan_Off_height, Fan_Off_bits);
      ST7920_UPG.drawStr(49,  51, "Aire");
    }
    
    if ( cropSection.irrigation_activate ) {
      if ( irrigationBlink ) {    
        ST7920_UPG.drawXBMP(69, 49, irrigation_On_width, irrigation_On_height, irrigation_On_bits);
      }else{        
        ST7920_UPG.drawXBMP(69, 49, irrigation_Off_width, irrigation_Off_height, irrigation_Off_bits);
      }  

      ST7920_UPG.setDefaultForegroundColor();
      ST7920_UPG.drawRBox(77, 50, ST7920_UPG.getStrWidth("Riego") + 1, hf, 2);
      ST7920_UPG.setDefaultBackgroundColor();
      ST7920_UPG.drawStr(78,  51, "Riego");
      ST7920_UPG.setDefaultForegroundColor();
    }else{
      ST7920_UPG.drawXBMP(69, 49, irrigation_On_width, irrigation_On_height, irrigation_On_bits);
      ST7920_UPG.drawStr(78,  51, "Riego");
    }
    
    if(confine.autom ){
      ST7920_UPG.drawXBMP(width-25, 49, Automatic_Black_width, Automatic_Black_height, Automatic_Black_bits);
      ST7920_UPG.drawXBMP(width-13, 50, Manual_width, Manual_height, Manual_bits);
    }else{
      ST7920_UPG.drawXBMP(width-25, 50, Automatic_width, Automatic_height, Automatic_bits);
      ST7920_UPG.drawXBMP(width-13, 49, Manual_Black_width, Manual_Black_height, Manual_Black_bits);
    }
    
    if( buttonCenter ){

          posSection = n;
          indMenu = GetIdJump(indMenu);
          posMenu[indMenu] = 0;
          delay(200);
          buttonCenter = false;
    }

}

void MenuDynamicUPG( upg_int n , upg_bool itemMode){

  
  ST7920_UPG.drawXBMP(0, 0, Leaf_White_width, Leaf_White_height, Leaf_White_bits);

  ST7920_UPG.setFont(UPG_FONT_FORMAT);  
  ST7920_UPG.setFontRefHeightText(); 
  ST7920_UPG.setFontPosTop();  
  hf = ST7920_UPG.getFontAscent()-ST7920_UPG.getFontDescent() + 3; 

  ST7920_UPG.setDefaultForegroundColor();
  ST7920_UPG.drawBox ( 24, 0, 99, 18 );
  ST7920_UPG.setDefaultBackgroundColor();
  ST7920_UPG.drawStr ( 29, 4, GetTextIdData(posMenu[indMenu]) );
  ST7920_UPG.setPrintPos ( 110, 5);
  if(itemMode){
    ST7920_UPG.print( n + 1);
  }
  ST7920_UPG.drawLine( 28, hf+2, ST7920_UPG.getStrWidth(GetTextIdData(posMenu[indMenu])) + 30, hf+2 );

  htop = UPG_LIST_MENU_TOP_MARGIN;

  if (htop != 0){
    ST7920_UPG.drawLine(0, htop-2, width, htop-2);
  }
  hmax = height - htop;

  c = hmax/hf;      
  
  for ( upg_int i = 0; i < SizeIndMenu(); i++ ) {                  
    t = posMenu[indMenu]/c; //Indica el nivel de conjuntos de impresión

    if( i < c*(t+1) && i >= c*t)  //Evita desborde o interrupciones de impresión de datos de menu
    {
      
      d = (width-ST7920_UPG.getStrWidth(GetTextId(i)))/2 ;
      ST7920_UPG.setDefaultForegroundColor();  
      
      y0 = ((i-(t*c))*hf) + htop;
      if (i == 0){
        if ( i == posMenu[indMenu] ){
          ST7920_UPG.drawXBMP(5, y0, Back_White_width, Back_White_height, Back_White_bits);
        }else{
          ST7920_UPG.drawXBMP(5, y0, Back_Black_width, Back_Black_height, Back_Black_bits);
        }
      }
      if ( i == posMenu[indMenu] ) {
        if(buttonCenter){
          indMenu = GetIdJump(i);
          //if (indMenu == UPG_MAIN ){ posMenu[indMenu] = posSection; }
          //else { posMenu[indMenu] = 0; }
          posMenu[indMenu] = 0;
          delay(100);
          buttonCenter = false;
          break;
        }
        if(i == 0){
          ST7920_UPG.drawBox(20, y0, 100, hf); //Crea seleccionado de previsualización negro
        }else{
          ST7920_UPG.drawBox(5, y0, 113, hf); //Crea seleccionado de previsualización negro
          
        }
        ST7920_UPG.setDefaultBackgroundColor();   
      }
      ST7920_UPG.drawStr(d, y0+1, GetTextId(i)); //Imprime elemento del vector
    }
  }
  ScrollVUPG(SizeIndMenu(), width - (UPG_SCROLL_WIDTH + 2));
}

void IncUPG ( upg_uint8 &var ){
    
    ST7920_UPG.drawXBMP(0, 0, Leaf_White_width, Leaf_White_height, Leaf_White_bits);

    ST7920_UPG.setFont(UPG_FONT_FORMAT);  // Establece fuente
    ST7920_UPG.setFontRefHeightText(); //Obtiene datos de la fuente
    ST7920_UPG.setFontPosTop();  // Establece posición de impresión de fuente
    hf = ST7920_UPG.getFontAscent()-ST7920_UPG.getFontDescent() + 3; 
    
    ST7920_UPG.setDefaultForegroundColor();
    ST7920_UPG.drawBox(24, 0, 103, 18);
    ST7920_UPG.setDefaultBackgroundColor();

    ST7920_UPG.drawStr(30, 4, GetTextIdData(posMenu[indMenu]));
    ST7920_UPG.drawLine(29, hf + 2, ST7920_UPG.getStrWidth(GetTextIdData(posMenu[indMenu])) + 30, hf + 2);
    ST7920_UPG.setDefaultForegroundColor();
    
    ST7920_UPG.setFont(UPG_FONT_FORMAT3);  // Establece fuente
    ST7920_UPG.setFontRefHeightText(); //Obtiene datos de la fuente
    ST7920_UPG.setFontPosTop();  // Establece posición de impresión de fuente
    hf = ST7920_UPG.getFontAscent()-ST7920_UPG.getFontDescent() + 2; 

    ST7920_UPG.drawRBox(39, 35, 50, hf, 1); 

    ST7920_UPG.setDefaultForegroundColor();
    ST7920_UPG.drawBox(39, 31, 50, 2);
    ST7920_UPG.drawBox(39, 51, 50, 2);
    ST7920_UPG.setDefaultBackgroundColor();

    if ( confine.varIncrea < 10){ ST7920_UPG.setPrintPos((width - ST7920_UPG.getStrWidth("0")) / 2, 37); }
    else if( confine.varIncrea < 100 && confine.varIncrea >= 10){ ST7920_UPG.setPrintPos((width - ST7920_UPG.getStrWidth("00")) / 2, 37); }
    else { ST7920_UPG.setPrintPos((width - ST7920_UPG.getStrWidth("000")) / 2, 37); }
    ST7920_UPG.print(confine.varIncrea);
    
    ST7920_UPG.setDefaultForegroundColor();
    ST7920_UPG.drawXBMP(20, 36, left_width, left_height, left_bits);
    ST7920_UPG.drawXBMP(92, 36, right_width, right_height, right_bits);

    if ( buttonCenter ){
      var = confine.varIncrea;  
      indMenu = GetIdJump(posMenu[indMenu]);
      posMenu[indMenu] = 0;
      delay(200);
      buttonCenter = !buttonCenter;
    }
}

void ScrollVUPG( upg_uint8 n, upg_int posX){
  upg_uint8 hs;
  hs = height / n;   
  
  for ( upg_int i = 0; i < n; i++ ){
    if (i == posMenu[indMenu]){
        ST7920_UPG.drawBox(posX , i*hs+1, width-2,hs); 
        ST7920_UPG.setDefaultBackgroundColor();
    }
      ST7920_UPG.setDefaultForegroundColor();
    }
    if (hs*n != height) {
      ST7920_UPG.drawLine(posX, 0, posX, hs*n);
      ST7920_UPG.drawLine(posX, 0, width-2, 0);
      ST7920_UPG.drawLine(posX, hs*n, width-2, hs*n);
      ST7920_UPG.drawLine(width-1, 0, width-1, hs*n);
    } else{
      ST7920_UPG.drawLine(posX, 0, posX, hs*n-1);
      ST7920_UPG.drawLine(posX, 0, width-2, 0);
      ST7920_UPG.drawLine(posX, hs*n-1, width-2, hs*n-1);
      ST7920_UPG.drawLine(width-1, 0, width-1, hs*n-1);
    }
}

//==========================================================================================================================
// Update data
//==========================================================================================================================
void ReadSensors(){
  for(upg_int i = 0; i < dhtCom.size();i++){
    UPG_section[i].Temp = dhtCom[i].readTemperature();
    UPG_section[i].Humd = dhtCom[i].readHumidity();

    if (isnan(UPG_section[i].Temp) || isnan(UPG_section[i].Humd)) {
      UPG_section[i].Temp = 0;
      UPG_section[i].Humd = 0;
    }
  }
}

void UpdateIcons(){
    if(upgSecondsBefore!= upgSeconds){
        blinkSeconds = !blinkSeconds; 
        fanBlink = !fanBlink;
        lightBlink = !lightBlink;
        irrigationBlink = !irrigationBlink;
        humdBlink = !humdBlink;
        upgTemBlink = !upgTemBlink;
        upgSecondsBefore = upgSeconds;
    }
}

void UpdateDatetime(){
    NOW_UPG = RTC_UPG.now();
    upgHour = NOW_UPG.hour();
    upgMinutes = NOW_UPG.minute();
    upgDay = NOW_UPG.day();
    upgSeconds = NOW_UPG.second();
    upgMonth = NOW_UPG.month();
    upgYear = NOW_UPG.year();
    upgDayOfTheWeek = NOW_UPG.dayOfTheWeek();
}

void Format24hours( DateTime format24 ){
  if ( format24.hour() >= 12 ){
    upgHour = (format24.hour()==12)? format24.hour(): format24.hour()-12;
    ST7920_UPG.drawStr(112, 0,"pm");
  } else if ( format24.hour() == 0 ){
    upgHour = 12;
    ST7920_UPG.setPrintPos(112, 0);
    ST7920_UPG.drawStr(112, 0,"am");
  }else{
    ST7920_UPG.setPrintPos(112, 0);
    ST7920_UPG.drawStr(112, 0,"am");
  }
}

/// ===============================================================================================================================================================
/// Manager Menu
/// ===============================================================================================================================================================

upg_uint8 SizeIndMenu(){

  cp = 0;
  for ( upg_int i = 0; i < sizeMenu; i++ ){
    if ( M[i].id == indMenu ){ cp++; }
  }
  return cp;
}

upg_v_char GetTextId ( upg_int position ){

  ttx = "No asigned";
  cc = SizeIndMenu();
  cp = 0;
  if ( position < cc && position >= 0 ){
    for ( upg_uint8 i = 0; i < sizeMenu; i++ ){
      if ( M[i].id == indMenu ){
        if ( cp == position ) { return M[i].name; }
        cp++;
      }
    }  
  }
  return ttx;  
}

upg_uint8 GetIdJump ( upg_uint8 position ){

  cc = SizeIndMenu();
  cp = 0;
  if ( position < cc && position >= 0 ){
    for ( upg_int i = 0; i < sizeMenu; i++ ){
      if ( M[i].id== indMenu ){
        if (cp == position ) { return M[i].idJump; }
        cp++;
      }
    }  
  }
  return 0;
}

upg_v_char GetTextIdData ( upg_int position ){

  ttx = "No asigned";
  cc = SizeIndMenu();
  cp = 0;

  if ( position < cc && position >= 0 ) {
    for ( upg_uint8 i = 0; i < sizeMenu; i++ ){
      if ( M[i].id == indMenu ) {
        if ( cp == position ) { return M[i].nameId; }
        cp++;
      }
    }  
  }
  return ttx;  
}

/// ===============================================================================================================================================================
/// Control of encoder
/// ===============================================================================================================================================================

void RotaryEncoder(){

  upg_int encodRotary  = ROTARY_UPG.read(); 

  switch (indMenu) {
      case screens:
        EncoderActionInt(encodRotary, posMenu[indMenu], 0, UPG_SECTIONS);
        break;
      case light_control: 
      case fan_control:  
      case pump_control: 
        EncoderActionInt(encodRotary, confine.varIncrea, 0, 101);
        break;
      default:  
        EncoderActionInt(encodRotary, posMenu[indMenu], 0, SizeIndMenu());
        break;      
  }
}

void EncoderActionInt(upg_int encoder, upg_int &var, upg_int min, upg_int max){
    if ( encoder > 0 ) {  
      var++; 
      if ( var >= max ) var = max-1; 
    }else if ( encoder < 0 ){  
      var--;
      if ( var < min ) var = min;
    }
    else{ return; } 
}

void PinCenterChanged() { 
  buttonCenter = true; 
}

void ErrorsSystem(upg_int error){

  ST7920_UPG.firstPage();  
  do {
        ST7920_UPG.setDefaultForegroundColor();
        ST7920_UPG.setFont(UPG_FONT_FORMAT3);  
        ST7920_UPG.setFontRefHeightText(); 
        ST7920_UPG.setFontPosTop();   
        hf = ST7920_UPG.getFontAscent()-ST7920_UPG.getFontDescent() + 3; 

        ST7920_UPG.drawStr( (width - ST7920_UPG.getStrWidth("Error"))/2, (height/2) - hf, "Error" );
        
        ST7920_UPG.drawStr( (width - ST7920_UPG.getStrWidth("UPG_"))/2 - 11, height/2, "UPG_" );
        ST7920_UPG.setPrintPos(width/2 + 5, height/2);

        if ( error < 10 ){
          ST7920_UPG.print("00");            
        }else if( error > 10 && error <100 ) {
          ST7920_UPG.print("0");  
        }else{
          ST7920_UPG.print("");    
        }
        ST7920_UPG.print(error);

    } while( ST7920_UPG.nextPage() );
}
