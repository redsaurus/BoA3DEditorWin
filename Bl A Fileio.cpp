// Note about windows and Mac file formats.

// Windows and Mac write short (2 byte) integers differently. One does
// high byte first, one does low byte first. In the Blades of Exile days, there
// were two scenario formats, one Mac and one Windows, and the first four bytes
// in a scenario file identified which was which.

// Of course, the game was savvy enough to tell which sort of scenario it was loading and
// adjust the value accordingly.

// But this was stupid. In BoA, it is done a much smarter way. Every file is
// written in Mac format, and only the Windows version has to flip the formats.

// Just FYI.

#include "stdafx.h"
#include <cctype>
/*
#include <Windows.h>
#include <stdio.h>
#include "string.h"
#include <ctype.h>
*/
#include <commdlg.h>

#include "Global.h"
#include "CMemStream.h"


#define	DONE_BUTTON_ITEM	1
#define	NIL		0L

#define kWARNING_BEEP	0x010000		// flag bit to beep when display warning

// Global variables

char scenario_path[_MAX_PATH] = ""; // just the path to the scenario file
char store_editor_path[_MAX_PATH + 1];
char appl_path[_MAX_PATH + 1];

// external global variables


extern HWND mainPtr;

extern scenario_data_type scenario;
extern town_record_type town;
extern big_tr_type t_d;
extern outdoor_record_type current_terrain;
extern scen_item_data_type scen_data;
// extern short borders[4][50];
// extern unsigned char border_floor[4][50];
// extern unsigned char border_height[4][50];

// q_3DModStart
extern outdoor_record_type border_terrains[3][3];
// q_3DModEnd

extern short cur_town;
extern short town_type ;
extern location cur_out;
extern Boolean editing_town;
extern short overall_mode;
extern short max_dim[3];
extern Boolean file_is_loaded;
extern Boolean showed_graphics_error;

// q_3DModStart
extern short cen_x, cen_y;
// q_3DModEnd

// q_3DModStart
extern Boolean change_made_town, change_made_outdoors;
// q_3DModEnd

// local variables

short data_dump_file_id;
char start_name[256];
short start_volume;
long start_dir;

// Boolean cur_scen_is_mac = TRUE;

ave_tr_type ave_t;
tiny_tr_type tiny_t;
scenario_data_type temp_scenario;

town_record_type warrior_grove_town;
ave_tr_type warrior_grove_terrain;
outdoor_record_type warrior_grove_out;

// data for Blades of Exile porting
old_blades_scenario_data_type boe_scenario;
old_blades_scen_item_data_type boe_scen_data;
old_blades_piles_of_stuff_dumping_type boe_scen_text;
old_blades_big_tr_type boe_big_town;
old_blades_ave_tr_type boe_ave_town;
old_blades_tiny_tr_type boe_tiny_town;
old_blades_town_record_type boe_town;
old_blades_outdoor_record_type boe_outdoor;
old_blades_talking_record_type boe_talk_data;

Boolean currently_editing_windows_scenario = FALSE;

// Big waste!
// char last_load_file[_MAX_PATH] = "";
char szFileName [_MAX_PATH] = ""; // full path and file name
char szFileName2 [_MAX_PATH] = "tempscen.bas";
char szTitleName [_MAX_FNAME + _MAX_EXT] = ""; // just file name
OPENFILENAME ofn;
OPENFILENAME ofn_import_boe;
OFSTRUCT store;

char *old_blades_button_strs[150] = {"Done ","Ask"," "," ","Keep", "Cancel","+","-","Buy","Leave",
						"Get","1","2","3","4","5","6","Cast"," "," ",
						" "," "," ","Buy","Sell","Other Spells","Buy x10"," "," ","Save",
						"Race","Train","Items","Spells","Heal Party","1","2","3","4","5",
						"6","7","8","9","10","11","12","13","14","15",
				/*50*/  "16","Take","Create","Delete","Race/Special","Skill","Name","Graphic","Bash Door","Pick Lock",
						"Leave","Steal","Attack","OK","Yes","No","Step In"," ","Record","Climb",
						"Flee","Onward","Answer","Drink","Approach","Mage Spells","Priest Spells","Advantages","New Game","Land",
						"Under","Restore","Restart","Quit","Save First","Just Quit","Rest","Read","Pull","Alchemy",
						"17","Push","Pray","Wait","","","Delete","Graphic","Create","Give",
				/*100*/		"Destroy","Pay","Free","Next Tip","Touch", "Select Icon","Create/Edit","Clear Special","Edit Abilities","Choose",
						"Go Back","Create New","General","One Shots","Affect PCs","If-Thens","Town Specs","Out Specs","Advanced","Weapon Abil",
						"General Abil.","NonSpell Use","Spell Usable","Reagents","Missiles","Abilities","Pick Picture","Animated","Enter","Burn",
						"Insert","Remove","Accept","Refuse","Open","Close","Sit","Stand","","",
						"18","19","20","Invisible!","","","","","",""};
short old_blades_available_dlog_buttons[NUM_DLOG_B] = {0,63,64,65,1,4,5,8, 
								128,
								9,10,11,12,13,
								14,15,16,17,29, 51,60,61,62,
								66,69,70, 71,72,73,74,79,
								80,83,86,87,88, 91,92,93,99,100,
								101,102,104, 129,130,131,132,133,
								134,135,136,137};
short old_monst_to_new[256] = {
	0,1,74,76,78, 2,75,77,79,86,
	86,3,4,6,5, 131,20,175,7,8,
	132,10,11,12,88, 13,89,14,15,16,
	105,17,82,81,81, 19,133,187,21,134,
	22,23,24,25,26, 32,33,34,35,36,
	
	37,85,182,38,39, 40,41,135,42,136,
	43,48,44,45,137, 46,47,138,87,49,
	139,140,51,52,141, 50,142,143,53,54,
	72,144,56,58,59, 60,95,61,145,146,
	148,149,150,151,152, 153,154,155,156,62,
	
	63,64,93,84,90, 66,66,157,67,80, // 100
	186,68,188,189,190, 191,192,193,69,70,
	130,91,99,158,55, 120,121,166,167,168,
	159,160,169,170,171, 172,173,174,195,196,
	197,198,199,200,201, 202,202,204,205,206,
	
	207,208,209,210,211, 212,213,214,215,216, // 150
	217,218,219,220,221, 222,223,224,225,226,
	227,228,229,230,231, 232,233,101,161,162,
	163,164,65,94,129, 102,103,128,235,236,
	237,238,239,240,241, 242,243,244,245,246,

	247,248,249,250,251, 252,253,254,255,255,
	255,255,255,255,255, 255,255,255,255,255,
	255,255,255,255,255, 255,255,255,255,255,
	255,255,255,255,255, 255,255,255,255,255,
	255,255,255,255,255, 255,255,255,255,255,

	255,255,255,255,255, 255
};

short old_ter_to_floor[256] = {
	0,1,37,38,39, 255,255,0,255,255,
	0,255,255,0,255, 255,0,255,255,255,
	255,255,255,255,255, 255,255,255,255,255,
	255,255,255,255,255, 255,41,42,47,48,
	49,50,51,52,45, 46,53,54,55,56,
	
	57,65,68,59,67, 61,70,63,69,64, //50,
	62,60,66,23,23, 63,23,59,61,23,
	65,58,58,58,58, 79,80,58,0,0,
	37,41,123,123,0, 0,0,37,37,37,
	71,0,0,0,0, 0,75,0,0,0,

	0,0,0,0,0, 0,0,0,0,0, // 100
	37,77,37,37,37, 37,37,37,72,37,
	37,37,95,95,95, 95,95,95,95,95, 
	95,95,95,95,95, 95,95,95,95,95, 
	95,95,95,95,95, 95,95,95,95,95, 
	
	95,95,95,95,95, 95,95,95,95,95, // 150
	95,95,95,95,95, 95,95,95,95,95, 
	95,129,95,95,95, 95,95,95,95,95, 
	95,95,95,95,95, 95,95,95,95,95, 
	95,95,95,95,95, 95,95,95,95,95, 

	95,95,95,95,95, 95,95,95,95,95,// 200
	98,130,98,98,98, 98,98,100,100,100,
	100,100,100,100,100, 100,100,100,100,100, 
	100,100,0,0,37, 37,37,37,37,37,
	37,37,37,0,0, 0,0,95,98,100,
	37,41,0,0,0, 0

};	

short old_ter_to_ter[256] = {
	0,0,0,0,0,  0,40,56,44,41,
	57,45,38,54,42, 39,55,43,0,0,
	0,0,0,0,40, 44,41,45,38,42,
	39,43,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	
	0,0,0,0,0, 0,0,0,0,0, // 50
	0,0,0,300,301, 298,298,298,299,299,
	299,0,299,298,300, 0,0,336,268,0,
	0,0,0,0,155, 156,155,157,158,157,
	0,147,148,141,142, 285,0,167,166,170,

	171,200,269,223,209, 211,213,215,216,243, // 100
	128,0,151,152,153, 144,319,318,283,170,
	171,214,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0, 
	
	0,0,0,0,0, 0,0,0,0,0, // 150
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,270,218, 229,257,257,257,258,
	258,258,221,222,221, 222,217,231,208,242,
	243,234,267,276,202, 225,226,227,228,206,
	
	278,238,248,277,240, 247,189,188,374,375,  // 200
	0,0,276,277,267, 278,232,0,345,346,
	241,348,349,350,270, 225,355,355,355,355,
	355,286,323,304,329, 305,0,332,333,332,
	333,302,305,311,313, 311,0,0,0,0, 
	
	0,0,0,0,0, 0


};
	
short old_item_to_new_item[400] = 
{
0,0,0,0,18, 19,20,21,22,109, 
109,79,79,163,168, 165,165,170,172,196,
197,198,196,192,191, 171,171,0,0,0,
385,45,45,50,50, 65,65,45,50,50,
55,55,65,55,55, 55,60,70,75,60,

60,46,51,56,56, 66,56,56,56,61,
71,76,61,61,47, 52,57,57,67,57,
57,57,62,72,77, 62,62,48,53,58,
58,68,58,58,58, 63,73,78,63,63,
370,371,380,380,381, 381,383,384,85,87,

88,85,86,100,101, 103,86,87,90,91, // 100
92,93,95,96,98, 105,106,108,314,315, 
25,26,25,30,35, 40,26,31,36,41,
27,32,37,42,28, 33,38,43,38,121,
122,122,123,124,126, 127,127,128,129,131,

132,132,133,134,136, 137,137,138,139,141,
142,142,143,144,146, 0,0,0,0,114,
114,174,499,499,499, 499,205,204,175,176,
499,499,326,499,499, 499,327,499,499,435,
436,437,499,499,499, 499,244,247,0,0,

246,248,251,254,499, 499,257,499,499,499, // 200
499,236,238,239,243, 0,204,140,356,357,
358,359,386,201,202, 420,360,361,362,363,
364,365,115,366,367, 368,113,421,422,423,
424,425,426,427,428, 130,229,228,231,235,

233,234,236,221,223, 231,227,225,226,236,
221,223,231,227,225, 226,236,237,395,242,
243,243,237,499,499, 499,353,354,352,355,
441,499,499,282,285, 282,499,284,286,289,
289,499,499,499,276, 281,278,264,208,499,

499,270,275,272,265, 207,499,499,270,275, // 300
272,266,206,499,430, 431,432,433,434,499,
499,303,305,499,499, 212,211,499,499,499,
311,212,211,499,420, 240,241,390,499,499,
343,342,351,346,369, 344,348,499,499,499,

438,439,499,308,273, 306,499,287,440,429,
499,499,280,215,214, 215,216,216,217,218,
219,499,49,49,54, 44,125,69,269,74,
389,388,442,450,451, 452,453,454,455,456,
457,458,459,460,461, 462,463,464,465,466
};

// function prototype
bool read_BoAFilesFolder_from_Pref( char * boaFolder );
void write_BoAFilesFolder_to_Pref( char * boaFolder );
bool check_BoAFilesFolder( char * boaFolder );
bool init_directories_with_pref( char * boaFolder );
bool init_directories_with_appl_folder( char * boaFolder );
bool init_directories_with_user_input( char * boaFolder );
bool init_directories( void );
void print_write_position ();
int make_backup_file( char * filePath );
int do_save_change_to_outdoor_size(short plus_north,short plus_west,short plus_south,short plus_east,short on_surface);
void load_outdoor( location which_out, outdoor_record_type & to_where );
void oops_error(short error);
// void extract_old_scen_text();
Boolean copy_script(char *script_source_name,char *script_dest_name,char *dest_directory);
// short str_to_num(char *str) ;
void port_boe_scenario_data();
void port_boe_out_data() ;
Boolean is_old_road(short i,short j);
Boolean is_old_wall(short ter);
void port_boe_town_data(short which_town,Boolean is_mac_scen) ;
void boe_port_talk_nodes();
void boe_port_town();
void boe_port_t_d();
void boe_port_scenario();
void boe_port_item_list();
void boe_port_out(old_blades_outdoor_record_type *out);
void boe_flip_spec_node(old_blades_special_node_type *spec);
void port_scenario_script(char *script_name,char *directory_id);
void port_a_special_node(old_blades_special_node_type *node,short node_num,FILE *file_id,short node_type);
void get_bl_str(char *str,short str_type,short str_num);
void add_ishtrs_string_to_file(FILE *file_id,char *str1,char *str2,char *str3);
void add_short_string_to_file(FILE *file_id,char *str1,short num,char *str2);
void add_ish_string_to_file(FILE *file_id,char *str1,short num1,char *str2,short num2,char *str3);
void add_big_string_to_file(FILE *file_id,char *str1,short num1,char *str2,short num2,char *str3,short num3,char *str4);
void add_string_to_file(FILE *file_id,char *str);
void add_cr(FILE *file_id);
void add_string(FILE *file_id,char *str);
void handle_messages(FILE *file_id,short node_type,short message_1,short message_2);
void port_town_script(char *script_name,char *directory_id,short which_town);
void trunc_str(char *str);
void port_outdoor_script(char *script_name,char *directory_id,short sector_x,short sector_y);
void port_town_dialogue_script(char *script_name,char *directory_id,short which_town);
void port_dialogue_intro_text(short *current_dialogue_node,short which_slot,FILE *file_id,short town_being_ported);
void port_dialogue_node(short *current_dialogue_node,short which_slot,FILE *file_id,short which_node,short town_being_ported);
void clean_str(char *str);
// void open_current_scenario_resources();
// void close_current_scenario_resources();
void kludge_correct_old_bad_data();
short SetFPos(FILE *file, short mode, long len);
short FSWrite(FILE *file_id, long *len, char *data);


// registry constant
const char* kRegistryKey = "Software\\Spiderweb Software\\BoA 3D Editor";
const char* kRegistryName = "BoA DATA Dirctory";


bool read_BoAFilesFolder_from_Pref( char * boaFolder )
{
	HKEY hKey; 
	
	if (RegOpenKey(HKEY_CURRENT_USER, kRegistryKey, &hKey) != ERROR_SUCCESS) 
		return false;
	LONG len = _MAX_PATH;
	if( RegQueryValue( hKey, kRegistryName, (LPTSTR)boaFolder, &len ) != ERROR_SUCCESS) 
		return false;
	return (len != 0);
}

void write_BoAFilesFolder_to_Pref( char * boaFolder )
{
	HKEY hKey; 

	if (RegCreateKey(HKEY_CURRENT_USER, kRegistryKey, &hKey) != ERROR_SUCCESS) 
		return;
    RegSetValue( hKey, kRegistryName, REG_SZ, boaFolder, (DWORD)strlen( boaFolder ) + 1 );
	RegCloseKey( hKey ); 
}

bool check_BoAFilesFolder( char * boaFolder )
{
	char path[ _MAX_PATH ];
	FILE * fp;

	strcpy( path, boaFolder );
	strcat( path, "\\Editor Graphics\\G4900.bmp" );
	if ( (fp = fopen( path, "rb" ) ) == NULL )
		return false;
	fclose( fp );
	return true;
}

bool init_directories_with_pref( char * boaFolder )
{
	if ( read_BoAFilesFolder_from_Pref( boaFolder ) )
		return check_BoAFilesFolder( boaFolder );
	return false;
}

bool init_directories_with_appl_folder( char * boaFolder )
{
	if ( GetCurrentDirectory( _MAX_PATH, boaFolder ) != 0 )
		return check_BoAFilesFolder( boaFolder );
	return false;
}

bool init_directories_with_user_input( char * boaFolder )
{
	char anAppPath[ _MAX_PATH ];
	OPENFILENAME anOfn;

	*anAppPath = '\0';
	ZeroMemory( &anOfn, sizeof(anOfn) );
	anOfn.lStructSize = sizeof(OPENFILENAME);
	anOfn.lpstrFilter = "Application (*.EXE)\0" "*.EXE\0";
	anOfn.lpstrFile = anAppPath;
	anOfn.nMaxFile = _MAX_PATH;
	anOfn.lpstrInitialDir = "C:\\Program Files\\Blades of Avernum";
	anOfn.lpstrTitle = "Select \"Blades of Avernum.exe\" application under \"Blades of Avernum\" folder";
	anOfn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NONETWORKBUTTON;

	if ( GetOpenFileName( &anOfn ) == 0 ) //according to MSDN function returns 0, not NULL if user doesn't click OK
		return false;

	char anDrv[ 8 ];
	char anDir[_MAX_PATH];
	char anFile[_MAX_FNAME];
	char anExt[_MAX_EXT];
	_splitpath( anAppPath, anDrv, anDir, anFile, anExt );
	_makepath( boaFolder, anDrv, anDir, "Data", NULL );
	return check_BoAFilesFolder( boaFolder );
}

bool init_directories( void )
{
	bool update_pref = false;
	char boaFolder[ _MAX_PATH ];

	if ( GetCurrentDirectory( _MAX_PATH, appl_path ) == 0 )
		return false;
	if ( !init_directories_with_pref( boaFolder ) ) {				// first, check preference
		update_pref = true;
		if ( !init_directories_with_appl_folder( boaFolder ) )		// next, check application folder
			if ( !init_directories_with_user_input( boaFolder ) )	// then, ask to user
				return false;
	}
	if ( update_pref )
		write_BoAFilesFolder_to_Pref( boaFolder );

	strcpy( store_editor_path, boaFolder );
	return true;
}


bool file_initialize()
{
// file extention filter - beware string concatenation
const char *szFilter =	"Blades of Avernum Scenarios (*.BAS)\0" "*.bas\0"
						"All Files (*.*)\0"						"*.*\0"
						"";
const char *szFilter2 =	"Blades of Exile Scenarios (*.EXS)\0"	"*.exs\0"
						"All Files (*.*)\0"						"*.*\0"
						"";

	if ( !init_directories() )
		return false;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = mainPtr;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = "txt";
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	
	ofn_import_boe = ofn;
	ofn_import_boe.lpstrFilter = szFilter2;

	return true;
}

// Here we go. this is going to hurt.
// Note no save as is available for scenarios.
//At this point, szFileName MUST contain the path and filename for the currently edited scen.
// Strategy ... assemble a big Dummy file containing the whole scenario 
//chunk by chunk, copy the dummy over the original, and delete the dummy
// the whole scenario is too big be be shifted around at once
void save_campaign()
{
	short i,j,k,num_outdoors;
	FILE *dummy_f,*scen_f;
	char *buffer = NULL;
	DWORD buf_len = 100000;
	short error;
	short out_num;
	long len,scen_ptr_move = 0,save_town_size = 0,save_out_size = 0;
	outdoor_record_type *dummy_out_ptr;
	HGLOBAL temp_buffer;

	// before saving, do all the final processing that needs to be done (liek readjusting lights)
	set_up_lights();
	
	//OK. FIrst find out what file name we're working with, and make the dummy file 
	// which we'll build the new scenario in
	// first, create the dummy file name and then aler it slightly so it different from original
	strcpy(szFileName2,szFileName);
	if (szFileName2[strlen(szFileName2) - 1] == 's') // this shoult be true, should end in .bas
		szFileName2[strlen(szFileName2) - 1] = 't';
		else szFileName2[strlen(szFileName2) - 1] = 's';
	if (NULL == (dummy_f = fopen(szFileName2, "wb"))) {
		oops_error(11);
		return;
		}	

	if (NULL == (scen_f = fopen(szFileName, "rb"))) {
		oops_error(12);
		return;
		}	

	// Now we need to set up a buffer for moving the data over to the dummy
	temp_buffer = GlobalAlloc(GMEM_FIXED,buf_len);
	if (temp_buffer == NULL) {
		FSClose(scen_f); FSClose(dummy_f); oops_error(14);
		return;
		}
	buffer = (char *) GlobalLock(temp_buffer);
	if (buffer == NULL) {
		FSClose(scen_f); FSClose(dummy_f); oops_error(14);
		return;
		}
	scenario.prog_make_ver[0] = 2;
	scenario.prog_make_ver[1] = 0;
	scenario.prog_make_ver[2] = 0;

	// Now, the pointer in scen_f needs to move along, so that the correct towns are sucked in.
	// To do so, we'll remember the size of the saved town and out now.
	// this is much simple thabn it was in Blades of Exile, since chunks have a constant length now
	out_num = cur_out.y * scenario.out_width + cur_out.x;
	save_out_size = (long) (sizeof (outdoor_record_type));
	save_town_size = (long) (sizeof (town_record_type));
	if (scenario.town_size[cur_town] == 0)
		save_town_size += (long) (sizeof (big_tr_type));
		else if (scenario.town_size[cur_town] == 1)
			save_town_size += (long) (sizeof (ave_tr_type));
			else save_town_size += (long) (sizeof (tiny_tr_type));
	scen_ptr_move = sizeof(scenario_data_type);

	scenario.last_town_edited = cur_town;
	scenario.last_out_edited = cur_out;

	// now, if editing windows scenario, we need to write it in a windows friendly
	// way.
	if (currently_editing_windows_scenario == FALSE)
		scenario.port();

	len = sizeof(scenario_data_type); // scenario data
	if ((error = FSWrite(dummy_f, &len, (char *) &scenario)) != 0) {
		EdSysBeep( /* 2 */ ); FSClose(scen_f); FSClose(dummy_f);oops_error(62);
		return;
		}	

	if (currently_editing_windows_scenario == FALSE)
		scenario.port();
	
	SetFPos(scen_f,1,scen_ptr_move);
	
	// OK ... scenario written. Now outdoors.
	num_outdoors = scenario.out_width * scenario.out_height;
	for (i = 0; i < num_outdoors; i++)
		if (i == out_num) {
			if (currently_editing_windows_scenario == FALSE)
				current_terrain.port();

			len = sizeof(outdoor_record_type);
			error = FSWrite(dummy_f, &len, (char *) &current_terrain); 

			if (currently_editing_windows_scenario == FALSE)
				current_terrain.port();

			if (error != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(63);}
			
			// Now skip ahead scen_f position to match
			// Kludge ... should be using SetFPos, but for some reason it now working
			//SetFPos(scen_f,3,sizeof (outdoor_record_type));
			len = (long) (sizeof (outdoor_record_type));
			error = FSRead(scen_f, &len, buffer);
			}
			else {
				len = (long) (sizeof (outdoor_record_type));
				error = FSRead(scen_f, &len, buffer);
				dummy_out_ptr = (outdoor_record_type *) buffer;
				if (error != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(64);}
				if ((error = FSWrite(dummy_f, &len, buffer)) != 0) {
					EdSysBeep(/* 2 */); FSClose(scen_f); FSClose(dummy_f);oops_error(65);
					return;
					}			


				}
				
	// now, finally, write towns.
	for (k = 0; k < scenario.num_towns; k++)
		if (k == cur_town) {
			
			// write towns
			if (currently_editing_windows_scenario == FALSE)
				town.port();
				
			len = sizeof(town_record_type);
			error = FSWrite(dummy_f, &len, (char *) &town); 

			if (currently_editing_windows_scenario == FALSE)
				town.port();
					
			if (error != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(66);}

			if (currently_editing_windows_scenario == FALSE)
				t_d.port();
			switch (scenario.town_size[cur_town]) {
				case 0:

					len = sizeof(big_tr_type);
					FSWrite(dummy_f, &len, (char *) &t_d);

					break;
			
				case 1:
					for (i = 0; i < 48; i++)
						for (j = 0; j < 48; j++) {
							ave_t.terrain[i][j] = t_d.terrain[i][j];
							ave_t.floor[i][j] = t_d.floor[i][j];
							ave_t.height[i][j] = t_d.height[i][j];
							ave_t.lighting[i][j] = t_d.lighting[i][j];					
							}					

					len = sizeof(ave_tr_type);
					FSWrite(dummy_f, &len, (char *) &ave_t);
				break;
			
			
				case 2:
					for (i = 0; i < 32; i++)
						for (j = 0; j < 32; j++) {
							tiny_t.terrain[i][j] = t_d.terrain[i][j];
							tiny_t.floor[i][j] = t_d.floor[i][j];
							tiny_t.height[i][j] = t_d.height[i][j];
							tiny_t.lighting[i][j] = t_d.lighting[i][j];					
							}

					len = sizeof(tiny_tr_type);
					FSWrite(dummy_f, &len, (char *) &tiny_t);
					break;

				}
			if (currently_editing_windows_scenario == FALSE)
				t_d.port();

			// Now skip ahead scen_f position to match
			//SetFPos(scen_f,3,save_town_size);
				len = (long) (sizeof(town_record_type));
				error = FSRead(scen_f, &len, buffer);
				switch (scenario.town_size[k]) {
					case 0: len = (long) ( sizeof(big_tr_type)); break;
					case 1: len = (long) ( sizeof(ave_tr_type)); break;
					case 2: len = (long) ( sizeof(tiny_tr_type)); break;
					}
				error = FSRead(scen_f, &len, buffer);

			}
			else { /// load unedited town into buffer and save, doing translataions when necessary

				len = (long) (sizeof(town_record_type));
				switch (scenario.town_size[k]) {
					case 0: len += (long) ( sizeof(big_tr_type)); break;
					case 1: len += (long) ( sizeof(ave_tr_type)); break;
					case 2: len += (long) ( sizeof(tiny_tr_type)); break;
					}


				//for (long count = 0; count < len; count++) {
				//	int next_char = fgetc(scen_f);
					
				//	if (count < 100)
				//		dbug_first_chars[count] = (char) next_char;
				//	fputc(next_char,dummy_f);
				//	}
				error = FSRead(scen_f, &len, buffer);
				if (error != 0) {
					FSClose(scen_f); 
					FSClose(dummy_f);
					oops_error(67);
					}
				if ((error = FSWrite(dummy_f, &len, buffer)) != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(68);return;}						


				}
	
// q_3DModStart
	change_made_town = change_made_outdoors = FALSE;
// q_3DModEnd
	
	// now, everything is moved over. Delete the original, and rename the dummy
	
	// first close everything up
	error = FSClose(scen_f); 
	if (error != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(71);}

	error = FSClose(dummy_f);		
	if (error != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(72);}

	// delete original
	if (OpenFile(szFileName,&store,OF_DELETE) == HFILE_ERROR) {
		if (remove(szFileName) != 0) {
			GlobalUnlock(temp_buffer);
			GlobalFree(temp_buffer);
			oops_error(101);
			give_error("File name that could not be deleted:",szFileName,0);
			return;
			}
		}

	// rename copy to be original
	rename(szFileName2,szFileName);
	
	GlobalUnlock(temp_buffer);
	GlobalFree(temp_buffer);	
}

