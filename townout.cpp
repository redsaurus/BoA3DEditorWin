
#include "stdafx.h"
#include "global.h"

// Global variables
short dialog_answer;

// external global variables
extern short cen_x, cen_y;
extern Boolean dialog_not_toast;
extern scenario_data_type scenario;
extern zone_names_data_type zone_names;
extern town_record_type town;
extern big_tr_type t_d;
extern outdoor_record_type current_terrain;
extern scen_item_data_type scen_data;
extern short cur_town;
extern location cur_out;
extern short current_drawing_mode;
extern short town_type ;
extern Boolean editing_town;
extern short overall_mode;
extern char *attitude_types[4];
extern  Boolean file_is_loaded;
extern short max_zone_dim[3];
// local variables
creature_start_type store_placed_monst;
short store_which_placed_monst;
short extra_dialog_answer[4];

short str_do_delete[16];
short a,b,c;
short store_which_out_wand;
short store_out_wand_mode;
out_wandering_type store_out_wand;

location store_cur_loc;
short store_horse_page,store_boat_page;
short store_which_dlog; // used for get_num
short store_which_str;
short store_dlog_min;
short store_dlog_max;
short store_which_part;
short store_max_x;
short store_max_y;

item_storage_shortcut_type store_storage;
short cur_shortcut;


// function prototype

void put_placed_monst_in_dlog();
Boolean get_placed_monst_in_dlog();
void put_placed_item_in_dlog();
Boolean get_placed_item_in_dlog();

void put_out_wand_in_dlog();
Boolean get_out_wand_in_dlog();
Boolean save_town_details();
void put_town_details_in_dlog();
Boolean save_town_wand();
void put_town_wand_in_dlog();

Boolean save_out_strs();
void put_out_strs_in_dlog();
Boolean save_town_strs();
void put_town_strs_in_dlog();
void put_item_placement_in_dlog();
Boolean save_item_placement();
void put_add_town_in_dlog();
Boolean save_add_town();
void put_make_scen_1_in_dlog();
short edit_make_scen_1(char *filename,char *title,short *start_on_surface);
void put_make_scen_2_in_dlog();
short edit_make_scen_2(short *val_array);
Boolean save_scen_details();
void put_scen_details_in_dlog();

extern in_town_on_ter_script_type copied_ter_script;
void put_placed_terrain_script_in_dlog();
Boolean get_placed_terrain_script_in_dlog();
short which_script;
short store_which_placed_script;
in_town_on_ter_script_type store_placed_script;

short which_item;
short store_which_placed_item;
item_type store_placed_item;
short which_town_group;
extern zone_names_data_type zone_names;

  		char *aaa;
char *BOACreatureTimeFlags[11] = {
"always here (unless town dead)","here at day d unless town dead",
"disappear at day d","here if event not done by day d",
"gone if event not done by day d","here if event happened",
"gone if event happened","here on day 0-2 of every 9 days",
"here on day 3-5 of every 9 days","here on day 6-8 of every 9 days",
"here if and only if town dead"};

void monst_replaceall()
{
	char temp_str[256];
	short i;
	
	cd_create_dialog_parent_num(843,0);
				CDSN(843,63,0);
				CDSN(843,64,0);

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(843,0);
}

void monst_replaceall_event_filter (short item_hit)
{
	short i;
	short dummy;
	short town_size = max_zone_dim[scenario.town_size[cur_town]];
	char str[14];

	switch (item_hit) {
		case 2:
		if (cre(CDGN(843,63),-1,255,"Find value error: Creature Type. |Town creatures must have a type number between 0 and 255 inclusive. | (-1 means no monster).","",843) == TRUE)
		break;
		if (cre(CDGN(843,64),-1,255,"Replace value error: Creature Type. |Town creatures must have a type number between 0 and 255 inclusive.  |(-1 means no monster).","",843) == TRUE)
		break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].number == CDGN(843,63))
					town.creatures[i].number = CDGN(843,64);
					}
		break;
		case 3:
					if (cre(CDGN(843,63),-1,499,"Find value error: Extra Item 1. |Town creatures must have extra item numbers between 0 and 499.","(-1 means no item.)",843) == TRUE)
					break;
					if (cre(CDGN(843,64),-1,499,"Replace value error: Extra Item 1. |Town creatures must have extra item numbers between 0 and 499.","(-1 means no item.)",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].extra_item == CDGN(843,63))
					town.creatures[i].extra_item = CDGN(843,64);
					}
			break;
		case 4:
					if (cre(CDGN(843,63),-1,499,"Find value error: Extra Item 2. |Town creatures must have extra item numbers between 0 and 499.","(-1 means no item.)",843) == TRUE)
					break;
					if (cre(CDGN(843,64),-1,499,"Replace value error: Extra Item 2. |Town creatures must have extra item numbers between 0 and 499.","(-1 means no item.)",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].extra_item_2 == CDGN(843,63))
					town.creatures[i].extra_item_2 = CDGN(843,64);
					}
			break;

		case 5:
				short l;
			CDGT(843,63,(char *) str);
			str[14] = 0;
			for (i = 0; i < 80; i++) {
				for (l = 0; l < 14; l++) {
				if (str[l] = town.creatures[i].char_script[l])
					CDGT(843,64,town.creatures[i].char_script);
					}
				}
			break;

		case 6:
					if (cre(CDGN(843,63),0,10,"Find value error: Creature Appearance Type. |Town creatures must have Creature Appearance Type numbers between 0 and 10.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,10,"Replace value error: Creature Appearance Type. |Town creatures must have Creature Appearance Type numbers between 0 and 10.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].time_flag == CDGN(843,63))
					town.creatures[i].time_flag = CDGN(843,64);
					}
			break;

		case 7:
					if (cre(CDGN(843,63),0,3,"Find value error: Facing. |Town creatures must have a facing number between 0 and 3.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,3,"Replace value error: Facing. |Town creatures must have a facing number between 0 and 3.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].facing == CDGN(843,63))
					town.creatures[i].facing = CDGN(843,64);
					}
			break;
		case 8:
					if (cre(CDGN(843,63),2,5,"Find value error: Start Attitude. |Town creatures must have an attitude number between 2 and 5.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),2,5,"Replace value error: Start Attitude. |Town creatures must have an attitude number between 2 and 5.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].start_attitude == CDGN(843,63))
					town.creatures[i].start_attitude = CDGN(843,64);
					}
			break;

		case 9:
					if (cre(CDGN(843,63),0,town_size,"Find value error: Start Loc X. |Town creatures must have a Start Loc X number between 0 and (Town width - 1).","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,town_size,"Replace value error: Start Loc X. |Town creatures must have a Start Loc X number between 0 and (Town width - 1).","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].start_loc.x == CDGN(843,63))
					town.creatures[i].start_loc.x = CDGN(843,64);
					}
			break;

		case 10:
					if (cre(CDGN(843,63),0,town_size,"Find value error: Start Loc Y. |Town creatures must have a Start Loc Y number between 0 and (Town width - 1).","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,town_size,"Replace value error: Start Loc Y. |Town creatures must have a Start Loc Y number between 0 and (Town width - 1).","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].start_loc.y == CDGN(843,63))
					town.creatures[i].start_loc.y = CDGN(843,64);
					}
			break;

		case 11:
					if (cre(CDGN(843,63),0,100,"Find value error: Extra Item Chance 1. |Town creatures must have a Extra Item Chance 1 number between 0 and 100.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,100,"Replace value error: Extra Item Chance 1. |Town creatures must have a Extra Item Chance 1 number between 0 and 100.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].extra_item_chance_1 == CDGN(843,63))
					town.creatures[i].extra_item_chance_1 = CDGN(843,64);
					}
			break;

		case 12:
					if (cre(CDGN(843,63),0,100,"Find value error: Extra Item Chance 2. |Town creatures must have a Extra Item Chance 2 number between 0 and 100.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,100,"Replace value error: Extra Item Chance 2. |Town creatures must have a Extra Item Chance 2 number between 0 and 100.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].extra_item_chance_2 == CDGN(843,63))
					town.creatures[i].extra_item_chance_2 = CDGN(843,64);
					}
			break;

		case 13:
					if (cre(CDGN(843,63),0,3999,"Find value error: Personality. |Town creatures must have a personality between 0 and 3,999.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,3999,"Replace value error: Personality. |Town creatures must have a personality between 0 and 3,999.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].personality == CDGN(843,63))
					town.creatures[i].personality = CDGN(843,64);
					}
			break;
		case 14:
					if (cre(CDGN(843,63),0,19999,"Find value error: Character Id. |Town creatures must have a Character Id between 0 and 19,999.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,19999,"Replace value error: Character Id. |Town creatures must have a Character Id between 0 and 19,999.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].character_id == CDGN(843,63))
					town.creatures[i].character_id = CDGN(843,64);
					}
			break;

		case 15:
					if (cre(CDGN(843,63),0,19,"Find value error: Hidden Class. |Town creatures must have hidden class numbers between 0 and 19.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,19,"Replace value error: Hidden Class. |Town creatures must have hidden class numbers between 0 and 19.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].hidden_class == CDGN(843,63))
					town.creatures[i].hidden_class = CDGN(843,64);
					}
			break;
			
		case 16:
					if (cre(CDGN(843,63),0,3,"Find value error: Act at Distance. |Town creatures must have numbers between 0 and 3 for this variable.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,3,"Replace value error: Act at Distance. |Town creatures must have numbers between 0 and 3 for this variable.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].act_at_distance == CDGN(843,63))
					town.creatures[i].act_at_distance = CDGN(843,64);
					}
			break;

		case 17:
					if (cre(CDGN(843,63),0,1,"Find value error: Unique NPC. |Town creatures must have numbers between 0 and 1 for this variable.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,1,"Replace value error: Unique NPC. |Town creatures must have numbers between 0 and 1 for this variable.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].unique_char == CDGN(843,63))
					town.creatures[i].unique_char = CDGN(843,64);
					}
			break;
		case 18:
					if (cre(CDGN(843,63),0,32767,"Find value error: Appear Day. |Town creatures must have numbers between 0 and 32,767 for this variable.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,32767,"Replace value error: Appear Day. |Town creatures must have numbers between 0 and 32,767 for this variable.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].creature_time == CDGN(843,63))
					town.creatures[i].creature_time = CDGN(843,64);
					}
			break;
		case 19:
					if (cre(CDGN(843,63),0,9,"Find value error: Linked Event. |Town creatures must have numbers between 0 and 9 for this variable.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),0,9,"Replace value error: Linked Event. |Town creatures must have numbers between 0 and 9 for this variable.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].attached_event == CDGN(843,63))
					town.creatures[i].attached_event = CDGN(843,64);
					}
			break;

		case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29:
					if (cre(CDGN(843,63),-32767,32767,"Find value error: Memory Cell #. |Town creatures must have numbers between -32,767 and +32,767 for this variable.","",843) == TRUE)
					break;
					if (cre(CDGN(843,64),-32767,32767,"Replace value error: Memory Cell #. |Town creatures must have numbers between -32,767 and +32,767 for this variable.","",843) == TRUE)
					break;
			for (i = 0; i < 80; i++) {
				if (town.creatures[i].memory_cells[item_hit - 20] == CDGN(843,63))
					town.creatures[i].memory_cells[item_hit - 20] = CDGN(843,64);
					}
			break;

		case 62:
			dialog_not_toast = FALSE;
		break;

		case 65: // Choose creature type: Find
			i = choose_text_res(-1,0,255,town.creatures[i].number,843,"Choose Which Creature Type: 0 - 255");
			if ((i >= 0) && (i < 256)) {
				CDSN(843,63,i);
				csit(843,75,scen_data.scen_creatures[i].name);
			}
			break;
		case 66: // Choose creature type: Replace
			i = choose_text_res(-1,0,255,town.creatures[i].number,843,"Choose Which Creature Type: 0 - 255");
			if ((i >= 0) && (i < 256)) {
				CDSN(843,64,i);
				csit(843,76,scen_data.scen_creatures[i].name);
			}
			break;
		case 67: case 69: // Choose item type: Find
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,town.creatures[i].extra_item,843,"Extra Item 1, which item: 0 - 499?");
			if ((i >= 0) && (i < NUM_SCEN_ITEMS)) {
				CDSN(843,63,i);
				csit(843,75,scen_data.scen_items[i].full_name);
			}
			break;
		case 68: case 70: // Choose item type: Replace
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,town.creatures[i].extra_item,843,"Extra Item 1, which item: 0 - 499?");
			if ((i >= 0) && (i < NUM_SCEN_ITEMS)) {
				CDSN(843,64,i);
				csit(843,76,scen_data.scen_items[i].full_name);
			}
			break;

		case 71: // Choose name of a char script already placed in this town: Find
			i = choose_text_res(-10,0,79,0,843,"Which Character Script Name?");
		 	CDST(843,63,town.creatures[i].char_script);
			break;

		case 72: // Choose name of a char script already placed in this town: Replace
			i = choose_text_res(-10,0,79,0,843,"Which Character Script Name?");
		 	CDST(843,64,town.creatures[i].char_script);
			break;


		case 73: //  // Choose time type: Find
			i = choose_text_res(4,1,11,town.creatures[i].time_flag,843,"Choose Time Type:");
			if ((i > 0) && (i < 12)) {
				CDSN(843,63,i - 1);
				csit(843,75,BOACreatureTimeFlags[i - 1]);
			}
			break;
		case 74: //  // Choose time type: Replace
			i = choose_text_res(4,1,11,town.creatures[i].time_flag,843,"Choose Time Type:");
			if ((i > 0) && (i < 12)) {
				CDSN(843,64,i - 1);
				csit(843,76,BOACreatureTimeFlags[i - 1]);
			}
			break;

		case 77: // Update creature type names
				i = CDGN(843,63);
				csit(843,75,scen_data.scen_creatures[i].name);
				i = CDGN(843,64);
				csit(843,76,scen_data.scen_creatures[i].name);
			break;
		case 78: case 79: // Update extra item 1 and extra item 2 names
				i = CDGN(843,63);
				csit(843,75,scen_data.scen_items[i].full_name);
				i = CDGN(843,64);
				csit(843,76,scen_data.scen_items[i].full_name);
			break;

		case 80: // Update creature script names
			i = CDGN(843,63);
		 	CDST(843,75,town.creatures[i].char_script);
			i = CDGN(843,64);
		 	CDST(843,76,town.creatures[i].char_script);
			break;

		case 81: // Update creature appearance type names
				i = CDGN(843,63);
				csit(843,75,BOACreatureTimeFlags[i]);
				i = CDGN(843,64);
				csit(843,76,BOACreatureTimeFlags[i]);
			break;

		default:
		break;

		}
}


