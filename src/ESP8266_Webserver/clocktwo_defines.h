
#define TIME_MINUTE_START 15
#define TIME_HOUR_START 20

#define NIGHT_MINUTE_START 23
#define NIGHT_HOUR_START 23
#define NIGHT_MINUTE_STOP 25
#define NIGHT_HOUR_STOP 5

#define COLOR_RED 255
#define COLOR_GREEN 255
#define COLOR_BLUE 255

#define ROW    0
#define COL    1
#define LENGTH 2

#define ES        0
#define ES_ROW    0
#define ES_COL    0
#define ES_LENGTH 2

#define IST        1
#define IST_ROW    0
#define IST_COL    3
#define IST_LENGTH 3

#define FUENF_MINUTE        2
#define FUENF_MINUTE_ROW    0
#define FUENF_MINUTE_COL    7
#define FUENF_MINUTE_LENGTH 4

#define ZEHN_MINUTE        3
#define ZEHN_MINUTE_ROW    1
#define ZEHN_MINUTE_COL    0
#define ZEHN_MINUTE_LENGTH 4

#define ZWANZIG_MINUTE        4
#define ZWANZIG_MINUTE_ROW    1
#define ZWANZIG_MINUTE_COL    4
#define ZWANZIG_MINUTE_LENGTH 7

#define DREIVIER_MINUTE        5
#define DREIVIER_MINUTE_ROW    2
#define DREIVIER_MINUTE_COL    0
#define DREIVIER_MINUTE_LENGTH 11

#define VOR        6
#define VOR_ROW    3
#define VOR_COL    0
#define VOR_LENGTH 3

#define NACH        7
#define NACH_ROW    3
#define NACH_COL    7
#define NACH_LENGTH 4

#define HALB        8
#define HALB_ROW    4
#define HALB_COL    0
#define HALB_LENGTH 4

#define ELF_HOUR        9
#define ELF_HOUR_ROW    4
#define ELF_HOUR_COL    5
#define ELF_HOUR_LENGTH 3

#define FUENF_HOUR        10
#define FUENF_HOUR_ROW     4
#define FUENF_HOUR_COL     7
#define FUENF_HOUR_LENGTH  4

#define EINS_HOUR        11
#define EINS_HOUR_ROW     5
#define EINS_HOUR_COL     0
#define EINS_HOUR_LENGTH  4

#define ZWEI_HOUR        12
#define ZWEI_HOUR_ROW     5
#define ZWEI_HOUR_COL     7
#define ZWEI_HOUR_LENGTH  4

#define DREI_HOUR        13
#define DREI_HOUR_ROW     6
#define DREI_HOUR_COL     0
#define DREI_HOUR_LENGTH  4

#define VIER_HOUR        14
#define VIER_HOUR_ROW     6
#define VIER_HOUR_COL     7
#define VIER_HOUR_LENGTH  4

#define SECHS_HOUR        15
#define SECHS_HOUR_ROW     7
#define SECHS_HOUR_COL     0
#define SECHS_HOUR_LENGTH  5

#define ACHT_HOUR        16
#define ACHT_HOUR_ROW     7
#define ACHT_HOUR_COL     7
#define ACHT_HOUR_LENGTH  4

#define SIEBEN_HOUR        17
#define SIEBEN_HOUR_ROW     8
#define SIEBEN_HOUR_COL     0
#define SIEBEN_HOUR_LENGTH  6

#define ZWOELF_HOUR        18
#define ZWOELF_HOUR_ROW     8
#define ZWOELF_HOUR_COL     6
#define ZWOELF_HOUR_LENGTH  5

#define ZEHN_HOUR        19
#define ZEHN_HOUR_ROW     9
#define ZEHN_HOUR_COL     0
#define ZEHN_HOUR_LENGTH  4

#define NEUN_HOUR        20
#define NEUN_HOUR_ROW     9
#define NEUN_HOUR_COL     3
#define NEUN_HOUR_LENGTH  4

#define UHR        21
#define UHR_ROW     9
#define UHR_COL     8
#define UHR_LENGTH  3

#define VIERTEL_MINUTE        22
#define VIERTEL_MINUTE_ROW     2
#define VIERTEL_MINUTE_COL     4
#define VIERTEL_MINUTE_LENGTH  7

#define EIN_HOUR        23
#define EIN_HOUR_ROW     5
#define EIN_HOUR_COL     0
#define EIN_HOUR_LENGTH  3



#define DOT_ONE   110
#define DOT_TWO   111
#define DOT_THREE 112
#define DOT_FOUR  113


#define NUM_LEDS_ALL 114
#define NUM_LEDS 110
#define NUM_LEDS_ROW 11
#define NUM_LEDS_COL 10


int timeArray[24][3] = {
                        {ES_ROW, ES_COL, ES_LENGTH},
                        {IST_ROW, IST_COL, IST_LENGTH},
                        {FUENF_MINUTE_ROW, FUENF_MINUTE_COL, FUENF_MINUTE_LENGTH},
                        {ZEHN_MINUTE_ROW, ZEHN_MINUTE_COL, ZEHN_MINUTE_LENGTH},
                        {ZWANZIG_MINUTE_ROW, ZWANZIG_MINUTE_COL, ZWANZIG_MINUTE_LENGTH},
                        {DREIVIER_MINUTE_ROW, DREIVIER_MINUTE_COL, DREIVIER_MINUTE_LENGTH},
                        {VOR_ROW, VOR_COL, VOR_LENGTH},
                        {NACH_ROW, NACH_COL, NACH_LENGTH},
                        {HALB_ROW, HALB_COL, HALB_LENGTH},
                        {ELF_HOUR_ROW, ELF_HOUR_COL, ELF_HOUR_LENGTH},
                        {FUENF_HOUR_ROW, FUENF_HOUR_COL, FUENF_HOUR_LENGTH},
                        {EINS_HOUR_ROW, EINS_HOUR_COL, EINS_HOUR_LENGTH},
                        {ZWEI_HOUR_ROW, ZWEI_HOUR_COL, ZWEI_HOUR_LENGTH},
                        {DREI_HOUR_ROW, DREI_HOUR_COL, DREI_HOUR_LENGTH},
                        {VIER_HOUR_ROW, VIER_HOUR_COL, VIER_HOUR_LENGTH},
                        {SECHS_HOUR_ROW, SECHS_HOUR_COL, SECHS_HOUR_LENGTH},
                        {ACHT_HOUR_ROW, ACHT_HOUR_COL, ACHT_HOUR_LENGTH},
                        {SIEBEN_HOUR_ROW, SIEBEN_HOUR_COL, SIEBEN_HOUR_LENGTH},
                        {ZWOELF_HOUR_ROW, ZWOELF_HOUR_COL, ZWOELF_HOUR_LENGTH},
                        {ZEHN_HOUR_ROW, ZEHN_HOUR_COL, ZEHN_HOUR_LENGTH},
                        {NEUN_HOUR_ROW, NEUN_HOUR_COL, NEUN_HOUR_LENGTH},
                        {UHR_ROW, UHR_COL, UHR_LENGTH},
                        {VIERTEL_MINUTE_ROW, VIERTEL_MINUTE_COL, VIERTEL_MINUTE_LENGTH},
                        {EIN_HOUR_ROW, EIN_HOUR_COL, EIN_HOUR_LENGTH},
                       };