int make_backup_file( char * filePath )
{
	char backupFile[_MAX_PATH];

	strcpy( backupFile, filePath );
	strcat( backupFile, ".bak" );
	// if the another backup file exists, delete it
	FILE* fp;
	if ( (fp = fopen( backupFile, "rb" )) != NULL ){
		fclose( fp );
		if ( remove( backupFile ) != 0 ) {
			give_error("File name that could not be deleted:",backupFile,0);
			return -1;
		}
	}
	// rename the original
	if ( rename( filePath, backupFile ) != 0 ) {
		give_error("File name that could not be renamed:", filePath,0);
		return -1;
	}
	return 0;
}


#define SetLocation( loc, x0, y0 )	loc.x = x0;	loc.y = y0

// q_3DModStart

//assumes scen has already been saved
void save_change_to_outdoor_size(short plus_north,short plus_west,short plus_south,short plus_east,short on_surface)
{
	int err = do_save_change_to_outdoor_size( plus_north, plus_west, plus_south, plus_east, on_surface );
	if (err != 0) {
		if ( kWARNING_BEEP & err )
			EdSysBeep( /* 2 */ );
		oops_error( (short)err );
	}
}

int do_save_change_to_outdoor_size(short plus_north,short plus_west,short plus_south,short plus_east,short on_surface)
{
	int i;
	bool needPort = (currently_editing_windows_scenario == FALSE);

	RECT oldRECT;	// the previous outdoor area, in the coordinate after conversion
	RECT newRECT;	// the new outdoor area, actually, (0,0)-(out_width,out_hight)
	RECT sumRECT;	// minimum RECT that includes both oldRect and newRect

	SetRECT( newRECT,
		0, 0,
		scenario.out_width + plus_west + plus_east,
		scenario.out_height + plus_north + plus_south );
	SetRECT( oldRECT,
		plus_west, plus_north,
		scenario.out_width + plus_west,
		scenario.out_height + plus_north );
	SetRECT( sumRECT,
		(plus_west > 0) ? newRECT.left : oldRECT.left,
		(plus_north > 0) ? newRECT.top : oldRECT.top,
		(plus_east > 0) ? newRECT.right : oldRECT.right,
		(plus_south > 0) ? newRECT.bottom : oldRECT.bottom );

	scenario.out_width = (short)newRECT.right;
	scenario.out_height = (short)newRECT.bottom;
	
	//keep the "same" sector selected, if it still exists
	OffsetLocation( cur_out, plus_west, plus_north );
	if( !LocationInRECT( cur_out, newRECT ) ) {
		SetLocation( cur_out, 0, 0 );
		if(editing_town == FALSE)
			cen_x = cen_y = 24;
	}
	//and the same starting place
	OffsetLocation( scenario.what_outdoor_section_start_in, plus_west, plus_north );
	if( !LocationInRECT( scenario.what_outdoor_section_start_in, newRECT ) ) {
		SetLocation( scenario.what_outdoor_section_start_in, 0, 0 );
		SetLocation( scenario.start_where_in_outdoor_section, 24, 24 );
	}

	scenario.prog_make_ver[0] = 2;
	scenario.prog_make_ver[1] = 0;
	scenario.prog_make_ver[2] = 0;
	scenario.last_town_edited = cur_town;
	scenario.last_out_edited = cur_out;

	// first, create the in/out stream on memory and read in the original file on the buffer
	CMemStream in_f;
	CMemStream out_f;

	if ( out_f.SetLength() ) return 60;
	if ( in_f.ReadFromFile( szFileName ) ) return 61;	// 62

	// write scenario data to stream and skip it on the original
	if ( needPort )		scenario.port();
	out_f.PutBytes( kSizeOfScenario_data_type, &scenario );		// return (kWARNING_BEEP | 62);
	if ( needPort )		scenario.port();
	in_f.SetMarker( kSizeOfScenario_data_type, CMemStream::eMsMarker );

	// OK ... scenario written. Now outdoors.
	outdoor_record_type empty_terrain;
	empty_terrain.SetSurface( on_surface );

	// check through sumRECT: this algorithm should be most comprehensive, though it isn't most efficient.
	char * outPtr;
	POINT here;
	for ( here.y = sumRECT.top; here.y < sumRECT.bottom; here.y++ ) {
		for ( here.x = sumRECT.left; here.x < sumRECT.right; here.x++ ) {
																	// First, determine what to be written if needed
			outPtr = (char *)&empty_terrain;						//   as the default, select the empty data
			if ( POINTInRECT( here, oldRECT ) ) {					//   if here is within the old area,
				outPtr = in_f.GetCurrentPtr();						//     select the area on the original file
				in_f.SetMarker( kSizeOfOutdoor_record_type, CMemStream::eMsMarker );	// and move the read marker
			}
			if ( (here.x == cur_out.x) && (here.y == cur_out.y) )	//   if here is the outdoor currently editing,
				outPtr = (char *)&current_terrain;					//     select the current outdoor data
																	// Next, determine to write or not.
			if ( POINTInRECT( here, newRECT ) )						//   if here is within the new area,
				out_f.PutBytes( kSizeOfOutdoor_record_type, outPtr );	// output selected data to the streem
		}															//   otherwise nothing to do
	}

	// now, finally, write towns.
	ave_tr_type* anAve_town_rec;
	tiny_tr_type* aTny_town_rec;
	for (i = 0; i < scenario.num_towns; i++) {
								// if the index matches to the current town,
		int anTownSize = scenario.town_size[cur_town];
		if (i == cur_town) {
								// write current town and read off the original
			if ( needPort )		town.port();
			out_f.PutBytes( kSizeOfTown_record_type, &town );	// return 68; //66, 67
			if ( needPort )		town.port();
			in_f.SetMarker( kSizeOfTown_record_type, CMemStream::eMsMarker );

			// resize map to the appropriate size by copying it
			// write town map and read off the original
			switch (anTownSize) {
				case 0:
					if ( needPort )		t_d.port();
					out_f.PutBytes( kSizeOfBig_tr_type, &t_d );
					if ( needPort )		t_d.port();
					in_f.SetMarker( kSizeOfBig_tr_type, CMemStream::eMsMarker );
					break;
				case 1:
					anAve_town_rec = new ave_tr_type( t_d );
					if ( needPort )		anAve_town_rec->port();
					out_f.PutBytes( kSizeOfAve_tr_type, anAve_town_rec );
					delete anAve_town_rec;
					in_f.SetMarker( kSizeOfAve_tr_type, CMemStream::eMsMarker );
					break;
				case 2:
					aTny_town_rec = new tiny_tr_type( t_d );
					if ( needPort )		aTny_town_rec->port();
					out_f.PutBytes( kSizeOfTiny_tr_type, aTny_town_rec );
					delete aTny_town_rec;
					in_f.SetMarker( kSizeOfTiny_tr_type, CMemStream::eMsMarker );
					break;
				default:
					break;
			}

		} else { // transfer unedited town
			if ( in_f.TransferBytes( (kSizeOfTown_record_type + max_dim[ anTownSize ]), &out_f ) != 0)
				return 68; //66, 67
		}
	}

	// rename the original to the backup and save new file
	if ( make_backup_file( szFileName ) != 0 )
		return 73;
	if ( out_f.WriteToFile( szFileName ) != 0 )
		return 73;		// 71, 72, need other error dialog ?

	//need to reload current outdoor sector, including bordering sectors
	location spot_hit = {cur_out.x,cur_out.y};
	//load_outdoors(spot_hit,0);
	load_outdoor_and_borders(spot_hit);
	set_up_terrain_buttons();

	return 0;
}
// q_3DModEnd

// Loads a given campaign. Loads the last zone edited into memory for immediate editing.
void load_campaign()
{
	short i;
	FILE *file_id;
	short error;
	long len;
	
	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName; // full path and file name
	ofn.lpstrFileTitle = szTitleName; // just file name
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0) {
		CommDlgExtendedError();		// for debug
		return;
		}
	if (NULL == (file_id = fopen(szFileName, "rb"))) {
		oops_error(28);
		SysBeep(/* 2 */);
		return;
		}	
	
	// generate scenario_path, which means chopping the file name
	// out of szFileName
	strcpy(scenario_path,szFileName);
	for (i = 0; i < _MAX_PATH - 1; i++)
		if (same_string((char *) (szFileName + i),szTitleName)) {
			scenario_path[i] = 0;
			break;
			}	

	len = (long) sizeof(scenario_data_type);
	if ((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
		FSClose(file_id); oops_error(75); return;
		}
	if (scenario.scenario_platform() < 0) {
		give_error("This file is not a legitimate Blades of Avernum scenario.","",0);
		file_is_loaded = FALSE;
		return;
		}
	
	currently_editing_windows_scenario = (Boolean)scenario.scenario_platform();
	
	FSClose(file_id);

	if (currently_editing_windows_scenario == FALSE)
		scenario.port();
		
	refresh_graphics_library();
	overall_mode = 0;	

// q_3DModStart
	change_made_town = change_made_outdoors = FALSE;
// q_3DModEnd

	load_town(scenario.last_town_edited);
	//load_town(0);

// q_3DModStart
//	load_outdoors(scenario.last_out_edited,0);
// q_3DModEnd

	load_outdoor_and_borders(scenario.last_out_edited);
	file_is_loaded = TRUE;
	clear_selected_copied_objects();
	//load_spec_graphics();

	// load in all the scenario data
	// First, initialize all scen data
	scen_data.clear_scen_item_data_type();

	if (load_core_scenario_data() == FALSE) {
		file_is_loaded = FALSE;
		return;
		}
	char scen_file_name [_MAX_FNAME + _MAX_EXT] = ""; // just file name
	get_name_of_current_scenario(scen_file_name);
	if (load_individual_scenario_data(scen_file_name) == FALSE) {
		file_is_loaded = FALSE;
		return;
		}
	set_up_terrain_buttons();
}

// returns the name fo the currently edited scenario, with the extension stripped off
void get_name_of_current_scenario(char *name)
{
	char file_name[256];
	strcpy(file_name,szTitleName);
	for (short i = 0; i < 252; i++)
		if ((file_name[i] == '.') && (file_name[i + 1] == 'b') 
		  && (file_name[i + 2] == 'a') && (file_name[i + 3] == 's'))
		  	file_name[i] = 0;
	strcpy(name,(char *) file_name);
}


// q_3DModStart
void load_outdoor_and_borders(location which_out)
{
	short i,j;
	location temp_current_out;
	
	if (overall_mode == 61)
		return;
	
	for(j = -1; j <= 1; j++) {
		for(i = -1; i <= 1; i++) {
			temp_current_out.x = which_out.x + (char)i;
			temp_current_out.y = which_out.y + (char)j;
			
			if(temp_current_out.x < 0 || temp_current_out.y < 0 || 
			temp_current_out.x >= scenario.out_width || temp_current_out.y >= scenario.out_height)
				continue;
			
			load_outdoor( temp_current_out, border_terrains[i + 1][j + 1] );
		}
	}

	current_terrain = border_terrains[1][1];
	cur_out = which_out;

	showed_graphics_error = FALSE;
	kludge_correct_old_bad_data();
}
// q_3DModEnd


void load_outdoor( location which_out, outdoor_record_type & to_where )
{
	FILE *file_id;
	long len,len_to_jump;
	short out_sec_num;
	short error;
	
	if (overall_mode == 61)
		return;
		
	if (NULL == (file_id = fopen(szFileName, "rb"))) {
		oops_error(28);
		SysBeep(/* 2 */);
		return;
		}
		
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	len_to_jump = sizeof(scenario_data_type)
					+ (long) (out_sec_num) * (long) (sizeof(outdoor_record_type));
	
	error = SetFPos (file_id, 1, len_to_jump);	
	if (error != 0) {FSClose(file_id);oops_error(77);return;}

	len = sizeof(outdoor_record_type);
	error = FSRead(file_id, &len, (char *) &to_where);
	if (error != 0) {FSClose(file_id);oops_error(78);return;}

	if (currently_editing_windows_scenario == FALSE)
		to_where.port();

	error = FSClose(file_id);
	if (error != 0) {FSClose(file_id);oops_error(79);return;}
}

void load_town(short which_town)
{
	short i,j;
	FILE *file_id;
	long len,len_to_jump = 0,store;
	short error;

	if (NULL == (file_id = fopen(szFileName, "rb"))) {
		oops_error(28);
		SysBeep(/* 2 */);
		return;
		}	
		
	len_to_jump = sizeof(scenario_data_type);

	store = (long) (scenario.out_width * scenario.out_height) * (long) (sizeof(outdoor_record_type));
	len_to_jump += store;

	store = 0;
	for (i = 0; i < which_town; i++)
		switch (scenario.town_size[i]) {
			case 0: store += sizeof (big_tr_type) + sizeof(town_record_type); break;
			case 1: store += sizeof (ave_tr_type) + sizeof(town_record_type); break;
			case 2: store += sizeof (tiny_tr_type) + sizeof(town_record_type); break;
			}
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);
	if (error != 0) {FSClose(file_id);oops_error(81);return;}
	
	len = sizeof(town_record_type);
	
	error = FSRead(file_id, &len , (char *) &town);
	//port_town();
	if (error != 0) {FSClose(file_id);oops_error(82);return;}

	if (currently_editing_windows_scenario == FALSE)
		town.port();

	switch (scenario.town_size[which_town]) {
		case 0:
			len =  sizeof(big_tr_type);
			error = FSRead(file_id, &len, (char *) &t_d);
			if (error != 0) {FSClose(file_id); oops_error(83);return;}
			if (currently_editing_windows_scenario == FALSE)
				t_d.port();
			break;
			
		case 1:
			len = sizeof(ave_tr_type);
			error = FSRead(file_id, &len, (char *) &ave_t);
			if (error != 0) {FSClose(file_id); oops_error(84);return;}
			if (currently_editing_windows_scenario == FALSE)
				ave_t.port();
				for (i = 0; i < 48; i++)
					for (j = 0; j < 48; j++) {
						t_d.terrain[i][j] = ave_t.terrain[i][j];
						t_d.floor[i][j] = ave_t.floor[i][j];
						t_d.height[i][j] = ave_t.height[i][j];
						t_d.lighting[i][j] = ave_t.lighting[i][j];					
						}

			break;
			
		case 2:
			len = sizeof(tiny_tr_type);
			error = FSRead(file_id,&len , (char *) &tiny_t);
			if (error != 0) {FSClose(file_id); oops_error(85);return;}
			if (currently_editing_windows_scenario == FALSE)
				tiny_t.port();
			for (i = 0; i < 32; i++)
				for (j = 0; j < 32; j++) {
					t_d.terrain[i][j] = tiny_t.terrain[i][j];
					t_d.floor[i][j] = tiny_t.floor[i][j];
					t_d.height[i][j] = tiny_t.height[i][j];
					t_d.lighting[i][j] = tiny_t.lighting[i][j];					
					}

			break;
		}

	
	clear_selected_copied_objects();
	town_type = scenario.town_size[which_town];
	cur_town = which_town;
	error = FSClose(file_id);
	if (error != 0) {FSClose(file_id);oops_error(86);return;}
	showed_graphics_error = FALSE;
	kludge_correct_old_bad_data();
}


void oops_error(short error)
{
	char error_str[256];
	
	beep();

	sprintf((char *) error_str,"Giving the scenario editor more memory might also help. Be sure to back your scenario up often. Error number: %d.", (int)error);
	give_error("The program encountered an error while loading/saving/creating the scenario. To prevent future problems, the program will now terminate. Trying again may solve the problem.",(char *) error_str,0);
	file_is_loaded = FALSE;
	redraw_screen();
}