void edit_placed_item(short which_item)
{
	short i;

	store_placed_item = town.preset_items[which_item];
	store_which_placed_item = which_item;

	cd_create_dialog_parent_num(842,0);
	cdsin(842,4,store_which_placed_item);
	put_placed_item_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(842,0);

	set_string("Select/edit placed object","Select object to edit");
	set_tool(40);
}

void put_placed_item_in_dlog()
{
	char str[256];
	short i;

	cdsin(842,4,store_which_placed_item);
 	csit(842,5,scen_data.scen_items[store_placed_item.which_item].full_name);
 	CDSN(842,15,store_placed_item.item_shift.x);
 	CDSN(842,16,store_placed_item.item_shift.y);
 	CDSN(842,17,store_placed_item.charges);
			if (store_placed_item.properties & 1)
				 cd_set_led(842,18,1);
			else
				cd_set_led(842,18,0);
			if (store_placed_item.properties & 2)
				 cd_set_led(842,19,1);
			else
				cd_set_led(842,19,0);
			if (store_placed_item.properties & 4)
				 cd_set_led(842,20,1);
			else
				cd_set_led(842,20,0);
			if (store_placed_item.properties & 8)
				 cd_set_led(842,21,1);
			else
				cd_set_led(842,21,0);
			if (store_placed_item.properties & 16)
				 cd_set_led(842,22,1);
			else
				cd_set_led(842,22,0);
 	cdsin(842,29,store_placed_item.item_loc.x);
 	cdsin(842,31,store_placed_item.item_loc.y);
 	CDSN(842,35,store_placed_item.which_item);
 	cdsin(842,38,store_placed_item.properties);
 	CDSN(842,45,store_which_placed_item);

}

void edit_placed_item_event_filter (short item_hit)
{
	short i;

	switch (item_hit) {

		case 23:
			if (store_which_placed_item == 0)
			   	store_which_placed_item = 143;
			else
				store_which_placed_item--;

				store_placed_item = town.preset_items[store_which_placed_item];
				put_placed_item_in_dlog();
			break;
		case 24:
			if (store_which_placed_item == 143)
			   	store_which_placed_item = 0;
			else
				store_which_placed_item++;

				store_placed_item = town.preset_items[store_which_placed_item];
				put_placed_item_in_dlog();
			break;
		case 25: // Save: save this item record, but don't exit the dialog.
			if (get_placed_item_in_dlog() == FALSE) {
					set_string("Item Save attempt failed","");
				break;
				}
				else
					set_string("Item Record Saved.","");
			break;
		case 26: // Cancel
			dialog_not_toast = FALSE;
			break;
		case 27:  // OK: save and exit.
			if (get_placed_item_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 34: // Delete this item
			store_placed_item.clear_item_type();
			put_placed_item_in_dlog();
			break;

		case 37:
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,store_placed_item.which_item,842,"Which item?");
			if (i >= 0)
			store_placed_item.which_item = i;
			put_placed_item_in_dlog();
			break;

		case 46:
			if (store_which_placed_item < 0)
			   	store_which_placed_item = 0;
			if (store_which_placed_item > 143)
			   	store_which_placed_item = 143;
			else
				store_which_placed_item = CDGN(842,45);

				store_placed_item = town.preset_items[store_which_placed_item];
				put_placed_item_in_dlog();
		break;


		default:
			cd_flip_led(842,18,item_hit);
			cd_flip_led(842,19,item_hit);
			cd_flip_led(842,20,item_hit);
			cd_flip_led(842,21,item_hit);
			cd_flip_led(842,22,item_hit);
		break;
		
		}
}

Boolean get_placed_item_in_dlog()
{
	char str[256];
	short i;
	short item_identified = cd_get_led(842,18);
	short item_property = cd_get_led(842,19);
	short item_contained = cd_get_led(842,20);
	short item_cursed = cd_get_led(842,21);
	short item_onceperday = cd_get_led(842,22);

 	store_placed_item.item_shift.x = CDGN(842,15);
 	store_placed_item.item_shift.y = CDGN(842,16);
 	store_placed_item.charges = CDGN(842,17);
 	store_placed_item.properties=
	item_identified +
	2 * item_property +
	4 * item_contained +
	8 * item_cursed +
	16 * item_onceperday;

 	store_placed_item.which_item = CDGN(842,35);

	town.preset_items[store_which_placed_item] = store_placed_item;
	return TRUE;
}

void edit_placed_script(short which_script)
{
	short i;

	store_placed_script = town.ter_scripts[which_script];
	store_which_placed_script = which_script;

	cd_create_dialog_parent_num(838,0);
	cdsin(838,4,store_which_placed_script);
	put_placed_terrain_script_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(838,0);

	set_string("Select/edit placed object","Select object to edit");
	set_tool(40);
}

void put_placed_terrain_script_in_dlog()
{
	char str[256];
	short i;

	cdsin(838,4,store_which_placed_script);
 	CDST(838,6,store_placed_script.script_name);
 	for (i = 0; i < 10; i++)
 	CDSN(838,18 + i,store_placed_script.memory_cells[i]);
 	cdsin(838,34,store_placed_script.exists);
 	cdsin(838,36,store_placed_script.loc.x);
 	cdsin(838,38,store_placed_script.loc.y);

}

