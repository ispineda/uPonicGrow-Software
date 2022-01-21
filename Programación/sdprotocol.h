#ifndef _SD_PROTOCOL_CROPS_
#define _SD_PROTOCOL_CROPS_
#include "uPonicGrow2.h"

class sdprotocol{

public:
	sdprotocol();
	upg_bool begin(upg_int pinSD);
	upg_bool assignedCrops(UPG_crops &cropsTemplate, upg_int item);
	upg_int CountFilesDirectory(File dir);
	upg_bool available();
private:

	upg_bool protAvailable;
	File fileSD;        // Control file SD
	File rootSD;        // Control root SD
	upg_bool fileNoNull;  // Corrobora que el archivo no este vacio
	upg_int numFilesTemplates;
	upg_int numFilesCrops;
	upg_int numberFiles;

	String buffer;               // string of buffer
	upg_int nLineSD;
	upg_int nLineCrops;
	upg_bool initPackage;
	upg_bool endPackage;
};

#endif 