void start_data_dump()
{
	short i;
	char get_text[280];
	FILE *data_dump_file_id;
	long len;

	if (NULL == (data_dump_file_id = fopen("Scenario Data", "wb"))) {
		oops_error(11);
		return;
		}	
	
	sprintf((char *)get_text,"Scenario data for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"Floor types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);


	for (i = 0; i < 255; i++) 
		if (scen_data.scen_floors[i].ed_pic.not_legit() == FALSE) {
			sprintf((char *)get_text,"  Floor %d: %s\r",(int)i,scen_data.scen_floors[i].floor_name);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}	

	sprintf((char *)get_text,"\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"Terrain types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);


	for (i = 0; i < 511; i++) 
		if (scen_data.scen_ter_types[i].ed_pic.not_legit() == FALSE) {
			sprintf((char *)get_text,"  Terrain %d: %s\r", (int)i,scen_data.scen_ter_types[i].ter_name);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}	

	sprintf((char *)get_text,"\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"Creature types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	for (i = 0; i < 256; i++) {
		sprintf((char *)get_text,"  Creature %d: %s\r",(int)i,scen_data.scen_creatures[i].name);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	
		}	

	sprintf((char *)get_text,"\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"Item types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	for (i = 0; i < NUM_SCEN_ITEMS; i++) {
		sprintf((char *)get_text,"  Item %d: %s\r", (int)i,scen_data.scen_items[i].full_name);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	
		}	
	FSClose(data_dump_file_id);
}	

/*
short str_to_num(char *str) 
{
	long l;
	
	StringToNum(str,&l);
	return (short) l;
}
*/

Boolean create_basic_scenario(char *scen_name_short,char *scen_name_with_ext,char *scen_full_name,short out_width,short out_height,short on_surface,Boolean use_warriors_grove)
{
	short i,j,num_outdoors;
	FILE *scen_file_id;
	short error;
	long len;
	char scen_file_name[_MAX_PATH],scen_script_name[_MAX_PATH];
	char slash[3] = " ";
	slash[0] = 92;

	char new_directory[_MAX_PATH];
	sprintf(new_directory,"%s%s%s",store_editor_path,slash,scen_name_short);
	error = (short)CreateDirectory(new_directory,NULL);

	sprintf((char *) scen_script_name,"%s.txt",scen_name_short);
	sprintf((char *) scen_file_name,"%s%s%s",new_directory,slash,scen_name_with_ext);
	
	if (NULL == (scen_file_id = fopen(scen_file_name, "wb"))) {
		SysBeep( /*2*/ );
		return FALSE;
		}	

	// now we have folder and have created scenario file. clear
	// records and start building.
	current_terrain.clear_outdoor_record_type();
	town.clear_town_record_type();
	t_d.clear_big_tr_type();
	ave_t.clear_ave_tr_type();
	scenario.clear_scenario_data_type();
	
	if (on_surface) {
		for (i = 0; i < 48; i++)
			for (j = 0; j < 48; j++)
				ave_t.floor[i][j] = 37;
		for (i = 0; i < 48; i++)
			for (j = 0; j < 48; j++)
				current_terrain.floor[i][j] = 37;
		}
	
	current_terrain.is_on_surface = on_surface;
	town.is_on_surface = on_surface;
	
	sprintf(scenario.scen_name,scen_full_name);
	scenario.out_width = out_width;
	scenario.out_height = out_height;

	if (use_warriors_grove) {
		scenario.what_start_loc_in_town.x = 24;
		scenario.what_start_loc_in_town.y = 24;
		}
	
	scenario.port();	
	len = sizeof(scenario_data_type); // write scenario data
	if ((error = FSWrite(scen_file_id, &len, (char *) &scenario)) != 0) {
		oops_error(103);
		return FALSE;
		}		
	scenario.port();

	num_outdoors = scenario.out_width * scenario.out_height;
	for (i = 0; i < num_outdoors; i++) {// write outdoor sections
		len = sizeof(outdoor_record_type);
		warrior_grove_out.port();
		current_terrain.port();
		
		if (( i == 0) && (use_warriors_grove))
			FSWrite(scen_file_id, &len, (char *) &warrior_grove_out); 
			else FSWrite(scen_file_id, &len, (char *) &current_terrain); 

		warrior_grove_out.port();
		current_terrain.port();
		}	

	
	// write towns	
	warrior_grove_town.port();
	warrior_grove_terrain.port();
	town.port();
	ave_t.port();
	if (use_warriors_grove) {
		len = sizeof(town_record_type);
		FSWrite(scen_file_id, &len, (char *) &warrior_grove_town); 
		len = sizeof(ave_tr_type);
		FSWrite(scen_file_id, &len, (char *) &warrior_grove_terrain); 
		}
		else {
			len = sizeof(town_record_type);
			FSWrite(scen_file_id, &len, (char *) &town); 
			len = sizeof(ave_tr_type);
			FSWrite(scen_file_id, &len, (char *) &ave_t); 
			}	
	warrior_grove_town.port();
	warrior_grove_terrain.port();
	town.port();
	ave_t.port();

	// now, everything is moved over. Delete the original, and rename the dummy
	FSClose(scen_file_id);		
	currently_editing_windows_scenario = FALSE;

	// copy over the basic needed scripts and give right names.
	copy_script("trap.txt","trap.txt",new_directory);
	copy_script("door.txt","door.txt",new_directory);
	copy_script("specobj.txt","specobj.txt",new_directory);
	copy_script("basicnpc.txt","basicnpc.txt",new_directory);
	copy_script("scen.txt",(char *) scen_script_name,new_directory);
	copy_script("guard.txt","guard.txt",new_directory);
	if (use_warriors_grove) {
		copy_script("t0wargrove.txt","t0wargrove.txt",new_directory);
		copy_script("t0wargrovedlg.txt","t0wargrovedlg.txt",new_directory);
		}		

	return TRUE;
	
}

// import a blades scenario

// import an outdoor section from other scenario
// returns true if the import completed without error
bool import_boa_town()
{
	short i,j, which_town;
	long len,len_to_jump = 0,store;
	//StandardFileReply s_reply;
	short error;
	FILE *file_id;
	char import_file_path[_MAX_PATH] = "";
	char import_file_name[_MAX_FNAME + _MAX_EXT] = "";
	
	which_town = pick_import_town(841 /*,0 */);
	if (which_town < 0)
		return(false);

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = import_file_path; // full path and file name
	ofn.lpstrFileTitle = import_file_name; // just file name
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0)
		return(false);
	if (NULL == (file_id = fopen(import_file_path, "rb"))) {
		oops_error(28);
		SysBeep(/* 2 */);
		return(false);
	}	

	
	len = (long) kSizeOfScenario_data_type;
	if ((error = FSRead(file_id, &len, (char *) &temp_scenario)) != 0){
		FSClose(file_id); oops_error(201); 
        return(false);
	}

	if (temp_scenario.scenario_platform() < 0) {
		FSClose(file_id);
		give_error("This file is not a legitimate Blades of Avernum scenario.","",0);
		return(false);
	}
	
	temp_scenario.port();
		
	if (temp_scenario.num_towns <= which_town) {
		give_error("The town number you picked is too high. The scenario you selected doesn't have enough towns.",
			"",0);
		FSClose(file_id);
		return(false);
	}
	if (temp_scenario.town_size[which_town] != scenario.town_size[cur_town]) {
		give_error("You must import a town of the same size/type as the current one.",
			"",0);
		FSClose(file_id);
		return(false);
	}

	len_to_jump = 0;

	store = (long) (temp_scenario.out_width * temp_scenario.out_height) * (long) kSizeOfOutdoor_record_type;
	len_to_jump += store;

	store = 0;
	for (i = 0; i < which_town; i++){
		switch (temp_scenario.town_size[i]) {
			case 0: store += kSizeOfBig_tr_type + kSizeOfTown_record_type; break;
			case 1: store += kSizeOfAve_tr_type + kSizeOfTown_record_type; break;
			case 2: store += kSizeOfTiny_tr_type + kSizeOfTown_record_type; break;
		}
	}
	len_to_jump += store;
	
	error = SetFPos (file_id, 3, len_to_jump);
	if (error != 0) {FSClose(file_id);oops_error(202);}

	len = kSizeOfTown_record_type;
	
	error = FSRead(file_id, &len , (char *) &town);
	//port_town();
	if (error != 0) {FSClose(file_id);oops_error(203);}

	town.port();
		
	switch (temp_scenario.town_size[which_town]) {
		case 0:
			len =  kSizeOfBig_tr_type;
			error = FSRead(file_id, &len, (char *) &t_d);
			if (error != 0) {FSClose(file_id); oops_error(204);}
			t_d.port();
			break;
			
		case 1:
			len = kSizeOfAve_tr_type;
			error = FSRead(file_id, &len, (char *) &ave_t);
			if (error != 0) {FSClose(file_id); oops_error(205);}
			ave_t.port();
			for (i = 0; i < 48; i++)
				for (j = 0; j < 48; j++) {
					t_d.terrain[i][j] = ave_t.terrain[i][j];
					t_d.floor[i][j] = ave_t.floor[i][j];
					t_d.height[i][j] = ave_t.height[i][j];
					t_d.lighting[i][j] = ave_t.lighting[i][j];					
					}
			break;
			
		case 2:
			len = kSizeOfTiny_tr_type;
			error = FSRead(file_id,&len , (char *) &tiny_t);
			if (error != 0) {FSClose(file_id); oops_error(206);}
			tiny_t.port();
			for (i = 0; i < 32; i++)
				for (j = 0; j < 32; j++) {
					t_d.terrain[i][j] = tiny_t.terrain[i][j];
					t_d.floor[i][j] = tiny_t.floor[i][j];
					t_d.height[i][j] = tiny_t.height[i][j];
					t_d.lighting[i][j] = tiny_t.lighting[i][j];					
					}
			break;

		}

	error = FSClose(file_id);
	if(error != 0) {
 		FSClose(file_id);
		oops_error(207);
		return(false);
	}
	return(true);
}

bool import_boa_outdoors()
{
	long len,len_to_jump = 0,store;
	FILE *file_id;
	short error;
	char import_file_path[_MAX_PATH] = "";
	char import_file_name[_MAX_FNAME + _MAX_EXT] = "";

	

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = import_file_path; // full path and file name
	ofn.lpstrFileTitle = import_file_name; // just file name
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0)
		return(false);
	if (NULL == (file_id = fopen(import_file_path, "rb"))) {
		oops_error(28);
		SysBeep(/* 2 */);
		return(false);
	}	
	
	len = (long) kSizeOfScenario_data_type;
	if ((error = FSRead(file_id, &len, (char *) &temp_scenario)) != 0){
		FSClose(file_id); oops_error(301); return(false);
	}
	if (temp_scenario.scenario_platform() < 0) {
		give_error("This file is not a legitimate Blades of Avernum scenario.","",0);
		return(false);
	}
	
	temp_scenario.port();

	location dummy_loc = {0,0};
	short which_section = pick_out(dummy_loc,temp_scenario.out_width,temp_scenario.out_height);
	if (which_section < 0) {
		FSClose(file_id); return(false);
	}
	location spot_hit = {(t_coord)(which_section / 100), (t_coord)(which_section % 100)};
	which_section = spot_hit.y * temp_scenario.out_width + spot_hit.x;
	
	len_to_jump = kSizeOfScenario_data_type;

	store = (long) (which_section) * (long) kSizeOfOutdoor_record_type;
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);
	if (error != 0) {FSClose(file_id);oops_error(302);}

	len = kSizeOfOutdoor_record_type;
	
	error = FSRead(file_id, &len , (char *) &current_terrain);

	current_terrain.port();

	error = FSClose(file_id);
	if (error != 0) {FSClose(file_id);oops_error(307); return(false);}
	return(true);
}

// Variables for importing and porting old scenarios
old_blades_scenario_data_type blades_scen;
old_blades_scen_item_data_type blades_scen_data;
old_blades_piles_of_stuff_dumping_type blades_data_dump;
 
 /*
void extract_old_scen_text()
{
	short i,j,k,l,file_id;
	//StandardFileReply s_reply;
	Boolean file_ok = FALSE;
	short error;
	long len;
	FSSpec old_scen_file_to_load;	
	FSSpec dump_file;
	
	//StandardGetFile(NULL,-1,NULL,&s_reply);
			
	//if (s_reply.sfGood == FALSE)
	//	return;
					
	//old_scen_file_to_load = s_reply.sfFile;

	if (SelectSaveFileToOpen(&default_directory,&old_scen_file_to_load) == FALSE)
		return;

	if ((error = FSpOpenDF(&old_scen_file_to_load,1,&file_id)) != 0) {
		oops_error(400);
		EdSysBeep(2);	return;
		}	
	
	len = (long) sizeof(old_blades_scenario_data_type);
	if ((error = FSRead(file_id, &len, (char *) &blades_scen)) != 0){
		FSClose(file_id); oops_error(401); return;
		}
	len = (long) sizeof(old_blades_scen_item_data_type);
	if ((error = FSRead(file_id, &len, (char *) &blades_scen_data)) != 0){
		FSClose(file_id); oops_error(402); return;
		}
	for (i = 0; i < 270; i++) {
		len = (long) (blades_scen.scen_str_len[i]);
		if ((error = FSRead(file_id, &len, (char *) &(blades_data_dump.scen_strs[i]))) != 0) {
			FSClose(file_id); oops_error(403); return;
			}
		blades_data_dump.scen_strs[i][len] = 0;
		}

	FSClose(file_id);
	
	// now dump to text file
	FSMakeFSSpec(start_volume,start_dir,"\pBlades Scenario Dump",&dump_file);
	FSpDelete(&dump_file);
	error = FSpCreate(&dump_file,'ttxt','TEXT',smSystemScript);
	if ((error = FSpOpenDF(&dump_file,3,&data_dump_file_id)) != 0) {
		SysBeep(50);
		return;
		}			

	SetFPos (data_dump_file_id, 2, 0);

	char get_text[256];
	short species_trans[15] = {0,5,6,12,0, 0,1,7,8,4, 10,11,9,5,6};

	for (i = 0; i < 256; i++) {
		sprintf((char *)get_text,"begindefinecreature %d;\r",i);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		sprintf((char *)get_text,"\tclear;\r",i);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);

		sprintf((char *)get_text,"\tcr_name = ~%s~;\r",blades_scen_data.monst_names[i]);
		for (j = 0; j < 256; j++)
			if (get_text[j] == '~')
				get_text[j] = 34;
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);	

		sprintf((char *)get_text,"\tcr_level = %d;\r",blades_scen.scen_monsters[i].level * 2);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);	
		
		if (blades_scen.scen_monsters[i].a[0] > 0) {
			sprintf((char *)get_text,"\tcr_attack_1 = 6;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}
		if (blades_scen.scen_monsters[i].a[1] > 0) {
			sprintf((char *)get_text,"\tcr_attack_2 = 6;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}
		if (blades_scen.scen_monsters[i].a[2] > 0) {
			sprintf((char *)get_text,"\tcr_attack_3 = 6;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}
		if (blades_scen.scen_monsters[i].a1_type > 0) {
			sprintf((char *)get_text,"\tcr_attack_1_type = %d;\r",blades_scen.scen_monsters[i].a1_type);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}
		if (blades_scen.scen_monsters[i].a23_type > 0) {
			sprintf((char *)get_text,"\tcr_attack_23_type = %d;\r",blades_scen.scen_monsters[i].a23_type);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}

		sprintf((char *)get_text,"\tcr_species = %d;\r",species_trans[blades_scen.scen_monsters[i].m_type]);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		
		switch (blades_scen.scen_monsters[i].default_attitude) {
			case 0: case 1: sprintf((char *)get_text,"\tcr_default_attitude = 2; \r"); break;
			case 2: sprintf((char *)get_text,"\tcr_default_attitude = 4; \r"); break;
			case 3: sprintf((char *)get_text,"\tcr_default_attitude = 5; \r"); break;
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
			
		if (blades_scen.scen_monsters[i].immunities & 8) {
			sprintf((char *)get_text,"\tcr_immunities 0 = 100;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
			else if (blades_scen.scen_monsters[i].immunities & 4) {
				sprintf((char *)get_text,"\tcr_immunities 0 = 50;\r");
				len = (long) (strlen((char *)get_text));
				FSWrite(data_dump_file_id, &len, (char *) get_text);
				}
		if (blades_scen.scen_monsters[i].immunities & 32) {
			sprintf((char *)get_text,"\tcr_immunities 1 = 100;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
			else if (blades_scen.scen_monsters[i].immunities & 16) {
				sprintf((char *)get_text,"\tcr_immunities 1 = 50;\r");
				len = (long) (strlen((char *)get_text));
				FSWrite(data_dump_file_id, &len, (char *) get_text);
				}
		if (blades_scen.scen_monsters[i].immunities & 2) {
			sprintf((char *)get_text,"\tcr_immunities 2 = 100;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
			else if (blades_scen.scen_monsters[i].immunities & 1) {
				sprintf((char *)get_text,"\tcr_immunities 2 = 50;\r");
				len = (long) (strlen((char *)get_text));
				FSWrite(data_dump_file_id, &len, (char *) get_text);
				}
		if (blades_scen.scen_monsters[i].immunities & 128) {
			sprintf((char *)get_text,"\tcr_immunities 4 = 100;\r");
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
			else if (blades_scen.scen_monsters[i].immunities & 64) {
				sprintf((char *)get_text,"\tcr_immunities 4 = 50;\r");
				len = (long) (strlen((char *)get_text));
				FSWrite(data_dump_file_id, &len, (char *) get_text);
				}
		sprintf((char *)get_text,"\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}

	sprintf((char *)get_text,"\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	FSClose(data_dump_file_id);	
}
*/

void EdSysBeep( /* short duration */ )
{
	beep();
}

// given a script name, copies that script from the directory the editor is in to 
// the directory of the current, active scenario
Boolean copy_script(char *script_source_name,char *script_dest_name,char *dest_directory)
{
	char file_name[256],dest_file_name[256];
	char slash[3] = " ";
	slash[0] = 92;
	
	sprintf(file_name,"%s%s%s",store_editor_path,slash,script_source_name);
	sprintf(dest_file_name,"%s%s%s",dest_directory,slash,script_dest_name);
	CopyFile(file_name,dest_file_name,FALSE);
	
	return TRUE;

}