void edit_placed_script_event_filter (short item_hit)
{
	short i;

	switch (item_hit) {
		case 28:
			if (store_which_placed_script == 0)
			   	store_which_placed_script = 99;
			else
				store_which_placed_script--;

				store_placed_script = town.ter_scripts[store_which_placed_script];
				put_placed_terrain_script_in_dlog();
			break;
		case 29:
			if (store_which_placed_script == 99)
			   	store_which_placed_script = 0;
			else
				store_which_placed_script++;

				store_placed_script = town.ter_scripts[store_which_placed_script];
				put_placed_terrain_script_in_dlog();
			break;
		case 30: // Save: save this script record, but don't exit the dialog.
			if (get_placed_terrain_script_in_dlog() == FALSE) {
					set_string("Save script attempt failed","");
				break;
				}
				else
					set_string("Script save succeeded.","");
			break;
		case 31: // Cancel
			dialog_not_toast = FALSE;
			break;
		case 32:  // OK: save and exit.
			if (get_placed_terrain_script_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 41: // Delete this script
				store_placed_script.clear_in_town_on_ter_script_type();
				put_placed_terrain_script_in_dlog();
			break;
		case 42: // Show a menu of all scripts already placed in this town.
			i = choose_text_res(-9,0,99,0,838,"Which Terrain Script Name?");
		 	CDST(838,6,town.ter_scripts[i].script_name);
		break;

		default:
		break;
		}
}

Boolean get_placed_terrain_script_in_dlog()
{
	char str[256];
	short i;

	CDGT(838,6,str);
	if (string_not_clean(str,SCRIPT_NAME_LEN,1,"This terrain script",838))
		return FALSE;
 	CDGT(838,6,store_placed_script.script_name);

 	for (i = 0; i < 10; i++)
	 	store_placed_script.memory_cells[i] = CDGN(838,18 + i);

	town.ter_scripts[store_which_placed_script] = store_placed_script;
	return TRUE;
}

void edit_placed_monst(short which_m)
{

	short i;

	store_placed_monst = town.creatures[which_m];
	store_which_placed_monst = which_m;
	cd_create_dialog_parent_num(837,0);
	cdsin(837,37,which_m + 6);
	put_placed_monst_in_dlog();
	for (i = 0; i < 4; i++)
		cd_add_label(837,27 + i,attitude_types[i],57);
	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(837,0);

	set_string("Select/edit placed object","Select object to edit");
	set_tool(40);

}

void put_placed_monst_in_dlog()
{
	char str[256];
	short i;

 	CDST(837,2,store_placed_monst.char_script);
 	CDSN(837,3,store_placed_monst.extra_item_chance_1);
 	CDSN(837,4,store_placed_monst.extra_item);
 	CDSN(837,5,store_placed_monst.extra_item_chance_2);
 	CDSN(837,6,store_placed_monst.extra_item_2);
 	CDSN(837,7,store_placed_monst.personality);
 	CDSN(837,8,store_placed_monst.character_id);
 	CDSN(837,9,store_placed_monst.hidden_class);
 	CDSN(837,10,store_placed_monst.act_at_distance);
 	CDSN(837,11,store_placed_monst.unique_char);
	CDSN(837,12,store_placed_monst.number);
 	CDSN(837,13,store_placed_monst.creature_time);
 	CDSN(837,14,store_placed_monst.attached_event);

 	for (i = 0; i < 10; i++)
	 	CDSN(837,15 + i,store_placed_monst.memory_cells[i]);
 	cd_set_led_range(837,27,30,store_placed_monst.start_attitude - 2);
	cdsin(837,37,store_which_placed_monst);
	csit(837,38,scen_data.scen_creatures[store_placed_monst.number].name);
 	get_str(str,4,store_placed_monst.time_flag + 1);
 	csit(837,39,(char *) str);
	csit(837,51,scen_data.scen_items[store_placed_monst.extra_item].full_name);
	csit(837,52,scen_data.scen_items[store_placed_monst.extra_item_2].full_name);
}

void edit_placed_monst_event_filter (short item_hit)
{
	short i;

	switch (item_hit) {
		case 31:
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 32:
			dialog_not_toast = FALSE;
			break;
		case 33: // choose m type
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_text_res(-1,0,255,store_placed_monst.number,837,"Choose Which Creature Type: 0 - 255");
			if ((i >= 0) && (i < 256)) {
				store_placed_monst.number = i;
				put_placed_monst_in_dlog();
				}
			break;

		case 34:
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,store_placed_monst.extra_item,837,"Which item?");
	if (i >= 0)
				store_placed_monst.extra_item = i;
			put_placed_monst_in_dlog();
			break;

		case 35:
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,store_placed_monst.extra_item_2,837,"Which item?");
			if (i >= 0)
				store_placed_monst.extra_item_2 = i;
			put_placed_monst_in_dlog();
			break;

		case 36: // choose time type
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_text_res(4,1,11,store_placed_monst.time_flag + 1,837,"Choose Time Type:");
			if (i >= 0) {
				store_placed_monst.time_flag = i - 1;
				put_placed_monst_in_dlog();
				}
			break;
		case 69:
			if (store_which_placed_monst == 79)
	   			store_which_placed_monst = 0;
			else
					store_which_placed_monst++;
					
					store_placed_monst = town.creatures[store_which_placed_monst];
					put_placed_monst_in_dlog();
		break;

		case 70:
			if (store_which_placed_monst == 0)
	   			store_which_placed_monst = 79;
			else
					store_which_placed_monst--;
					
					store_placed_monst = town.creatures[store_which_placed_monst];
					put_placed_monst_in_dlog();
			break;

		case 71: // delete this record
			store_placed_monst.clear_creature_start_type();
					put_placed_monst_in_dlog();
			break;

		case 72: // Save: save this Creature record, but don't exit the dialog.
			if (get_placed_monst_in_dlog() == FALSE) {
					set_string("Save Creature attempt failed","");
				break;
				}
				else
					set_string("Creature save succeeded.","");
			break;

		case 73: // Update display of names
				csit(837,38,scen_data.scen_creatures[CDGN(837,12)].name);
				csit(837,51,scen_data.scen_items[CDGN(837,4)].full_name);
	 			csit(837,52,scen_data.scen_items[CDGN(837,5)].full_name);
			break;

		case 74: // Choose name of a char script already placed in this town.
			i = choose_text_res(-10,0,79,0,837,"Which Character Script Name?");
		 	CDST(837,2,town.creatures[i].char_script);
			break;

		default:
		break;
		}
}


Boolean get_placed_monst_in_dlog()
{
	char str[256];
	short i;

	// check for errors
	CDGT(837,2,str);
	if (string_not_clean(str,SCRIPT_NAME_LEN,1,"This creature's script",837))
		return FALSE;

 	CDGT(837,2,store_placed_monst.char_script);
 	store_placed_monst.extra_item_chance_1 = CDGN(837,3);
 	store_placed_monst.extra_item = CDGN(837,4);
	store_placed_monst.extra_item_chance_2 = CDGN(837,5);
 	store_placed_monst.extra_item_2 = CDGN(837,6);
 	store_placed_monst.personality = CDGN(837,7);
 	store_placed_monst.character_id = CDGN(837,8);
 	store_placed_monst.hidden_class = CDGN(837,9);
 	store_placed_monst.act_at_distance = CDGN(837,10);
 	store_placed_monst.unique_char = CDGN(837,11);
 	store_placed_monst.creature_time = CDGN(837,13);
 	store_placed_monst.attached_event = CDGN(837,14);
 	for (i = 0; i < 10; i++)
	 	store_placed_monst.memory_cells[i] = CDGN(837,15 + i);
 	store_placed_monst.start_attitude = cd_get_led_range(837,27,30) + 2;
	town.creatures[store_which_placed_monst] = store_placed_monst;
	return TRUE;
}


void edit_sign(short which_sign)
{
	char new_text[256];
	
	if (editing_town)
		get_str_dlog(town.sign_text[which_sign],"What should this sign say?",new_text);
		else get_str_dlog(current_terrain.sign_text[which_sign],"What should this sign say?",new_text);

	if (editing_town)
		strcpy(town.sign_text[which_sign],new_text);
		else strcpy(current_terrain.sign_text[which_sign],new_text);


}

void get_a_number_event_filter (short item_hit)
{

	 short x;
	 char *str;

	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_dlog,2);
				if (store_which_dlog == 855) {
				if (cre(dialog_answer,0,scenario.num_towns - 1,"Town number must be from 0 to (Number of scenario towns - 1).","",857) == TRUE)
				 break;
				}
			dialog_answer = minmax(store_dlog_min,store_dlog_max,dialog_answer);
			dialog_not_toast = FALSE;
			break;
		case 4:
			dialog_answer = -1;
			dialog_not_toast = FALSE;
			break;
		case 7:
				if ((store_which_dlog != 855) && (store_which_dlog != 856))
				break;
				else {
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(store_which_dlog,2),store_which_dlog,"Which Town?");
				if (x >= 0) {
				CDSN(store_which_dlog,2,x);
				csit(store_which_dlog,9,zone_names.town_names[x]);
				}
			}
			break;
		case 10:
				if ((store_which_dlog != 855) && (store_which_dlog != 856))
				break;
				else {
					x = CDGN(store_which_dlog,2);
					csit(store_which_dlog,9,zone_names.town_names[x]);
				}
			break;

		}
}

// which_dlog is the number of a dialog box that takes a number in text edit field 2
// gets a number and returns it. returns -1 if canceled
short get_a_number(short which_dlog,short default_value,short min,short max)
{
	char temp_str[256],str2[256];
	short x;
	
	 store_which_dlog = which_dlog;
	store_dlog_min = min;
	store_dlog_max = max;
	cd_create_dialog_parent_num(store_which_dlog,0);
	
	CDSN(store_which_dlog,2,default_value);

	if ((which_dlog == 855) || (which_dlog == 856) ){
		cd_get_item_text(which_dlog,6,(char *) temp_str);
		sprintf((char *) str2,"%s (0 - %d)",(char *) temp_str, (int)(scenario.num_towns - 1));
		csit(which_dlog,6,(char *) str2);
		x = CDGN(store_which_dlog,2);
		csit(store_which_dlog,9,zone_names.town_names[x]);
		}		

	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(store_which_dlog,0);
	return dialog_answer;
}

void change_ter_event_filter (short item_hit)
{
	short i;
	
			a = CDGN(857,2);
			b = CDGN(857,3);
			c = CDGN(857,4);

	switch (item_hit) {
		case 5:
			if (current_drawing_mode == 0) {
				if (cre(a,0,255,"Both floor types must be from 0 to 255.","",857) == TRUE) break;
				if (cre(b,0,255,"Both floor types must be from 0 to 255.","",857) == TRUE) break;
				}
				else {
					if (cre(a,0,511,"Both terrain types must be from 0 to 511.","",857) == TRUE) break;
					if (cre(b,0,511,"Both terrain types must be from 0 to 511.","",857) == TRUE) break;
					}
			if (cre(c,0,100,"The Chance must be from 0 to 100.","",857) == TRUE) break;

			 dialog_not_toast = FALSE; 
			break;
		case 6:
			dialog_not_toast = FALSE;
			break;
		case 9: case 10:
			i = CDGN(857,item_hit - 7);
			if (current_drawing_mode == 0) {
				i = choose_text_res(-6,0,255,i,857,"Which Floor? (0:255)");
				csit(857,item_hit + 5,scen_data.scen_floors[i].floor_name);
				}
				else {
				i = choose_text_res(-4,0,511,i,857,"Which Terrain? (0:511)");
				csit(857,item_hit + 5,scen_data.scen_ter_types[i].ter_name);
				}
			if (i >= 0)
				CDSN(857,item_hit - 7,i);
			break;
		}
}

void change_ter(short *change_from,short *change_to,short *chance)
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(857,0);

	CDSN(857,2,0);
	CDSN(857,3,0);
	CDSN(857,4,100);

			if (current_drawing_mode == 0) {
			csit(857,7,"Change Floor Randomly");
			csit(857,8,"Select a floor type and another floor type to turn it into. |Also give a percentage chance of changing it (0 - 100). |Automatic corrections (like adjusting cave walls) will not be made to these changes.");
			csit(857,11,"Change this floor type:");
			csit(857,12,"To this floor type:");
			}
			else {
			csit(857,7,"Change Terrain Randomly");
			csit(857,8,"Select a terrain type and another terrain type to turn it into. |Also give a percentage chance of changing it (0 - 100). |Automatic corrections (like adjusting cave walls) will not be made to these changes.");
			csit(857,11,"Change this terrain type:");
			csit(857,12,"To this terrain type:");
			}

	while (dialog_not_toast)
		ModalDialog();

	*change_from = a;
	*change_to = b;
	*chance = c;

	cd_kill_dialog(857,0);
}

