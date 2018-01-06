#define ALEF					"\xA0"
#define BET						"\xA1"
#define GIMEL					"\xA2"
#define DALED					"\xA3"
#define HEY						"\xA4"
#define VAV						"\xA5"
#define ZAIN					"\xA6"
#define HET						"\xA7"
#define TET						"\xA8"
#define YOD						"\xA9"
#define HAF_S					"\xAA"
#define HAF						"\xAB"
#define LAMED					"\xAC"
#define MEM_S					"\xAD"
#define MEM						"\xAE"
#define NON_S					"\xAF"
#define NON						"\xB0"
#define SAMECH					"\xB1"
#define AIN						"\xB2"
#define PEY_S					"\xB3"
#define PEY						"\xB4"
#define ZADI_S					"\xB5"
#define ZADI					"\xB6"
#define KOF						"\xB7"
#define REISH					"\xB8"
#define SHIN					"\xB9"
#define TAF						"\xBA"

#define TEXT_INIT_1				("GPS Logger V1.0")
#define TEXT_INIT_2				("!2016 " REISH MEM GIMEL " " TET KOF YOD VAV REISH PEY)
#define TEXT_SATELLITES			(MEM_S YOD NON YOD VAV VAV LAMED)
#define TEXT_DURATION			(NON_S MEM ZAIN " " HAF_S SHIN MEM)
#define TEXT_UTC_TIME			("UTC " NON_S MEM ZAIN)
#define TEXT_ACCURACY			(KOF VAV YOD DALED)
#define TEXT_LATITUDE			(BET HET VAV REISH " " VAV KOF)
#define TEXT_LONGITUDE			(HAF_S REISH VAV ALEF " " VAV KOF)
#define TEXT_ALTITUDE			(MEM_S YOD " " YOD NON PEY " " HEY BET VAV GIMEL)
#define TEXT_SPEED				(TAF VAV REISH YOD HEY MEM)
#define TEXT_DIRECTION			(NON_S VAV VAV YOD HAF)
#define TEXT_SAVED_LANDMARKS	(VAV REISH MEM SHIN NON SHIN " " ZADI "\"" NON)
#define TEXT_REMOVE_SD_1		("     " ALEF YOD ZADI VAV HEY LAMED " " NON_S TAF YOD NON)
#define TEXT_REMOVE_SD_2		(" !.." NON_S VAV REISH HAF YOD ZAIN " " SAMECH YOD TET REISH HAF)
#define TEXT_FOUND_SD			("   :" NON_S VAV REISH HAF YOD ZAIN " " SAMECH YOD TET REISH HAF)