void init_warriors_grove()
{
	FILE *file_id;
	short error;
	long len;
//	char grove_file[256] = "warriorgrove.bas";
	char grove_file[256];

	strcpy( grove_file, store_editor_path );
	strcat( grove_file, "\\" );
	strcat( grove_file, "warriorgrove.bas" );

	if (NULL == (file_id = fopen(grove_file, "rb"))) {
		give_error("The editor was unable to find and open the file warriorgrove.bas. Until this file is restored, you will not be able to make a new scenario with Warrior's Grove in it.","",0);
		SysBeep(/* 2 */);
		return;
		}	
	
	len = (long) sizeof(scenario_data_type);
	if ((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
		FSClose(file_id); return;
		}

	len = (long) sizeof(outdoor_record_type);
	if ((error = FSRead(file_id, &len, (char *) &warrior_grove_out)) != 0){
		FSClose(file_id); return;
		}
	warrior_grove_out.port();
	
	len = (long) sizeof(town_record_type);
	if ((error = FSRead(file_id, &len, (char *) &warrior_grove_town)) != 0){
		FSClose(file_id); return;
		}
	warrior_grove_town.port();
	
	len = (long) sizeof(ave_tr_type);
	if ((error = FSRead(file_id, &len, (char *) &warrior_grove_terrain)) != 0){
		FSClose(file_id); return;
		}
	warrior_grove_terrain.port();

	FSClose(file_id);
}

// data for Blades of Exile porting

void import_blades_of_exile_scenario()
{
	short i,j;
	long len;
	FILE *file_id,*new_scen_id;
	Boolean file_ok = FALSE;
	short error;
	Boolean cur_scen_is_mac = FALSE;
	char importFileName [_MAX_PATH] = ""; // full path and file name
	char importTitleName [_MAX_FNAME + _MAX_EXT] = ""; // just file name
	char import_source[_MAX_FNAME + _MAX_EXT] = ""; // full path and file name
	char slash[3] = " ";
	slash[0] = 92;
	
	// Initialize scen data. We'll need it.
	load_core_scenario_data();
	
	// STEP 1 select BoE file
	file_is_loaded = FALSE;
	
	ofn_import_boe.hwndOwner = mainPtr;
	ofn_import_boe.lpstrFile = importFileName; // full path and file name
	ofn_import_boe.lpstrFileTitle = importTitleName; // just file name
	ofn_import_boe.Flags = 0;

	if (GetOpenFileName(&ofn_import_boe) == 0) {
		CommDlgExtendedError();		// for debug
		return;
		}
					
	if (NULL == (file_id = fopen(importFileName, "rb"))) {
		SysBeep(/* 2 */);
		return;
		}	

	
	// STEP 2 load all BoE scenario data
	len = (long) sizeof(old_blades_scenario_data_type);
	if ((error = FSRead(file_id, &len, (char *) &boe_scenario)) != 0){
		FSClose(file_id); 
		oops_error(301); 
		return;
		}

	// check if legit scenario and whether check mac or win
	if ((boe_scenario.flag1 == 10) && (boe_scenario.flag2 == 20)
	 && (boe_scenario.flag3 == 30)
	  && (boe_scenario.flag4 == 40)) {
	  	cur_scen_is_mac = TRUE;
	  	file_ok = TRUE;
	  	boe_port_scenario();
	  	}
	if ((boe_scenario.flag1 == 20) && (boe_scenario.flag2 == 40)
	 && (boe_scenario.flag3 == 60)
	  && (boe_scenario.flag4 == 80)) {
	  	cur_scen_is_mac = FALSE;
	  	file_ok = TRUE;
	  	}
	 if (file_ok == FALSE) {
		FSClose(file_id); 
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return;	 
		}

	len = sizeof(old_blades_scen_item_data_type); // item data
	if ((error = FSRead(file_id, &len, (char *) &boe_scen_data)) != 0) {
		FSClose(file_id); 
		oops_error(302); 
		return;
		}
	if (cur_scen_is_mac == TRUE)
		boe_port_item_list();
		
	for (i = 0; i < 270; i++) {
		len = (long) (boe_scenario.scen_str_len[i]);
		if ((len > 0) && ((error = FSRead(file_id, &len, (char *) &(boe_scen_text.scen_strs[i]))) != 0)) {
			FSClose(file_id); 
			oops_error(303); 
			return;
			}
		boe_scen_text.scen_strs[i][len] = 0;
		}

	// STEP 2 Start building new scenario
	
	// alter scenario file name. we need to make sure to change name somewhat
	strcpy(import_source,importTitleName);
	for (i = 0; i < _MAX_FNAME + _MAX_EXT - 3; i++)
		if (((import_source[i] == 'e') || (import_source[i] == 'E'))
		  && ((import_source[i + 1] == 'x')|| (import_source[i + 1] == 'X'))
		   && ((import_source[i + 2] == 's') || (import_source[i + 2] == 'S'))) {
			import_source[i] = 'b';
			import_source[i + 1] = 'a';
			import_source[i + 2] = 's';
			}
	char new_scen_name[_MAX_FNAME + _MAX_EXT];
	strcpy(new_scen_name,import_source);

	// make name of scen_script
	char scen_script_name[_MAX_FNAME + _MAX_EXT];
	strcpy(scen_script_name,import_source);
	for (i = 0; i < _MAX_FNAME + _MAX_EXT - 3; i++)
		if (((scen_script_name[i] == 'b') || (scen_script_name[i] == 'B')) && 
		 ((scen_script_name[i + 1] == 'a') || (scen_script_name[i + 1] == 'A')) && 
		 ((scen_script_name[i + 2] == 's') || (scen_script_name[i + 2] == 'S'))) {
			scen_script_name[i] = 't';
			scen_script_name[i + 1] = 'x';
			scen_script_name[i + 2] = 't';
			}

	// create new directory and scen file
	char new_directory[_MAX_PATH];
	sprintf(new_directory,"%s%s%s",store_editor_path,slash,new_scen_name);
	error = (short)CreateDirectory(new_directory,NULL);
	if (error == FALSE) { //failed to make, try another name
		// construct name of new scenairo
		for (i = 0; i < _MAX_PATH - 6; i++)
			if (new_directory[i] == 0) {
				new_directory[i] = ' ';
				new_directory[i + 1] = 'c';
				new_directory[i + 2] = 'o';
				new_directory[i + 3] = 'n';
				new_directory[i + 4] = 'v';
				new_directory[i + 5] = 0;
				i = _MAX_PATH;
				}
		error = (short)CreateDirectory(new_directory,NULL);
		if (error == FALSE) {
			char error[256];
			sprintf(error,"Tried to create scenario with name %s, but there was already a file or folder there with that name.",(char *) new_scen_name);
			give_error(error,"",0);
			return;
			}
		}

	char new_scen_path[_MAX_PATH];
	sprintf(new_scen_path,"%s%s",new_directory,slash);
	char new_scen_path_and_name[_MAX_PATH];
	sprintf(new_scen_path_and_name,"%s%s%s",new_directory,slash,new_scen_name);
	if (NULL == (new_scen_id = fopen(new_scen_path_and_name, "wb"))) {
		oops_error(11);
		return;
		}	
		
	currently_editing_windows_scenario = FALSE;
	
	// initialize data and write scenario data
	current_terrain.clear_outdoor_record_type();
	town.clear_town_record_type();
	t_d.clear_big_tr_type();
	ave_t.clear_ave_tr_type();
	scenario.clear_scenario_data_type();

	port_boe_scenario_data();
	port_scenario_script(scen_script_name,new_scen_path);
	
	scenario.port();
	len = kSizeOfScenario_data_type; 
	if ((error = FSWrite(new_scen_id, &len, (char *) &scenario)) != 0) 
		{oops_error(307); 
		return;}	
	scenario.port();
		
	// STEP 3 Load in old outdoor sections, one at a time, and port them.
	for (short m = 0; m < boe_scenario.out_height; m++)
		for (short n = 0; n < boe_scenario.out_width; n++) {
			// load outdoor terrain
			len = (long) sizeof(old_blades_outdoor_record_type);
			if ((error = FSRead(file_id, &len, (char *) &boe_outdoor)) != 0){
				FSClose(file_id); 
				oops_error(308); 
				return;
				}
			if (cur_scen_is_mac == TRUE)
				boe_port_out(&boe_outdoor);

			for (i = 0; i < 120; i++) {
				len = (long) (boe_outdoor.strlens[i]);
				FSRead(file_id, &len, (char *) &(boe_scen_text.out_strs[i]));
				boe_scen_text.out_strs[i][len] = 0;
				}

			// port outdoor terrain
			port_boe_out_data();	

			// tweak for windows. if windows scen, we need to flip rectangles
			if (cur_scen_is_mac == FALSE) {
				for (i = 0; i < 8; i++) 
					alter_rect(&current_terrain.info_rect[i]);
				}

			// port outdoor scripts
			char out_script_name[256];
			char dummy_str[256];
			trunc_str(current_terrain.name);
			sprintf((char *) dummy_str,"o%d%d%s",n,m,current_terrain.name);
			clean_str(dummy_str);
			dummy_str[SCRIPT_NAME_LEN - 1] = 0;
			strcpy(current_terrain.section_script,dummy_str);
			sprintf((char *) out_script_name,"%s.txt",dummy_str);
			port_outdoor_script(out_script_name,new_scen_path,n,m);
			
			// write ported outdoor section
			current_terrain.port();
			len = kSizeOfOutdoor_record_type;
			if ((error = FSWrite(new_scen_id, &len, (char *) &current_terrain)) != 0) 
				{oops_error(309); 
				return;}	
			current_terrain.port();
			}

	// STEP 4 Load in towns, one at a time, and port them.
	for (short m = 0; m < boe_scenario.num_towns; m++) {
		len = sizeof(old_blades_town_record_type);
		error = FSRead(file_id, &len , (char *) &boe_town);
		if (error != 0) 
		   {FSClose(file_id);oops_error(310); 
		   return;
		   }
		if (cur_scen_is_mac == TRUE)
			boe_port_town();
		
		switch (boe_scenario.town_size[m]) {
			case 0:
				len =  sizeof(old_blades_big_tr_type);
				error = FSRead(file_id, &len, (char *) &boe_big_town);
				break;
				
			case 1:
				len = sizeof(old_blades_ave_tr_type);
				error = FSRead(file_id, &len, (char *) &boe_ave_town);
				break;
				
			case 2:
				len = sizeof(old_blades_tiny_tr_type);
				error = FSRead(file_id,&len , (char *) &boe_tiny_town);
				break;
			}
		if (error != 0) {FSClose(file_id);oops_error(311); }
		
		for (i = 0; i < 140; i++) {
			len = (long) (boe_town.strlens[i]);
			FSRead(file_id, &len, (char *) &(boe_scen_text.town_strs[i]));
			boe_scen_text.town_strs[i][len] = 0;
			}

		len = sizeof(old_blades_talking_record_type);
		error = FSRead(file_id, &len , (char *) &boe_talk_data);
		if (error != 0) {
		   		  FSClose(file_id);
	 			  oops_error(312); 
  				  return;
				  }
		if (cur_scen_is_mac == TRUE)
			boe_port_talk_nodes();
		
		for (i = 0; i < 170; i++) {
			len = (long) (boe_talk_data.strlens[i]);
			FSRead(file_id, &len, (char *) &(boe_scen_text.talk_strs[i]));
			boe_scen_text.talk_strs[i][len] = 0;
			}


		// port town terrain
		port_boe_town_data(m,cur_scen_is_mac);

		set_all_items_containment();
		
		// Port town scripts
		char town_script_name[256];
		char dummy_str[256];
		trunc_str(town.town_name);
		sprintf((char *) dummy_str,"t%d%s",m,town.town_name);
		clean_str(dummy_str);
		dummy_str[SCRIPT_NAME_LEN - 1] = 0;
		strcpy(town.town_script,dummy_str);
		sprintf((char *) town_script_name,"%s.txt",dummy_str);
		port_town_script(town_script_name,new_scen_path,m);

		// port town dlog script
		char town_dlg_script_name[256];
		char dummy_str2[256];
		sprintf((char *) dummy_str2,"%sdlg",dummy_str);
		sprintf((char *) town_dlg_script_name,"%s.txt",dummy_str2);
		port_town_dialogue_script(town_dlg_script_name,new_scen_path,m);

		// write ported town
		town.port();
		len = kSizeOfTown_record_type;
		if ((error = FSWrite(new_scen_id, &len, (char *) &town)) != 0) 
			{oops_error(313); 
			return;
			}
		town.port();
		
		town_type = scenario.town_size[m];
		set_up_lights();

		t_d.port();
		switch (scenario.town_size[m]) {
			case 0:
				len = kSizeOfBig_tr_type;
				FSWrite(new_scen_id, &len, (char *) &t_d);
				break;
		
			case 1:
				for (i = 0; i < 48; i++)
					for (j = 0; j < 48; j++) {
						ave_t.terrain[i][j] = t_d.terrain[i][j];
						ave_t.floor[i][j] = t_d.floor[i][j];
						ave_t.height[i][j] = t_d.height[i][j];
						ave_t.lighting[i][j] = t_d.lighting[i][j];					
						}					

				len = kSizeOfAve_tr_type;
				FSWrite(new_scen_id, &len, (char *) &ave_t);
			break;
		
		
			case 2:
				for (i = 0; i < 32; i++)
					for (j = 0; j < 32; j++) {
						tiny_t.terrain[i][j] = t_d.terrain[i][j];
						tiny_t.floor[i][j] = t_d.floor[i][j];
						tiny_t.height[i][j] = t_d.height[i][j];
						tiny_t.lighting[i][j] = t_d.lighting[i][j];					
						}

				len = kSizeOfTiny_tr_type;
				FSWrite(new_scen_id, &len, (char *) &tiny_t);
				break;

			}
		t_d.port();
			
		}
		
	// STEP 5 close stuff up
	FSClose(new_scen_id);
	FSClose(file_id);
	
	// copy over needed generic scripts
	copy_script("trap.txt","trap.txt",new_directory);
	copy_script("door.txt","door.txt",new_directory);
	copy_script("specobj.txt","specobj.txt",new_directory);
	copy_script("basicnpc.txt","basicnpc.txt",new_directory);
	copy_script("guard.txt","guard.txt",new_directory);
}

void port_boe_scenario_data()
{
	short i;
	
	scenario.num_towns = boe_scenario.num_towns;
	scenario.out_width = boe_scenario.out_width;
	scenario.out_height = boe_scenario.out_height;
	scenario.rating = boe_scenario.rating;
	scenario.min_level = boe_scenario.difficulty * 8 + 1;
	scenario.max_level = (boe_scenario.difficulty + 1) * 10;
	
	strcpy(scenario.scen_name,boe_scen_text.scen_strs[0]);
	strcpy(scenario.scen_desc,boe_scen_text.scen_strs[1]);
	strcpy(scenario.credits_text[0],boe_scen_text.scen_strs[2]);
	strcpy(scenario.credits_text[1],boe_scen_text.scen_strs[3]);
	
	for ( i = 0; i < 6; i++)
		strcpy(scenario.intro_text[0][i],boe_scen_text.scen_strs[4 + i]);

	for ( i = 0; i < scenario.num_towns; i++) {
		scenario.town_size[i] = boe_scenario.town_size[i];
		scenario.town_starts_hidden[i] = boe_scenario.town_hidden[i];
		}
		
	scenario.start_in_what_town = boe_scenario.which_town_start;
	scenario.what_start_loc_in_town = boe_scenario.where_start;
	scenario.what_outdoor_section_start_in = boe_scenario.out_sec_start;
	scenario.start_where_in_outdoor_section = boe_scenario.out_start;

	for ( i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = boe_scenario.town_to_add_to[i];
		scenario.flag_to_add_to_town[i][0] = boe_scenario.flag_to_add_to_town[i][0];
		scenario.flag_to_add_to_town[i][1] = boe_scenario.flag_to_add_to_town[i][1];
		}
	
	// Port scenario script
	
}

void port_boe_out_data() 
{
	short i,j;
	Boolean current_section_on_surface = TRUE;
	
	current_terrain.clear_outdoor_record_type();

	boe_scen_text.out_strs[0][19] = 0;
	strcpy(current_terrain.name,boe_scen_text.out_strs[0]);
	trunc_str(current_terrain.name);
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			current_terrain.floor[i][j] = (unsigned char)old_ter_to_floor[boe_outdoor.terrain[i][j]];
			current_terrain.terrain[i][j] = old_ter_to_ter[boe_outdoor.terrain[i][j]];
			current_terrain.height[i][j] = 9;
			
			if (current_terrain.floor[i][j] < 4)
				current_section_on_surface = FALSE;
				else if ((current_terrain.floor[i][j] >= 37) && (current_terrain.floor[i][j] <= 40))
					current_section_on_surface = TRUE;
			}
	for (i = 0; i < 18; i++) 
		if ((boe_outdoor.special_locs[i].x >= 0) && (boe_outdoor.special_locs[i].y >= 0) && (boe_outdoor.special_id[i] >= 0)) {
			current_terrain.special_rects[i].left = current_terrain.special_rects[i].right = boe_outdoor.special_locs[i].x;
			current_terrain.special_rects[i].top = current_terrain.special_rects[i].bottom = boe_outdoor.special_locs[i].y;
			current_terrain.spec_id[i] = boe_outdoor.special_id[i] + 10;		
			}
	for (i = 0; i < 8; i++) 
		if ((boe_outdoor.exit_locs[i].x > 0) && (boe_outdoor.exit_locs[i].y > 0) && (boe_outdoor.exit_dests[i] >= 0)) {
			current_terrain.exit_rects[i].right = current_terrain.exit_rects[i].left = boe_outdoor.exit_locs[i].x;
			current_terrain.exit_rects[i].top = current_terrain.exit_rects[i].bottom = boe_outdoor.exit_locs[i].y;
			current_terrain.exit_dests[i] = boe_outdoor.exit_dests[i];
			}
	for (i = 0; i < 8; i++)
		if ((boe_outdoor.sign_locs[i].x > 0) && (boe_outdoor.sign_locs[i].y > 0)) {
			current_terrain.sign_locs[i] = boe_outdoor.sign_locs[i];
			strcpy(current_terrain.sign_text[i],boe_scen_text.out_strs[100 + i]);
			}	
	
	short num_in_slot[7] = {15,7,4,3,3,2,1},current_out_slot;
	for (i = 0; i < 4; i++) {
		current_out_slot = 0;
		for (j = 0; j < 7; j++) 
			if ((boe_outdoor.wandering[i].monst[j] > 0) && (current_out_slot < 4)) {
				current_terrain.wandering[i].hostile[current_out_slot] = old_monst_to_new[boe_outdoor.wandering[i].monst[j]];
				current_terrain.wandering[i].hostile_amount[current_out_slot] = num_in_slot[j];
				current_out_slot++;
				}
		for (j = 0; j < 3; j++) 
			if (boe_outdoor.wandering[i].friendly[j] > 0) {
				current_terrain.wandering[i].friendly[j] = old_monst_to_new[boe_outdoor.wandering[i].friendly[j]];
				current_terrain.wandering[i].friendly_amount[j] = 1 + (2 - j) * 2;
				}
		current_terrain.wandering[i].cant_flee = boe_outdoor.wandering[i].cant_flee;

		if (boe_outdoor.wandering[i].spec_on_meet >= 0)
			current_terrain.wandering[i].start_state_when_encountered = boe_outdoor.wandering[i].spec_on_meet + 10;
		if (boe_outdoor.wandering[i].spec_on_win >= 0)
			current_terrain.wandering[i].start_state_when_defeated = boe_outdoor.wandering[i].spec_on_win + 10;
		if (boe_outdoor.wandering[i].spec_on_flee >= 0)
			current_terrain.wandering[i].start_state_when_fled = boe_outdoor.wandering[i].spec_on_flee + 10;
		}
	for (i = 0; i < 4; i++) {
		current_out_slot = 0;
		for (j = 0; j < 7; j++) 
			if ((boe_outdoor.special_enc[i].monst[j] > 0) && (current_out_slot < 4)) {
				current_terrain.special_enc[i].hostile[current_out_slot] = old_monst_to_new[boe_outdoor.special_enc[i].monst[j]];
				current_terrain.special_enc[i].hostile_amount[current_out_slot] = num_in_slot[j];
				current_out_slot++;
				}
		for (j = 0; j < 3; j++) 
			if (boe_outdoor.special_enc[i].friendly[j] > 0) {
				current_terrain.special_enc[i].friendly[j] = old_monst_to_new[boe_outdoor.special_enc[i].friendly[j]];
				current_terrain.special_enc[i].friendly_amount[j] = 1 + (2 - j) * 2;
				}
		current_terrain.special_enc[i].cant_flee = boe_outdoor.special_enc[i].cant_flee;

		if (boe_outdoor.special_enc[i].spec_on_meet >= 0)
			current_terrain.special_enc[i].start_state_when_encountered = boe_outdoor.special_enc[i].spec_on_meet + 10;
		if (boe_outdoor.special_enc[i].spec_on_win >= 0)
			current_terrain.special_enc[i].start_state_when_defeated = boe_outdoor.special_enc[i].spec_on_win + 10;
		if (boe_outdoor.special_enc[i].spec_on_flee >= 0)
			current_terrain.special_enc[i].start_state_when_fled = boe_outdoor.special_enc[i].spec_on_flee + 10;
		}
	for (i = 0; i < 4; i++)
		current_terrain.wandering_locs[i] = boe_outdoor.wandering_locs[i];
	for (i = 0; i < 8; i++) 
		if ((boe_outdoor.info_rect[i].right > 0) && (boe_outdoor.info_rect[i].left > 0)) {
			boe_scen_text.out_strs[i + 1][29] = 0;
			current_terrain.info_rect[i].top = boe_outdoor.info_rect[i].top;
			current_terrain.info_rect[i].left = boe_outdoor.info_rect[i].left;
			current_terrain.info_rect[i].bottom = boe_outdoor.info_rect[i].bottom;
			current_terrain.info_rect[i].right = boe_outdoor.info_rect[i].right;
			
			strcpy(current_terrain.info_rect_text[i],boe_scen_text.out_strs[i + 1]);
			}
	current_terrain.is_on_surface = current_section_on_surface;

	// process roads. first, roads inside of terrain
	for (i = 1; i < 47; i++)
		for (j = 1; j < 47; j++) 
			if ((is_old_road(i,j)) && (boe_outdoor.terrain[i][j] > 70)) {
				if ((is_old_road(i,j-1)) && (is_old_road(i,j+1)))
					current_terrain.terrain[i][j] = 410;
				if ((is_old_road(i-1,j)) && (is_old_road(i-1,j)))
					current_terrain.terrain[i][j] = 411;
				if ((is_old_road(i+1,j)) && (is_old_road(i,j+1)))
					current_terrain.terrain[i][j] = 412;
				if ((is_old_road(i+1,j)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 413;
				if ((is_old_road(i-1,j)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 414;
				if ((is_old_road(i-1,j)) && (is_old_road(i,j+1)))
					current_terrain.terrain[i][j] = 415;
				if ((is_old_road(i-1,j)) && (is_old_road(i,j+1)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 416;
				if ((is_old_road(i+1,j)) && (is_old_road(i-1,j)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 417;
				if ((is_old_road(i+1,j)) && (is_old_road(i,j+1)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 418;
				if ((is_old_road(i+1,j)) && (is_old_road(i-1,j)) && (is_old_road(i,j+1)))
					current_terrain.terrain[i][j] = 419;
				if ((is_old_road(i+1,j)) && (is_old_road(i-1,j)) && (is_old_road(i,j+1)) && (is_old_road(i,j-1)))
					current_terrain.terrain[i][j] = 420;
				}

	// process roads. next, roads on edge of terrain
	for (i = 0; i < 48; i++)
		if ((is_old_road(i,0)) && (boe_outdoor.terrain[i][0] > 70))
			current_terrain.terrain[i][0] = 410;
	for (i = 0; i < 48; i++)
		if ((is_old_road(i,47)) && (boe_outdoor.terrain[i][47] > 70))
			current_terrain.terrain[i][47] = 410;
	for (i = 0; i < 48; i++)
		if ((is_old_road(0,i)) && (boe_outdoor.terrain[0][1] > 70))
			current_terrain.terrain[0][i] = 411;
	for (i = 0; i < 48; i++)
		if ((is_old_road(47,i)) && (boe_outdoor.terrain[47][i] > 70))
			current_terrain.terrain[47][i] = 411;
	

	
}

Boolean is_old_road(short i,short j)
{
	if ((boe_outdoor.terrain[i][j] >= 79) && (boe_outdoor.terrain[i][j] <= 81) )
		return TRUE;
	if ((boe_outdoor.terrain[i][j] >= 65) && (boe_outdoor.terrain[i][j] <= 70) )
		return TRUE;
	if ((boe_outdoor.terrain[i][j] >= 72) && (boe_outdoor.terrain[i][j] <= 73) )
		return TRUE;
	return FALSE;
}

Boolean is_old_wall(short ter)
{
	if ((ter >= 5) && (ter <= 35))
		return TRUE;
	if ((ter >= 122) && (ter <= 169))
		return TRUE;
	return FALSE;
}

void port_boe_town_data(short which_town,Boolean is_mac_scen) 
{
	short i,j,k;
	Boolean current_section_on_surface = TRUE;
	Boolean erase_list[64][64];
	short town_size = max_dim[scenario.town_size[which_town]];
	
	town.clear_town_record_type();
	t_d.clear_big_tr_type();
	
	boe_scen_text.town_strs[0][19] = 0;
	strcpy(town.town_name,boe_scen_text.town_strs[0]);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			town.respawn_monsts[i][j] = old_monst_to_new[boe_town.wandering[i].monst[j]];
	for (i = 0; i < 4; i++)
		town.respawn_locs[i] = boe_town.wandering_locs[i];
	town.respawn_locs[4] = boe_town.wandering_locs[0];
	town.respawn_locs[5] = boe_town.wandering_locs[1];

	for (i = 0; i < 50; i++)
		if ((boe_town.special_locs[i].x > 0) && (boe_town.special_locs[i].y > 0) && (boe_town.spec_id[i] != kNO_TOWN_SPECIALS)) {
			town.special_rects[i].left = town.special_rects[i].right = boe_town.special_locs[i].x;
			town.special_rects[i].top = town.special_rects[i].bottom = boe_town.special_locs[i].y;
			
//			if (is_mac_scen == FALSE)
//				alter_rect(&town.special_rects[i]);
				
			town.spec_id[i] = boe_town.spec_id[i] + 10;
			}

	town.lighting = boe_town.lighting;

	for (i = 0; i < 15; i++)
		if ((boe_town.sign_locs[i].x > 0) && (boe_town.sign_locs[i].y > 0)) {
			town.sign_locs[i] = boe_town.sign_locs[i];
			strcpy(town.sign_text[i],boe_scen_text.town_strs[120 + i]);
			}

	for (i = 0; i < 4; i++)
		town.start_locs[i] = boe_town.start_locs[i];
		town.start_locs[1] = boe_town.start_locs[3];
		town.start_locs[3] = boe_town.start_locs[1];
	for (i = 0; i < 4; i++)
		town.exit_locs[i] = boe_town.exit_locs[i];
	for (i = 0; i < 4; i++)
		if (boe_town.exit_specs[i] >= 0)
			town.exit_specs[i] = boe_town.exit_specs[i] + 10;
			else town.exit_specs[i] = -1;
			
	town.in_town_rect = boe_town.in_town_rect;
	if (is_mac_scen == FALSE)
		alter_rect(&town.in_town_rect);

	
	// process old fields
	for (i = 0; i < 50; i++)
		if (boe_town.preset_fields[i].field_type >= 0) {
			switch (boe_town.preset_fields[i].field_type) {
				case 3: make_web(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y); 
				break;
				case 4: make_crate(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y); 
				break;
				case 5: make_barrel(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y); 
				break;
				case 6: make_fire_barrier(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y); 
				break;
				case 7: make_force_barrier(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y); 
				break;
				case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: 
					make_sfx(boe_town.preset_fields[i].field_loc.x,boe_town.preset_fields[i].field_loc.y,
					  boe_town.preset_fields[i].field_type - 14); 
				break;
				}
			}

	town.town_kill_day = boe_town.town_chop_time;
	town.town_linked_event = boe_town.town_chop_key;
	town.spec_on_entry = boe_town.spec_on_entry;
	town.spec_on_entry_if_dead = boe_town.spec_on_entry_if_dead;


	short old_appear_type_to_new[8] = {0,3,4,7,8,9,5,6};
	
	switch (boe_scenario.town_size[which_town]) {
		case 0:
		break;
		case 1:
			for (i = 0; i < 48; i++)
				for (j = 0; j < 48; j++) {
					boe_big_town.terrain[i][j] = boe_ave_town.terrain[i][j];
					}
			for (i = 0; i < 60; i++)
				boe_big_town.creatures[i].number = 0;					
			for (i = 0; i < 40; i++)
				boe_big_town.creatures[i] = boe_ave_town.creatures[i];					
			for (i = 0; i < 16; i++) {
				boe_big_town.room_rect[i] = boe_ave_town.room_rect[i];
				}	
		break;
		case 2:
			for (i = 0; i < 32; i++)
				for (j = 0; j < 32; j++) {
					boe_big_town.terrain[i][j] = boe_tiny_town.terrain[i][j];
					}
			for (i = 0; i < 60; i++)
				boe_big_town.creatures[i].number = 0;					
			for (i = 0; i < 30; i++)
				boe_big_town.creatures[i] = boe_tiny_town.creatures[i];					
			for (i = 0; i < 16; i++) {
				boe_big_town.room_rect[i] = boe_tiny_town.room_rect[i];
				}	
		break;
		}

	if (is_mac_scen == TRUE)
		boe_port_t_d();
		
	for (i = 0; i < 16; i++)
		if ((boe_big_town.room_rect[i].left > 0) && (boe_big_town.room_rect[i].right > 0)) {
			town.room_rect[i] = boe_big_town.room_rect[i];
			strcpy(town.info_rect_text[i],boe_scen_text.town_strs[i + 1]);
			if (is_mac_scen == FALSE)
				alter_rect(&town.room_rect[i]);
			}

	for (i = 0; i < 60; i++) 
		if (boe_big_town.creatures[i].number > 0) {
			town.creatures[i].number = old_monst_to_new[boe_big_town.creatures[i].number];
			if (boe_big_town.creatures[i].start_attitude == 1)
				town.creatures[i].start_attitude = 4;
				else if (boe_big_town.creatures[i].start_attitude == 3)
					town.creatures[i].start_attitude = 5;
					else town.creatures[i].start_attitude = 2;
			town.creatures[i].start_loc = boe_big_town.creatures[i].start_loc;
			town.creatures[i].personality = 20 * which_town + (boe_big_town.creatures[i].personality % 10);
			town.creatures[i].hidden_class = boe_big_town.creatures[i].spec_enc_code;
			town.creatures[i].character_id = which_town * 100 + i;
			town.creatures[i].time_flag = old_appear_type_to_new[boe_big_town.creatures[i].time_flag];
			town.creatures[i].creature_time = boe_big_town.creatures[i].extra1;
			town.creatures[i].attached_event = boe_big_town.creatures[i].extra2;
			
			// this creature will get the basicnpc script, so set the flags appropriately
			if (boe_big_town.creatures[i].mobile == 0)
				town.creatures[i].memory_cells[0] = (boe_big_town.creatures[i].number == 12) ? 1 : 2;
				
			if ((boe_big_town.creatures[i].spec1 < 0) || (boe_big_town.creatures[i].spec1 >= 300))
				town.creatures[i].memory_cells[1] = 0;
			else town.creatures[i].memory_cells[1] = boe_big_town.creatures[i].spec1;
			if ((boe_big_town.creatures[i].spec2 < 0) || (boe_big_town.creatures[i].spec2 >= 30))
				town.creatures[i].memory_cells[2] = 0;
			else town.creatures[i].memory_cells[2] = boe_big_town.creatures[i].spec2;
			if ((boe_big_town.creatures[i].special_on_kill < 0) || (boe_big_town.creatures[i].special_on_kill >= 256))
				town.creatures[i].memory_cells[4] = 0;
			else town.creatures[i].memory_cells[4] = boe_big_town.creatures[i].special_on_kill + 10;
				
		
			}
			
	for (i = 0; i < town_size; i++)
		for (j = 0; j < town_size; j++) {
			t_d.floor[i][j] = (unsigned char)old_ter_to_floor[boe_big_town.terrain[i][j]];
			t_d.terrain[i][j] = old_ter_to_ter[boe_big_town.terrain[i][j]];

			if (t_d.floor[i][j] < 4)
				current_section_on_surface = FALSE;
				else if ((t_d.floor[i][j] >= 37) && (t_d.floor[i][j] <= 40))
					current_section_on_surface = TRUE;
			}

			
	town.is_on_surface = current_section_on_surface;

	// Process the items
	for (i = 0; i < 64; i++) 
		if (boe_town.preset_items[i].item_code >= 0) {
			if (old_item_to_new_item[boe_town.preset_items[i].item_code] > 0) {
				create_new_item(old_item_to_new_item[boe_town.preset_items[i].item_code],
				  boe_town.preset_items[i].item_loc,boe_town.preset_items[i].property,NULL);
				}
				else if (boe_town.preset_items[i].item_code == 0) {
					if (get_ran(1,0,100) < 8)
						create_new_item(3,boe_town.preset_items[i].item_loc,boe_town.preset_items[i].property,NULL);
						else if (get_ran(1,0,100) < 25)
							create_new_item(2,boe_town.preset_items[i].item_loc,boe_town.preset_items[i].property,NULL);
						else if (get_ran(1,0,100) < 101)
							create_new_item(1,boe_town.preset_items[i].item_loc,boe_town.preset_items[i].property,NULL);
					}
			}

	short wall_type,wall_adj; 
	
	// Now we'll go in and eradicate all terrains surrounded by walls
	for (i = 0; i < town_size; i++)
		for (j = 0; j < town_size; j++)
			erase_list[i][j] = FALSE;
	for (i = 0; i < town_size; i++)
		for (j = 0; j < town_size; j++)
			if (((i == 0) || (is_old_wall(boe_big_town.terrain[i - 1][j]))) &&
				((j == 0) || (is_old_wall(boe_big_town.terrain[i][j - 1]))) &&
				((i == town_size - 1) || (is_old_wall(boe_big_town.terrain[i + 1][j]))) &&
				((j == town_size - 1) || (is_old_wall(boe_big_town.terrain[i][j + 1]))) )
					erase_list[i][j] = TRUE;
	//for (i = 0; i < 64; i++)
	//	for (j = 0; j < 64; j++)
	//		if (erase_list[i][j])
	//			boe_big_town.terrain[i][j] = 0;
	
	// Set up walls
	for (i = 1; i < town_size - 1; i++) {
		for (j = 1; j < town_size - 1; j++) { 
			if ((is_old_wall(boe_big_town.terrain[i][j])) && (boe_big_town.terrain[i][j] != 5)
			  && (boe_big_town.terrain[i][j] > 35)) {
				// pick wall type. 0 east 1 s & e 2 s 3 none
				if (is_old_wall(boe_big_town.terrain[i - 1][j])) { // wall to west
					// if wall to w and n, L wall.
					if (is_old_wall(boe_big_town.terrain[i][j - 1]))
						wall_type = 1;
						else wall_type = 2;
					//if (is_old_wall(boe_big_town.terrain[i][j - 1]))
					//	wall_type = (is_old_wall(boe_big_town.terrain[i + 1][j])) ? 2 : 1;
					//	else wall_type = 2;
					}
					else wall_type = 0;
				if ((is_old_wall(boe_big_town.terrain[i - 1][j]) == FALSE) && 
				  (is_old_wall(boe_big_town.terrain[i][j - 1])) == FALSE)
					wall_type = 3;				
					
				if (wall_type < 3) {
					if (wall_type == 1)
						t_d.terrain[i][j] = 8;
						else {
							wall_adj = (wall_type == 0) ? 3 : 2;
							
							in_town_on_ter_script_type door_script;
							door_script.exists = TRUE;
							door_script.loc.x = (t_coord)i;
							door_script.loc.y = (t_coord)j;
							sprintf(door_script.script_name,"door");
							
							switch (boe_big_town.terrain[i][j]) {
								case 123: case 124: 
								case 140: case 141: case 155: case 156: 
								t_d.terrain[i][j] = 18 + wall_adj; 
								break; // s door
								case 125: case 142: case 157: 
									t_d.terrain[i][j] = 10 + wall_adj; 
									door_script.memory_cells[0] = 0;
									create_new_ter_script("door",door_script.loc,&door_script);
								break; // door
								case 126: case 143: case 158:
									t_d.terrain[i][j] = 10 + wall_adj; 
									door_script.memory_cells[0] = 5;
									create_new_ter_script("door",door_script.loc,&door_script);
								break; // locked door
								case 127: case 144: case 159:
									t_d.terrain[i][j] = 10 + wall_adj; 
									door_script.memory_cells[0] = 15;
									create_new_ter_script("magicdoor",door_script.loc,&door_script);
								break; // m locked door
								case 128: case 145: case 160:
									t_d.terrain[i][j] = 10 + wall_adj; 
									door_script.memory_cells[0] = 200;
									create_new_ter_script("door",door_script.loc,&door_script);
								break; // impass door
								case 129: case 146: case 161:
									t_d.terrain[i][j] = 14 + wall_adj; 
									door_script.memory_cells[0] = 0;
									create_new_ter_script("door",door_script.loc,&door_script);
								break; // open door
								case 138: case 153: case 169:
									t_d.terrain[i][j] = 22 + wall_adj; 
								break; // window
								case 130: case 147: case 162:
									t_d.terrain[i][j] = 26 + wall_adj; 
								break; // closed gate
								case 131: case 148: case 163:
									t_d.terrain[i][j] = 30 + wall_adj; 
								break; // open gate
								case 133: case 150: case 165:
								case 134: case 151: case 166:
								case 135: case 152: case 167:
									t_d.terrain[i][j] = 34 + wall_adj; 
								break; // cracked
								case 132: case 149: case 164:  
									t_d.terrain[i][j] = 2 + wall_adj; 
									// sign
									
									for (k = 0; k < 15; k++)
										if ((town.sign_locs[k].x == i) && (town.sign_locs[k].y == j)) {
											if (wall_adj == 3) {
												town.sign_locs[k].x++;
												t_d.terrain[i + 1][j] = 195;
												}
												else {
													town.sign_locs[k].y++;
													t_d.terrain[i][j + 1] = 194;
													}
											}
									break;
								default: 
								 t_d.terrain[i][j] = 2 + wall_adj; 
								 break;
								}
							}
					}
				if (wall_type == 0)
					t_d.floor[i][j] = t_d.floor[i - 1][j];
					else t_d.floor[i][j] = t_d.floor[i][j - 1];
					
				}
			}
		}
	for (i = 0; i < town_size; i++)
		for (j = 0; j < town_size; j++)
			if (erase_list[i][j]) {
				Boolean dont_erase_after_all = FALSE;
				
				// if not a needed corner of a room, erase
				if ((i > 0) && (j > 0) && (t_d.terrain[i][j] == 8) &&
					(erase_list[i - 1][j] == FALSE) && (erase_list[i][j - 1] == FALSE)) 
						dont_erase_after_all = TRUE;
				if ((i < 63) && (erase_list[i + 1][j] == FALSE))
					dont_erase_after_all = TRUE;
				if ((j < 63) && (erase_list[i][j + 1] == FALSE))
					dont_erase_after_all = TRUE;
				
				if (dont_erase_after_all == FALSE) {
					t_d.terrain[i][j] = 0;
					t_d.floor[i][j] = 255;
					}
				}
	for (i = 1; i < town_size - 1; i++)
		for (j = 1; j < town_size - 1; j++) 
			if ((t_d.terrain[i][j] == 8) && (t_d.terrain[i + 1][j] == 8))
				t_d.terrain[i][j] = 4;
	for (i = 1; i < town_size - 1; i++)
		for (j = 1; j < town_size - 1; j++) 
			if ((t_d.terrain[i][j] == 8) && (t_d.terrain[i][j + 1] == 8))
				t_d.terrain[i][j] = 5;

	// place walls around edges
	j = town_size - 1;
	for (i = 1; i < town_size - 1; i++)
		if ((is_old_wall(boe_big_town.terrain[i][0])) && ((erase_list[i][j] == FALSE) || (erase_list[i][j + 1] == FALSE))) {
			t_d.terrain[i][0] = 4;
			t_d.floor[i][0] = 255;
			}
			
	for (i = 1; i < town_size - 1; i++)
		if ((is_old_wall(boe_big_town.terrain[i][town_size - 1])) && ((erase_list[i][j] == FALSE) || (erase_list[i][j - 1] == FALSE))) {
			t_d.terrain[i][town_size - 1] = 2;
			t_d.floor[i][town_size - 1] = 255;
			}
			
	for (i = 1; i < town_size - 1; i++)
		if ((is_old_wall(boe_big_town.terrain[0][i])) && ((erase_list[i][j] == FALSE) || (erase_list[i + 1][j] == FALSE)))  {
			t_d.terrain[0][i] = 5;
			t_d.floor[0][i] = 255;
			}
			
	for (i = 1; i < town_size - 1; i++)
		if ((is_old_wall(boe_big_town.terrain[town_size - 1][i])) && ((erase_list[i][j] == FALSE) || (erase_list[i - 1][j] == FALSE))) {
			t_d.terrain[town_size - 1][i] = 3;
			t_d.floor[town_size - 1][i] = 255;
			}
			

	
}

// BEGIN ROUTINES TO ADAPT BLADES OF EXILE SCENARIOS MADE IN WINDOWS
void boe_port_talk_nodes()
{
	short i;
	
	for (i = 0; i < 60; i++) {
		flip_short(&boe_talk_data.talk_nodes[i].personality);
		flip_short(&boe_talk_data.talk_nodes[i].type);
		flip_short(&boe_talk_data.talk_nodes[i].extras[0]);
		flip_short(&boe_talk_data.talk_nodes[i].extras[1]);
		flip_short(&boe_talk_data.talk_nodes[i].extras[2]);
		flip_short(&boe_talk_data.talk_nodes[i].extras[3]);
		}
}

void boe_port_town()
{
	short i;

	flip_short(&boe_town.town_chop_time);
	flip_short(&boe_town.town_chop_key);
	flip_short(&boe_town.lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&boe_town.exit_specs[i]);
	flip_rect(&boe_town.in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&boe_town.preset_items[i].item_code);
		flip_short(&boe_town.preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&boe_town.preset_fields[i].field_type);
		}
	flip_short(&boe_town.max_num_monst);
	flip_short(&boe_town.spec_on_entry);
	flip_short(&boe_town.spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++) 
		flip_short(&boe_town.timer_spec_times[i]);
	for (i =0 ; i < 8; i++) 
		flip_short(&boe_town.timer_specs[i]);
	flip_short(&boe_town.difficulty);
	for (i =0 ; i < 100; i++) 
		boe_flip_spec_node(&boe_town.specials[i]);
	
}

void boe_port_t_d()
{
	short i;

	for (i =0 ; i < 16; i++) 
		flip_rect(&boe_big_town.room_rect[i]);
	for (i =0 ; i < 60; i++) {
		flip_short(&boe_big_town.creatures[i].spec1);
		flip_short(&boe_big_town.creatures[i].spec2);
		flip_short(&boe_big_town.creatures[i].monster_time);
		flip_short(&boe_big_town.creatures[i].personality);
		flip_short(&boe_big_town.creatures[i].special_on_kill);
		flip_short(&boe_big_town.creatures[i].facial_pic);
	
		} 
}

void boe_port_scenario()
{
	short i,j;
	
	flip_short(&boe_scenario.flag_a);
	flip_short(&boe_scenario.flag_b);
	flip_short(&boe_scenario.flag_c);
	flip_short(&boe_scenario.flag_d);
	flip_short(&boe_scenario.flag_e);
	flip_short(&boe_scenario.flag_f);
	flip_short(&boe_scenario.flag_g);
	flip_short(&boe_scenario.flag_h);
	flip_short(&boe_scenario.flag_i);
	flip_short(&boe_scenario.intro_mess_pic);
	flip_short(&boe_scenario.intro_mess_len);
	flip_short(&boe_scenario.which_town_start);
	for (i = 0; i < 200; i++)
		for (j = 0; j < 5; j++)
			flip_short(&boe_scenario.town_data_size[i][j]);
	for (i = 0; i < 10; i++)
		flip_short(&boe_scenario.town_to_add_to[i]);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 2; j++)
			flip_short(&boe_scenario.flag_to_add_to_town[i][j]);
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			flip_short(&boe_scenario.out_data_size[i][j]);
	for (i = 0; i < 3; i++)
		flip_rect(&boe_scenario.store_item_rects[i]);
	for (i = 0; i < 3; i++)
		flip_short(&boe_scenario.store_item_towns[i]);
	for (i = 0; i < 50; i++)
		flip_short(&boe_scenario.special_items[i]);
	for (i = 0; i < 50; i++)
		flip_short(&boe_scenario.special_item_special[i]);
	flip_short(&boe_scenario.rating);
	flip_short(&boe_scenario.uses_custom_graphics);
	for (i = 0; i < 256; i++) {
		flip_short(&boe_scenario.scen_monsters[i].health);
		flip_short(&boe_scenario.scen_monsters[i].m_health);
		flip_short(&boe_scenario.scen_monsters[i].max_mp);
		flip_short(&boe_scenario.scen_monsters[i].mp);
		flip_short(&boe_scenario.scen_monsters[i].a[1]);
		flip_short(&boe_scenario.scen_monsters[i].a[0]);
		flip_short(&boe_scenario.scen_monsters[i].a[2]);
		flip_short(&boe_scenario.scen_monsters[i].morale);
		flip_short(&boe_scenario.scen_monsters[i].m_morale);
		flip_short(&boe_scenario.scen_monsters[i].corpse_item);
		flip_short(&boe_scenario.scen_monsters[i].corpse_item_chance);
		flip_short(&boe_scenario.scen_monsters[i].picture_num);
		}

	for (i = 0; i < 256; i++) {
		flip_short(&boe_scenario.ter_types[i].picture);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&boe_scenario.scen_boats[i].which_town);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&boe_scenario.scen_horses[i].which_town);
		}
	for (i = 0; i < 20; i++) 
		flip_short(&boe_scenario.scenario_timer_times[i]);
	for (i = 0; i < 20; i++) 
		flip_short(&boe_scenario.scenario_timer_specs[i]);
	for (i = 0; i < 256; i++) {
		boe_flip_spec_node(&boe_scenario.scen_specials[i]);
		}
	for (i = 0; i < 10; i++)  {
		flip_short(&boe_scenario.storage_shortcuts[i].ter_type);
		flip_short(&boe_scenario.storage_shortcuts[i].property);
		for (j = 0; j < 10; j++)  {
			flip_short(&boe_scenario.storage_shortcuts[i].item_num[j]);
			flip_short(&boe_scenario.storage_shortcuts[i].item_odds[j]);
			}
		}
	flip_short(&boe_scenario.last_town_edited);
}


void boe_port_item_list()
{
	short i;
	
	for (i = 0; i < 400; i++) {
		flip_short(&boe_scen_data.scen_items[i].variety);
		flip_short(&boe_scen_data.scen_items[i].item_level);
		flip_short(&boe_scen_data.scen_items[i].value);
		}
}

void boe_port_out(old_blades_outdoor_record_type *out)
{
	short i;
	
		
	for (i = 0; i < 4; i++) {
		flip_short(&(out->wandering[i].spec_on_meet));
		flip_short(&(out->wandering[i].spec_on_win));
		flip_short(&(out->wandering[i].spec_on_flee));
		flip_short(&(out->wandering[i].cant_flee));
		flip_short(&(out->wandering[i].end_spec1));
		flip_short(&(out->wandering[i].end_spec2));
		flip_short(&(out->special_enc[i].spec_on_meet));
		flip_short(&(out->special_enc[i].spec_on_win));
		flip_short(&(out->special_enc[i].spec_on_flee));
		flip_short(&(out->special_enc[i].cant_flee));
		flip_short(&(out->special_enc[i].end_spec1));
		flip_short(&(out->special_enc[i].end_spec2));	
		}
	for (i = 0; i < 8; i++) 
		flip_rect(&(out->info_rect[i]));
	for (i = 0; i < 60; i++) 
		boe_flip_spec_node(&(out->specials[i]));
}

void boe_flip_spec_node(old_blades_special_node_type *spec)
{
	flip_short(&(spec->type));
	flip_short(&(spec->sd1));
	flip_short(&(spec->sd2));
	flip_short(&(spec->pic));
	flip_short(&(spec->m1));
	flip_short(&(spec->m2));
	flip_short(&(spec->ex1a));
	flip_short(&(spec->ex1b));
	flip_short(&(spec->ex2a));
	flip_short(&(spec->ex2b));
	flip_short(&(spec->jumpto));
}

// SPECIAL NODE/DIALOGUE PORTING FCNS
void port_scenario_script(char *script_name,char *directory_id)
{
	FILE *file_id;
	char new_line[500];
	char file_name[_MAX_PATH];

	sprintf(file_name,"%s%s",directory_id,script_name);
	if (NULL == (file_id = fopen(file_name, "wb"))) {
		oops_error(11);
		return;
		}	

	add_string(file_id,"// SCENARIO SCRIPT");
	add_cr(file_id);
	add_string(file_id,"// This is the special script for your scenario, where you will");
	add_string(file_id,"// write special encounters that can happen anywhere in the scenario.");
	add_string(file_id,"// The states LOAD_SCEN_STATE, START_SCEN_STATE, and START_STATE have");
	add_string(file_id,"// meanings that are described in the documenation. States you write");
	add_string(file_id,"// yourself should be numbered from 10-100.");
	add_cr(file_id);
	add_string(file_id,"beginscenarioscript;");
	add_cr(file_id);
	add_string(file_id,"variables;");
	add_cr(file_id);
	add_string(file_id,"short i,j,k,r1,choice;");
	add_cr(file_id);
	add_string(file_id,"body;");
	add_cr(file_id);
	add_string(file_id,"beginstate LOAD_SCEN_STATE;");
	add_string(file_id,"// This state called whenever this scenario is loaded, including when");
	add_string(file_id,"// a save file is loaded.");

		for (short i = 0; i < 50; i++)
		if (same_string( boe_scen_text.scen_strs[60 + i * 2],"Unused Special Item") == FALSE) {
			sprintf(new_line,"\tinit_special_item(%d,\"%s\",\"%s\");",i,boe_scen_text.scen_strs[60 + i * 2],boe_scen_text.scen_strs[61 + i * 2]);
			add_string(file_id,new_line);
			if (boe_scenario.special_items[i] % 10 == 1) {
				add_string(file_id,"// WARNING: Usable special items no longer supported. Make this special item a regular item with a custom effect.");
				}
			}
	
	for (short i = 0; i < 50; i++)
		if (same_string( boe_scen_text.scen_strs[60 + i * 2],"Unused Special Item") == FALSE) {
			if (boe_scenario.special_item_special[i] >= 0) {
				sprintf(new_line,"// Special Item %d invokes scenario state %d when used.", i, 
				boe_scenario.special_item_special[i] + 10);
				add_string(file_id,new_line);				
				}
			}

	add_string(file_id,"break;");
	add_cr(file_id);
	add_string(file_id,"beginstate START_SCEN_STATE;");
	add_string(file_id,"// This state called whenever this scenario is started, and only");
	add_string(file_id,"// when it's started.");
	
	for (short i = 0; i < 30; i++) 
		if (boe_scenario.scen_boats[i].which_town >= 0) {
			sprintf(new_line,"\tcreate_boat(%d,%d,%d,%d,%d);", i,
				boe_scenario.scen_boats[i].which_town, boe_scenario.scen_boats[i].boat_loc.x,
				boe_scenario.scen_boats[i].boat_loc.y, boe_scenario.scen_boats[i].property);
			add_string(file_id,new_line);
			}
	for (short i = 0; i < 30; i++) 
		if (boe_scenario.scen_horses[i].which_town >= 0) {
			sprintf(new_line,"\tcreate_horse(%d,%d,%d,%d,%d);", i,
				boe_scenario.scen_horses[i].which_town, boe_scenario.scen_horses[i].horse_loc.x,
				boe_scenario.scen_horses[i].horse_loc.y, boe_scenario.scen_horses[i].property);
			add_string(file_id,new_line);
			}

	for (short i = 0; i < 50; i++)
		if (same_string( boe_scen_text.scen_strs[60 + i * 2],"Unused Special Item") == FALSE) {
			if (boe_scenario.special_items[i] >= 10) {
				sprintf(new_line,"\tchange_spec_item(%d,1);", i);
				add_string(file_id,new_line);
				}
			}

	add_string(file_id,"break;");
	add_cr(file_id);
	add_string(file_id,"beginstate START_STATE;");
	add_string(file_id,"// This state will be called every turn the player is in the scenario,");
	add_string(file_id,"// whether in town, combat, or outdoors.");
	for (short i = 0; i < 20; i++) 
		if (boe_scenario.scenario_timer_times[i] > 0) {
			sprintf(new_line,"\t// Scenario timer %d: %d moves till scenario state %d", i,
			boe_scenario.scenario_timer_times[i], boe_scenario.scenario_timer_specs[i] + 10);
			add_string(file_id,new_line);
			}
	add_string(file_id,"break;");
	add_cr(file_id);

	for (short i = 0; i < 256; i++)
			port_a_special_node(&boe_scenario.scen_specials[i],i,file_id,0);
	
	FSClose(file_id);
}

void port_town_script(char *script_name,char *directory_id,short which_town)
{
	FILE *file_id;
	char file_name[_MAX_PATH];
	char str[512];
	char new_line[512];	
	short which_slot;
	sprintf(file_name,"%s%s",directory_id,script_name);
	if (NULL == (file_id = fopen(file_name, "wb"))) {
		oops_error(11);
		return;
		}
		
	add_string(file_id,"// TOWN SCRIPT");
	sprintf(str,"//    Town %d: %s", which_town,town.town_name);
	add_string(file_id,str);
	add_cr(file_id);
	add_string(file_id,"// This is the special encounter script for this town.");
	add_string(file_id,"// The states INIT_STATE, EXIT_STATE, and START_STATE have");
	add_string(file_id,"// meanings that are described in the documenation. States you write");
	add_string(file_id,"// yourself should be numbered from 10-100.");
	add_cr(file_id);
	add_string(file_id,"begintownscript;");
	add_cr(file_id);
	add_string(file_id,"variables;");
	add_cr(file_id);
	add_string(file_id,"short i,j,k,r1,choice;");
	add_cr(file_id);
	add_string(file_id,"body;");
	add_cr(file_id);
	add_string(file_id,"beginstate INIT_STATE;");
	add_string(file_id,"// This state called whenever this town is entered.");
	add_string(file_id,"\t// BoA numbers, BoE personality numbers");
	for (short i = 0; i < 60; i++) {
		if ((boe_big_town.creatures[i].personality >= 0) && (boe_big_town.creatures[i].number > 0)) {		
			sprintf(str,"\t// set_name(%d,\"%d\");", i + 6, boe_big_town.creatures[i].personality);
			add_string(file_id,str);
		}  
		} 
	add_string(file_id,"\t// BoE personality numbers and names.");		
	for (short i = 0; i < 10; i++) {
		if (same_string(boe_scen_text.talk_strs[i],"Unused") == FALSE) {
			sprintf(new_line,"\t// set_name(%d,\"%s\");", (10 * which_town + i), boe_scen_text.talk_strs[which_slot = i]);
			add_string(file_id,new_line);
			}
			}
	add_string(file_id,"\t// Any town entry states.");			
	if (boe_town.spec_on_entry >= 0) {
		add_string(file_id,"\tif (town_status(ME) < 3)");
		add_short_string_to_file(file_id,"\t\tset_state_continue(",10 + boe_town.spec_on_entry,");");
		}
	if (boe_town.spec_on_entry_if_dead >= 0) {
		add_string(file_id,"\tif (town_status(ME) == 3)");
		add_short_string_to_file(file_id,"\t\tset_state_continue(",10 + boe_town.spec_on_entry_if_dead,");");
		}

	add_string(file_id,"break;");
	add_cr(file_id);
	add_string(file_id,"beginstate EXIT_STATE;");
	add_string(file_id,"// Always called when the town is left.");
	add_string(file_id,"break;");
	add_cr(file_id);
	add_string(file_id,"beginstate START_STATE;");
	add_string(file_id,"// This state is called every turn the party is in this town.");
	for (short i = 0; i < 8; i++) 
		if (boe_town.timer_spec_times[i] > 0) {
			sprintf(new_line,"\t// Town timer %d: %d moves till town state %d", i,
			boe_town.timer_spec_times[i], boe_town.timer_specs[i] + 10);
			add_string(file_id,new_line);
			}
	add_string(file_id,"break;");
	add_cr(file_id);
	
	for (short i = 0; i < 100; i++)
		if ((boe_town.specials[i].type > 0) || (boe_town.specials[i].jumpto > 0))
			port_a_special_node(&boe_town.specials[i],i,file_id,1);
	
	FSClose(file_id);
}

void port_town_dialogue_script(char *script_name,char *directory_id,short which_town)
{
	FILE *file_id;
	char file_name[_MAX_PATH];
	char str[500];
		
	// first, we have dialogue at all?
	Boolean have_dialogue = FALSE;
	for (short i = 0; i < 10; i++)
		if (same_string(boe_scen_text.talk_strs[i],"Unused") == FALSE) 
			have_dialogue = TRUE;
	if (have_dialogue == FALSE)
		return;


	sprintf(file_name,"%s%s",directory_id,script_name);
	if (NULL == (file_id = fopen(file_name, "wb"))) {
		oops_error(11);
		return;
		}

	add_string(file_id,"// TOWN DIALOGUE SCRIPT");
	sprintf(str,"//    Town %d: %s", which_town,town.town_name);
	add_string(file_id,str);
	add_cr(file_id);
	add_string(file_id,"// This is the dialogue for this town.");
	add_string(file_id,"// You can use states numbered from 1 to 199.");
	add_cr(file_id);
	add_string(file_id,"begintalkscript;");
	add_cr(file_id);
	add_string(file_id,"variables;");
	add_cr(file_id);
	add_string(file_id,"short i,j,k,r1,choice;");
	add_cr(file_id);

	short current_dialogue_node = 1;
	
	for (short i = 0; i < 10; i++)
		if (same_string(boe_scen_text.talk_strs[i],"Unused") == FALSE) {
			port_dialogue_intro_text(&current_dialogue_node,i,file_id,which_town);
			
			for (short j = 0; j < 60 ; j++) {
				if (boe_talk_data.talk_nodes[j].personality % 10 == i)
					port_dialogue_node(&current_dialogue_node,i,file_id,j,which_town);
				}
			
			current_dialogue_node++;
			current_dialogue_node++;
			}
	for (short j = 0; j < 60 ; j++) {
		if (boe_talk_data.talk_nodes[j].personality == -2)
			port_dialogue_node(&current_dialogue_node,0,file_id,j,which_town);
		}

	FSClose(file_id);
}


void port_outdoor_script(char *script_name,char *directory_id,short sector_x,short sector_y)
{
	FILE *file_id;
	char file_name[_MAX_PATH];
	char str[500];

	sprintf(file_name,"%s%s",directory_id,script_name);
	if (NULL == (file_id = fopen(file_name, "wb"))) {
		oops_error(11);
		return;
		}

	add_string(file_id,"// OUTDOOR SECTION SCRIPT");
	sprintf(str,"//    Section: X = %d, Y = %d", sector_x, sector_y);
	add_string(file_id,str);
	add_cr(file_id);
	add_string(file_id,"// This is the special encounter script for this town.");
	add_string(file_id,"// The states INIT_STATE and START_STATE have");
	add_string(file_id,"// meanings that are described in the documenation. States you write");
	add_string(file_id,"// yourself should be numbered from 10-100.");
	add_cr(file_id);
	add_string(file_id,"beginoutdoorscript;");
	add_cr(file_id);
	add_string(file_id,"variables;");
	add_cr(file_id);
	add_string(file_id,"short i,j,k,r1,choice;");
	add_cr(file_id);
	add_string(file_id,"body;");
	add_cr(file_id);
	add_string(file_id,"beginstate INIT_STATE;");
	add_string(file_id,"// This state called whenever this section is loaded into memory.");
	add_string(file_id,"break;");
	add_cr(file_id);
	add_string(file_id,"beginstate START_STATE;");
	add_string(file_id,"// This state is called every turn the party is in this outdoor section. ");
	add_string(file_id,"break;");
	add_cr(file_id);
		
//			if ((sector_x == 2) & (sector_y == 1))
//				short dbug = 1;
	for (short i = 0; i < 60; i++)
		if ((boe_outdoor.specials[i].type > 0) || (boe_outdoor.specials[i].jumpto > 0)) {
			port_a_special_node(&boe_outdoor.specials[i],i,file_id,2);
			}
	
	FSClose(file_id);
}

//node_type: 0 - scenario, 1 - town, 2 - outdoor
void port_a_special_node(old_blades_special_node_type *node,short node_num,FILE *file_id,short node_type)
{
	char temp_str1[400];
	char new_line[500];	
	add_short_string_to_file(file_id,"beginstate ",node_num + 10,";");


	// handle messages for one shots
	if ((node->type >= 50) && (node->type <= 62) &&
	  (node->sd1 >= 0) && (node->sd2 >= 0)) {
		add_ish_string_to_file(file_id,"\tif (get_flag(",node->sd1,",",node->sd2,") == 250)");
			add_string(file_id,"\t\tend();");
		}

		if ((node->type >= 0) && (node->type != 5) && ((node->type < 55)  || (node->type > 60)))
					handle_messages(file_id,node_type,node->m1,node->m2);		
					
	switch (node->type) {
		case 0:
			break;
		case 1: // set_flag
			add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",node->ex1a,");");
			break;
		case 2: // inc_flag
			if (node->ex1b == 0)
				add_big_string_to_file(file_id,"\tinc_flag(",node->sd1,",",node->sd2,",",node->ex1a,");");
				else add_big_string_to_file(file_id,"\tinc_flag(",node->sd1,",",node->sd2,",",-1 * node->ex1a,");");
			break;
		case 3: // display mess

			break;
		case 4: // secret door
			add_string(file_id,"// OBSOLETE NODE: Secret door special encounters no longer supported. Manually");
			add_string(file_id,"// create a terrain type which can be walked though.");
			break;
		case 5: // display small mess
			if (node->m1 >= 0) {
				get_bl_str(temp_str1,node_type,node->m1);
				sprintf(new_line,"\tprint_str(\"%s\");", temp_str1);
				add_string(file_id,new_line);
				}
			if (node->m2 >= 0) {
				get_bl_str(temp_str1,node_type,node->m2);
				sprintf(new_line,"\tprint_str(\"%s\");", temp_str1);
				add_string(file_id,new_line);
				}
			break;
		case 6: // flip flag
			add_ish_string_to_file(file_id,"\tif (get_flag(",node->sd1,",",node->sd2,") == 0)");
			add_ish_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,",1);");
			add_ish_string_to_file(file_id,"\t\telse set_flag(",node->sd1,",",node->sd2,",0);");
			break;
		case 7: // out block
			if (node->ex1a > 0) {
				add_string(file_id,"\tif (is_outdoor()) {");
				add_string(file_id,"\t\tblock_entry(1);");
				add_string(file_id,"\t\t end();");				
				add_string(file_id,"\t\t };");
               }
			else {
                add_string(file_id,"\tif (is_outdoor())");
				add_string(file_id,"\t\t end();");
                }
			break;
		case 8: // town block
			if (node->ex1a > 0) {
				add_string(file_id,"\tif (is_town()) {");
				add_string(file_id,"\t\tblock_entry(1);");
				add_string(file_id,"\t\t end();");				
				add_string(file_id,"\t\t };");								
                }
			else {
                add_string(file_id,"\tif (is_town())");
				add_string(file_id,"\t\t end();");				
                }
			break;
		case 9: // combat block;
			if (node->ex1a > 0) {
				add_string(file_id,"\tif (is_combat()) {");
				add_string(file_id,"\t\tblock_entry(1);");
				add_string(file_id,"\t\t end();");				
				add_string(file_id,"\t\t };");								
                }	
			else {
                add_string(file_id,"\tif (is_combat())");
				add_string(file_id,"\t\t end();");				
                }
			break;
		case 10: // looking block
			add_string(file_id,"// OBSOLETE NODE: Special encounters caused by looking at/searching things");
			add_string(file_id,"// are now handled in terrain scripts. So if there was a trap activated by looking");
			add_string(file_id,"// at a box, that is now handled in a script written for a box terrain type.");
			break;
		case 11: // can't enter
			if (node->ex1a != 1)
				add_string(file_id,"\tblock_entry(0);");
				else 		add_string(file_id,"\tblock_entry(1);");
			break;
		case 12: // change time
			add_short_string_to_file(file_id,"\tset_ticks_forward(",node->ex1a,");");
			break;
		case 13:
			add_string(file_id,"// OBSOLETE NODE: Timers are no longer used. Manually write scenario timers in the ");
			add_string(file_id,"// scenario script.");
			add_short_string_to_file(file_id,"// Number of moves = (",node->ex1a,");");			
			add_short_string_to_file(file_id,"// Scenario state called: (",node->ex1b + 10,");");						
			break;
			
		case 14: // play sound
			add_string(file_id,"// WARNING: Many sounds have changed from Blades of Exile.");
			add_short_string_to_file(file_id,"\tplay_sound(",node->ex1a,");");
			break;
		case 15: // horse possession
			add_ish_string_to_file(file_id,"\tset_horse_property(",node->ex1a,",",1 - node->ex2a,");");		
			break;
		case 16: // boat possession
			add_ish_string_to_file(file_id,"\tset_boat_property(",node->ex1a,",",1 - node->ex2a,");");		
			break;
		case 17: // show hide town
			add_ish_string_to_file(file_id,"\tset_town_visibility(",node->ex1a,",",node->ex1b,");");		
			break;
		case 18: // major event occured
			add_short_string_to_file(file_id,"\tday_event_happened(",node->ex1a,") = what_day_of_scenario();");
			break;
			
		case 19: // forced give
			if (node->ex2b < 0) {
				add_short_string_to_file(file_id,"\treward_give(",node->ex1a,");")	;	
				}
				else {
					add_short_string_to_file(file_id,"\tif (reward_give(",node->ex1a,") == FALSE)");			
					add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");
					}
			add_string(file_id,"// Potential translation problems");
					
			break;
		case 20: // buy items of type
			add_ish_string_to_file(file_id,"\tif (take_all_of_item_class(",node->ex1a,",",node->ex2a,") > 0) ");		
			if (node->ex1b >= 0) 
				add_short_string_to_file(file_id,"\telse set_state_continue(",node->ex1b + 10,");");
			add_string(file_id,"// Potential translation problems");				
			break;
		case 21: // global special
			add_short_string_to_file(file_id,"\trun_scenario_script(",node->jumpto + 10,");");
			break;
		case 22: // set many flags
			for (short i = 0; i < 10; i++)
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",i,",",node->ex1a,");");
			break;
		case 23: // copy flags
			sprintf(temp_str1,"\tset_flag(%d,%d,get_flag(%d,%d));", node->sd1, node->sd2, node->ex1a, node->ex1b);
			add_string(file_id,temp_str1);
			break;
		case 24: // ritual of sanct
			add_string(file_id,"// OBSOLETE NODE: Special encounters caused by Ritual of Sanctification");
			add_string(file_id,"// are now handled in terrain scripts. When a ritual is used, nearby terrain ");
			add_string(file_id,"// scripts have the state SANCTIFICATION_STATE called.");
			add_short_string_to_file(file_id,"// Sanctification calls state: (",node->jumpto + 10,");");			
			add_short_string_to_file(file_id,"// Otherwise: (",node->ex1b + 10,");");						
			break;
		case 25: // have rest
			add_string(file_id,"\trevive_party();");
			add_short_string_to_file(file_id,"\tset_ticks_forward(",node->ex1a,");");
			break;
		case 26: // wandering will fight
			if (node->ex1a == 0)
				add_string(file_id,"\toutdoor_enc_result(1);");
				else add_string(file_id,"\toutdoor_enc_result(0);");			
			break;
		case 27: // end scen
			add_string(file_id,"\tend_scenario(1);");
			break;
		case 50: // give item
			add_short_string_to_file(file_id,"\tif (reward_give(",node->ex1a,") > 0) {");	
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_ish_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,");");
			if (node->ex1b > 0) 
				add_short_string_to_file(file_id,"\t\tchange_coins(",node->ex1b,");");
			add_string(file_id,"\t\t}");
			if (node->ex2b >= 0) {
				add_short_string_to_file(file_id,"\t\telse set_state_continue(",node->ex2b + 10,");");
								}
			add_string(file_id,"// Potential translation problems");								
			break;
		case 51: // give spec item
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			add_ish_string_to_file(file_id,"\tchange_spec_item(",node->ex1a,",",(node->ex1b == 0) ? 1 : -1,");");			
			break;
		case 52: // one time do nothing
			break;
		case 53: // one time do nothing and set
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			break;
		case 54: // one time do nothing and set
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			break;
		case 55: case 56: case 57: // display dialog
			add_string(file_id,"\treset_dialog();");
			for (short i = 0; i < 6; i++) {
				get_bl_str(temp_str1,node_type,node->m1 + i);
				if (strlen(temp_str1) > 0) {
				sprintf(new_line,"\tadd_dialog_str(%d,\"%s\",0);", i, temp_str1);
				add_string(file_id,new_line);
				}
				}
			if (node->m2 > 0) {
				add_string(file_id,"\tadd_dialog_choice(0,OK.);");
				if (node->ex1a >= 0) {
				sprintf(new_line,"\tadd_dialog_choice(1,\"%s\".);", old_blades_button_strs[old_blades_available_dlog_buttons[node->ex1a]]);
				add_string(file_id,new_line);				
				}
				if (node->ex2a >= 0) {
				sprintf(new_line,"\tadd_dialog_choice(2,\"%s\".);", old_blades_button_strs[old_blades_available_dlog_buttons[node->ex2a]]);
				add_string(file_id,new_line);
				}	
				}
				else {
				if (node->ex1a >= 0) {
					sprintf(new_line,"\tadd_dialog_choice(0,\"%s\".);", old_blades_button_strs[old_blades_available_dlog_buttons[node->ex1a]]);
						add_string(file_id,new_line);
				}
				if (node->ex2a >= 0) {
					sprintf(new_line,"\tadd_dialog_choice(1,\"%s\".);", old_blades_button_strs[old_blades_available_dlog_buttons[node->ex2a]]);
						add_string(file_id,new_line);
					}
					}
			add_string(file_id,"\tchoice = run_dialog(1);");
			if (node->m2 > 0) {
				if ((node->sd1 >= 0) && (node->sd2 >= 0)) { 
					if ((node->ex1a < 0) && (node->ex2a < 0))
						add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
						else {
							add_string(file_id,"\tif (choice != 1)");
							add_big_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,",",250,");");
							}
					}
				if (node->ex1a >= 0) {
					add_string(file_id,"\tif (choice == 2)");
					if (node->ex1b >= 0)
						add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");
						else add_string(file_id,"\t\tend();");
					}
				if (node->ex2a >= 0) {
					add_string(file_id,"\tif (choice == 3)");
					if (node->ex2b >= 0)
						add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");
						else add_string(file_id,"\t\tend();");
					}
				}
				else {
					if ((node->sd1 >= 0) && (node->sd2 >= 0)) { 
						//add_string(file_id,"\tif (choice != 1)");
						//add_big_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,",",250,");");
						if (node->ex2a < 0)
							add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
							else {
								add_string(file_id,"\tif (choice != 1)");
								add_big_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,",",250,");");
								}
						}
					if (node->ex1a >= 0) {
						add_string(file_id,"\tif (choice == 1)");
						if (node->ex1b >= 0)
							add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");
							else add_string(file_id,"\t\tend();");
						}
					if (node->ex2a >= 0) {
						add_string(file_id,"\tif (choice == 2)");
						if (node->ex2b >= 0)
							add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");
							else add_string(file_id,"\t\tend();");
						}
					}
			
			break;	
		case 58: case 59: case 60: // display item dialog
			add_string(file_id,"\treset_dialog_preset_options(1);");
			for (short i = 0; i < 6; i++) {
				get_bl_str(temp_str1,node_type,node->m1 + i);
				if (strlen(temp_str1) > 0) {
					sprintf(new_line,"\tadd_dialog_str(%d,\"%s\",0);", i, temp_str1);
				add_string(file_id,new_line);
				}
				}
			add_string(file_id,"\tchoice = run_dialog(1);");
			add_string(file_id,"\tif (choice == 2) {");
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\t\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			if (node->ex1a > 0) {
				add_short_string_to_file(file_id,"\t\tif (reward_give(",node->ex1a,")) {");
				if (node->ex1b > 0) 
					add_short_string_to_file(file_id,"\t\t\tchange_coins(",node->ex1b,");");
				if (node->ex2a != 0) 
					add_string(file_id,"\t\t\t// OBSOLETE NODE: Food works completely differently now.");
				if (node->m2 >= 0) 
					add_short_string_to_file(file_id,"\t\tchange_spec_item(",node->m2,",1);");
				
				add_string(file_id,"\t\t\t}");
				}
				else { 
					if (node->ex1b > 0) 
						add_short_string_to_file(file_id,"\t\tchange_coins(",node->ex1b,");");
					if (node->m2 >= 0) 
						add_short_string_to_file(file_id,"\t\tchange_spec_item(",node->m2,",1);");
					}			
			if (node->ex2b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");
			add_string(file_id,"\t\t}");
			add_string(file_id,"// Potential translation problems");			
			break;
			
			
		case 61: // place outdoor enc
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			add_short_string_to_file(file_id,"\tcreate_out_spec_enc(",node->ex1a,");");
			break;
		case 62: // place town enc
			if ((node->sd1 >= 0) && (node->sd2 >= 0)) 
				add_big_string_to_file(file_id,"\tset_flag(",node->sd1,",",node->sd2,",",250,");");
			add_short_string_to_file(file_id,"\tactivate_hidden_group(",node->ex1a,");");
			break;
		case 63: // trap
			add_string(file_id,"// OBSOLETE NODE: Traps are now handled by terrain type scripts.");
			add_string(file_id,"// Use the predefined script trap.txt.");
			add_short_string_to_file(file_id,"// Trap type = (",node->ex1a,");");			
			add_short_string_to_file(file_id,"// Trap severity, 0 thru 3, = (",node->ex1b,");");			
			add_short_string_to_file(file_id,"// Trap disarming penalty, 0 thru 100, = (",node->ex2a,");");									
			break;

		case 80: // select pc 
			if (node->ex1a == 2) {
				add_string(file_id,"\trun_select_a_pc(2);");		
				}
				else {
					add_short_string_to_file(file_id,"\tchoice = run_select_a_pc(",node->ex1a,");");
					if (node->ex1b >= 0) {
						add_string(file_id,"\tif (choice == 0)");		
						add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");
						}
					}
			break;
		case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: 
		case 90: case 92: case 93: case 94: case 95: case 96: case 97: case 98: case 99: 
		case 100: 
			add_string(file_id,"\tj = get_selected_pc();");		
			add_string(file_id,"\t// Note that if run_select_a_pc hasn't been called then j is -1 and");		
			add_string(file_id,"\t//   effect happens on whole party.");		
			add_string(file_id,"\ti = 0;");		
			add_string(file_id,"\twhile (i < 6) {");		
			add_string(file_id,"\t\tif (((j < 0) || (j == i)) && (char_ok(i))) {");		
			switch (node->type) {
				case 81: // damage
					add_ish_string_to_file(file_id,"\t\t\tr1 = get_ran(",node->ex1a,",1,",node->ex1b,");");
					add_ish_string_to_file(file_id,"\t\t\tdamage_char(i,r1 + ",node->ex2a,",",node->ex2b,");");
					break;
				case 82: // change health
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tchange_char_health(i,",node->ex1a,");");
					else 	add_short_string_to_file(file_id,"\t\t\tchange_char_health(i,-",node->ex1a,");");
					break;
				case 83: // change energy
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tchange_char_energy(i,",node->ex1a,");");
					else 	add_short_string_to_file(file_id,"\t\t\tchange_char_energy(i,-",node->ex1a,");");
					break;
				case 84: // change xp
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tchange_char_xp(i,",node->ex1a,");");
					else 	add_short_string_to_file(file_id,"\t\t\tchange_char_xp(i,-",node->ex1a,");");
					break;
				case 85: // change pc skill pts
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tchange_pc_skill_pts(i,",node->ex1a,");");
					else 	add_short_string_to_file(file_id,"\t\t\tchange_pc_skill_pts(i,-",node->ex1a,");");
					break;
				case 86: // kill
					if (node->ex1b == 0)
						add_string(file_id,"\t\trestore_pc(i);");		
							else add_short_string_to_file(file_id,"\t\t\tkill_char(i,",node->ex1a + 2,",0);");
					break;
				case 87: // poison
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,0,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,0,-",node->ex1a,",0,1);");
					break;
					
				case 88: // slow/haste
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,3,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,3,-",node->ex1a,",0,1);");
					break;
				case 89: // invuln
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,4,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,4,-",node->ex1a,",0,1);");
					break;
				case 90: // mr
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,5,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,5,-",node->ex1a,",0,1);");
					break;
				case 91: // webs
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,6,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,6,-",node->ex1a,",0,1);");
					break;
				case 92: // disease
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,7,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,7,-",node->ex1a,",0,1);");
					break;
				case 93: // sanct
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,14,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,14,-",node->ex1a,",0,1);");
					break;
				case 94: // curse/bless
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,2,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,2,-",node->ex1a,",0,1);");
					break;
				case 95: // dumbfound
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,13,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,13,-",node->ex1a,",0,1);");
					break;
				case 96: // sleep
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,10,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,10,-",node->ex1a,",0,1);");
					break;
				case 97: // paral
					if (node->ex1b == 0)
					add_short_string_to_file(file_id,"\t\t\tset_char_status(i,11,",node->ex1a,",0,1);");
					else 	add_short_string_to_file(file_id,"\t\t\tset_char_status(i,11,-",node->ex1a,",0,1);");
					break;
				case 98: // stat
					add_string(file_id,"// OBSOLETE VALUE WARNING: Many statistics have changed between");
					add_string(file_id,"// BoE and BoA. Make sure you're adjusting right statistic.");
					add_short_string_to_file(file_id,"\t\t\tif (get_ran(1,0,100) < ",node->pic,")");
					add_ish_string_to_file(file_id,"\t\t\t\talter_stat(i,",node->ex2a,",",((node->ex1b == 0) ? 1 : -1) * node->ex1a,");");
					break;
				case 99: // mage spell
					add_short_string_to_file(file_id,"\t\tchange_spell_level(i,0,",node->ex1a,",1);");				
					add_string(file_id,"// Potential translation problems");								 
					break;
				case 100: // priest spell
					add_short_string_to_file(file_id,"\t\tchange_spell_level(i,1,",node->ex1a,",1);");				
					add_string(file_id,"// Potential translation problems");								 
					break;
				}

			add_string(file_id,"\t\t\t}");		
			add_string(file_id,"\t\ti = i + 1;");		
			add_string(file_id,"\t\t}");		
			break;
		case 101: // gold
			add_short_string_to_file(file_id,"\tchange_coins(", ((node->ex1b == 0) ? 1 : -1) * node->ex1a,");");
			break;
		case 102: // food
			add_string(file_id,"// OBSOLETE NODE: Food works completely differently now.");
			break;
		case 103: // recipe
			add_short_string_to_file(file_id,"\tgive_recipe(",node->ex1a,");");
			add_string(file_id,"// Potential translation problems");						
			break;
		case 104: // stealth
			add_string(file_id,"// OBSOLETE NODE: Stealth doesn't exist anymore.");
			break;
		case 105: // firewalk
			add_short_string_to_file(file_id,"\tset_party_status(27,",((node->ex1b == 0) ? -1 : 1) * node->ex1a,");");
			break;
		case 106: // flying
			add_short_string_to_file(file_id,"\tset_party_status(25,",((node->ex1b == 0) ? -1 : 1) * node->ex1a,");");
			break;
			
		case 130: // sdf?
			if ((node->ex1a >= 0) && (node->ex1b >= 0)) {
				add_big_string_to_file(file_id,"\tif (get_flag(",node->sd1,",",node->sd2,") >= ",node->ex1a,")");
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				}
			if ((node->ex2a >= 0) && (node->ex2b >= 0)) {
				add_big_string_to_file(file_id,"\tif (get_flag(",node->sd1,",",node->sd2,") < ",node->ex2a,")");
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
				}
			break;
		case 131: // town?
			add_short_string_to_file(file_id,"\tif (current_town() == ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 132: // rand
			add_short_string_to_file(file_id,"\tif (get_ran(1,1,100) < ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 133: // spec i
			add_short_string_to_file(file_id,"\tif (has_special_item(",node->ex1a,") > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 134: // compare flags
			sprintf(temp_str1,"\tif (get_flag(%d,%d) > get_flag(%d,%d))", node->sd1, node->sd2, node->ex1a, node->ex1b);
			add_string(file_id,temp_str1);
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 135: case 136: // ter is type
			add_big_string_to_file(file_id,"\tif (get_terrain(",node->ex1a,",",node->ex1b,") == ",node->ex2a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			add_string(file_id,"// Potential translation problems");							
			break;
		case 137: // has gold
			add_short_string_to_file(file_id,"\tif (coins_amount() >= ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 138: case 143: // has food
			add_string(file_id,"// OBSOLETE NODE: Food works completely differently now.");
			break;
		case 139: // item class on space
			add_big_string_to_file(file_id,"\tif (item_of_class_on_spot(",node->ex1a,",",node->ex1b,",",node->ex2a,") > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 140: // has item of class
			add_short_string_to_file(file_id,"\tif (has_item_of_class(",node->ex1a,",0) > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 141: // equip item of class
			add_string(file_id,"\ti = 0;");
			add_string(file_id,"\twhile (i < 4) {");
			add_short_string_to_file(file_id,"\t\tif (char_has_item_of_class_equip(i,",node->ex1a,",0) > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\t\tend();");			
			add_string(file_id,"\t\ti = i + 1;");					
			add_string(file_id,"\t\t}");					
			
			break;
		case 142: // has gold, take
			add_short_string_to_file(file_id,"\tif (coins_amount() >= ",node->ex1a,") {");
			add_short_string_to_file(file_id,"\t\tchange_coins(-1 * ",node->ex1a,");");			
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			add_string(file_id,"\t\t}");
			break;
		case 144: // item class on space, take
			add_big_string_to_file(file_id,"\tif (item_of_class_on_spot(",node->ex1a,",",node->ex1b,",",node->ex2a,") > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 145: // has item of class, take
			add_short_string_to_file(file_id,"\tif (has_item_of_class(",node->ex1a,",1) > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 146: // equip item of class, take
			add_short_string_to_file(file_id,"\tif (char_has_item_of_class_equip(",node->ex1a,",1) > 0)");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 147: // day reached
			add_short_string_to_file(file_id,"\tif (what_day_of_scenario() >= ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 148: case 149: // any barrels/crates
			add_string(file_id,"\ti = 0;");		
			add_string(file_id,"\tk = 0;");		
			add_string(file_id,"\twhile (i < current_town_size()) {");		
			add_string(file_id,"\t\tj = 0;");		
			add_string(file_id,"\t\twhile (j < current_town_size()) {");		
			if (node->type == 148)
				add_string(file_id,"\t\t\tif (is_object_on_space(i,j,2))");					
				else add_string(file_id,"\t\t\tif (is_object_on_space(i,j,1))");					
			add_string(file_id,"\t\t\t\tk = 1;");					
			add_string(file_id,"\t\t\tj = j + 1;");					
			add_string(file_id,"\t\t\t}");					
			add_string(file_id,"\t\ti = i + 1;");					
			add_string(file_id,"\t\t}");					
			add_string(file_id,"\tif (k > 0)");		
			add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
			break;
		case 150: // event happened
			if (node->ex1b >= 0) {
				add_short_string_to_file(file_id,"\tif (what_day_of_scenario() >= ",node->ex1a,")");
				add_big_string_to_file(file_id,"\t\tif ((day_event_happened(",node->ex1b," < 0) || (day_event_happened(",node->ex1b,") > ",node->ex1a,"))");
				add_short_string_to_file(file_id,"\t\t\tset_state_continue(",node->ex2b + 10,");");			
				}
				else {
					add_short_string_to_file(file_id,"\tif (what_day_of_scenario() >= ",node->ex1a,")");
					add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex2b + 10,");");			
					}
			break;
		case 151: // Cave Lore present
			add_string(file_id,"// OBSOLETE NODE: Cave Lore Trait doesn't exist anymore.");
			add_string(file_id,"//   Try checking to see if party has enough Nature Lore skill instead.");
			add_short_string_to_file(file_id,"// set_state_continue(",node->ex1b + 10,");");						
			break;
		case 152: // Woodsman present
			add_string(file_id,"// OBSOLETE NODE: Woodsman Trait doesn't exist anymore.");
			add_string(file_id,"//   Try checking to see if party has enough Nature Lore skill instead.");
			add_short_string_to_file(file_id,"// set_state_continue(",node->ex1b + 10,");");						
			break;
		case 153: // enough mage lore
			add_short_string_to_file(file_id,"\tif (get_skill_total(13) >= ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;
		case 154: // text response
			add_string(file_id,"// WARNING, NODE ALTERED: The program now looks for exact matches of text");
			add_string(file_id,"//   (not just the first few characters). It is not case sensitive ");
			add_string(file_id,"//   (so CHEese counts the same as CHEESE or cheESE). ");
			add_string(file_id,"\tget_text_response(You Respond:);");
			if (node->ex1a >= 0) {
				add_ishtrs_string_to_file(file_id,"\t\tcheck_text_response_match(",boe_scen_text.scen_strs[node->ex1a],");");
				add_string(file_id,"\t\t if (got_text_match())");
				add_short_string_to_file(file_id,"\t\t\tset_state_continue(",node->ex1b + 10,");");			
				}
			if (node->ex2a >= 0) {
				add_ishtrs_string_to_file(file_id,"\t\tcheck_text_response_match(",boe_scen_text.scen_strs[node->ex2a],");");
				add_string(file_id,"\t\t if (got_text_match())");
				add_short_string_to_file(file_id,"\t\t\tset_state_continue(",node->ex2b + 10,");");			
				}
			break;
			
		case 155: // stuff done equal
			add_big_string_to_file(file_id,"\tif (get_flag(",node->sd1,",",node->sd2,") == ",node->ex1a,")");
			if (node->ex1b >= 0)
				add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
				else add_string(file_id,"\t\tend();");			
			break;

		case 170: // town host
			add_string(file_id,"\tmake_town_hostile();");
			break;
		case 171: // set ter
			add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
			add_string(file_id,"// You will need to find a new value for this function.");
			add_big_string_to_file(file_id,"\t set_terrain(",node->ex1a,",",node->ex1b,",",node->ex2a,");");
			add_string(file_id,"// Potential translation problems");										  
			break;
		case 172: // swap ter
			add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
			add_string(file_id,"// You will need to find a new value for this function.");
			add_big_string_to_file(file_id,"\t if (get_terrain(",node->ex1a,",",node->ex1b,") == ",node->ex2a,")");
			add_big_string_to_file(file_id,"\t\t set_terrain(",node->ex1a,",",node->ex1b,",",node->ex2b,");");
			add_big_string_to_file(file_id,"\t else if (get_terrain(",node->ex1a,",",node->ex1b,") == ",node->ex2b,")");
			add_big_string_to_file(file_id,"\t\t set_terrain(",node->ex1a,",",node->ex1b,",",node->ex2a,");");
			add_string(file_id,"// Potential translation problems");										  
			break;
		case 173: // transform ter
			add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
			add_string(file_id,"// You will need to find a new value for this function. Also, the swap terrain");
			add_string(file_id,"// values for the terrain types might work differently in BoA");
			add_ish_string_to_file(file_id,"\t// flip_terrain(",node->ex1a,",",node->ex1b,");");			
			add_string(file_id,"// Potential translation problems");										  
			break;
		case 174: // move party
			add_big_string_to_file(file_id,"\tteleport_party(",node->ex1a,",",node->ex1b,",",(node->ex2a == 0) ? 1 : 0,");");
			break;
		case 175: // hit_space
			sprintf(temp_str1,"\tdamage_near_loc(%d,%d,%d,0,%d);", node->ex1a, node->ex1b, node->ex2a, node->ex2b);
			add_string(file_id,temp_str1);
			break;
		case 176: // explode on space
			sprintf(temp_str1,"\tdamage_near_loc(%d,%d,%d,%d,%d);", node->ex1a, node->ex1b, node->ex2a, node->pic, node->ex2b);
			add_string(file_id,temp_str1);
			break;
		case 177: case 178: case 217: case 218: // unlock fcns
			add_string(file_id,"// OBSOLETE NODE: Doors are handled completely differently in BoA.");
			add_string(file_id,"//   You may need to use flip_terrain to change the doors, or send.");
			add_string(file_id,"//   messages to the door scripts.");
			add_ish_string_to_file(file_id,"\t// flip_terrain(",node->ex1a,",",node->ex1b,");");
			add_string(file_id,"// Potential translation problems");										  
			break;

		case 179: // sfx burst
			sprintf(temp_str1,"\tput_boom_on_space(%d,%d,%d,1);", node->ex1a, node->ex1b, node->ex2a);
			add_string(file_id,temp_str1);
			sprintf(temp_str1,"\tput_boom_on_space(%d,%d,%d,1);", node->ex1a, node->ex1b, node->ex2a);
			add_string(file_id,temp_str1);
			sprintf(temp_str1,"\tput_boom_on_space(%d,%d,%d,1);", node->ex1a, node->ex1b, node->ex2a);
			add_string(file_id,temp_str1);
			sprintf(temp_str1,"\tput_boom_on_space(%d,%d,%d,1);", node->ex1a, node->ex1b, node->ex2a);
			add_string(file_id,temp_str1);
			sprintf(temp_str1,"\tput_boom_on_space(%d,%d,%d,1);", node->ex1a, node->ex1b, node->ex2a);
			add_string(file_id,temp_str1);
			add_string(file_id,"\trun_animation();");			
			break;
		case 180: // wand monst
			add_string(file_id,"\tmake_wandering_monst();");
			break;
		case 181: // place m
			add_big_string_to_file(file_id,"\tplace_monster(",node->ex1a,",",node->ex1b,",",node->ex2a,",0);");			
			add_string(file_id,"// Potential translation problems");										  			
			break;
		case 182: // destroy m
			add_string(file_id,"\ti = 6;");
			add_string(file_id,"\twhile (i < NUM_CHARS) {");
			add_short_string_to_file(file_id,"\t\tif ((char_ok(i)) && (creature_type(i) == ",node->ex1a,"))");			
			add_string(file_id,"\t\t\terase_char(i);");					
			add_string(file_id,"\t\ti = i + 1;");					
			add_string(file_id,"\t\t}");					
			add_string(file_id,"// Potential translation problems");										  			
			break;
		case 183: // destroy all m
			add_string(file_id,"\ti = 6;");
			add_string(file_id,"\twhile (i < NUM_CHARS) {");
			if (node->ex1a == 0) {
				add_string(file_id,"\t\terase_char(i);");					
				}
			if (node->ex1a == 1) {
				add_string(file_id,"\t\tif ((char_ok(i)) && (get_attitude(i) < 10))");			
				add_string(file_id,"\t\t\terase_char(i);");					
				}
			if (node->ex1a == 2) {
				add_string(file_id,"\t\tif ((char_ok(i)) && (get_attitude(i) >= 10))");			
				add_string(file_id,"\t\t\terase_char(i);");					
				}
			add_string(file_id,"\t\ti = i + 1;");					
			add_string(file_id,"\t\t}");					
			break;
		case 184: // levers
			add_string(file_id,"\treset_dialog_preset_options(2);");
			add_string(file_id,"\tchoice = run_dialog(0);");
			add_string(file_id,"\tif (choice == 2) {");
			add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");			
			add_string(file_id,"\t\t}");
			break;
		case 185: // portal
			add_string(file_id,"\treset_dialog_preset_options(3);");
			add_string(file_id,"\tchoice = run_dialog(0);");
			add_string(file_id,"\tif (choice == 2) {");
			add_string(file_id,"\t\tblock_entry(1);");
			add_ish_string_to_file(file_id,"\t\tteleport_party(",node->ex1a,",",node->ex1b,",0);");			
			add_string(file_id,"\t\tend();");
			add_string(file_id,"\t\t}");
			break;
 		case 186: // button
			add_string(file_id,"\treset_dialog_preset_options(4);");
			add_string(file_id,"\tchoice = run_dialog(0);");
			add_string(file_id,"\tif (choice == 2) {");
			add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");
			add_string(file_id,"\t\t}");
			break;
		case 187: // stairs
			add_string(file_id,"\tblock_entry(1);");
			add_string(file_id,"\treset_dialog_preset_options(5);");
			add_string(file_id,"\tchoice = run_dialog(0);");
			add_string(file_id,"\tif (choice == 2) {");
			add_big_string_to_file(file_id,"\t\tmove_to_new_town(",node->ex2a,",",node->ex1a,",",node->ex1b,");");			
			add_string(file_id,"\t\tend();");
			add_string(file_id,"\t\t}");
			break;
		case 188: // lever
			add_string(file_id,"// OBSOLETE NODE: Levers are now handled by terrain scripts.");
			add_string(file_id,"//   Look for examples of levers in the scenarios that come with Blades.");
			add_string(file_id,"\treset_dialog_preset_options(2);");
			add_string(file_id,"\tchoice = run_dialog(0);");
			add_string(file_id,"\tif (choice == 2) {");
			add_short_string_to_file(file_id,"\t\tset_state_continue(",node->ex1b + 10,");");
			add_string(file_id,"\t\t}");
			
			break;
		case 189: // portal
			add_string(file_id,"\treset_dialog_preset_options(3);");
			if (node->m1 < 0) {
				add_string(file_id,"\t\tblock_entry(1);");
				add_ish_string_to_file(file_id,"\t\tteleport_party(",node->ex1a,",",node->ex1b,",0);");			
				add_string(file_id,"\t\tend();");
				}
				else {
					for (short i = 0; i < 6; i++) {
						get_bl_str(temp_str1,node_type,node->m1 + i);
						if (strlen(temp_str1) > 0) {
							sprintf(temp_str1,"\tadd_dialog_str(%d,\"%s\",0);",i,temp_str1);
							add_string(file_id,temp_str1);
							}
						}
					add_string(file_id,"\tchoice = run_dialog(0);");
					add_string(file_id,"\tif (choice == 2) {");
					add_string(file_id,"\t\tblock_entry(1);");
					add_ish_string_to_file(file_id,"\t\tteleport_party(",node->ex1a,",",node->ex1b,",0);");
					add_string(file_id,"\t\tend();");
					add_string(file_id,"\t\t}");
					}
			break;
		case 190: // stairs
			add_string(file_id,"\tblock_entry(1);");
			if (node->m1 < 0) {
				add_big_string_to_file(file_id,"\t\tmove_to_new_town(",node->ex2a,",",node->ex1a,",",node->ex1b,");");			
				add_string(file_id,"\t\tend();");
				}
				else {
					add_string(file_id,"\treset_dialog_preset_options(5);");
					for (short i = 0; i < 6; i++) {
						get_bl_str(temp_str1,node_type,node->m1 + i);
						if (strlen(temp_str1) > 0) {
							sprintf(temp_str1,"\tadd_dialog_str(%d,\"%s\",0);",i,temp_str1);
							add_string(file_id,temp_str1);
							}
						}
					add_string(file_id,"\tchoice = run_dialog(0);");
					add_string(file_id,"\tif (choice == 2) {");
					add_big_string_to_file(file_id,"\t\tmove_to_new_town(",node->ex2a,",",node->ex1a,",",node->ex1b,");");			
					add_string(file_id,"\t\tend();");
					add_string(file_id,"\t\t}");
					}
			break;
		case 191: // move outd
			sprintf(temp_str1,"\tchange_outdoor_location(%d,%d,%d,%d);", node->ex1a, node->ex1b, node->ex2a, node->ex2b);
			add_string(file_id,temp_str1);
			break;
		case 192: // place item
			add_big_string_to_file(file_id,"\t\tput_item_on_spot(",node->ex1a,",",node->ex1b,",",node->ex2a,");");			
			add_string(file_id,"// Potential translation problems");										  						
			break;
		case 193: // split party
			add_string(file_id,"\tblock_entry(1);");
			add_big_string_to_file(file_id,"\ttry_to_split_party(",node->ex1a,",",node->ex1b,",",node->ex2a,");");			
			break;
		case 194: // reunite party
			add_string(file_id,"\tblock_entry(1);");
			add_string(file_id,"\treunite_party();");
			break;
		case 195: // timer
			add_string(file_id,"// OBSOLETE NODE: Timers are no longer used. Manually write scenario timers in the ");
			add_string(file_id,"// scenario script. Pick a stuff done flag for the timer, set it to the number of turns");
			add_string(file_id,"// for the timer, and have it be decreased 1 in state START_STATE in the scenario script.");
			add_short_string_to_file(file_id,"// Number of moves = (",node->ex1a,");");			
			add_short_string_to_file(file_id,"// Town state called: (",node->ex1b + 10,");");						
			
			break;

		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209: 
		case 210: case 212: case 213: case 214: case 215: case 216: 
			add_short_string_to_file(file_id,"\ti = ",node->ex1b,";");
			add_short_string_to_file(file_id,"\twhile (i <= ",node->ex2b,") {");
			add_short_string_to_file(file_id,"\t\tj = ",node->ex1a,";");
			add_short_string_to_file(file_id,"\t\twhile (j <= ",node->ex2a,") {");
			
			if ((node->type != 214) && (node->type != 215) && (node->sd1 < 100))
				add_short_string_to_file(file_id,"\t\t\tif (get_ran(1,1,100) <= ",node->sd1,")");

			switch(node->type) {
				case 200:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,1);");
					break;
				case 201:
					add_string(file_id,"// WARNING: Force fields don't exist in BoA. Placing fire walls instead.");
					add_string(file_id,"\t\t\tput_field_on_space(i,j,1);");
					break;
				case 202:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,4);");
					break;
				case 203:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,5);");
					break;
				case 204:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,3);");
					break;
				case 205:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,0);");
					break;
				case 206:
					add_string(file_id,"\t\t\tput_field_on_space(i,j,6);");
					break;
				case 207:
					add_string(file_id,"\t\t\tput_object_on_space(i,j,3);");
					break;
				case 208:
					add_string(file_id,"\t\t\tput_object_on_space(i,j,4);");
					break;
				case 209:
					add_string(file_id,"\t\t\tput_stain_on_space(i,j,-1);");
					add_string(file_id,"\t\t\tput_field_on_space(i,j,-1);");
					if (node->sd1 != 0)
						add_string(file_id,"\t\t\tput_object_on_space(i,j,-1);");
					break;
				case 210:
					add_short_string_to_file(file_id,"\t\t\tput_stain_on_space(i,j,",node->sd2,");");
					break;
				case 211:
					add_short_string_to_file(file_id,"\t\t\tput_object_on_space(i,j,",node->sd2,");");
					break;
				case 212: // move item on spot
					add_ish_string_to_file(file_id,"\t\tmove_item_on_spot(i,j,",node->sd1,",",node->sd2,");");			
					break;
				case 213: // dest item on spot
					add_string(file_id,"\t\t\tmove_item_on_spot(i,j,-1,-1);");
					break;
				case 214: // change ter
					add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
					add_string(file_id,"// You will need to find a new value for this function.");
					add_short_string_to_file(file_id,"\t\t\t//set_terrain(i,j,",node->sd1,");");
					add_string(file_id,"// Potential translation problems");										  											  					
					break;
				case 215: // swap ter
					add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
					add_string(file_id,"// You will need to find a new value for this function.");
					add_short_string_to_file(file_id,"\t// if (get_terrain(i,j) == ",node->sd1,")");
					add_short_string_to_file(file_id,"\t\t// set_terrain(i,j,",node->sd2,");");
					add_short_string_to_file(file_id,"\t// else if (get_terrain(i,j) == ",node->sd2,")");
					add_short_string_to_file(file_id,"\t\t// set_terrain(i,j,",node->sd1,");");
					add_string(file_id,"// Potential translation problems");										  											  
					break;
				case 216: // transform
					add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
					add_string(file_id,"// You will need to find a new value for this function. Also, the swap terrain");
					add_string(file_id,"// values for the terrain types might work differently in BoA");
					add_string_to_file(file_id,"\t flip_terrain(i,j);");
					add_string(file_id,"// Potential translation problems");										  											  					
					break;
				}	

			add_string(file_id,"\t\t\tj = j + 1;");					
			add_string(file_id,"\t\t\t}");					
			add_string(file_id,"\t\ti = i + 1;");					
			add_string(file_id,"\t\t}");					
			break;
		case 225: // wand monst
			add_string(file_id,"\tmake_wandering_monst();");
			break;
		case 226: // set ter
			add_string(file_id,"// OBSOLETE VALUE WARNING: Terrain works completely differently now.");
			add_string(file_id,"// You will need to find a new value for this function.");
			add_big_string_to_file(file_id,"\t set_terrain(",node->ex1a,",",node->ex1b,",",node->ex2a,");");
			break;
		case 227: // out special
			add_short_string_to_file(file_id,"\tcreate_out_spec_enc(",node->ex1a,");");			
			break;
		case 228: // out move
			add_string(file_id,"\tblock_entry(1);");
			add_ish_string_to_file(file_id,"\tout_move_party(",node->ex1a,",",node->ex1b,");");
			break;
		case 229: // out store
			add_string(file_id,"// OBSOLETE VALUE WARNING: Shops work very differently now.");
//			void begin_shop_mode(char shop_name,char shop_desc,short which_shop,short price_adjust,short sell_adjustment)
//			Immediately puts the party in shopping mode, buying from which_shop, which has name shop_name and description
//			 shop_desc. price_adjust is a number from 0 to 6 determining how expensive the shop items are (0 - cheaper, 
//			6 - more expensive). sell_adjustment is either -1 (which means the shop doesn't buy items) or 0 to 6, a number
//			 which determines how much the shop pays for items (0 means the store pays more, 6 means pays less).
			add_short_string_to_file(file_id,"// Number of first item in store = (",node->ex1a,");");
			add_short_string_to_file(file_id,"// Store type (see docs. for list) = (",node->ex1b,");");
			add_short_string_to_file(file_id,"// Number of items in store (1 .. 40) = (",node->ex2a,");");
			add_short_string_to_file(file_id,"// Cost adjust (0 .. 6, lower = cheaper) = (",node->ex2b,");");
			break;

		}

	if (node->jumpto >= 0)
		add_short_string_to_file(file_id,"\tset_state_continue(",node->jumpto + 10,");");
		
	add_string(file_id,"break;");
	add_cr(file_id);
}


/*
	
	for (short i = 0; i < 10; i++)
		if (same_string(boe_scen_text.talk_strs[i],"Unused" == FALSE)) {
			port_dialogue_intro_text(&current_dialogue_node,i);
			
			for (j = 0; j < 60 ; j++) {
				if (boe_talk_data.talk_nodes[j].personality % 10 == i)
					port_dialogue_node(&current_dialogue_node,i,j,which_town);
				}
			}
			
*/

// which_slot is 0 .. 9. it is which of the 10 personalities in the town being ported
void port_dialogue_intro_text(short *current_dialogue_node,short which_slot,FILE *file_id,short town_being_ported)
{
	char str[256];
	
	add_short_string_to_file(file_id,"begintalknode ",*current_dialogue_node,";");
	add_string(file_id,"\tstate = -1;");
	add_short_string_to_file(file_id,"\tpersonality = ",town_being_ported * 20 + which_slot,";");
	add_short_string_to_file(file_id,"\tnextstate = ",which_slot * 10 + 1,";");
	add_string(file_id,"\tcondition = 1;");
	add_ishtrs_string_to_file(file_id,"\tquestion = \"",boe_scen_text.talk_strs[which_slot],"\";");
	get_bl_str(str,3,which_slot + 10);
	add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
	get_bl_str(str,3,which_slot + 20);
	add_ishtrs_string_to_file(file_id,"\ttext5 = \"",str,"\";");
	if (which_slot == 0)
		add_string(file_id,"\taction = INTRO; // This line only does anything if the character has a personality set.");
		else add_string(file_id,"\taction = INTRO;");
	add_cr(file_id);
	
	// see if we can find wharacter who ties into this and set its memory cell
	for (short i = 0; i < 60; i++) 
		if ((town.creatures[i].personality >= 0) && (boe_big_town.creatures[i].number > 0) &&
		  (town.creatures[i].personality == 20 * town_being_ported + which_slot)) {
			town.creatures[i].memory_cells[3] = *current_dialogue_node;
			}
				
	*current_dialogue_node = *current_dialogue_node + 1;

	add_short_string_to_file(file_id,"begintalknode ",*current_dialogue_node,";");
	add_short_string_to_file(file_id,"\tstate = ",which_slot * 10 + 1,";");
	add_short_string_to_file(file_id,"\tpersonality = ",town_being_ported * 20 + which_slot,";");
	add_short_string_to_file(file_id,"\tnextstate = ",which_slot * 10 + 1,";");
	add_string(file_id,"\tcondition = 1;");
	
	add_string(file_id,"\tquestion = \"_What is your job?_\";");	
	get_bl_str(str,3,which_slot + 30);
	add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
	add_cr(file_id);

	*current_dialogue_node = *current_dialogue_node + 1;

}
void port_dialogue_node(short *current_dialogue_node,short which_slot,FILE *file_id,short which_node,short town_being_ported)

{
	char str[256],str2[256],str3[256];
	char new_line[512];	
	if ((boe_talk_data.talk_nodes[which_node].personality < 0) && (boe_talk_data.talk_nodes[which_node].personality != -2))
		return;
	
	if (boe_talk_data.talk_nodes[which_node].personality == -2) {
		add_string(file_id,"// OBSOLETE: Nodes following this point had a personality of -2.");
		add_string(file_id,"//  Nodes reachable from all personalities are obsolete in BoA.");
		}
	add_short_string_to_file(file_id,"begintalknode ",*current_dialogue_node,";");
	add_short_string_to_file(file_id,"\tstate = ",which_slot * 10 + 1,";");
	add_short_string_to_file(file_id,"\tpersonality = ",town_being_ported * 20 + which_slot,";");
	add_string(file_id,"\tnextstate = -1;");
	//add_short_string_to_file(file_id,"\tnextstate = ",which_slot * 10 + 1,";");
	add_string(file_id,"\tcondition = 1;");
	sprintf(str,"\tquestion = _xxxx xxxx_;");
	str[12] = 34;
	str[22] = 34;
	str[13] = boe_talk_data.talk_nodes[which_node].link1[0];
	str[14] = boe_talk_data.talk_nodes[which_node].link1[1];
	str[15] = boe_talk_data.talk_nodes[which_node].link1[2];
	str[16] = boe_talk_data.talk_nodes[which_node].link1[3];
	str[18] = boe_talk_data.talk_nodes[which_node].link2[0];
	str[19] = boe_talk_data.talk_nodes[which_node].link2[1];
	str[20] = boe_talk_data.talk_nodes[which_node].link2[2];
	str[21] = boe_talk_data.talk_nodes[which_node].link2[3];
	add_string(file_id,str);
	
	get_bl_str(str,3,which_node * 2 + 40);
	get_bl_str(str2,3,which_node * 2 + 40 + 1);
	switch (boe_talk_data.talk_nodes[which_node].type) {
		case 0: // talk
			if (strlen(str) > 0) 
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0) 
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			break;
		case 1: // dep on sdf
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
				add_big_string_to_file(file_id,"\taction = DEP_ON_SDF ",boe_talk_data.talk_nodes[which_node].extras[0]," ",boe_talk_data.talk_nodes[which_node].extras[1]," ",boe_talk_data.talk_nodes[which_node].extras[2],";");
			break;
		case 2: // set sdf to 1
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");			
				add_ish_string_to_file(file_id,"\taction = SET_SDF ",boe_talk_data.talk_nodes[which_node].extras[0]," ",boe_talk_data.talk_nodes[which_node].extras[1]," 1;");
			break;
		case 3: // inn
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string_to_file(file_id,"\ttext5 = \"You can't stay in an inn when on horseback.\";");
			add_big_string_to_file(file_id,"\taction = INN ",boe_talk_data.talk_nodes[which_node].extras[0]," ",boe_talk_data.talk_nodes[which_node].extras[2]," ",boe_talk_data.talk_nodes[which_node].extras[3],";");
			add_short_string_to_file(file_id,"\t\t// Quality of inn, 0 to 3, = ",boe_talk_data.talk_nodes[which_node].extras[1],".");
			break;
		case 4: // dep on day
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			add_short_string_to_file(file_id,"\t\tif (what_day_of_scenario() >= ",boe_talk_data.talk_nodes[which_node].extras[0],")");
			add_string(file_id,"\t\t\tremove_string(1);");
			add_string(file_id,"\t\t\telse remove_string(3);");	
			add_string(file_id,"\tbreak;");
			break;
		case 5: // dep on day with event
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			if (boe_talk_data.talk_nodes[which_node].extras[1] > 0) {
				sprintf(str3,"\t\tif ((what_day_of_scenario() >= %d) && ((day_event_happened(%d) < 0) || (day_event_happened(%d) >= %d)))", boe_talk_data.talk_nodes[which_node].extras[0],boe_talk_data.talk_nodes[which_node].extras[1], boe_talk_data.talk_nodes[which_node].extras[1],boe_talk_data.talk_nodes[which_node].extras[0]);
				add_string(file_id,str3);
				}
				else add_short_string_to_file(file_id,"\t\tif (what_day_of_scenario() >= ",boe_talk_data.talk_nodes[which_node].extras[0],")");
			add_string(file_id,"\t\t\tremove_string(3);");
			add_string(file_id,"\t\t\telse remove_string(1);");	
			add_string(file_id,"\tbreak;");
			break;
		case 6: // dep on town
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			add_short_string_to_file(file_id,"\t\tif (current_town() == ",boe_talk_data.talk_nodes[which_node].extras[0],")");
			add_string(file_id,"\t\t\tremove_string(3);");
			add_string(file_id,"\t\t\telse remove_string(1);");	
			add_string(file_id,"\tbreak;");
			break;
		case 7: // shop
			add_string(file_id,"\ttext1 = \"You finish shopping.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"//WARNING: Shops work differently now. You'll need to modify this.");
			sprintf(str3,"\t\tbegin_shop_mode(\"%s\",\"Shop description here.\",0,%d,-1);",str, boe_talk_data.talk_nodes[which_node].extras[0]);
			add_string(file_id,str3);
			add_short_string_to_file(file_id,"\t\t// Number of first item = ",boe_talk_data.talk_nodes[which_node].extras[1],".");
			add_short_string_to_file(file_id,"\t\t// Total number of items in shop = ",boe_talk_data.talk_nodes[which_node].extras[2],".");
			add_string(file_id,"\tbreak;");			
			break;
		case 8: // train
			add_string(file_id,"//OBSOLETE: Training works differently now. You can train anywhere in towns.");
			add_string(file_id,"//You might want to completely rewrite or delete this node.");
			break;
		case 9: case 10: case 11: // shop
			add_string(file_id,"\ttext1 = \"You finish shopping.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"//WARNING: Shops work differently now. You'll need to modify this.");
			sprintf(str3,"\t\tbegin_shop_mode(\"%s\",\"Shop description here.\",0,%d,-1);",str, boe_talk_data.talk_nodes[which_node].extras[0]);
			add_string(file_id,str3);
			add_string(file_id,"//WARNING: Also note that spell and recipe shops work the same as other shops.");
			add_short_string_to_file(file_id,"\t\t// Number of first item = ",boe_talk_data.talk_nodes[which_node].extras[1],".");
			add_short_string_to_file(file_id,"\t\t// Total number of items in shop = ",boe_talk_data.talk_nodes[which_node].extras[2],".");
			add_string(file_id,"\tbreak;");			
			break;
			
		case 12: // healer
			add_string(file_id,"//OBSOLETE: Healers no longer work like a shop.");
			add_string(file_id,"//Use the restore_pc command to heal characters.");
			add_short_string_to_file(file_id,"\t\t// Cost adjustment = ",boe_talk_data.talk_nodes[which_node].extras[0],".");			
			break;
		case 13: case 14: case 15: // sell
			add_string(file_id,"//OBSOLETE: Selling no longer gets its own window.");
			add_string(file_id,"//  You sell items while in shop mode.");
			break;
		case 16: // id
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");			
			add_short_string_to_file(file_id,"\taction = ID ",boe_talk_data.talk_nodes[which_node].extras[0],";");
			break;
		case 17: // enchant weapons
			add_string(file_id,"//OBSOLETE: Individual items can no longer be augmented.");
			add_string(file_id,"//  You can hardcode this using the it_augment_item field and.");
			add_string(file_id,"//  the augment_item_on_space function.");
			add_short_string_to_file(file_id,"\t\t// Type of enchantment = ",boe_talk_data.talk_nodes[which_node].extras[0],".");			
			break;
		case 18: // buy response
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
				sprintf(str3,"\taction = PAY %d %d %d %d;",-1,-1,-1,boe_talk_data.talk_nodes[which_node].extras[0]);
				add_string(file_id,str3);
			break;
		case 19: // buy response set flag
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
				add_string(file_id,"\ttext5 = \"You have already bought this.\";");
				sprintf(str3,"\taction = PAY %d %d %d %d;",boe_talk_data.talk_nodes[which_node].extras[1],boe_talk_data.talk_nodes[which_node].extras[2],boe_talk_data.talk_nodes[which_node].extras[3],boe_talk_data.talk_nodes[which_node].extras[0]);
				add_string(file_id,str3);
			break;
		case 20: // ship shop
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\ttext5 = \"There are no ships left.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tclear_strings();");
			add_short_string_to_file(file_id,"\t\tif (coins_amount() >= ",boe_talk_data.talk_nodes[which_node].extras[0],") {");
			add_ish_string_to_file(file_id,"\t\t\tif (set_boat_range_property(",boe_talk_data.talk_nodes[which_node].extras[1],",",boe_talk_data.talk_nodes[which_node].extras[1] + boe_talk_data.talk_nodes[which_node].extras[2]," >= 0) {");
			add_string(file_id,"\t\t\t\tadd_string(1);");	
			add_short_string_to_file(file_id,"\t\t\t\tchange_coins(-",boe_talk_data.talk_nodes[which_node].extras[0],");");
			add_string(file_id,"\t\t\t\t}");
			add_string(file_id,"\t\t\t\telse add_string(5);");
			add_string(file_id,"\t\t\t}");
			add_string(file_id,"\t\t\telse add_string(3);");	
			add_string(file_id,"\tbreak;");
			break;
		case 21: // horse shop
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\ttext5 = \"There are no horses left.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tclear_strings();");
			add_short_string_to_file(file_id,"\t\tif (coins_amount() >= ",boe_talk_data.talk_nodes[which_node].extras[0],") {");
			add_ish_string_to_file(file_id,"\t\t\tif (set_horse_range_property(",boe_talk_data.talk_nodes[which_node].extras[1],",",boe_talk_data.talk_nodes[which_node].extras[1] + boe_talk_data.talk_nodes[which_node].extras[2]," >= 0) {");
			add_string(file_id,"\t\t\t\tadd_string(1);");	
			add_short_string_to_file(file_id,"\t\t\t\tchange_coins(-",boe_talk_data.talk_nodes[which_node].extras[0],");");
			add_string(file_id,"\t\t\t\t}");
			add_string(file_id,"\t\t\t\telse add_string(5);");
			add_string(file_id,"\t\t\t}");
			add_string(file_id,"\t\t\telse add_string(3);");	
			add_string(file_id,"\tbreak;");
			break;
		case 22: // buy spec item
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\ttext5 = \"You already have this item.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tclear_strings();");
			add_short_string_to_file(file_id,"\t\tif (has_special_item(",boe_talk_data.talk_nodes[which_node].extras[0],") > 0)");
			add_string(file_id,"\t\t\tadd_string(5);");	
			add_string(file_id,"\t\t\telse {");	
			add_short_string_to_file(file_id,"\t\t\t\tif (pay_coins(",boe_talk_data.talk_nodes[which_node].extras[1],")) {");
			add_string(file_id,"\t\t\t\t\tadd_string(1);");	
			add_short_string_to_file(file_id,"\t\t\t\t\tchange_spec_item(",boe_talk_data.talk_nodes[which_node].extras[0],",1);");
			add_string(file_id,"\t\t\t\t\t}");	
			add_string(file_id,"\t\t\t\t\telse add_string(3);");	
			add_string(file_id,"\t\t\t\t}");	
			add_string(file_id,"\tbreak;");			
			break;
		case 23: // shop
			add_string(file_id,"\ttext1 = \"You finish shopping.\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"//WARNING: Shops work differently now. You'll need to modify this.");
			sprintf(str3,"\t\tbegin_shop_mode(\"%s\",\"Shop description here.\",0,%d,-1);",str, boe_talk_data.talk_nodes[which_node].extras[0]);
			add_string(file_id,str3);
			add_short_string_to_file(file_id,"\t\t// Number of first item = ",boe_talk_data.talk_nodes[which_node].extras[1],".");
			add_short_string_to_file(file_id,"\t\t// Total number of items in shop = ",boe_talk_data.talk_nodes[which_node].extras[2],".");
			add_string(file_id,"\tbreak;");			
			break;
			
		case 24: // buy town location
			add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			add_ishtrs_string_to_file(file_id,"\ttext3 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tclear_strings();");
			add_string(file_id,"\t\t\telse {");	
			add_short_string_to_file(file_id,"\t\t\t\tif (pay_coins(",boe_talk_data.talk_nodes[which_node].extras[0],")) {");
			add_string(file_id,"\t\t\t\t\tadd_string(1);");	
			add_short_string_to_file(file_id,"\t\t\t\t\tset_town_visibility(",boe_talk_data.talk_nodes[which_node].extras[1],",1);");
			add_string(file_id,"\t\t\t\t\t}");	
			add_string(file_id,"\t\t\t\t\telse add_string(3);");	
			add_string(file_id,"\t\t\t\t}");	
			add_string(file_id,"\tbreak;");			
			break;
		case 25: // force conv end
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\taction = END_TALK;");	
			break;
		case 26: // hostile conv end
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\taction = END_TALK;");	
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tset_attitude(character_talking_to(),10);");
			add_string(file_id,"\tbreak;");			
			break;
		case 27: // town hostile conv end
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\taction = END_TALK;");	
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\tmake_town_hostile();");
			add_string(file_id,"\tbreak;");			
			break;
		case 28: // kill char, conv end
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\taction = END_TALK;");	
			add_string(file_id,"\tcode =");
			add_string(file_id,"\t\terase_char(character_talking_to());");
			add_string(file_id,"\tbreak;");			
			break;
		case 29: // run town node
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			add_short_string_to_file(file_id,"\trun_town_script(",boe_talk_data.talk_nodes[which_node].extras[0] + 10,");");			
			add_string(file_id,"\tbreak;");			
			break;
		case 30: // run scenario node
			if (strlen(str) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext1 = \"",str,"\";");
			if (strlen(str2) > 0)
				add_ishtrs_string_to_file(file_id,"\ttext2 = \"",str2,"\";");
			add_string(file_id,"\tcode =");
			add_short_string_to_file(file_id,"\trun_scenario_script(",boe_talk_data.talk_nodes[which_node].extras[0] + 10,");");			
			add_string(file_id,"\tbreak;");			
			break;
	}
	*current_dialogue_node = *current_dialogue_node + 1;
	add_cr(file_id);
}


void handle_messages(FILE *file_id,short node_type,short message_1,short message_2)

{
	char str1[512] = "";
	char str2[512] = "";
	
	if ((message_1 < 0) && (message_2 < 0))
		return;

	if (message_1 >= 0) {
		get_bl_str(str1,node_type,message_1);
		add_ishtrs_string_to_file(file_id,"\tmessage_dialog(\"",str1,"\");");
		}
	if (message_2 >= 0) {
		get_bl_str(str2,node_type,message_2);
		add_ishtrs_string_to_file(file_id,"\tmessage_dialog(\"",str2,"\");");
		}
}


//str_type: 0 - scenario, 1 - town, 2 - outdoor 3 - dialogue
void get_bl_str(char *str,short str_type,short str_num)
{
	if (str_num < 0) {
		str[0] = 0;
		return;
		}
	switch (str_type) {
		case 0: 
		strcpy(str,boe_scen_text.scen_strs[str_num + 160]); 
		break;
		case 1: 
		strcpy(str,boe_scen_text.town_strs[str_num + 20]); 
		break;
		case 2: 
		strcpy(str,boe_scen_text.out_strs[str_num + 10]); 
		break;
		case 3: 
		strcpy(str,boe_scen_text.talk_strs[str_num]); 
		break;
		}
	for (short i = 0; i < (short)strlen(str); i++)
		if (str[i] == 34)
			str[i] = '_';
}

void add_ishtrs_string_to_file(FILE *file_id,char *str1,char *str2,char *str3)
{
	char message[512];
	
		sprintf(message,"%s%s%s",str1,str2,str3);

	add_string_to_file(file_id,message);
	add_cr(file_id);
}
void add_short_string_to_file(FILE *file_id,char *str1,short num,char *str2)
{
	char message[512];
	
	if (num == -999)
		sprintf(message,"%s%s",str1,str2);
		else sprintf(message,"%s%d%s", str1, num, str2);

	add_string_to_file(file_id,message);
	add_cr(file_id);
}

void add_ish_string_to_file(FILE *file_id,char *str1,short num1,char *str2,short num2,char *str3)
{
	char message[512];
	
		sprintf(message,"%s%d%s%d%s",str1, num1,str2, num2,str3);

	add_string_to_file(file_id,message);
	add_cr(file_id);
}

void add_big_string_to_file(FILE *file_id,char *str1,short num1,char *str2,short num2,char *str3,short num3,char *str4)
{
	char message[512];
	
	if ((num1 == -999) && (num2 == -999) && (num3 == -999))
		sprintf(message,"%s%s%s%s",str1,str2,str3,str4);
		else if ((num2 == -999) && (num3 == -999))
			sprintf(message,"%s%d%s%s%s",str1, num1,str2,str3,str4);
		else if (num3 == -999)
			sprintf(message,"%s%d%s%d%s%s", str1, num1, str2, num2, str3, str4);

	add_string_to_file(file_id,message);
	add_cr(file_id);
}

void add_string(FILE *file_id,char *str)
{
	add_string_to_file(file_id,str);
	add_cr(file_id);
}
void add_string_to_file(FILE *file_id,char *str)
{
	if (strlen(str) == 0)
		return;
	
	long len = (long) (strlen((char *)str));
	FSWrite(file_id, &len, (char *) str);
	
}

void add_cr(FILE *file_id)
{
	add_string_to_file(file_id,"\r");
}

void trunc_str(char *str)
{
	short null_spot = 0;
	
	for (short i = 0; i < (short)strlen(str); i++)
		if (str[i] != ' ')
			null_spot = i + 1;
	str[null_spot] = 0;
}

void clean_str(char *str)
{
	
	for (short i = 0; i < (short)strlen(str); i++) {
		if ((isalpha(str[i]) == FALSE) && (isdigit(str[i]) == FALSE))
			str[i] = ' ';
		}
}

// this is to fix a bug from a while ago. it can also
// eliminate random bad data from file corruptions
void kludge_correct_old_bad_data()
{
	short q,r;
	
	for (q = 0; q < ((editing_town) ? max_dim[town_type] : 48); q++) 
		for (r = 0; r < ((editing_town) ? max_dim[town_type] : 48); r++) {
			if (editing_town) {
				t_d.terrain[q][r] = minmax(0,511,t_d.terrain[q][r]);
				t_d.floor[q][r] = (unsigned char)minmax(0,255,t_d.floor[q][r]);
				}
				else {
					current_terrain.terrain[q][r] = minmax(0,511,current_terrain.terrain[q][r]);
					current_terrain.floor[q][r] = (unsigned char)minmax(0,255,current_terrain.floor[q][r]);
					}
			}	
}

// return nonzero if error
short FSRead(FILE *file_id, long *len, char *data)
{
	if (fread(data,*len,1,file_id) == 0)
		return 1;
	return 0;
}

short FSClose(FILE *file_id)
{
	fclose(file_id);
	return 0;
}
short FSWrite(FILE *file_id, long *len, char *data)
{
	if (fwrite(data,*len,1,file_id) == 0)
		return 1;
	return 0;
}

short SetFPos(FILE *file, short mode, long len)
{
	long error = 0; 

// long int old_pos = ftell(file);
	
	switch (mode) {
		case 1: error = fseek(file,len,SEEK_SET); break; // from begin
		case 2: error = fseek(file,len,SEEK_END); break; // from end
		case 3: error = fseek(file,len,SEEK_CUR); break; // from current
		}
// long int new_pos = ftell(file);

	//if (error == HFILE_ERROR)
	//	return -1;
	return 0;
}

/*
if (k ==
12)
	short dbug3 = 1;
				len = (long) (sizeof(town_record_type));
				error = FSRead(scen_f, &len, buffer);
				if (error != 0) {
					FSClose(scen_f); 
					FSClose(dummy_f);
					oops_error(67);
					}
				//port_dummy_town();
				if ((error = FSWrite(dummy_f, &len, buffer)) != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(68);return;}						
				switch (scenario.town_size[k]) {
					case 0: len = (long) ( sizeof(big_tr_type)); break;
					case 1: len = (long) ( sizeof(ave_tr_type)); break;
					case 2: len = (long) ( sizeof(tiny_tr_type)); break;
					}

town_record_type *dbug = (town_record_type *) buffer;
town_record_type dbug_town = *dbug;
dbug_town.port();

				error = FSRead(scen_f, &len, buffer);

				if (error != 0) {
					FSClose(scen_f); 
					FSClose(dummy_f);
					oops_error(69);
					}
				//port_dummy_t_d(scenario.town_size[k],buffer);
				if ((error = FSWrite(dummy_f, &len, buffer)) != 0) {FSClose(scen_f); FSClose(dummy_f);oops_error(70);return;}						

*/