void outdoor_details_event_filter (short item_hit)
{
	char str[256];
	
	switch (item_hit) {
		case 10:
			CDGT(851,3,(char *) str);
			if (string_not_clean((char *) str,SCRIPT_NAME_LEN,1,"The outdoor section script name",851))
				break;

			CDGT(851,2,(char *) str);
			str[19] = 0;
			sprintf(current_terrain.name,"%s",(char *) str);
			strcpy(&zone_names.section_names[zone_names.out_width * cur_out.y + cur_out.x][0],&current_terrain.name[0]);
			CDGT(851,3,(char *) str);
			str[SCRIPT_NAME_LEN - 1] = 0;
			sprintf(current_terrain.section_script,"%s",(char *) str);

			current_terrain.extra[0] = CDGN(851,4);

			current_terrain.is_on_surface = cd_get_led(851,8);
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(851,8,item_hit);
			break;
		}
}

void outdoor_details()
// ignore parent in Mac version
{
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(851,0);
	
	CDST(851,2,current_terrain.name);
	CDST(851,3,current_terrain.section_script);
	CDSN(851,4,current_terrain.extra[0]);
	sprintf((char *) temp_str,"X = %d, Y = %d", (int)cur_out.x, (int)cur_out.y);
	csit(851,7,(char *) temp_str);
	cd_set_led(851,8,current_terrain.is_on_surface);

	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(851,0);
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode)
{
	store_which_out_wand = 0;
	store_out_wand_mode = mode;
	switch (store_out_wand_mode) {
		case 0: store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
		case 1: store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
		case 2: store_out_wand = current_terrain.preset[store_which_out_wand];  break;
		}

	cd_create_dialog_parent_num(852,0);

	if (mode == 1)
		csit(852,53,"Outdoor Special Encounter:");
	if (mode == 2)
		csit(852,53,"Outdoor Preplaced Encounter:");

	put_out_wand_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(852,0);

}

void put_out_wand_in_dlog()
{
	char str[256];
	short i;
	
	cdsin(852,41,store_which_out_wand);
	for (i = 0; i < 4; i++) {
		if (store_out_wand.hostile[i] <= 0)
			csit(852, i + 31, "Empty");
			else {
				get_str(str,-1,store_out_wand.hostile[i]);
				csit(852,i + 31,(char *) str);
				}
		CDSN(852,2 + i,store_out_wand.hostile_amount[i]);
		}
	for (i = 0; i < 3; i++) {
		if (store_out_wand.friendly[i] <= 0)
			csit(852, i + 35, "Empty");
			else {
				get_str(str,-1,store_out_wand.friendly[i]);
				csit(852,i + 35,(char *) str);
				}
		CDSN(852,6 + i,store_out_wand.friendly_amount[i]);
		}
	cd_set_led(852,38,store_out_wand.cant_flee); 
	cd_set_led(852,39,store_out_wand.forced); 
	cd_set_led(852,40,store_out_wand.check_every_turn); 
	CDSN(852,12,store_out_wand.end_spec1);
	CDSN(852,13,store_out_wand.end_spec2);
	CDSN(852,14,store_out_wand.killed_flag1);
	CDSN(852,15,store_out_wand.killed_flag2);
	CDSN(852,16,store_out_wand.move_type);
	CDSN(852,17,store_out_wand.start_loc.x);
	CDSN(852,18,store_out_wand.start_loc.y);
	CDSN(852,19,store_out_wand.random_move_chance);
	CDSN(852,9,store_out_wand.start_state_when_encountered);
	CDSN(852,10,store_out_wand.start_state_when_defeated);
	CDSN(852,11,store_out_wand.start_state_when_fled);
}

void edit_out_wand_event_filter (short item_hit)
{
	short i;
	creature_start_type store_m;

	switch (item_hit) {
		case 21:
			if (get_out_wand_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 20:
			dialog_not_toast = FALSE;
			break;
		case 22:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand--;

			switch (store_out_wand_mode) {
				case 0:
					if (store_which_out_wand < 0) store_which_out_wand = 3;
					store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
				case 1:
					if (store_which_out_wand < 0) store_which_out_wand = 3;
					store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
				case 2:
					if (store_which_out_wand < 0) store_which_out_wand = 7;
					store_out_wand = current_terrain.preset[store_which_out_wand];  break;
				}
			put_out_wand_in_dlog();
			break;
		case 23:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand++;
			switch (store_out_wand_mode) {
				case 0:
					if (store_which_out_wand > 3) store_which_out_wand = 0;
					store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
				case 1:
					if (store_which_out_wand > 3) store_which_out_wand = 0;
					store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
				case 2:
					if (store_which_out_wand > 7) store_which_out_wand = 0;
					store_out_wand = current_terrain.preset[store_which_out_wand];  break;
				}
			put_out_wand_in_dlog();
			break;

		case 67:
			store_out_wand.clear_out_wandering_type();
			put_out_wand_in_dlog();
			break;

		default:
			if (get_out_wand_in_dlog() == FALSE)
				 break;
			cd_flip_led(852,38,item_hit);
			cd_flip_led(852,39,item_hit);
			cd_flip_led(852,40,item_hit);
			if ((item_hit >= 24) && (item_hit <= 27)) {
				i = choose_text_res(-1,0,255,store_out_wand.hostile[item_hit - 24],852,"Choose Which Creature:");
				if (i >= 0) store_out_wand.hostile[item_hit - 24] = i;
				put_out_wand_in_dlog();
				}
			if ((item_hit >= 28) && (item_hit <= 30)) {
				i = choose_text_res(-1,0,255,store_out_wand.friendly[item_hit - 28],852,"Choose Which Creature:");
				if (i >= 0) store_out_wand.friendly[item_hit - 28] = i;
				put_out_wand_in_dlog();
				}
			break;
		}
}

Boolean get_out_wand_in_dlog()
{
	short i;
	
	store_out_wand.cant_flee = cd_get_led(852,38); 
	store_out_wand.forced = cd_get_led(852,39); 
	store_out_wand.check_every_turn = cd_get_led(852,40); 

	for (i = 0; i < 4; i++) {
		store_out_wand.hostile_amount[i] = CDGN(852,2 + i);
		}
	for (i = 0; i < 3; i++) {
		store_out_wand.friendly_amount[i] = CDGN(852,6 + i);
		}
	
	store_out_wand.end_spec1 = CDGN(852,12);
	store_out_wand.end_spec2 = CDGN(852,13);
	store_out_wand.killed_flag1 = CDGN(852,14);
	store_out_wand.killed_flag2 = CDGN(852,15);
	if (cre(store_out_wand.end_spec1,
		-1,SDF_RANGE_X - 1,"First part of Stuff Done flags must be from 0 to 299 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.end_spec2,
		-1,SDF_RANGE_Y - 1,"Second part of Stuff Done flags must be from 0 to 29 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.killed_flag2,
		-1,SDF_RANGE_X - 1,"First part of Stuff Done flags must be from 0 to 299 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.killed_flag2,
		-1,SDF_RANGE_Y - 1,"Second part of Stuff Done flags must be from 0 to 29 (or -1 if it's not used).","",852) == TRUE) return FALSE;

	store_out_wand.move_type = CDGN(852,16);
	store_out_wand.start_loc.x = (t_coord)CDGN(852,17);
	store_out_wand.start_loc.y = (t_coord)CDGN(852,18);
	if (cre(store_out_wand.start_loc.x,
		-1,47,"All locations in outdoors must be in the 0..47 range (or -1 if encounter unused or placed by special script)","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.start_loc.y,
		-1,47,"All locations in outdoors must be in the 0..47 range (or -1 if encounter unused or placed by special script)","",852) == TRUE) return FALSE;

	store_out_wand.random_move_chance = CDGN(852,19);
	store_out_wand.start_state_when_encountered = CDGN(852,9);
	store_out_wand.start_state_when_defeated = CDGN(852,10);
	store_out_wand.start_state_when_fled = CDGN(852,11);

	switch (store_out_wand_mode) {
		case 0: current_terrain.wandering[store_which_out_wand] = store_out_wand; break;
		case 1: current_terrain.special_enc[store_which_out_wand] = store_out_wand;  break;
		case 2: current_terrain.preset[store_which_out_wand] = store_out_wand;  break;
		}
	return TRUE;
}

void edit_town_details()
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(832,0);

	put_town_details_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(832,0);
}

void put_town_details_in_dlog()
{
	short i;

	CDST(832,2,town.town_name);
	cd_set_led_range(832,33,36,town.lighting);
	cd_set_led(832,32,town.is_on_surface);
	CDSN(832,3,town.monster_respawn_chance);
	CDSN(832,4,town.wall_1_sheet);
	CDSN(832,5,town.wall_1_height);
	CDSN(832,6,town.wall_2_sheet);
	CDSN(832,7,town.wall_2_height);
	CDSN(832,8,town.cliff_sheet);
	CDST(832,9,town.town_script);
	CDSN(832,10,town.beam_type);
	CDSN(832,11,town.environmental_sound);
	CDSN(832,12,town.town_kill_day);
	CDSN(832,13,town.town_linked_event);
	CDSN(832,14,scenario.town_starts_hidden[cur_town]);
	CDSN(832,17,town.external_floor_type);
	for (i = 0; i < 4; i++) {
		CDSN(832,18 + i * 2,town.exit_locs[i].x);
		CDSN(832,19 + i * 2,town.exit_locs[i].y);
		CDSN(832,26 + i,town.exit_specs[i]);
		}		
}

void edit_town_details_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 30:
			if (save_town_details() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 31:
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(832,33,36,item_hit);
			cd_flip_led(832,32,item_hit);
			break;
		}
}

