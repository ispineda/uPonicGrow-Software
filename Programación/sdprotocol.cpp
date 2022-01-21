#include "sdprotocol.h"

sdprotocol::sdprotocol(){
	protAvailable = false;
}

upg_bool sdprotocol::begin(upg_int pinSD){

  if(!SD.begin(pinSD)){ 
    Serial.println("Not exists SD");
    return false;
  }
  else{
        SD.mkdir("ConfigCropsDefault");
        SD.mkdir("ConfigCropsDynamic");
        protAvailable = true;
  }
  return true;
}

upg_bool sdprotocol::available(){
	return protAvailable;
}

upg_bool sdprotocol::assignedCrops(UPG_crops &cropsTemplate, upg_int item){

	fileSD = SD.open("ConfigCropsDynamic", FILE_READ);
	fileSD = fileSD.openNextFile();

/*
	cropsTemplate.alarm = false;
    cropsTemplate.hours_alarm = 0;
    cropsTemplate.day_alarm = 0;
	cropsTemplate.NameCrop = "";
    cropsTemplate.Temp = 0;
    cropsTemplate.Humd = 0;
    cropsTemplate.Ph = 0;
    cropsTemplate.Ec = 0;
	cropsTemplate.SpeedFan = 255;
    cropsTemplate.Light = 255;
    cropsTemplate.irrigation = 255;
          
    cropsTemplate.lapseHFan = 0;
    cropsTemplate.lapseHLight = 0;
    cropsTemplate.lapseHIrrigation = 0;

    cropsTemplate.maintenance = false;

    cropsTemplate.light_activate = true;    
    cropsTemplate.fan_activate   = true;
    cropsTemplate.irrigation_activate = true;
    cropsTemplate.R = 255;
    cropsTemplate.G = 255;
    cropsTemplate.B = 255;
*/
    fileSD.close();

    return true;
}



upg_int sdprotocol::CountFilesDirectory(File dir){

  upg_int numberFiles = 0;
  while(true){
    File entry = dir.openNextFile();
    if(!entry){
      break;
    }
    if(!entry.isDirectory()){
      numberFiles++;    // Number of files
    }else{
      break;
    }
    entry.close();
  }
  return numberFiles;
}