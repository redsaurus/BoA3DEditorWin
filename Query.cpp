
#include "Global.h"


extern scenario_data_type scenario;
extern scen_item_data_type scen_data;
extern short data_dump_file_id;
extern OPENFILENAME ofn;


void start_data_dump()
{
	short i;
	char get_text[280];
	FILE *data_dump_file_id;
	long len;

	 sprintf(get_text,"%s - Scenario Data.txt",scenario.scen_name);
	if (NULL == (data_dump_file_id = fopen(get_text, "wb"))) {
		oops_error(11);
		return;
		}

	sprintf((char *)get_text,"\r\rScenario data for %s:\r\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"Floor types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	for (i = 0; i < 256; i++) {
			sprintf((char *)get_text,"  Floor %d: \t%s\r",(int)i,scen_data.scen_floors[i].floor_name);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
	sprintf((char *)get_text,"\r\rTerrain types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	for (i = 0; i < 512; i++) {
			sprintf((char *)get_text,"  Terrain %d: \t%s\r", (int)i,scen_data.scen_ter_types[i].ter_name);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}
	sprintf((char *)get_text,"\r\rCreature types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	for (i = 0; i < 256; i++) {
		sprintf((char *)get_text,"  Creature %d: \t%s\r",(int)i,scen_data.scen_creatures[i].name);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
	sprintf((char *)get_text,"\r\rItem types for %s:\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	for (i = 0; i < NUM_SCEN_ITEMS; i++) {
		sprintf((char *)get_text,"  Item %d: \t%s\r", (int)i,scen_data.scen_items[i].full_name);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
		sprintf((char *)get_text,"\r\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	FSClose(data_dump_file_id);
}	


void floor_type_data_dump()
{
	short i;
	char get_text[1280];
	FILE *data_dump_file_id;
	long len;

	 sprintf(get_text,"%s - Floor Type Data.txt",scenario.scen_name);
	if (NULL == (data_dump_file_id = fopen(get_text, "wb"))) {
		oops_error(11);
		return;
		}

	sprintf((char *)get_text,"\r\rScenario Floor Type Data for %s:\r\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	
	sprintf((char *)get_text,"i\tfloor_name\tpic.which_sheet\tpic.which_icon\tpic.graphic_adjust\ted_pic.which_sheet\ted_pic.which_icon\ted_pic.graphic_adjust\tblocked\tstep_sound\tlight_radius\tfloor_height\tnum_anim_steps\tspecial\teffect_adjust\tis_water\tis_floor\tis_ground\tis_rough\tfly_over\tshimmers\toutdoor_combat_town_used\tshortcut_key\textra[0]\textra[1]\textra[2]\textra[3]\textra[4]\textra[5]\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\t23\t24\t25\t26\t27\t28\t29\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

		for (i = 0; i < 256; i++) {
		sprintf((char *)get_text,"%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r",i,scen_data.scen_floors[i].floor_name,scen_data.scen_floors[i].pic.which_sheet,scen_data.scen_floors[i].pic.which_icon,scen_data.scen_floors[i].pic.graphic_adjust,scen_data.scen_floors[i].ed_pic.which_sheet,scen_data.scen_floors[i].ed_pic.which_icon,scen_data.scen_floors[i].ed_pic.graphic_adjust,scen_data.scen_floors[i].blocked,scen_data.scen_floors[i].step_sound,scen_data.scen_floors[i].light_radius,scen_data.scen_floors[i].floor_height,scen_data.scen_floors[i].num_anim_steps,scen_data.scen_floors[i].special,scen_data.scen_floors[i].effect_adjust,scen_data.scen_floors[i].is_water,scen_data.scen_floors[i].is_floor,scen_data.scen_floors[i].is_ground,scen_data.scen_floors[i].is_rough,scen_data.scen_floors[i].fly_over,scen_data.scen_floors[i].shimmers,scen_data.scen_floors[i].outdoor_combat_town_used,scen_data.scen_floors[i].shortcut_key,scen_data.scen_floors[i].extra[0],scen_data.scen_floors[i].extra[1],scen_data.scen_floors[i].extra[2],scen_data.scen_floors[i].extra[3],scen_data.scen_floors[i].extra[4],scen_data.scen_floors[i].extra[5]);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
		sprintf((char *)get_text,"\r\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	FSClose(data_dump_file_id);
}	

void terrain_type_data_dump()
{
	short i;
	char get_text[1280];
	FILE *data_dump_file_id;
	long len;

	 sprintf(get_text,"%s - Terrain Type Data.txt",scenario.scen_name);
	if (NULL == (data_dump_file_id = fopen(get_text, "wb"))) {
		oops_error(11);
		return;
		}

	sprintf((char *)get_text,"\r\rScenario Terrain Type Data for %s:\r\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"i\tter_name\tpic.which_sheet\tpic.which_icon\tpic.graphic_adjust\tcut_away_pic.which_sheet\tcut_away_pic.which_icon\tcut_away_pic.graphic_adjust\ted_pic.which_sheet\ted_pic.which_icon\ted_pic.graphic_adjust\ticon_offset_x\ticon_offset_y\tsecond_icon\tsecond_icon_offset_x\tsecond_icon_offset_y\tcut_away_second_icon\tcut_away_offset_x\tcut_away_offset_y\tanim_steps\tmove_block[0]\tmove_block[1]\tmove_block[2]\tmove_block[3]\tsee_block[0]\tsee_block[1]\tsee_block[2]\tsee_block[3]\tblocks_view[0]\tblocks_view[1]\tblocks_view[2]\tblocks_view[3]\theight_adj\tsuppress_floor\tlight_radius\tstep_sound\tshortcut_key\tcrumble_type\tbeam_hit_type\tterrain_to_crumble_to\thidden_town_terrain\tswap_terrain\tis_bridge\tis_road\tcan_look_at\tdraw_on_automap\tdefault_script\tshimmers\toutdoor_combat_town_used\tspecial\teffect_adjust\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\t23\t24\t25\t26\t27\t28\t29\t30\t31\t32\t33\t34\t35\t36\t37\t38\t39\t40\t41\t42\t43\t44\t45\t46\t47\t48\t49\t50\t51\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

		for (i = 0; i < 512; i++) {
		sprintf((char *)get_text,"%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\r",(int)i,scen_data.scen_ter_types[i].ter_name,scen_data.scen_ter_types[i].pic.which_sheet,scen_data.scen_ter_types[i].pic.which_icon,scen_data.scen_ter_types[i].pic.graphic_adjust,scen_data.scen_ter_types[i].cut_away_pic.which_sheet,scen_data.scen_ter_types[i].cut_away_pic.which_icon,scen_data.scen_ter_types[i].cut_away_pic.graphic_adjust,scen_data.scen_ter_types[i].ed_pic.which_sheet,scen_data.scen_ter_types[i].ed_pic.which_icon,scen_data.scen_ter_types[i].ed_pic.graphic_adjust,scen_data.scen_ter_types[i].icon_offset_x,scen_data.scen_ter_types[i].icon_offset_y,scen_data.scen_ter_types[i].second_icon,scen_data.scen_ter_types[i].second_icon_offset_x,scen_data.scen_ter_types[i].second_icon_offset_y,scen_data.scen_ter_types[i].cut_away_second_icon,scen_data.scen_ter_types[i].cut_away_offset_x,scen_data.scen_ter_types[i].cut_away_offset_y,scen_data.scen_ter_types[i].anim_steps,scen_data.scen_ter_types[i].move_block[0],scen_data.scen_ter_types[i].move_block[1],scen_data.scen_ter_types[i].move_block[2],scen_data.scen_ter_types[i].move_block[3],scen_data.scen_ter_types[i].see_block[0],scen_data.scen_ter_types[i].see_block[1],scen_data.scen_ter_types[i].see_block[2],scen_data.scen_ter_types[i].see_block[3],scen_data.scen_ter_types[i].blocks_view[0],scen_data.scen_ter_types[i].blocks_view[1],scen_data.scen_ter_types[i].blocks_view[2],scen_data.scen_ter_types[i].blocks_view[3],scen_data.scen_ter_types[i].height_adj,scen_data.scen_ter_types[i].suppress_floor,scen_data.scen_ter_types[i].light_radius,scen_data.scen_ter_types[i].step_sound,scen_data.scen_ter_types[i].shortcut_key,scen_data.scen_ter_types[i].crumble_type,scen_data.scen_ter_types[i].beam_hit_type,scen_data.scen_ter_types[i].terrain_to_crumble_to,scen_data.scen_ter_types[i].hidden_town_terrain,scen_data.scen_ter_types[i].swap_terrain,scen_data.scen_ter_types[i].is_bridge,scen_data.scen_ter_types[i].is_road,scen_data.scen_ter_types[i].can_look_at,scen_data.scen_ter_types[i].draw_on_automap,scen_data.scen_ter_types[i].default_script,scen_data.scen_ter_types[i].shimmers,scen_data.scen_ter_types[i].outdoor_combat_town_used,scen_data.scen_ter_types[i].special,scen_data.scen_ter_types[i].effect_adjust);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
		sprintf((char *)get_text,"\r\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	FSClose(data_dump_file_id);
}	


void creature_type_data_dump()
{
	short i;
	char get_text[1280];
	FILE *data_dump_file_id;
	long len;

	 sprintf(get_text,"%s - Creature Type Data.txt",scenario.scen_name);
	if (NULL == (data_dump_file_id = fopen(get_text, "wb"))) {
		oops_error(11);
		return;
		}

	sprintf((char *)get_text,"\r\rScenario Creature Type Data for %s:\r\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"i\tname\tlevel\twhich_to_add_to[0]\twhich_to_add_to[1]\twhich_to_add_to[2]\twhich_to_add_to[3]\twhich_to_add_to[4]\twhich_to_add_to[5]\thow_much[0]\thow_much[1]\thow_much[2]\thow_much[3]\thow_much[4]\thow_much[5]\titems[0]\titems[1]\titems[2]\titems[3]\titems[4]\titems[5]\titems[6]\titems[7]\titem_chances[0]\titem_chances[1]\titem_chances[2]\titem_chances[3]\titem_chances[4]\titem_chances[5]\titem_chances[6]\titem_chances[7]\thp_bonus\tsp_bonus\tspec_abil\tdefault_attitude\tspecies\timmunities[0]\timmunities[1]\timmunities[2]\timmunities[3]\timmunities[4]\timmunities[5]\tnatural_armor\tchar_graphic.which_sheet\tchar_graphic.which_icon\tchar_graphic.graphic_adjust\tchar_upper_graphic_sheet\tsmall_or_large_template\ta[0]\ta[1]\ta[2]\ta1_type\ta23_type\tap_bonus\tdefault_script\tdefault_strategy\tdefault_aggression\tdefault_courage\tsummon_class\textra[0]\textra[1]\textra[2]\textra[3]\textra[4]\textra[5]\textra[6]\textra[7]\textra[8]\textra[9]\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\t23\t24\t25\t26\t27\t28\t29\t30\t31\t32\t33\t34\t35\t36\t37\t38\t39\t40\t41\t42\t43\t44\t45\t46\t47\t48\t49\t50\t51\t52\t53\t54\t55\t56\t57\t58\t59\t60\t61\t62\t63\t64\t65\t66\t67\t68\t69\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

		for (i = 0; i < 256; i++) {
		sprintf((char *)get_text,"%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r",i,scen_data.scen_creatures[i].name,scen_data.scen_creatures[i].level,scen_data.scen_creatures[i].which_to_add_to[0],scen_data.scen_creatures[i].which_to_add_to[1],scen_data.scen_creatures[i].which_to_add_to[2],scen_data.scen_creatures[i].which_to_add_to[3],scen_data.scen_creatures[i].which_to_add_to[4],scen_data.scen_creatures[i].which_to_add_to[5],scen_data.scen_creatures[i].how_much[0],scen_data.scen_creatures[i].how_much[1],scen_data.scen_creatures[i].how_much[2],scen_data.scen_creatures[i].how_much[3],scen_data.scen_creatures[i].how_much[4],scen_data.scen_creatures[i].how_much[5],scen_data.scen_creatures[i].items[0],scen_data.scen_creatures[i].items[1],scen_data.scen_creatures[i].items[2],scen_data.scen_creatures[i].items[3],scen_data.scen_creatures[i].items[4],scen_data.scen_creatures[i].items[5],scen_data.scen_creatures[i].items[6],scen_data.scen_creatures[i].items[7],scen_data.scen_creatures[i].item_chances[0],scen_data.scen_creatures[i].item_chances[1],scen_data.scen_creatures[i].item_chances[2],scen_data.scen_creatures[i].item_chances[3],scen_data.scen_creatures[i].item_chances[4],scen_data.scen_creatures[i].item_chances[5],scen_data.scen_creatures[i].item_chances[6],scen_data.scen_creatures[i].item_chances[7],scen_data.scen_creatures[i].hp_bonus,scen_data.scen_creatures[i].sp_bonus,scen_data.scen_creatures[i].spec_abil,scen_data.scen_creatures[i].default_attitude,scen_data.scen_creatures[i].species,scen_data.scen_creatures[i].immunities[0],scen_data.scen_creatures[i].immunities[1],scen_data.scen_creatures[i].immunities[2],scen_data.scen_creatures[i].immunities[3],scen_data.scen_creatures[i].immunities[4],scen_data.scen_creatures[i].immunities[5],scen_data.scen_creatures[i].natural_armor,scen_data.scen_creatures[i].char_graphic.which_sheet,scen_data.scen_creatures[i].char_graphic.which_icon,scen_data.scen_creatures[i].char_graphic.graphic_adjust,scen_data.scen_creatures[i].char_upper_graphic_sheet,scen_data.scen_creatures[i].small_or_large_template,scen_data.scen_creatures[i].a[0],scen_data.scen_creatures[i].a[1],scen_data.scen_creatures[i].a[2],scen_data.scen_creatures[i].a1_type,scen_data.scen_creatures[i].a23_type,scen_data.scen_creatures[i].ap_bonus,scen_data.scen_creatures[i].default_script,scen_data.scen_creatures[i].default_strategy,scen_data.scen_creatures[i].default_aggression,scen_data.scen_creatures[i].default_courage,scen_data.scen_creatures[i].summon_class,scen_data.scen_creatures[i].extra[0],scen_data.scen_creatures[i].extra[1],scen_data.scen_creatures[i].extra[2],scen_data.scen_creatures[i].extra[3],scen_data.scen_creatures[i].extra[4],scen_data.scen_creatures[i].extra[5],scen_data.scen_creatures[i].extra[6],scen_data.scen_creatures[i].extra[7],scen_data.scen_creatures[i].extra[8],scen_data.scen_creatures[i].extra[9]);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
		sprintf((char *)get_text,"\r\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	FSClose(data_dump_file_id);
}	


void item_data_dump()
{
	short i;
	char get_text[580];
	FILE *data_dump_file_id;
	long len;

	 sprintf(get_text,"%s - Item Data.txt",scenario.scen_name);
	if (NULL == (data_dump_file_id = fopen(get_text, "wb"))) {
		oops_error(11);
		return;
		}

	sprintf((char *)get_text,"\r\rScenario Item Data for %s:\r\r",scenario.scen_name);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"\r\ritem#\tfull_name\tvariety\tdamage\tbonus\tweap_type\tprotection\tcharges\tencumbrance\twhich_sheet\twhich_icon\tgraphic_adjust\tinven_icon\tmissile_anim_type\tability_in_slot[0]\tability_strength[0]\tability_in_slot[1]\tability_strength[1]\tability_in_slot[2]\tability_strength[2]\tability_in_slot[3]\tability_strength[3]\tspecial_class\tvalue\tweight\tvalue_class\tident\tmagic\tcursed\tonce_per_day\tjunk_item\textra[0]\textra[1]\textra[2]\textra[3]\tname\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);

	sprintf((char *)get_text,"1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\t23\t24\t25\t26\t27\t28\t29\t30\t31\t32\t33\t34\t35\t36\r\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	
		for (i = 0; i < NUM_SCEN_ITEMS; i++) {
		sprintf((char *)get_text,"%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\r",(int)i,scen_data.scen_items[i].full_name,scen_data.scen_items[i].variety,scen_data.scen_items[i].damage,scen_data.scen_items[i].bonus,scen_data.scen_items[i].weap_type,scen_data.scen_items[i].protection,scen_data.scen_items[i].charges,scen_data.scen_items[i].encumbrance,scen_data.scen_items[i].item_floor_graphic.which_sheet ,scen_data.scen_items[i].item_floor_graphic.which_icon ,scen_data.scen_items[i].item_floor_graphic.graphic_adjust,scen_data.scen_items[i].inven_icon,scen_data.scen_items[i].missile_anim_type,scen_data.scen_items[i].ability_in_slot[0],scen_data.scen_items[i].ability_strength[0],scen_data.scen_items[i].ability_in_slot[1],scen_data.scen_items[i].ability_strength[1],scen_data.scen_items[i].ability_in_slot[2],scen_data.scen_items[i].ability_strength[2],scen_data.scen_items[i].ability_in_slot[3],scen_data.scen_items[i].ability_strength[3],scen_data.scen_items[i].special_class,scen_data.scen_items[i].value,scen_data.scen_items[i].weight,scen_data.scen_items[i].value_class,scen_data.scen_items[i].ident,scen_data.scen_items[i].magic,scen_data.scen_items[i].cursed,scen_data.scen_items[i].once_per_day,scen_data.scen_items[i].junk_item,scen_data.scen_items[i].extra[0],scen_data.scen_items[i].extra[1],scen_data.scen_items[i].extra[2],scen_data.scen_items[i].extra[3],scen_data.scen_items[i].name);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		}
		sprintf((char *)get_text,"\r\r");
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
	FSClose(data_dump_file_id);
}	