Boolean save_town_details()
{
	char str[256];
	short i;
	short dummy;

	// Check for errors before saving anything
	dummy = CDGN(832,10);
	if (cre(dummy,0,2,"The beam type must be 0 (no beams), 1 (damage beams), or 2 (impassable beams).","",832) == TRUE) return FALSE;
	dummy = CDGN(832,11);
	if (cre(dummy,-1,200,"The background sound must be -1 (no sound) or between 0 and 200.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,12);
	if (cre(dummy,-1,10000,"The town death day must be -1 (never dies) or between 0 and 10000.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,13);
	if (cre(dummy,-1,9,"The town death prevention event must be -1 (never dies) or between 0 and 9.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,17);
	if (cre(dummy,-1,255,"The external floor type must be -1 (uses edge terrain) or between 0 and 255.","",832) == TRUE) return FALSE;
	for (i = 0; i < 8; i++) {
		dummy = CDGN(832,18 + i);
		if (cre(dummy,-1,47,"The outdoor exit coordinates must all be -1 (game determines best location) or between 0 and 47.","",832) == TRUE) return FALSE;
		}
	CDGT(832,2,str);
	if (string_not_clean(str,20,0,"The name of the town",832))
		return FALSE;
	CDGT(832,9,str);
	if (string_not_clean(str,SCRIPT_NAME_LEN,1,"The town script",832))
		return FALSE;

	// save values
	CDGT(832,2,town.town_name);
	strcpy(&zone_names.town_names[cur_town][0],&town.town_name[0]);
	town.lighting = cd_get_led_range(832,33,36);
	town.is_on_surface = cd_get_led(832,32);
	town.monster_respawn_chance = CDGN(832,3);
	town.wall_1_sheet = CDGN(832,4);
	town.wall_1_height = CDGN(832,5);
	town.wall_2_sheet = CDGN(832,6);
	town.wall_2_height = CDGN(832,7);
	town.cliff_sheet = CDGN(832,8);
	CDGT(832,9,town.town_script);
	town.beam_type = CDGN(832,10);
	town.environmental_sound = CDGN(832,11);
	town.town_kill_day = CDGN(832,12);
	town.town_linked_event = CDGN(832,13);
	scenario.town_starts_hidden[cur_town] = (CDGN(832,14) == 0) ? 0 : 1;
	town.external_floor_type = CDGN(832,17);
	for (i = 0; i < 4; i++) {
		town.exit_locs[i].x = (t_coord)CDGN(832,18 + i * 2);
		town.exit_locs[i].y = (t_coord)CDGN(832,19 + i * 2);
		town.exit_specs[i] = CDGN(832,26 + i);
		}


	return TRUE;
}

void edit_town_wand()
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(835,0);

	put_town_wand_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(835,0);
}

void put_town_wand_in_dlog()
{
	short i,j,x;

	for (i = 0; i < 4; i++)
	for (j = 0; j < 6; j++)
		{
		CDSN(835,2 + i * 6 + j,town.respawn_monsts[i][j]);
		}

	for (j = 0; j < 6; j++)
		CDSN(835,j + 49,town.respawn_monsts[0][j]);
	for (x = 2; x < 8; x++) {
		csit(835,x + 59,scen_data.scen_creatures[CDGN(835,x)].name);
		cdsin(835,x + 74,scen_data.scen_creatures[CDGN(835,x)].level);
		}
}

void edit_town_wand_event_filter (short item_hit)
{
	short i,j,x,y;

	switch (item_hit) {
		case 38:
			if (save_town_wand() == TRUE)
				 dialog_not_toast = FALSE;
			break;
		case 39: case 40: case 41: case 42:
			for (i = 0; i < 6; i++) {
				j = choose_text_res(-1,0,255,town.respawn_monsts[item_hit - 39][i],835,"Choose Which Monster: 0 - 255");
				if (j <= 0)
					j = -1;
				if (j < 0)
					i = 7;
					else {
						CDSN(835,2 + (item_hit - 39) * 6 + i,j);
						}
				}
			break;

		case 55: case 56: case 57: case 58: case 59: case 60:
				j = choose_text_res(-1,0,255,town.respawn_monsts[which_town_group][item_hit - 55],835,"Choose Which Monster: 0 - 255");
			if (j >= 0) {
				CDSN(835,(item_hit - 6),j);
				csit(835,item_hit + 6,scen_data.scen_creatures[CDGN(835,item_hit - 6)].name);
				cdsin(835,item_hit + 21,scen_data.scen_creatures[CDGN(835,item_hit - 6)].level);
				}
			break;

		case 67: case 68: case 69: case 70:
				which_town_group = item_hit - 67;
				for (j = 0; j < 6; j++) {
					CDSN(835,j + 49,town.respawn_monsts[which_town_group][j]);
					csit(835,j + 61,scen_data.scen_creatures[CDGN(835,j + 49)].name);
					cdsin(835,j + 76,scen_data.scen_creatures[CDGN(835,j + 49)].level);
			}
		break;

		case 71: // Delete
			for (x = 0; x < 6; x++) {
				CDSN(835,x + 49,-1);
				csit(835,x + 61,"Empty");
				CDSN(835,x + 76,1);
				}
			break;
		case 72: // Name
			for (x = 61; x < 67; x++) {
					csit(835,x,scen_data.scen_creatures[CDGN(835,x - 12)].name);
					cdsin(835,x + 15,scen_data.scen_creatures[CDGN(835,x - 12)].level);
				}
			break;
	case 73: // Save
			for (x = 2; x < 8; x++) {
					short y = CDGN(835,x + 47);
					CDSN(835,which_town_group * 6 + x,y);
					}
			break;

		default:
			break;

		}
}

Boolean save_town_wand()
{
	short i,j;

	for (i = 2; i < 26; i++) {
	if (cre(CDGN(835,i),-1,255,"Creature numbers must be greater than or equal to 0 and less than or equal to 255.","(-1 means no monster exists for that slot.)",835) == TRUE)
	return FALSE;
	}

	for (i = 0; i < 4; i++)
	for (j = 0; j < 6; j++)
		{
		town.respawn_monsts[i][j] = CDGN(835,2 + i * 6 + j);
		}

	return TRUE;
}

// Needs a name button
void pick_out_event_filter (short item_hit)
{
	char temp_str[256];
	short i,j,n;
	
	switch (item_hit) {
		case 2:
			dialog_answer = store_cur_loc.x * 100 + store_cur_loc.y;
			 dialog_not_toast = FALSE; 
			break;
		case 3:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;

		case 12:
			if (store_cur_loc.x == 0) EdSysBeep( );  // (20)
				else {
					store_cur_loc.x--;
					i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
					csit(854,16,zone_names.section_names[i]);
				}
			break;
		case 13:
			if (store_cur_loc.x >= store_max_x - 1)  EdSysBeep( );  // (20)
				else {
					store_cur_loc.x++;
					i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
					csit(854,16,zone_names.section_names[i]);
				}
			break;
		case 14:
			if (store_cur_loc.y == 0)  EdSysBeep( );  // (20)
				else {
					store_cur_loc.y--;
					i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
					csit(854,16,zone_names.section_names[i]);
				}
			break;
		case 15:
			if (store_cur_loc.y >= store_max_y - 1) EdSysBeep( );  // (20)
				else {
					store_cur_loc.y++;
					i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
					csit(854,16,zone_names.section_names[i]);
				}
			break;

		case 17:
			i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
			j = scenario.out_width * scenario.out_height;

				n = choose_text_res(-8,0,j - 1,i,854,"Which Outdoor Section?");
				if (n >= 0) {
				store_cur_loc.x = n % scenario.out_width;
				store_cur_loc.y = (n - store_cur_loc.x ) / scenario.out_width;
				CDSN(854,18,store_cur_loc.x);
				CDSN(854,19,store_cur_loc.y);
				csit(854,16,zone_names.section_names[n]);
			}
			break;

		case 20:
					i = (scenario.out_width * CDGN(854,19)) + CDGN(854,18);
					csit(854,16,zone_names.section_names[i]);
			break;

		default:
			break;

		}
		 	CDSN(854,18,store_cur_loc.x);
 			CDSN(854,19,store_cur_loc.y);

}

short pick_out(location default_loc,short max_x,short max_y)
// ignore parent in Mac version
{
	char temp_str[256];
	short i;
	store_cur_loc = default_loc;
	store_max_x = max_x;
	store_max_y = max_y;
	
	cd_create_dialog_parent_num(854,0);
	
	cdsin(854,7,store_max_x);	
	cdsin(854,10,store_max_y);
 	CDSN(854,18,(int)store_cur_loc.x);
 	CDSN(854,19,(int)store_cur_loc.y);
	i = (scenario.out_width * store_cur_loc.y) + store_cur_loc.x;
		csit(854,16,zone_names.section_names[i]);


	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(854,0);
	return dialog_answer;
}

// q_3DModStart
void change_outdoor_size_event_filter (short item_hit)
{
	char temp_str[256];
	short new_x_size,new_y_size;
	short extra_dialog_answer_add = 0, extra_dialog_answer_index = 0;
	
	new_x_size = extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x;
	new_y_size = extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y;
	
	switch (item_hit) {
		case 2:
			dialog_answer = 1;
			 dialog_not_toast = FALSE; 
			break;
		case 3:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		case 10:
			extra_dialog_answer_index = 0;
			extra_dialog_answer_add = -1;
			new_y_size = new_y_size + extra_dialog_answer_add;
			break;
		case 11:
			extra_dialog_answer_index = 0;
			extra_dialog_answer_add = 1;
			new_y_size = new_y_size + extra_dialog_answer_add;
			break;
		case 13:
			extra_dialog_answer_index = 1;
			extra_dialog_answer_add = -1;
			new_x_size = new_x_size + extra_dialog_answer_add;
			break;
		case 14:
			extra_dialog_answer_index = 1;
			extra_dialog_answer_add = 1;
			new_x_size = new_x_size + extra_dialog_answer_add;
			break;
		case 16:
			extra_dialog_answer_index = 2;
			extra_dialog_answer_add = -1;
			new_y_size = new_y_size + extra_dialog_answer_add;
			break;
		case 17:
			extra_dialog_answer_index = 2;
			extra_dialog_answer_add = 1;
			new_y_size = new_y_size + extra_dialog_answer_add;
			break;
		case 19:
			extra_dialog_answer_index = 3;
			extra_dialog_answer_add = -1;
			new_x_size = new_x_size + extra_dialog_answer_add;
			break;
		case 20:
			extra_dialog_answer_index = 3;
			extra_dialog_answer_add = 1;
			new_x_size = new_x_size + extra_dialog_answer_add;
			break;
		case 31:
			cd_flip_led(991,31,item_hit);
			break;
		}
	if (cre(new_x_size,1,50,"Outdoors width must be between 1 and 50.","",991) == TRUE) return ;
	if (cre(new_y_size,1,50,"Outdoors height must be between 1 and 50.","",991) == TRUE) return ;
	if (cre(new_x_size * new_y_size,1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",991) == TRUE) return ;
	
	extra_dialog_answer[extra_dialog_answer_index] = extra_dialog_answer[extra_dialog_answer_index] + extra_dialog_answer_add;
	
	sprintf(temp_str,"%s%d",extra_dialog_answer[0] < 0 ? "-" : "+",(int)n_abs(extra_dialog_answer[0]));
	csit(991,12,temp_str);
	sprintf(temp_str,"%s%d",extra_dialog_answer[1] < 0 ? "-" : "+",(int)n_abs(extra_dialog_answer[1]));
	csit(991,15,temp_str);
	sprintf(temp_str,"%s%d",extra_dialog_answer[2] < 0 ? "-" : "+",(int)n_abs(extra_dialog_answer[2]));
	csit(991,18,temp_str);
	sprintf(temp_str,"%s%d",extra_dialog_answer[3] < 0 ? "-" : "+",(int)n_abs(extra_dialog_answer[3]));
	csit(991,21,temp_str);

	cdsin(991,28,new_x_size);
	cdsin(991,29,new_y_size);
}

Boolean change_outdoor_size()
{
	short on_surface_answer;
	
	extra_dialog_answer[0] = extra_dialog_answer[1] = extra_dialog_answer[2] = extra_dialog_answer[3] = 0;
	store_max_x = scenario.out_width;
	store_max_y = scenario.out_height;
	
	cd_create_dialog_parent_num(991,0);
	
	csit(991,12,"+0");
	csit(991,15,"+0");
	csit(991,18,"+0");
	csit(991,21,"+0");
	
	cdsin(991,26,store_max_x);
	cdsin(991,27,store_max_y);
	
	cdsin(991,28,store_max_x);
	cdsin(991,29,store_max_y);
	
	while (dialog_not_toast)
		ModalDialog();
	
	on_surface_answer = cd_get_led(991,31);
	
	cd_kill_dialog(991,0);
	
	if(dialog_answer == -1)
		return FALSE;
	/*
	//keep the "same" sector selected, if it still exists
	cur_out.x += extra_dialog_answer[1];
	cur_out.y += extra_dialog_answer[0];
	if(cur_out.x < 0 || cur_out.x >= extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x ||
	cur_out.y < 0 || cur_out.y >= extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y) {
		cur_out.x = cur_out.y = 0;
		if(editing_town = FALSE)
			cen_x = cen_y = 24;
	}
	//and the same starting place
	scenario.what_outdoor_section_start_in.x += extra_dialog_answer[1];
	scenario.what_outdoor_section_start_in.y += extra_dialog_answer[0];
	if(scenario.what_outdoor_section_start_in.x < 0 || scenario.what_outdoor_section_start_in.x >= extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x ||
	scenario.what_outdoor_section_start_in.y < 0 || scenario.what_outdoor_section_start_in.y >= extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y) {
		scenario.what_outdoor_section_start_in.x = scenario.what_outdoor_section_start_in.y = 0;
		scenario.start_where_in_outdoor_section.x = scenario.start_where_in_outdoor_section.y = 24;
	}*/
	
	save_change_to_outdoor_size(extra_dialog_answer[0],extra_dialog_answer[1],extra_dialog_answer[2],extra_dialog_answer[3],on_surface_answer);
	
	return TRUE;
}
// q_3DModEnd


void new_town_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 3:
			dialog_answer = 1;
			 dialog_not_toast = FALSE; 
			break;
		case 15:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(830,10,12,item_hit);
			cd_flip_led(830,18,item_hit);
			break;
		}

}

Boolean new_town()
// ignore parent in Mac version
{
	char temp_str[256];
	short size = 0;
	short on_surface_answer;
	
	short new_town = scenario.num_towns;
	 
	cd_create_dialog_parent_num(830,0);
	
	cdsin(830,14,new_town);
	cd_set_led(830,11,1);
	cd_set_led(830,18,town.is_on_surface);
	sprintf((char *) temp_str,"New Town Name");
	CDST(830,2,(char *) temp_str);
	
	while (dialog_not_toast)
		ModalDialog();	

	size = cd_get_led_range(830,10,12);
	on_surface_answer = cd_get_led(830,18);
	CDGT(830,2,(char *) temp_str);
	temp_str[19] = 0;
	cd_kill_dialog(830,0);
	if (dialog_answer < 0)
		return FALSE;
	
	scenario.num_towns++;
	scenario.town_size[new_town] = (unsigned char)size;
	cur_town = new_town;
	scenario.last_town_edited = new_town;
	town.clear_town_record_type();
	town.set_start_locs(size);
	t_d.clear_big_tr_type();
	strcpy(town.town_name,(char *) temp_str);
	strcpy(&zone_names.town_names[new_town][0],&town.town_name[0]);
	town_type = size;
	town.is_on_surface = on_surface_answer;
	if (on_surface_answer) {
		for (short i = 0; i < 64; i++)
			for (short j = 0; j < 64; j++)
				t_d.floor[i][j] = 37;
		}
		
	// set walls appropriately
	if (town.is_on_surface) {
		town.wall_1_sheet = 600;
		town.wall_2_sheet = 605;
		town.cliff_sheet = 650;
		}
		else {
			town.wall_1_sheet = 601;
			town.wall_2_sheet = 600;
			town.cliff_sheet = 651;
			}
			
	return TRUE;
}


// before calling this, be sure to do all checks to make sure it's safe.
void delete_last_town()
{
	scenario.num_towns--;

	save_campaign();
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_town()
{
	for(int i=cur_town; i<scenario.num_towns; i++){
		scenario.town_size[i]=scenario.town_size[i+1];
		scenario.town_starts_hidden[i]=scenario.town_starts_hidden[i+1];
	}
	if(scenario.start_in_what_town == cur_town){
		scenario.start_in_what_town=0;
	}
	scenario.num_towns--;
	save_remove_town();
}

void pick_import_town_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_dlog,2);
			if (dialog_answer < 0) {
				give_error("This number is out of the correct range. It must be at least 0.","",store_which_dlog);
				break;
				}
			 dialog_not_toast = FALSE; 
			break;
		case 7:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;

		}
}

short pick_import_town(short which_dlog /*,short def */)
// ignore parent in Mac version
{
	store_which_dlog = which_dlog;
	
	cd_create_dialog_parent_num(store_which_dlog,0);
	
	CDSN(store_which_dlog,2,0);
	
	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(store_which_dlog,0);
	return dialog_answer;
}

// Used for editing terrain. Called when user calls floor_drawn onto floor_there.
// returns TRUE is editor should erase spot
Boolean floors_match(short floor_drawn,short floor_there)
{
	// matching rough stone
	if ((((floor_drawn >= 4) && (floor_drawn <= 7)) || ((floor_drawn >= 11) && (floor_drawn <= 22))) &&
	  (((floor_there >= 4) && (floor_there <= 7)) || ((floor_there >= 11) && (floor_there <= 22))))
		return TRUE;
	// matching desert
	if ((floor_drawn == 41) &&
	  ((floor_there >= 41) && (floor_there <= 56)))
		return TRUE;
	// matching cave floor
	//if (((floor_drawn >= 0) && (floor_drawn <= 3)) &&
	 // ((floor_there >= 0) && (floor_there <= 3)))
	//	return TRUE;
	// matching grass
	//if (((floor_drawn >= 37) && (floor_drawn <= 40)) &&
	 // ((floor_there >= 37) && (floor_there <= 40)))
	//	return TRUE;
	// matching cave water
	if (((floor_drawn >= 11) && (floor_drawn <= 22)) &&
	  ((floor_there >= 11) && (floor_there <= 22)))
		return TRUE;
	// matching surfacxe water
	if (((floor_drawn >= 57) && (floor_drawn <= 70)) &&
	  ((floor_there >= 57) && (floor_there <= 70)))
		return TRUE;
	if (floor_drawn == floor_there)
		return TRUE;
	return FALSE;
}

// Used for editing terrain. Called when user calls terrain_drawn onto terrain_there.
// returns TRUE is editor should erase spot
Boolean terrains_match(short terrain_drawn,short terrain_there)
{
	if (terrain_drawn == terrain_there)
		return TRUE;
	return FALSE;
}


void edit_area_rect_event_filter (short item_hit)
{
	char str[256];
	
	switch (item_hit) {
		case 3:
			dialog_answer = FALSE;
			dialog_not_toast = FALSE;
			break;

		case 5:
			dialog_answer = TRUE;
			dialog_not_toast = FALSE; 
			CDGT(840,2,(char *) str);
			str[29] = 0;
			if (editing_town == FALSE)
				sprintf(current_terrain.info_rect_text[store_which_str],"%s",(char *) str);
				else sprintf(town.info_rect_text[store_which_str],"%s",(char *) str);
			break;
		}
}

// mode 0 - out 1 - town
Boolean edit_area_rect_str(short which_str /*,short mode */)
// ignore parent in Mac version
{
	store_which_str = which_str;
	
	cd_create_dialog_parent_num(840,0);
	
	if (editing_town == FALSE)
		CDST(840,2,current_terrain.info_rect_text[store_which_str]);
		else CDST(840,2,town.info_rect_text[store_which_str]);
		
	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(840,0);
	
	return (Boolean)dialog_answer;
}

void edit_out_strs()
// ignore parent in Mac version
{
	short i;

	for (i = 0; i < 8; i++)
		str_do_delete[i] = 0;

	cd_create_dialog_parent_num(850,0);

	put_out_strs_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(850,0);
}

void put_out_strs_in_dlog()
{
	char str[256];
	short i;

	for (i = 0; i < 8; i++) {
		if ((current_terrain.info_rect[i].right <= 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			csit(850,13 + i,(char *) str);
			csit(850,33 + i,(char *) str);			
			cd_activate_item(850,25 + i,0);
			}
			else {
			sprintf((char *) str,"L = %d, T = %d", (int)current_terrain.info_rect[i].left,(int)current_terrain.info_rect[i].top);
		csit(850,13 + i,(char *) str);
			sprintf((char *) str,"R = %d, B = %d",(int)current_terrain.info_rect[i].right, (int)current_terrain.info_rect[i].bottom);
		csit(850,33 + i,(char *) str);
		}
		CDST(850,2 + i,current_terrain.info_rect_text[i]);
		}
}

void edit_out_strs_event_filter (short item_hit)
{
	switch (item_hit) {
		case 10:
			if (save_out_strs() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 11:
				 dialog_not_toast = FALSE; 
			break;

		case 25:	case 26:	case 27:	case 28:
		case 29:	case 30:	case 31:	case 32:
				CDST(850,item_hit - 23,"");
				current_terrain.info_rect[item_hit - 25].top = -1;
				current_terrain.info_rect[item_hit - 25].left = -1;
				current_terrain.info_rect[item_hit - 25].bottom = -1;
				current_terrain.info_rect[item_hit - 25].right = -1;
				str_do_delete[item_hit - 25] = 1;
				put_out_strs_in_dlog();
			break;
			
		default:
			break;
		}
}

Boolean save_out_strs()
{
	char str[256];
	short i;

	for (i = 0; i < 8; i++) {
		CDGT(850,2 + i,(char *) str);
		str[29] = 0;
		sprintf((char *)current_terrain.info_rect_text[i],"%s",str);
		if (str_do_delete[i] > 0)
			current_terrain.info_rect[i].right = -1;
		}
	return TRUE;
}

void edit_town_strs()
// ignore parent in Mac version
{
	short i;
	
	for (i = 0; i < 16; i++)
		str_do_delete[i] = 0;
		
	cd_create_dialog_parent_num(839,0);
	
	put_town_strs_in_dlog();

	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(839,0);
}

void put_town_strs_in_dlog()
{
	char str[256];
	short i;

	for (i = 0; i < 16; i++) {
		if ((town.room_rect[i].right <= 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			csit(839,21 + i,(char *) str);
			csit(839,57 + i,(char *) str);
			cd_activate_item(839,41 + i,0);
			}
			else {
			sprintf((char *) str,"L = %d, T = %d",(int)town.room_rect[i].left,(int)town.room_rect[i].top);
		csit(839,21 + i,(char *) str);
			sprintf((char *) str,"R = %d, B = %d",(int)town.room_rect[i].right, (int)town.room_rect[i].bottom);
		csit(839,57 + i,(char *) str);
		}
		CDST(839,2 + i,town.info_rect_text[i]);
		}

}

void edit_town_strs_event_filter (short item_hit)
{
	switch (item_hit) {
		case 18:
			if (save_town_strs() == TRUE)
				 dialog_not_toast = FALSE;
			break;
		case 19:
				 dialog_not_toast = FALSE;
			break;
		default:
			if ((item_hit >= 41) && (item_hit <= 56)) {
				CDST(839,item_hit - 39,"");
				town.room_rect[item_hit - 41].top = -1;
				town.room_rect[item_hit - 41].left = -1;
				town.room_rect[item_hit - 41].bottom = -1;
				town.room_rect[item_hit - 41].right = -1;
				str_do_delete[item_hit - 41] = 1;
				put_town_strs_in_dlog();
				}
			break;
		}
}

Boolean save_town_strs()
{
	char str[256];
	short i;

	for (i = 0; i < 16; i++) {
		CDGT(839,2 + i,(char *) str);
		str[29] = 0;
		sprintf((char *)town.info_rect_text[i],"%s",str);
		if (str_do_delete[i] > 0)
			town.room_rect[i].right = -1;
		}
	return TRUE;
}

void edit_item_placement()
// ignore parent in Mac version
{
	store_storage = scenario.storage_shortcuts[0];
	cur_shortcut = 0;

	cd_create_dialog_parent_num(812,0);

	put_item_placement_in_dlog();

	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(812,0);
}

void put_item_placement_in_dlog()
{
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(812,2 + i,store_storage.item_num[i]);
		CDSN(812,12 + i,store_storage.item_odds[i]);
		}
	CDSN(812,22,store_storage.ter_type);
	cdsin(812,27,cur_shortcut);
	csit(812,36,scen_data.scen_ter_types[CDGN(812,22)].ter_name);
	cd_set_led(812,38,store_storage.property);

	for (i = 0; i < 10; i++) {
			csit(812,51 + i,scen_data.scen_items[CDGN(812,2 + i)].full_name);
			}
}

void edit_item_placement_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 23:
			if (save_item_placement() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 24:
				 dialog_not_toast = FALSE; 
			break;
		case 26:
			if (save_item_placement() == FALSE) break;
			cur_shortcut--;
			if (cur_shortcut < 0) cur_shortcut = 9;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 25:
			if (save_item_placement() == FALSE) break;
			cur_shortcut++;
			if (cur_shortcut > 9) cur_shortcut = 0;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 37: // Name button
		csit(812,36,scen_data.scen_ter_types[CDGN(812,22)].ter_name);
		for (i = 0; i < 10; i++) {
			csit(812,51 + i,scen_data.scen_items[CDGN(812,2 + i)].full_name);
			}
			break;

		case 40:
			store_storage.ter_type = CDGN(812,22);
			i = choose_text_res(-4,0,511,store_storage.ter_type,812,"Which Terrain?");
			store_storage.ter_type = i;
			CDSN(812,22,i);
			csit(812,36,scen_data.scen_ter_types[i].ter_name);
			break;

		case 62:
				for (i = 0; i < 10; i++) {
				 CDSN(812,2 + i,-1);
				 CDSN(812,12 + i,0);
				 }
		 		CDSN(812,22,-1);
				cd_set_led(812,38,0);
				csit(812,36,scen_data.scen_ter_types[CDGN(812,22)].ter_name);
				for (i = 0; i < 10; i++) {
				csit(812,51 + i,scen_data.scen_items[CDGN(812,2 + i)].full_name);
				}
			break;
				
		default:
			if ((item_hit >= 41) && (item_hit <= 50)) {
				i = CDGN(812,item_hit - 39);
				i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,i,812,"Place which item?"); 
				if (i >= 0) {
					CDSN(812,item_hit - 39,i);
					csit(812,item_hit  + 10,scen_data.scen_items[i].full_name);
					}
				break;
				}
			cd_flip_led(812,38,item_hit);
			break;
		}
}

Boolean save_item_placement()
{
	short i;

	store_storage.property = cd_get_led(812,38);
	store_storage.ter_type = CDGN(812,22);
	for (i = 0; i < 10; i++) {
		store_storage.item_num[i] = CDGN(812,2 + i);
		store_storage.item_odds[i] = CDGN(812,12 + i);
		}
	scenario.storage_shortcuts[cur_shortcut] = store_storage;

	return TRUE;
}

void edit_add_town()
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(810,0);

	put_add_town_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(810,0);
}

void put_add_town_in_dlog()
{
	short i;
	short x;
	
	for (i = 0; i < 10; i++) {
		CDSN(810,2 + i,scenario.town_to_add_to[i]);
		CDSN(810,12 + i,scenario.flag_to_add_to_town[i][0]);
		CDSN(810,22 + i,scenario.flag_to_add_to_town[i][1]);
		x = CDGN(810,2 + i);
		csit(810,49 + i,zone_names.town_names[x]);
		}

}

void edit_add_town_event_filter (short item_hit)
{	
	short i;
	short x;
	
	switch (item_hit) {
		case 32:
			if (save_add_town() == TRUE)
				 dialog_not_toast = FALSE; 
			break;

		case 39:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,2),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,2,x);
				csit(810,49,zone_names.town_names[x]);
			}
			break;
		case 40:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,3),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,3,x);
				csit(810,50,zone_names.town_names[x]);
			}
			break;
		case 41:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,4),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,4,x);
				csit(810,51,zone_names.town_names[x]);
			}
			break;
		case 42:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,5),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,5,x);
				csit(810,52,zone_names.town_names[x]);
			}
			break;
		case 43:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,6),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,6,x);
				csit(810,53,zone_names.town_names[x]);
			}
			break;
		case 44:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,7),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,7,x);
				csit(810,54,zone_names.town_names[x]);
			}
			break;
		case 45:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,8),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,8,x);
				csit(810,55,zone_names.town_names[x]);
			}
			break;
		case 46:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,9),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,9,x);
				csit(810,56,zone_names.town_names[x]);
			}
			break;
		case 47:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,10),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,10,x);
				csit(810,57,zone_names.town_names[x]);
			}
			break;
		case 48:
				x = choose_text_res(-7,0,scenario.num_towns - 1,CDGN(810,11),810,"Which Town?");
				if (x >= 0) {
				CDSN(810,11,x);
				csit(810,58,zone_names.town_names[x]);
			}
			break;

		case 59:
			for (i = 0; i < 10; i++) {
					x = CDGN(810,2 + i);
					csit(810,49 + i,zone_names.town_names[x]);
		}
			break;
			
		}
}

Boolean save_add_town()
{
	short i;

	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = CDGN(810,2 + i);
		if (cre(scenario.town_to_add_to[i],
			-1,199,"Town number must be from 0 to 199 (or -1 for no effect).","",810) == TRUE) return FALSE;
		scenario.flag_to_add_to_town[i][0] = CDGN(810,12 + i);
		if (cre(scenario.flag_to_add_to_town[i][0],
			0,299,"First part of flag must be from 0 to 299.","",810) == TRUE) return FALSE;
		scenario.flag_to_add_to_town[i][1] = CDGN(810,22 + i);
		if (cre(scenario.flag_to_add_to_town[i][1],
			0,29,"Second part of flag must be from 0 to 29.","",810) == TRUE) return FALSE;
		}
	return TRUE;
}

short edit_make_scen_1(char *filename,char *title,short *start_on_surface)
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(800,0);

	put_make_scen_1_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	CDGT(800,2,title);
	title[49] = 0;
	CDGT(800,3,filename);
	filename[21] = 0;
	*start_on_surface = cd_get_led(800,10);
	cd_kill_dialog(800,0);
	return dialog_answer;
}

void put_make_scen_1_in_dlog()
{
	CDST(800,2,"Scenario name");
	CDST(800,3,"filename");
}


void edit_make_scen_1_event_filter (short item_hit)
{
	char str[256];
	
	switch (item_hit) {
		case 4:
			CDGT(800,2,str);
			if (string_not_clean(str,30,0,"The scenario name",800))
				return;
			if (strlen(str) == 0) {
				give_error("You've left the scenario name empty.","",800);
				return;
				}
			CDGT(800,3,str);
			if (string_not_clean(str,21,1,"The scenario file name",800))
				return;
			if (strlen(str) == 0) {
				give_error("You've left the scenario file name empty.","",800);
				return;
				}

			dialog_answer = 1;
			dialog_not_toast = FALSE; 
			break;
		case 8:
			dialog_answer = 0;
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(800,10,item_hit);
			break;
		}
}

short edit_make_scen_2(short *val_array)
// ignore parent in Mac version
{
	short i;;

	//array = val_array;
	cd_create_dialog_parent_num(801,0);

	put_make_scen_2_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	for (i = 0; i < 2; i++)
		val_array[i] = CDGN(801,2 + i);
	val_array[5] = cd_get_led(801,17);
	cd_kill_dialog(801,0);
	return dialog_answer;
}

void put_make_scen_2_in_dlog()
{
	CDSN(801,2,1);
	CDSN(801,3,1);
}

void edit_make_scen_2_event_filter (short item_hit)
{
	short i,j;
	
	switch (item_hit) {
		case 7:
			i = CDGN(801,2);
			if (cre(i,1,50,"Outdoors width must be between 1 and 50.","",801) == TRUE) return ;
			j = CDGN(801,3);
			if (cre(j,1,50,"Outdoors height must be between 1 and 50.","",801) == TRUE) return ;
			if (cre(i * j,1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",801) == TRUE) return ;
	
			dialog_not_toast = FALSE; 
			break;
		case 6:
			dialog_answer = 0;
				 dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(801,17,item_hit);
			break;
		}
}

void build_scenario()
{
	short two_flags[6]; // width, height, large, med, small, default_town
	char f_name[256],f_name2[256],title[256];
	short on_surface;
	
	file_is_loaded = FALSE;

	if (edit_make_scen_1((char *) f_name,(char *) title,&on_surface) == FALSE)
		return;
	sprintf((char *) f_name2,"%s.bas",f_name);
	if (edit_make_scen_2((short *) two_flags) == FALSE)
		return;
	if (fancy_choice_dialog(860,0) == 2)
		return;
	
	if (create_basic_scenario(f_name,f_name2,title,two_flags[0],two_flags[1], on_surface, (Boolean)two_flags[5]))	
		fancy_choice_dialog(874,0);
	else fancy_choice_dialog(875,0);
}

void edit_scen_details()
// ignore parent in Mac version
{
	cd_create_dialog_parent_num(803,0);

	put_scen_details_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(803,0);
}

void put_scen_details_in_dlog()
{
  short x,y,z;
	char * str;
  
	cd_set_led_range(803,11,14,scenario.rating);
	CDSN(803,2,scenario.ver[0]);
	CDSN(803,3,scenario.ver[1]);
	CDSN(803,4,scenario.ver[2]);
	CDST(803,5,scenario.credits_text[1]);
	CDST(803,6,scenario.scen_desc);
	CDST(803,7,scenario.scen_name);
	CDSN(803,8,scenario.min_level);
	CDSN(803,9,scenario.max_level);

}

void edit_scen_details_event_filter (short item_hit)
{
  short x,y,z;
	char * str;

	switch (item_hit) {
		case 10:
			if (save_scen_details() == TRUE)
				 dialog_not_toast = FALSE;
			break;
		case 28:
				 dialog_not_toast = FALSE;
			break;

		default:
			cd_hit_led_range(803,11,14,item_hit);
			break;
		}
}

Boolean save_scen_details()
{
	char str[256];
	short i;

	scenario.rating = cd_get_led_range(803,11,14);
	scenario.ver[0] = (unsigned char)CDGN(803,2);
	scenario.ver[1] = (unsigned char)CDGN(803,3);
	scenario.ver[2] = (unsigned char)CDGN(803,4);
	for (i = 0; i < 3; i++)
		if (cre(scenario.ver[i],
			0,9,"The digits in the version number must be in the 0 to 9 range.","",803) == TRUE) return FALSE;

	CDGT(803,5,scenario.credits_text[1]);
	CDGT(803,6,scenario.scen_desc);
	CDGT(803,7,(char *) str);
	str[49] = 0;
	strcpy(scenario.scen_name,(char *) str);

	scenario.min_level = CDGN(803,8);
	if (cre(scenario.min_level,
		0,100,"The minimum level must be between 0 and 100.","",803) == TRUE) return FALSE;
	scenario.max_level = CDGN(803,9);
	if (cre(scenario.max_level,
		0,100,"The maximum level must be between 0 and 100.","",803) == TRUE) return FALSE;

	return TRUE;
}

void edit_scen_intro_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 9:
			scenario.intro_pic_resources[store_which_part] = CDGN(804,8);
			if ((scenario.intro_pic_resources[store_which_part] < -1) || (scenario.intro_pic_resources[store_which_part] > 3000)) {
				give_error("WARNING: Intro picture number is out of range.","",804);
				break;
				}
			for (i = 0; i < 6; i++)
				CDGT(804, 2 + i,scenario.intro_text[store_which_part][i]);
			dialog_not_toast = FALSE; 
			break;
		case 10: 
			dialog_not_toast = FALSE; 
			break;

		}
}

void edit_scen_intro(short which_part)
{
	short i;
	
	store_which_part = which_part;
	cd_create_dialog_parent_num(804,0);
	
	CDSN(804,8,scenario.intro_pic_resources[which_part]);
	for (i = 0; i < 6; i++)
		CDST(804, 2 + i,scenario.intro_text[which_part][i]);
	cdsin(804,16,store_which_part);
		
	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(804,0);
}

void edit_scen_intro_pic_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 3:
			scenario.scen_label_pic = CDGN(805,2);
			if ((scenario.scen_label_pic < 0) || (scenario.scen_label_pic > 3000)) {
				give_error("WARNING: Label picture number is out of range.","",8805);
				break;
				}

			dialog_not_toast = FALSE; 
			break;
		case 4: 
			dialog_not_toast = FALSE; 
			break;

		}
}

void edit_scen_intro_pic()
{
	cd_create_dialog_parent_num(805,0);
	
	CDSN(805,2,scenario.scen_label_pic);
		
	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(805,0);
}

void change_town_size_event_filter(short item_hit)
{
	//10,11,12 are the leds
	//15 is cancel, 3 is ok
	switch (item_hit) {
		case 3:
			dialog_answer = 1;
			dialog_not_toast = FALSE; 
			break;
		case 15:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(831,10,12,item_hit);
			cd_flip_led(831,18,item_hit);
			break;
	}
}

Boolean change_town_size()
{
	short i,j;
	int size = -1;// 0 - large, 1 - medium, 2 - small
	short old_size=scenario.town_size[cur_town];
	short basic_dlog_hit;
	cd_create_dialog_parent_num(831,0);
	cd_set_led(831,10+old_size,1);
	while (dialog_not_toast)
		ModalDialog();
	size = cd_get_led_range(831,10,12);
	cd_kill_dialog(831,0);
	if (dialog_answer<0 || size<0 || size>2)
		return FALSE;
	if(old_size==size)
		return(FALSE);//no change made, so do nothing
	scenario.town_size[cur_town] = size;
	scenario.last_town_edited = cur_town;
	town.clear_town_record_type();
	town.set_start_locs(size);
	t_d.clear_big_tr_type();
	town_type = size;
	if (town.is_on_surface) {
		for (i = 0; i < 64; i++){
			for (j = 0; j < 64; j++){
				t_d.floor[i][j] = 37;
			}
		}
	}
	// set walls appropriately
	if (town.is_on_surface) {
		town.wall_1_sheet = 600;
		town.wall_2_sheet = 605;
		town.cliff_sheet = 650;
	}
	else {
		town.wall_1_sheet = 601;
		town.wall_2_sheet = 600;
		town.cliff_sheet = 651;
	}
	return TRUE;
}

void edit_horses()
// ignore parent in Mac version
{
	store_horse_page = 0;

	cd_create_dialog_parent_num(808,0);

	put_horses_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(808,0);
}

void put_horses_in_dlog()
{
	short i;

	for (i = 0; i < 6; i++) {
		cdsin(808,23 + i,6 * store_horse_page + i);
		CDSN(808,2 + i,scenario.scen_horses[6 * store_horse_page + i].which_town);
		CDSN(808,8 + i,scenario.scen_horses[6 * store_horse_page + i].horse_loc.x);
		CDSN(808,14 + i,scenario.scen_horses[6 * store_horse_page + i].horse_loc.y);
		cd_set_led(808,43 + i,scenario.scen_horses[6 * store_horse_page + i].property);
		}

}

void edit_horses_event_filter (short item_hit)
{
	short i;
	short j;
	
	switch (item_hit) {
		case 20:
			if (save_horses() == TRUE)
				 dialog_not_toast = FALSE;
		break;
		case 21:
			if (save_horses() == FALSE) break;
			store_horse_page--;
			if (store_horse_page < 0)
				 store_horse_page = 4;
			put_horses_in_dlog();
		break;
		case 22:
			if (save_horses() == FALSE) break;
			store_horse_page++;
			if (store_horse_page > 4)
				 store_horse_page = 0;
			put_horses_in_dlog();
		break;
		
		case 49: case 50: case 51: case 52: case 53: case 54:
				 i = (item_hit - 49);
				 CDSN(808,2 + i,-1);
				 CDSN(808,8 + i,0);
				 CDSN(808,14 + i,0);
				 cd_set_led(808,43 + i,0);
		break;

		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(808,43 + i,item_hit);
		break;
		}
}

Boolean save_horses()
{
	short i;

	for (i = 0; i < 6; i++) {
		scenario.scen_horses[6 * store_horse_page + i].which_town = CDGN(808,2 + i);
		if (cre(scenario.scen_horses[6 * store_horse_page + i].which_town,
			-1,199,"Town number must be from 0 to 199 (or -1 for horse to not exist).","",808) == TRUE) return FALSE;
		scenario.scen_horses[6 * store_horse_page + i].horse_loc.x = CDGN(808,8 + i);
		if (cre(scenario.scen_horses[6 * store_horse_page + i].horse_loc.x,
			0,63,"Horse location coordinates must be from 0 to 63.","",808) == TRUE) return FALSE;
		scenario.scen_horses[6 * store_horse_page + i].horse_loc.y = CDGN(808,14 + i);
		if (cre(scenario.scen_horses[6 * store_horse_page + i].horse_loc.y,
			0,63,"Horse location coordinates must be from 0 to 63.","",808) == TRUE) return FALSE;
		scenario.scen_horses[6 * store_horse_page + i].property = cd_get_led(808,43 + i);
		}
	return TRUE;
}

void edit_boats()
// ignore parent in Mac version
{
	store_boat_page = 0;

	cd_create_dialog_parent_num(809,0);

	put_boats_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(809,0);
}

void put_boats_in_dlog()
{
	short i;

	for (i = 0; i < 6; i++) {
		cdsin(809,23 + i,6 * store_boat_page + i);
		CDSN(809,2 + i,scenario.scen_boats[6 * store_boat_page + i].which_town);
		CDSN(809,8 + i,scenario.scen_boats[6 * store_boat_page + i].boat_loc.x);
		CDSN(809,14 + i,scenario.scen_boats[6 * store_boat_page + i].boat_loc.y);
		cd_set_led(809,43 + i,scenario.scen_boats[6 * store_boat_page + i].property);
		}

}

void edit_boats_event_filter (short item_hit)
{
	short i;

	switch (item_hit) {
		case 20:
			if (save_boats() == TRUE)
				 dialog_not_toast = FALSE;
		break;
		case 21:
			if (save_boats() == FALSE)
		break;
			store_boat_page--;
			if (store_boat_page < 0)
				 store_boat_page = 4;
			put_boats_in_dlog();
		break;
		case 22:
			if (save_boats() == FALSE)
			break;
			store_boat_page++;
			if (store_boat_page > 4)
				 store_boat_page = 0;
			put_boats_in_dlog();
		break;

		case 49: case 50: case 51: case 52: case 53: case 54:
				 i = (item_hit - 49);
				 CDSN(809,2 + i,-1);
				 CDSN(809,8 + i,0);
				 CDSN(809,14 + i,0);
				 cd_set_led(809,43 + i,0);
		break;

		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(809,43 + i,item_hit);
		break;
		}
}

Boolean save_boats()
{
	short i;

	for (i = 0; i < 6; i++) {
		scenario.scen_boats[6 * store_boat_page + i].which_town = CDGN(809,2 + i);
		if (cre(scenario.scen_boats[6 * store_boat_page + i].which_town,
			-1,199,"Town number must be from 0 to 199 (or -1 for boat to not exist).","",809) == TRUE) return FALSE;
		scenario.scen_boats[6 * store_boat_page + i].boat_loc.x = CDGN(809,8 + i);
		if (cre(scenario.scen_boats[6 * store_boat_page + i].boat_loc.x,
			0,63,"Boat location coordinates must be from 0 to 63.","",809) == TRUE) return FALSE;
		scenario.scen_boats[6 * store_boat_page + i].boat_loc.y = CDGN(809,14 + i);
		if (cre(scenario.scen_boats[6 * store_boat_page + i].boat_loc.y,
			0,63,"Boat location coordinates must be from 0 to 63.","",809) == TRUE) return FALSE;
		scenario.scen_boats[6 * store_boat_page + i].property = cd_get_led(809,43 + i);
		}
	return TRUE;
}

