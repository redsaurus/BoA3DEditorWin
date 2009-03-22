// global.h

// These are the variable structures. You really shouldn't change
// these.

// The comments are partial, intermittent, and may be wrong.

#pragma once

typedef char	t_coord;	// variable type of outdoor and town location coordinate

const size_t kSizeOfScenario_data_type	=  7480;	// actual sizeof() value is 4 byte larger than this value
const size_t kSizeOfOutdoor_record_type	= 12892;	// because of inheritance
const size_t kSizeOfTown_record_type	= 16346;	// but I dont't know the reason, 
const size_t kSizeOfBig_tr_type			= 20480;	// only town_record_type is 6 byte larger than original
const size_t kSizeOfAve_tr_type			= 11520;	// Anyway, these constant is needed because the structure (class) size
const size_t kSizeOfTiny_tr_type		=  5120;	// is varied by the byte alignment setting.

#define	SLEEP_TICKS		0L
#define	MOUSE_REGION	0L
#define DRAG_EDGE		15
#define	NIL				0L


#define	NUM_BUTTONS		16
#define	NUM_P_PER_TOWN		20

#define	NUM_DLOG_B		53

#define	MAX_TOWN_SIZE	64

#define	TER_RECT_UL_X	20
#define	TER_RECT_UL_Y	20
#define	TERRAIN_BORDER_WIDTH	15
#define	LEFT_TEXT_LINE_ULX	15
#define	LEFT_TEXT_LINE_ULY	485
#define	LEFT_TEXT_LINE_WIDTH	240
#define	TEXT_LINE_HEIGHT	12
#define	RIGHT_TEXT_LINE_ULX	5
#define	RIGHT_TEXT_LINE_ULY	487
#define	RIGHT_BUTTONS_X_SHIFT	540

#define	PALETTE_BUT_UL_X	5
#define	PALETTE_BUT_UL_Y	382

#define	NUM_PC_I	34

#define NUM_TALK_NODES	100
#define NUM_SCEN_ITEMS	500

#define	SMALL_SPACE_SIZE	8
#define	BIG_SPACE_SIZE	48

// q_3DModStart
#define SPACE_X_DISPLACEMENT_3D  24
#define SPACE_Y_DISPLACEMENT_3D  16
#define ELEVATION_Y_DISPLACEMENT_3D  23
#define PICT_BOX_WIDTH_3D	46
#define PICT_BOX_HEIGHT_3D   55
#define ITEM_BOX_SIZE_3D   28
#define OUTDOOR_CREATURE_WIDTH_3D   11
#define OUTDOOR_CREATURE_HEIGHT_3D	16

#define	TER_BUTTON_HEIGHT_3D	19
// q_3DModEnd

// new blades consts
#define	SCRIPT_NAME_LEN	14

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

#define NUM_WAYPOINTS	10

#define	TER_BUTTON_SIZE	16
#define	PALETTE_BUT_HEIGHT		17
#define	PALETTE_BUT_WIDTH		25

#define NUM_TER_SCRIPTS	100

#define MAX_NUM_SHEETS_IN_LIBRARY	200

#define NUM_TOWN_PLACED_ITEMS	144
#define NUM_TOWN_PLACED_FIELDS	60
#define NUM_TOWN_PLACED_CREATURES	80
#define NUM_TOWN_PLACED_SPECIALS	60
#define NUM_OUT_PLACED_SPECIALS	30

#define kNO_TOWN_SPECIALS		0xFF	// No Special encounter on town map
#define kNO_OUT_SPECIALS		-1		// No special encounter on outdoor map

#define kINVAL_LOC_XY			-1		// invalid location, x or y
#define kINVAL_LOC				{kINVAL_LOC_XY, kINVAL_LOC_XY}	// invalid location


#define SDF_RANGE_X	300
#define SDF_RANGE_Y	30




typedef char Boolean;

typedef struct
{
     PBYTE    * ppRow ;            // must be first field for macros!
     int        iSignature ;
     HBITMAP    hBitmap ;
     BYTE     * pBits ;
     DIBSECTION ds ;
     int        iRShift[3] ;
     int        iLShift[3] ;
}
DIBSTRUCT, * PDIBSTRUCT ;
typedef void * HDIB ;

typedef struct {
	char x,y;
	} location;
typedef struct {
	short top,left,bottom,right;
	} macRECT;

//typedef struct {
//	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
//	} special_node_type;

typedef struct {
	short which_state,which_personality;
	short condition1[4],condition2[4];
	short action_when_selected,default_jump_state;
	short action_mods[4];
	} talking_node_type;

typedef struct {
	unsigned char short_strlens[NUM_TALK_NODES];
	unsigned char strlens[NUM_TALK_NODES * 2];
	char char_names[NUM_P_PER_TOWN][20];
	short p_start_node[NUM_P_PER_TOWN];
	short p_start_state[NUM_P_PER_TOWN];
	talking_node_type talk_nodes[NUM_TALK_NODES];
	} talking_record_type;



// The data for the locaiton for an icon in memory.
class graphic_id_type {
public:
	graphic_id_type();
	void clear_graphic_id_type();
	Boolean not_legit();
	
	short which_sheet; // the resource number of the graphic
	short which_icon; // the icon in the resource. icons are in resources 
		// 16 icons per row
	short graphic_adjust; // not yet used
	};


class floor_type_type {
public:
	floor_type_type();
	void clear_floor_values();
	
	char floor_name[20];
	graphic_id_type pic; // regular picture location for ground
	graphic_id_type ed_pic; // picture location for editor graphic for icon
	short blocked; // 0 no 1 yes
 	char step_sound; // -1 no sound, otherwise sound when stepped on
 	char light_radius; // radius terrain sends its light out to, 0 - no light
 	short floor_height; // in pixels, positive is up, shows how many vertical pixels
 						// to offset graphics when drawn
	short num_anim_steps;
	
	// special properties of floor
	short special; // floor special property
	short effect_adjust; // intensity of special effect
	// described in the editor docs
	
	// special editor and movement toggles
	Boolean is_water; // water?
	Boolean is_floor; // a human built (stone or wood) floor
	Boolean is_ground; // regular grass, cave floor?
	Boolean is_rough; // rought errain? (i.e. desert or rough cave)
	Boolean fly_over; // can this terrain be flied over?
	Boolean shimmers; // does this graphic get drawn with graphical effects?
		// 0 - none
		// 1 - glows light/dark
		// 2 - water effect
	
	short outdoor_combat_town_used; // The town that is loaded when party fights an outdoor
		// combat on this terrain type. If 0-999, loads the town in the current scenario/
		// If 1000 + x, loads town x in file Blades of Avernum Out Fight
		// 0 - 
	unsigned char shortcut_key;
	short extra[6];
	};

class terrain_type_type {
public:
	terrain_type_type();
	void clear_terrain_values();

	char ter_name[20];
	graphic_id_type pic; // regular picture location for when above ground
	graphic_id_type cut_away_pic; // picture when the terrain is being drawn cut away
	graphic_id_type ed_pic; // picture location for editor graphic for icon
	
	// the icon can be drawn with a certain pixel offset.
	short icon_offset_x,icon_offset_y;
	
	// a terrain spot can have a second icon drawn, which must be in the same sheet as the first icon.
	// it affects the outdoor and underground terrain versions equally.
	// there can be a different icon for the front and cut away view.
	short second_icon,second_icon_offset_x,second_icon_offset_y;
	short cut_away_second_icon,cut_away_offset_x,cut_away_offset_y;
		// For now, cut_away_offset_x & cut_away_offset_y are UNUSED
		// Use other offsets instead.
	short anim_steps; // number of icons in this terrain's animation. uses icons after
		// first icon in sheet for animation
	unsigned char move_block[4]; // 0 - no block, 1 - block movement, 0 - n, 1 - w, 2 - s, 3 - e
	unsigned char see_block[4]; // 0 - no block, 1 - sight movement, 0 - n, 1 - w, 2 - s, 3 - e
	unsigned char blocks_view[4]; // 0 - draw after pc in spot, 1 - draw before, 0 - n, 1 - w, 2 - s, 3 - e
	short height_adj; // pixels up to draw chars/items in this space
	unsigned char suppress_floor; // 0 - no, if 1, floor type under terrain type is ignored
	unsigned char light_radius; // radius of light coming from this spot
	char step_sound; // -1 - no sound, takes precedence over floor sound

	unsigned char shortcut_key;
	
	// various special properties
	short crumble_type; // 0 - won't crumble, 1-5 - strength of speel needed to make crumble
	short beam_hit_type; // 0 - blocks beam, 1 - allows beam through
		// 3 - crumbles when beam hits
	short terrain_to_crumble_to;
	short hidden_town_terrain; // -1 - unused. otherwise, terrain to place here if this is
		// icon for a hidden town
	short swap_terrain; // ter to swap to if a swap terrain special call is used
	Boolean is_bridge; // allows people to cross this space even if floor is a blocking type
	Boolean is_road;
	Boolean can_look_at; // a terrain which is always selected to be inspected when player looks
	Boolean draw_on_automap;
	char default_script[SCRIPT_NAME_LEN]; // the name of the default script
	Boolean shimmers; // does this graphic get drawn with graphical effects?
		// 0 - none
		// 1 - glows light/dark
		// 2 - water effect

	short outdoor_combat_town_used; // The town that is loaded when party fights an outdoor
		// combat on this terrain type. If 0-999, loads the town in the current scenario/
		// If 1000 + x, loads town x in file Blades of Avernum Out Fight
		// 0 - 
	
	// special properties of terrain
	short special; // floor special property
	short effect_adjust; // intensity of special effect
	// below may be out of date. full info inj editor docs
	// special		effect			effect_adjust
	// 0			none
	// 1			fire damage		amt. of damage
	// 2			cold damage		amt. of damage
	// 3			magic damage	amt. of damage
	// 4			poison			amt. of damage
	// 5			disease			amt. of damage
	// 6			blocks monsts
	
	// Hill abilities
	// 19			hill, up to west
	// 20			hill, up to southwest
	// 21			hill, up to south
	// 22			hill, up to southeast
	// 23			hill, up to east
	// 24			hill, up to northeast
	// 25			hill, up to north
	// 26			hill, up to northwest
	// 27			hill, down to southeast
	// 28			hill, down to northeast
	// 29			hill, down to northwest
	// 30			hill, down to southwest
	
	// Beam abilities
	// 31			beam power source, if charged by beam	
	// 32			fires beam north
	// 33			fires beam west
	// 34			fires beam south
	// 35			fires beam east
	// 36			nw/se mirror
	// 37			ne/sw mirror
	// 38			beam power source

	// 39			sign
	// 40			container
	// 41			acts as table
	// 42			glows
	// 43			waterfall - south
	// 44			waterfall - west
	// 45			destroyed by quickfire	
	} ;

class out_wandering_type {
public:
	out_wandering_type();
	void clear_out_wandering_type();
	void port();
	
	short hostile[4],hostile_amount[4];
	short friendly[3],friendly_amount[3];

	short cant_flee;
		// this encounter can ever be evaded?
	short end_spec1,end_spec2;
		// if this flag is set, this encounter never appears.
	short killed_flag1,killed_flag2;
		// this flag is set to 1 when enc killed.
		// can, and probably should, be same as end_spec
		
	short forced; // if > 0, party encoutners this immediately
	short check_every_turn; // most encoutners checked every 10 turns.
		// if this is 1, check this one every turn
	short move_type;
		// 0 - seek party  1 - no move  2 - random move  3 - follow roads  4 - flee party
		// +10 - always stays within 10 spaces of starting spot
	location start_loc; // location encounter first appears
	short start_state_when_encountered;
	short start_state_when_defeated;
	short start_state_when_fled;
	short random_move_chance; // chance (0-100) that this creature will move randomly at
		// any move instead of doing what it wants
} ;

class outdoor_record_type {
public:
	outdoor_record_type();
	void clear_outdoor_record_type();
	void port();
	void SetSurface( short onSurface );

	char name[20];
	unsigned char floor[48][48],height[48][48];
	short terrain[48][48];
	macRECT special_rects[NUM_OUT_PLACED_SPECIALS];
	short spec_id[NUM_OUT_PLACED_SPECIALS];
	macRECT exit_rects[8];
	short exit_dests[8];

	// signs
	location sign_locs[8];
	char sign_text[8][256];
	
	// wandering
	out_wandering_type wandering[4],special_enc[4],preset[8];
	location wandering_locs[4];

	macRECT info_rect[8];
	char info_rect_text[8][30];

	// scripts and special flags
	char section_script[SCRIPT_NAME_LEN]; // the name of the default script
	short is_on_surface;
	
	short extra[10];
	} ;


class creature_type {
public:
	creature_type();
	void clear_creature_type();
	
	char name[20];
	short level; // level of creature
	short which_to_add_to[6]; // which 6 skills to give bonuses to
	short how_much[6]; // how much to add to each skill
	short items[8]; // numbers of items it has
	short item_chances[8]; // chance, from 0 to 100, of the item being present
	short hp_bonus;
	short sp_bonus;
	short spec_abil; 
		// 0 - 
	short default_attitude; // 0 - PC, 1 - PC aid, 2 friendly, 3 - neutral, 4 - hostile A, 5 - hostile B
	short species;
		// 0 - human
		// 1 - humanoid
		// 2 - nephil
		// 3 - slith
		// 4 - giant
		// 5 - reptile
		// 6 - beast
		// 7 - demon
		// 8 - undead
		// 9 - insect
		// 10 - slime
		// 11 - stone/golem
		// 12 - special
		// 13 - vahnatai
		// 14-19 - other/ scenario designed
	unsigned char immunities[6]; 
		// 0 - fire, 1 - cold, 2 - magic, 3 - mental, 4 - poison/acid, 5 - melee
	short natural_armor; // like armor, starts at 0

	// graphic
	graphic_id_type char_graphic;
	short char_upper_graphic_sheet;
	short small_or_large_template;

	short a[3],a1_type,a23_type;
		// attack types
		// 0 - strike
		// 1 - claw
		// 2 - bite
		// 3 - slimes
		// 4 - punches
		// 5 - stings
		// 6 - clubs
		// 7 - burns
		// 8 - harms
		// 9 - stabs
	short ap_bonus;

	char default_script[SCRIPT_NAME_LEN]; 
	short default_strategy;
		// 0 - default, regular attack
		// 1 - archer/caster, so maintain distance
		// 2-9 - reserved
	short default_aggression,default_courage;
	short summon_class; // -1 - never summoned, otherwise, number from 0-6, higher = harder to summon
	short extra[10];
	};

class item_record_type {
public:
	item_record_type();
	void clear_item_record_type();

	short variety;
		// 0 - null item
		// 1 - 1 handed weapon
		// 2 - 2 handed Weapon
		// 3 - Gold
		// 4 - Food
		// 5 - Thrown Missile
		// 6 - Bow
		// 7 - Potion
		// 8 - Scroll
		// 9 - Wand
		// 10 - Tool
		// 11 - Pants
		// 12 - Shield
		// 13 - Armor
		// 14 - Helm
		// 15 - Gloves
		// 16 - Boots
		// 17 - Cloak
		// 18 - Ring
		// 19 - Necklace
		// 20 - Bracelet
		// 21 - Object
		// 22 - Crossbow
		// 23 - Arrows
		// 24 - Bolts
	short damage; // weapons do (1 - damage/2) + (1 - damage/2)
	char bonus; // amt. added to weapon damage, and to hit increased by %5 * bonus
	char weap_type; // number of skill used for weapon, such as 4 for melee
	char protection; // prevents (1 - protection/2) + (1 - protection/2) melee damage
	char charges;
	short encumbrance;
	graphic_id_type item_floor_graphic;
	short inven_icon;
	short missile_anim_type;

	// item abilities
	short ability_in_slot[4]; // -1 or 0 - no abil
		// otherwise, number of ability
		// probably out of date. consult editor docs for full list
		// PASSIVE ABILITIES
		// 1 - 49 - Affects statistic x + 1. So if 3, affects statistic 2 (Intelligence)
		// 50 - Melee to hit chance
		// 51 - Melee damage
		// 52 - Missile to hit chance
		// 53 - Missile damage
		// 54 - Resist all hostile effects
		// 55 - Resist fire
		// 56 - Resist cold
		// 57 - Resist magic
		// 58 - Resist mental
		// 59 - Resist poison
		// 60 - Resist acid
		// 61 - Affect action points
		// 62 - Affect melee statistics
		// 63 - Affect magic statistics
		// 64 - Affect all statistics
		// 65 - Affect rune reading
		// 70 - Extra fire damage (only has effect on a weapon)
		// 71 - Extra acid damage (only has effect on a weapon)
		// 72 - Extra poison damage (only has effect on a weapon)
		// 73 - Extra damage to humanoids (only has effect on a weapon)
		// 74 - Extra damage to undead (only has effect on a weapon)
		// 75 - Extra damage to demons (only has effect on a weapon)
		// 76 - Extra damage to giants (only has effect on a weapon)
		// 77 - Drains life, adds to wielder (only has effect on a melee weapon)
		// 78 - Extra damage to reptiles (only has effect on a weapon)
		// 79 - Extra damage to aliens (only has effect on a weapon)
		// 80 - Encumbers
		// 81 - Resist melee damage
		// 82 - Resist all damage
		// 83 - Affects chance to be hit in combat
		// 84 - Protects from petrification
		// ACTIVE ABILITIES
		// 100-119 - Casts mage spell 100 + x.
		// 120-139 - Casts priest spell 120 + x.
		// 150 - 199 - Affects status 150 + x.
		// 200 - heals damage
		// 201 - cures bad effects
		// 202 - Adds experience.
		// 203 - Adds skill points.
		// 204 - Adds spell energy.
		// 205 - Cleanses webs and disease.
		// 207 - Calls a scenario script state. Doesn't lose a charge when used. Items with this ability can't be carried between scenarios
		// 208 - Calls a scenario script state. Loses a charge when used. Items with this ability can't be carried between scenarios
		// 209 - Whole party resists mental effects
		// 210 - Increases light.
		// 211 - is a lockpick.
		// 212 - Is a First Aid Kit.
		// 213 - Gives whole party flight.
		// 214 - Afflicts user with bad effects.
		// 215 - Permanently change 1 statistic.
	short ability_strength[4]; // strength of ability, -50 to 50
		
	short special_class; // 0 - no class
	short value;
	short weight;
	short value_class; // Represents the item's value for random treasures
		// from 0 - 10, if -1, no class.
	
	// property flags
	unsigned char ident; // if 1, always ident
	unsigned char magic; // if 1, magic item
	unsigned char cursed; // if 1, cursed
	unsigned char once_per_day; // if 1, always ident
	unsigned char junk_item; // if 1, can delete this item to make room in town
	unsigned char extra[4];
	char full_name[30],name[20];
	short augment_item;
} ;

class item_type {
public:
	item_type();
	void clear_item_type();
	Boolean exists();
	void port();
	
	short which_item; 
		// -1 - null item
		// if 0 - 399, item from scenario list
		// if 1000 + x, is item x is party imported_items list
	location item_loc;
	unsigned char charges;
	unsigned char properties; 
		// bit 0 - ident   1 - property   2 - contained   3 - cursed 4 - used today
	location item_shift;
	} ;


class creature_start_type {
public:
	creature_start_type();
	void clear_creature_start_type();
	Boolean exists();
	void port();
	
	short number; // numebr of creature in roster, -1 for none here
	short facing;
	short start_attitude;// 0 - PC, 1 - PC aid, 2 friendly, 3 - neutral, 4 - hostile A, 5 - hostile B
	location start_loc;
	short personality;
	short extra_item;
	short extra_item_chance_1;
	short extra_item_2;
	short extra_item_chance_2;
	short hidden_class;
	short character_id;
		// a number from 0-20000, a unique id for this character

	// timing variable
	short time_flag;
		// Indicates when this char appears. Types 0-9 are only there if
		// the town has not been destroyed.
		// 0 - always here (unless town has been destroyed)
		// 1 - appear at time t, unless town destroyed
		// 2 - disappear at time t
		// 3 - appear if event attached_event not done by time t
		// 4 - disappear if event attached_event not done by time t
		// 5 - appear if event attached_event done
		// 6 - disappear if event attached_event done
		// 7 - here on day 0-2 of every 9 days
		// 8 - here on day 3-5 of every 9 days
		// 9 - here on day 6-8 of every 9 days
		// 10 - here if and only if town has been destroyed
	short creature_time;
	short attached_event;
		// the event that this character is tied to (like a troglo to the
		// troglos killed event)
	short unique_char;
	
	// scripting
	char char_script[SCRIPT_NAME_LEN]; // the name of the default script
	short memory_cells[10];
	short act_at_distance;
	
	short extra[4];
	};
	
class preset_field_type {
public:
	preset_field_type();
	void clear_preset_field_type();
	void port();
	
	location field_loc;
	short field_type;
	} ;

// when a terrain spot has a script, the detains for that script are remembered in
// this record. It a terrain spot has a script, it MUST has a corresponding
// record of this sort active
class in_town_on_ter_script_type {
public:
	in_town_on_ter_script_type();
	void clear_in_town_on_ter_script_type();
	void port();

	short exists;
	location loc; // location of the terrain spot this is for
	char script_name[SCRIPT_NAME_LEN]; 
	short memory_cells[10];
	};


class town_record_type {
public:
	town_record_type();
	void clear_town_record_type();
	void set_start_locs(short town_size);
	void port();

	char town_name[20];
	short respawn_monsts[4][6];
	location respawn_locs[6];
	macRECT special_rects[NUM_TOWN_PLACED_SPECIALS];
	unsigned char spec_id[NUM_TOWN_PLACED_SPECIALS];
	short lighting;

	// signs
	location sign_locs[15];
	char sign_text[15][256];

	location start_locs[4];
	location exit_locs[4];
	macRECT in_town_rect;
	item_type preset_items[NUM_TOWN_PLACED_ITEMS];	
	preset_field_type preset_fields[NUM_TOWN_PLACED_FIELDS];

	short wall_1_sheet,wall_1_height,wall_2_sheet,wall_2_height,cliff_sheet;
		// how high to make walls and what sheets to look in to get wall graphics
	short beam_type;
		// sort of beams which might be present in level
		// 0 - never, 1 - damage, 2 - impass
	short environmental_sound; // sound to play in background
	short is_on_surface;
	short town_kill_day,town_linked_event;
		// town is cleaned out is event town_linked_event is not done by day town_kill_day
	short external_floor_type;
	short monster_respawn_chance;
	
	// scripts and specials
	char town_script[SCRIPT_NAME_LEN]; 
	in_town_on_ter_script_type ter_scripts[NUM_TER_SCRIPTS];
	
	macRECT room_rect[16];
	char info_rect_text[16][30];
	creature_start_type creatures[NUM_TOWN_PLACED_CREATURES];
	short extra[20];
	
	location waypoints[NUM_WAYPOINTS];

	short exit_specs[4];
	short spec_on_entry,spec_on_entry_if_dead;

	} ;


class ave_tr_type;
class tiny_tr_type;

class big_tr_type {
public:
	big_tr_type();
	big_tr_type( big_tr_type& bigTR );
	void clear_big_tr_type();
	void port();
	big_tr_type& operator = ( const ave_tr_type& aveTR );
	big_tr_type& operator = ( const tiny_tr_type& tnyTR );

	short terrain[64][64];
	unsigned char floor[64][64];
	unsigned char height[64][64];
	unsigned char lighting[64][64];
	} ;

class ave_tr_type {
public:
	ave_tr_type();
	ave_tr_type( big_tr_type& bigTR );
	void clear_ave_tr_type();
	void port();
	ave_tr_type& operator = ( const big_tr_type& bigTR );

	short terrain[48][48];
	unsigned char floor[48][48];
	unsigned char height[48][48];
	unsigned char lighting[48][48];
	} ;
	
class tiny_tr_type {
public:
	tiny_tr_type();
	tiny_tr_type( big_tr_type& bigTR );
	void clear_tiny_tr_type();
	void port();
	tiny_tr_type& operator = ( const big_tr_type& bigTR );

	short terrain[32][32];
	unsigned char floor[32][32];
	unsigned char height[32][32];
	unsigned char lighting[32][32];
	} ;	


typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
	} item_storage_shortcut_type;

class scen_item_data_type {
public:
	scen_item_data_type();
	void clear_scen_item_data_type();
	
	item_record_type scen_items[NUM_SCEN_ITEMS];
	floor_type_type scen_floors[256];
	terrain_type_type scen_ter_types[512];
	creature_type scen_creatures[256];
	} ;

class boat_record_type {
public:
	boat_record_type();
	void clear_boat_record_type();
	
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	Boolean exists,property;
	} ;
class horse_record_type {
public:
	horse_record_type();
	void clear_horse_record_type();
	
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	Boolean exists,property;
	} ;


class scenario_data_type {
public:
	scenario_data_type();
	void clear_scenario_data_type();
	short scenario_platform();
	void port();

	unsigned char flag1, flag2, flag3, flag4;
	// for mac, flags are 97 215 7 33
	// for win, flags are 199 61 2 53
	unsigned char ver[3];
	unsigned char min_run_ver;
	unsigned char prog_make_ver[3];
	unsigned char num_towns;
	short out_width,out_height;
	short min_level,max_level;
	short rating;
		
	// labels when selecting scen
	char scen_name[50];
	short scen_label_pic;
	char scen_desc[256];
	
	// credits text
	char credits_text[2][256];
	char comments_text[2][256];
	
	// intro stuff, plays when scen started
	short intro_pic_resources[3];
	char intro_text[3][6][256];
	
	// town data
	unsigned char town_size[200]; // 0 - large, 1 - medium, 2 - small
	unsigned char town_starts_hidden[200];
	
	// starting loc
	short start_in_what_town;
	location what_start_loc_in_town;
	location what_outdoor_section_start_in;
	location start_where_in_outdoor_section;

	// changing town entrances
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];

	boat_record_type scen_boats[30];
	horse_record_type scen_horses[30];
	item_storage_shortcut_type storage_shortcuts[10];

	location last_out_edited;
	short last_town_edited;
} ;

// general script records

// Script stuff
#define MAX_DESCRIBER_LEN	30
#define	NUM_SCRIPT_STRINGS	750
#define	SCRIPT_VAR_NAME_LEN	20
#define	NUM_SCRIPT_INTS	20
#define	NUM_SCRIPT_LOCATIONS	10
#define	NUM_SCRIPT_STRING_VARS	20
#define	NUM_PROCEDURE_PASS_VARS	5

// a block of text, loaded in from a file.
class text_block_type
{
public:
	text_block_type();
	~text_block_type();
	Boolean load_text_file_into_buffer(char *file_name_to_load,short file_location);
	void preprocess_text();
	short estimate_num_of_tokens();
	
	char *text_block;
	long block_length;
};

// A token, a basic unit of meaning in a script (like a procedure name, or a number).
// The list of 20-odd different token types can be found in tokntype.h.
class token_type
{
public:
	token_type();
	
	unsigned char type;
	short line;
	short what_sort;
};

// A token, a basic unit of meaning in a script.
typedef struct
{
	char token_text[MAX_DESCRIBER_LEN];
} token_describer_type;



// Stores a loaded script, in tokenized form.
class script_type
{
public:
	script_type();
	~script_type();
	void flush_data();
	Boolean load_script(short type_of_script_to_load,char *script_to_load,short file_location);
	Boolean IsWhiteSpace(char c);
	Boolean IsIdentifier(char c);
	Boolean IsNumber(char c);
	Boolean IsOperatorCharacter (char c);
	short IsVariable (char * string, int length, token_type *token);
	short IsBlockDefiner (char * string, int length, short *value);
	short IsVarDefiner (char * string, int length, short *value);
	short IsVarArrayDefiner (char * string, int length, short *value);
	short IsVarStringDefiner (char * string, int length, short *value);
	short IsNewVariableDefiner (char * string, int length, short *value);
	short IsFlowController (char * string, int length, short *value);
	short IsConstant (char * string, int length, short *value);
	short IsOperator (char * string, int length, short *value);
	short IsFunction (char * string, int length, short *value);
	short IsProcedure (char * string, int length, short *value);
	short IsBinaryFunction (char * string, int length, short *value);
	short IsTrinaryFunction (char * string, int length, short *value);
	short IsLocationFunction (char * string, int length, short *value);
	Boolean process_scenario_data();
//	Boolean process_creature_data();
	Boolean token_type_match(short which_token,short type);
//	Boolean token_sort_match(short which_token,short what_sort);
	Boolean token_match(short which_token,short type,short what_sort);
//	short find_first_matching_token(short type_of_token,short what_sort = -1);
//	short find_next_matching_token(short start_token,short type_of_token,short what_sort = -1);
//	Boolean semicolon_check(short check_token);
//	short find_start_of_state(short which_state);
//	Boolean run_script(short next_token);
//	Boolean evaluate_int_expression(short next_token,short *result,short *new_position);
//	Boolean higher_precedence (token_type op1, token_type op2);
//	short evaluate_unary_function(short fcn_type,short op1,short which_line);
//	short evaluate_binary_function(short fcn_type,short op1,short op2,short which_line);
//	Boolean run_procedure(short which_procedure,short which_line);
//	short find_previous_matching_token(short start_token,short type_of_token,short what_sort);
//	short find_matching_bracket(short start_token);
//	short end_of_current_line(short start_token);
//	short find_conditional_matching_bracket(short start_token);
//	location location_function_value(short what_function,short what_passed,short what_line);
//	Boolean run_creature_script();
//	short evaluate_trinary_function(short fcn_type,short op1,short op2,short op3,short which_line);
	short IsNoParameterFunction (char * string, int length, short *value);
//	short evaluate_no_param_function(short fcn_type,short which_line);
//	Boolean run_zone_script();
//	Boolean process_dialogue_data();
	
	// basic script information
	char script_name[20];
	short type_of_script;
		// -1 - undefined
		// 0 - Scenario data
		// 1 - Scenario script
		// 2 - Town - specials
		// 3 - Town - dialogue
		// 4 - Outdoor - specials
		// 5 - Outdoor - dialogue
		// 6 - Creature
		// 7 - Terrain Spot		
	token_type *token_list;
	short num_tokens;
	Boolean script_killed; // If the script hits a run-time error,
		// this makes sure it's never called again.
	short which_object; // The number of the creature/object this is for.
		// only relevant if type_of_script is 2,3.
			
	// script variables
	char int_var_names[NUM_SCRIPT_INTS][SCRIPT_VAR_NAME_LEN];
	short int_var_values[NUM_SCRIPT_INTS];
	char location_var_names[NUM_SCRIPT_LOCATIONS][SCRIPT_VAR_NAME_LEN];
	location location_var_values[NUM_SCRIPT_LOCATIONS];
	char string_var_names[NUM_SCRIPT_STRING_VARS][SCRIPT_VAR_NAME_LEN];
	char *string_var_values[NUM_SCRIPT_STRING_VARS];

	// temporary holder for the script text
	char *string_data[NUM_SCRIPT_STRINGS];
	
	// stacks for running procedures
	short procedure_passed_variable_types[NUM_PROCEDURE_PASS_VARS];
	//	0 - int 1 - loc 2 - string constant 3 - string variable
	short procedure_passed_values[NUM_PROCEDURE_PASS_VARS];
	location procedure_passed_locations[NUM_PROCEDURE_PASS_VARS];
};

/*
class boe_outdoor_record_type{
public:
	void clear_boe_outdoor_record_type();
	unsigned char	terrain_variety[48][48];
	old_blades_outdoor_record_type;
	};
*/

// begin Blades of Exile records
// This junk only used for routines to port BoE scenarios.

typedef struct {
	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
	} old_blades_special_node_type;

typedef struct {
	short personality,type;
	char link1[4],link2[4];
	short extras[4];
	} old_blades_talking_node_type;

typedef struct {
	unsigned char strlens[200];
	old_blades_talking_node_type talk_nodes[60];
	} old_blades_talking_record_type;

typedef struct {
	short picture;
	unsigned char blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over;
	unsigned char block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3;
	} old_blades_terrain_type_type;
	
typedef	struct {
	unsigned char monst[4];
	} old_blades_wandering_type;

typedef struct {
	unsigned char monst[7];
	unsigned char friendly[3];
	short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
	short end_spec1,end_spec2;
} old_blades_out_wandering_type;

typedef struct {
	unsigned char	terrain[48][48];
	location	special_locs[18];
	unsigned char		special_id[18];
	location	exit_locs[8];
	char		exit_dests[8];
	location	sign_locs[8];
	old_blades_out_wandering_type	wandering[4],special_enc[4];
	location	wandering_locs[4];
	macRECT info_rect[8];
	unsigned char strlens[180];
	old_blades_special_node_type specials[60];
	} old_blades_outdoor_record_type;

typedef struct {
	unsigned char number;
	unsigned char start_attitude;
	location start_loc;
	unsigned char  mobile;
	unsigned char  time_flag;
	unsigned char  extra1,extra2;
	short spec1, spec2;
	char spec_enc_code,time_code;
	short monster_time,personality;
	short special_on_kill,facial_pic;
	
	} old_blades_creature_start_type;


typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type;
	unsigned char graphic_num,ability, type_flag, is_special;
	short value;
	Boolean identified, magic;
	unsigned char weight, description_flag;
	char full_name[25], name[15];
	unsigned char reserved1,reserved2;
	unsigned char magic_use_type, ability_strength, treas_class, real_abil;
} old_blades_short_item_record_type;


typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type, magic_use_type;
	unsigned char graphic_num,ability, ability_strength,type_flag, is_special;
	short value;
	unsigned char weight, special_class;
	location item_loc;
	char full_name[25], name[15];
	unsigned char treas_class,item_properties,reserved1,reserved2;
} old_blades_item_record_type;

typedef struct {
	location item_loc;
	short item_code,ability;
	unsigned char charges,always_there,property,contained;
	} old_blades_preset_item_type;
	
typedef struct {
	location field_loc;
	short field_type;
	} old_blades_preset_field_type;
	
typedef struct {
	short town_chop_time,town_chop_key;
	old_blades_wandering_type	wandering[4];
	location	wandering_locs[4];
	location	special_locs[50];
	unsigned char		spec_id[50];
	location	sign_locs[15];
	short		lighting;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	macRECT in_town_rect;
	old_blades_preset_item_type preset_items[64];
	short max_num_monst;
	old_blades_preset_field_type preset_fields[50];
	short spec_on_entry,spec_on_entry_if_dead;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	old_blades_special_node_type specials[100];
	unsigned char specials1,specials2,res1,res2;
	short difficulty;
	} old_blades_town_record_type;



typedef struct {
	unsigned char terrain[64][64];
	macRECT room_rect[16];
	old_blades_creature_start_type creatures[60];
	unsigned char lighting[8][64];
	} old_blades_big_tr_type;

typedef struct {
	unsigned char terrain[48][48];
	macRECT room_rect[16];
	old_blades_creature_start_type creatures[40];
	unsigned char lighting[6][48];
	} old_blades_ave_tr_type;
	
typedef struct {
	unsigned char terrain[32][32];
	macRECT room_rect[16];
	old_blades_creature_start_type creatures[30];
	unsigned char lighting[4][32];
	} old_blades_tiny_tr_type;	

typedef struct {
	old_blades_item_record_type scen_items[400];
	char monst_names[256][20];
	char ter_names[256][30];
	} old_blades_scen_item_data_type;

typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
	} old_blades_item_storage_shortcut_type;

typedef struct {
	unsigned char m_num,level,m_name[26];
	short health,m_health,mp,max_mp;
	unsigned char armor,skill;
	short a[3];
	unsigned char a1_type,a23_type,m_type,speed,ap,mu,cl,breath,breath_type,treasure,spec_skill,poison;
	short morale,m_morale;
	short corpse_item,corpse_item_chance;
	short status[15];
	unsigned char direction,immunities,x_width,y_width,radiate_1,radiate_2;
	unsigned char default_attitude,summon_type,default_facial_pic,res1,res2,res3;
	short picture_num;
	
	} old_blades_monster_record_type;
	
typedef struct {
	short active,attitude;
	unsigned char number;
	location m_loc;
	old_blades_monster_record_type m_d;
	Boolean mobile;
	short summoned;
	old_blades_creature_start_type monst_start;
	} old_blades_creature_data_type;
	
	
typedef struct {
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	Boolean exists,property;
	} old_blades_horse_record_type;
typedef struct {
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	Boolean exists,property;
} old_blades_boat_record_type;

typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	} scen_header_type;
	
	typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	unsigned char town_size[200];
	unsigned char town_hidden[200];
	short flag_a;
	short intro_mess_pic,intro_mess_len;
	location where_start,out_sec_start,out_start;
	short which_town_start;
	short flag_b;
	short town_data_size[200][5];
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];
	short flag_c;
	short out_data_size[100][2];
	macRECT store_item_rects[3];
	short store_item_towns[3];
	short flag_e;
	short special_items[50];
	short special_item_special[50];
	short rating,uses_custom_graphics;
	short flag_f;
	old_blades_monster_record_type scen_monsters[256];
	//old_blades_boat_record_type scen_boats[30];
	//old_blades_horse_record_type scen_horses[30];
	boat_record_type scen_boats[30];
	horse_record_type scen_horses[30];
	short flag_g;
	old_blades_terrain_type_type ter_types[256];
	short scenario_timer_times[20];
	short scenario_timer_specs[20];
	short flag_h;
	old_blades_special_node_type scen_specials[256];
	old_blades_item_storage_shortcut_type storage_shortcuts[10];
	short flag_d;
	unsigned char scen_str_len[300];
	short flag_i;
	location last_out_edited;
	short last_town_edited;

	} old_blades_scenario_data_type;

typedef struct {
	char town_strs[180][256];
	char out_strs[120][256];
	char scen_strs[270][256];
	char talk_strs[170][256];
	} old_blades_piles_of_stuff_dumping_type;
	
// end BoE records

//undo system data types
	
//The printable class was made the parent of a number of other classes 
//for debugging purposes. These other classes override the print() 
//function to print a description of themselves to the standard output.
class printable{
public:
	virtual void print()
	{
		printf("printable!");
	}
	virtual ~printable(){}
};

//A listNode is a wrapper object so that items can be stored in a 
//linkedList. It should not be needed except by the linkedList class.
class listNode: public printable{
public:
	listNode* next;
	void* data;
	
	listNode():next(NULL), data(NULL)
	{}
	listNode(listNode* n, void* d):next(n), data(d)
	{}
	void print()
	{
		printf("listnode with data=%p and next=%p",data,next);
	}
};

//This is a linked list class mostly intended for use as a stack,
//items are added and removed using the push() and pop() functions
//However, it can also be accessed like an array; fetching the item
//at an arbitrary index. This is normally a slow operation on a 
//linked list, so this class caches the address of the most recently
//accessed item, allowing that item or the item after it to be retrieved 
//in constant time. This means that iterating through the elements of 
//the list in order is must faster than it would otherwise be. 
//And CFArray is EVIL! EVIL!!!
class linkedList: public printable{
private:
	listNode* start;
	int numItems;
	listNode* currentItem;
	int indexOfCurrentItem;
public:
		linkedList()
	{
			start=NULL;
			numItems=0;
			currentItem=NULL;
			indexOfCurrentItem=-1;
	}
	~linkedList()
	{
		this->clear();
	}
	int size()
	{
		return(numItems);
	}
	void push(void* newItem)
	{
		listNode* temp = new listNode(start,newItem);
		start=temp;
		currentItem=start;
		indexOfCurrentItem=0;
		numItems++;
	}
	void* pop()
	{
		if(start == NULL)
			return(NULL);
		listNode* temp = start;
		start = temp->next;
		currentItem=start;
		if(start==NULL)
			indexOfCurrentItem=-1;
		else
			indexOfCurrentItem=0;
		void* data = temp->data;
		delete temp;
		numItems--;
		return(data);
	}
	void* itemAtIndex(int i)
	{
		if(i<0 || i>=numItems)
			return(NULL);
		if(indexOfCurrentItem>=0){//we have a cached index, test if it's useful
			if(i==indexOfCurrentItem)//the caller wants the item at the cached index
				return(currentItem->data);
			if(i==(indexOfCurrentItem+1)){//the caller wants the next item after the cached index
				currentItem=currentItem->next;
				indexOfCurrentItem++;
				return(currentItem->data);
			}
		}
		//the user wants an item we can't get to easily from the cached position, so find it the hard way
		indexOfCurrentItem=0;
		currentItem=start;
		while(indexOfCurrentItem<i){
			currentItem=currentItem->next;
			indexOfCurrentItem++;
		}
		if(indexOfCurrentItem!=i){//something must have gone wrong; bail out
			currentItem=NULL;
			indexOfCurrentItem=-1;
			return(NULL);
		}
		return(currentItem->data);
	}
	void clear()
	{
		if(numItems==0)
			return;
		currentItem=start;
		while(currentItem!=NULL){
			listNode* temp=currentItem;
			currentItem=currentItem->next;
			delete temp;
		}
		numItems=0;
		indexOfCurrentItem=-1;
		start=NULL;
	}
	void print()
	{
		printf("linkedList: there are %i nodes\n",numItems);
		listNode* temp=start;
		int i=0;
		while(temp!=NULL){
			printf("node %i: &[i]=%p, i=\n",i,temp);
			temp->print();
			printf("\n");
			temp=temp->next;
			i++;
		}
	}
};

//a drawing change object represents a change in one of the floor, terrain 
//or hieght at a single space in the current zone. Both the new and old
//floor/terrain/height values are stored so that the operation can be undone
//or redone. The invert function reverses the old and new values so that a 
//change stored for undo purposes is usable for redo purposes
class drawingChange: public printable{
public:
	short x;
	short y;
	short newval;
	short oldval;
	short type;
	//-1 = null step
	//0 = nothing, needs to be set
	//1 = floor change
	//2 = terrain change
	//3 = height change
	
	drawingChange(short xloc, short yloc, short nval, short oval, short t):x(xloc), y(yloc), newval(nval), oldval(oval), type(t)
	{}
	void invert()
	{
		short temp=oldval;
		oldval=newval;
		newval=temp;
	}
	void print()
	{
		printf("change of type %i at (%i,%i) from %i to %i",type,x,y,oldval,newval);
	}
};

//A drawingUndoStep object stores a set of related changes to the current zone 
//which should be undone or redone as a group changes are registered using the 
//appendChange functions. The invert function can be used to invert all of the 
//changes stored so that the undo step becomes usable as a redo step. 
class drawingUndoStep: public printable{
public:
	Boolean locked;
	linkedList changes;
	drawingUndoStep()
	{
		changes = linkedList();
		locked=FALSE;
	}
	~drawingUndoStep()
	{
		this->clear();
	}
	bool collapseChange(drawingChange* c)
	{
		for(int i=0; i<changes.size(); i++){
			drawingChange* temp = ((drawingChange*)(changes.itemAtIndex(i)));
			if((temp->x == c->x) && (temp->y == c->y) && (temp->type == c->type)){
				temp->newval=c->newval;
				return(true);
			}
		}
		return(false);
	}
	bool appendChange(int x,int y, int newval, int oldval, int type)
	{
		if(locked)
			return(false);
		drawingChange* c = new drawingChange(x,y,newval,oldval,type);
		changes.push(&c);
		return(true);
	}
	bool appendChange(drawingChange* c)
	{
		if(locked)
			return(false);
		if(collapseChange(c)){
			delete(c);
			return(true);
		}
		changes.push(c);
		return(true);
	}
	void clear()
	{
		changes.clear();
	}
	void invert()
	{
		for(int i=0; i<changes.size(); i++)
			((drawingChange*)(changes.itemAtIndex(i)))->invert();
	}
	drawingChange* getChange(int i)
	{
		return((drawingChange*)(changes.itemAtIndex(i)));
	}
	int numChanges()
	{
		return(changes.size());
	}
	void print()
	{
		printf("drawingUndoStep:");
		printf("%i changes in this step.\n",changes.size());
		for(int i=0; i<changes.size(); i++){
			printf("\t");
			((drawingChange*)(changes.itemAtIndex(i)))->print();
			printf("\n");
		}
		printf("linked list dump:\n");
		changes.print();
	}
};
//end undo system types
			

typedef struct {
	/*char strings_ls[NLS][40];
	char strings_rs[NRS][50];
	char town_strs[180][256];
	char out_strs[120][256];
	char short_talk_strs[NUM_TALK_NODES][120];
	char talk_strs[NUM_TALK_NODES * 2][256];*/
	scen_item_data_type scen_item_list;
	} piles_of_stuff_dumping_type;

// Declared in EDGlobal.c
// short get_ran (short times,short  min,short  max);
// short s_pow(short x,short y);
// short dist(location p1,location p2);

// short max(short a,short b);
// short min(short a,short b);
// short minmax(short min,short max,short k);
// short n_abs(short x);
// Boolean same_point(location l1,location l2);

// Declared in Bl A Fileio
// void EdSysBeep();

// Declared in EDUtils.c
// void debug_stop();

// BL A Editor
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_edit_menu(int item_hit);
// Bl A Fileio
bool file_initialize();
void save_campaign();
void load_campaign();
void load_outdoor_and_borders(location which_out);
void save_change_to_outdoor_size(short plus_north,short plus_west,short plus_south,short plus_east,short on_surface);
void load_town(short which_town);
void start_data_dump();
Boolean create_basic_scenario(char *scen_name_short,char *scen_name_with_ext,char *scen_full_name,short out_width,short out_height,short on_surface,Boolean use_warriors_grove);
bool import_boa_town();
void EdSysBeep( /* short duration */ );
void get_name_of_current_scenario(char *name);
void init_warriors_grove();
void import_blades_of_exile_scenario();
bool import_boa_outdoors();
short FSRead(FILE *file_id, long *len, char *data);
short FSClose(FILE *file_id);

// Query
void start_full_data_dump();

// dlogtool
void cd_init_dialogs();
short cd_create_dialog_parent_num(short dlog_num,short parent);
short cd_kill_dialog(short dlog_num,short parent_message);
short cd_process_click(HWND window,POINT the_point, WPARAM wparam,/* LPARAM lparam,*/ short *item);
short cd_process_keystroke(HWND window,WPARAM wparam,/* LPARAM lparam,*/ short *item);
void cd_activate_item(short dlog_num, short item_num, short status);
void cd_get_item_text(short dlog_num, short item_num, char *str);
void csit(short dlog_num, short item_num, char *str);
void cd_retrieve_text_edit_str(short dlog_num,short item_num, char *str);
void cd_set_text_edit_str(short dlog_num, short item_num, char *str);
void cdsin(short dlog_num, short item_num, short num) ;
void cd_set_led(short dlog_num,short item_num,short state);
short cd_get_led(short dlog_num,short item_num);
void cd_add_label(short dlog_num, short item_num, char *label, short label_flag);
void cd_redraw(HWND window);
short cd_find_dlog(HWND window, short *dlg_num, short *dlg_key);
void center_window(HWND window);
// void beep() ;
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
short cd_get_led_range(short dlog_num,short first_led,short last_led);
void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
void cd_set_text_edit_num(short dlog_num, short item_num, short num);
void cd_flip_led(short dlog_num,short item_num,short item_hit);
short cd_retrieve_text_edit_num(short dlog_num, short item_num);
void ModalDialog();
void center_window(HWND window);
short cd_process_syskeystroke(HWND window,WPARAM wparam,/* LPARAM lparam,*/ short *item);

// EdFcns
void init_screen_locs();
Boolean handle_action(POINT the_point, WPARAM wparam, LPARAM lparam );
void swap_terrain();
Boolean handle_keystroke(WPARAM wParam,LPARAM lParam);
void set_terrain(location l,short terrain_type);
void shut_down_menus(/* short mode */);
Boolean save_check(short which_dlog);
void update_item_menu();
void delete_selected_instance();
void copy_selected_instance();
void cut_selected_instance();
void frill_terrain();
void unfrill_terrain();
Boolean create_new_item(short item_to_create,location create_loc,Boolean property,item_type *i_to_make);
void place_items_in_town();
void clean_walls();
void set_all_items_containment();
void set_up_lights();
// q_3DModStart
Boolean get_see_in(short sector_offset_x, short sector_offset_y, short x, short y);
Boolean get_see_to(short sector_offset_x, short sector_offset_y, short x, short y);
short get_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y);
short get_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y);
short get_se_corner(short sector_offset_x, short sector_offset_y, short x, short y);
short get_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y);
void set_up_corner_and_sight_map();
void pushNewUndoStep();
void pushUndoStep(drawingUndoStep* s);
drawingUndoStep* popUndoStep();
void appendChangeToLatestStep(drawingChange* c);
void lockLatestStep();
void purgeUndo();
void performUndo();
void pushRedoStep(drawingUndoStep* s);
drawingUndoStep* popRedoStep();
void purgeRedo();
void performRedo();
// q_3DModEnd
void clear_selected_copied_objects();
void reset_drawing_mode();
Boolean create_new_ter_script(char *ter_script_name,location create_loc,in_town_on_ter_script_type *script_to_make);
Boolean handle_syskeystroke(WPARAM wParam,LPARAM lParam,short *handled);

// EDGlobal.c
short get_ran (short times,short  min,short  max);
// short s_pow(short x,short y);
short dist(location p1,location p2);
short smax(short a,short b);
short smin(short a,short b);
short minmax(short min,short max,short k);
short n_abs(short x);
Boolean same_point(location l1,location l2);
void pause(short length);
void beep();
void SysBeep(/* short a */);
// void StringToNum(char *the_string,long *number_given) ;
void MacInsetRect(RECT *rect,short x, short y) ;
// void MeasureText(short str_len,char *str, short *len_array,HDC hdc);
void PaintRect(RECT *rect,HDC hdc,short r, short g, short b);
short GetControlValue(HWND sbar);
void mes_box(char *str);
void GetIndString(char *str,short i, short j);
// Boolean point_in_rect(POINT the_point,RECT *world_screen);


// EDParser
void init_scripts();
Boolean load_individual_scenario_data(char *scenario_name /*,Boolean force_reload = FALSE */);
Boolean load_core_scenario_data();

// EDUtils.c
void SetRECT( RECT& rect, int left, int top, int right, int bottom );
void OffsetLocation( location& loc, int xofst, int yofst );
bool LocationInRECT( location& loc, RECT& rect );
bool POINTInRECT( POINT p, RECT r );

void ZeroRectCorner (RECT *theRect);
short rect_width (RECT *theRect);
short rect_height (RECT *theRect);
Boolean rects_touch(RECT *r1,RECT *r2);
// Boolean rect_empty(RECT *r);
// RECT rect_sect(RECT *r1,RECT *r2);
Boolean same_string(char *str1,char *str2);
Boolean loc_in_active_area(location loc);
Boolean loc_touches_rect(location loc,macRECT r);
terrain_type_type get_ter(short which_ter);
void ASB (char *theStr);
void ASB_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5);
// void ASB_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short dummy);
void CenterRectInRect (RECT *rectA, RECT *rectB);
void SetMacRect(macRECT *rect,short top, short left, short bottom, short right);

// Graphics.c
void Set_up_win ();
void redraw_screen();
void draw_main_screen();
// void update_main_screen( void );
void set_up_terrain_buttons();
RECT get_template_from_rect(short x,short y);
void draw_terrain();
void place_right_buttons(/* short mode */);
void set_string(char *string,char *string2);
void char_win_draw_string(HDC dest_window,RECT dest_rect,char *str,short mode,short line_height);
// void c2p(char *str) ;
void get_str(char *str,short i, short j);
// short string_length(char *str,HDC hdc);
Boolean refresh_graphics_library();
void make_web(short i,short j);
void take_web(short i,short j);
Boolean is_crate(short i,short j);
void make_crate(short i,short j);
void take_crate(short i,short j);
Boolean is_barrel(short i,short j);
void make_barrel(short i,short j);
void take_barrel(short i,short j);
void make_fire_barrier(short i,short j);
void take_fire_barrier(short i,short j);
void make_force_barrier(short i,short j);
void take_force_barrier(short i,short j);
void make_blocked(short i,short j);
void take_blocked(short i,short j);
void make_sfx(short i,short j,short type);
void take_sfx(short i,short j,short type);
void reset_small_drawn();
void place_dlog_borders_around_rect(HDIB to_gworld,HWND win,
	RECT border_to_rect);
void lose_graphics();

// graphutl
HDIB load_pict(short which_pict);
void rect_draw_some_item(HDIB src_hdib,RECT src_rect,HDIB dest_hdib,RECT dest_rect, short trans, short main_win) ;
void fry_dc(HWND hwnd,HDC dc) ;
// void SectRect(RECT *a, RECT *b, RECT *c) ;
void paint_pattern(HDIB dest,short which_mode,RECT dest_rect,short which_pattern);

// keydlgs
void fancy_choice_dialog_event_filter (short item_hit);
short fancy_choice_dialog(short which_dlog,short parent);
Boolean cre(short val,short min,short max,char *text1, char *text2,short parent_num) ;
void give_error(char *text1, char *text2,short parent_num);
void display_strings_event_filter (short item_hit);
void choose_text_res_event_filter (short item_hit);
short choose_text_res(short res_list,short first_t,short last_t,short cur_choice,short parent_num,char *title);
void edit_special_num_event_filter (short item_hit);
short edit_special_num(/* short mode,*/ short what_start);
void how_many_dlog_event_filter (short item_hit);
short how_many_dlog(short what_start,short minimum,short maximum,char *what_text);
void get_str_dlog_event_filter (short item_hit);
void get_str_dlog(char *start_str,char *header_str,char *response,Boolean string_string = FALSE);
void make_cursor_sword() ;
void set_cursor(short which_c) ;
void restore_cursor();
Boolean string_not_clean(char *str,short max_length,short strict_file_naming,char *beginning_of_error,short parent_num);

// Library
void flip_short(short *s);
void alter_rect(macRECT *r);
void flip_rect(macRECT *s);

// sound
void load_sounds ();
void play_sound(short which);

// townout
void edit_placed_monst_event_filter (short item_hit);
void edit_placed_monst(short which_m);
void edit_sign(short which_sign);
void get_a_number_event_filter (short item_hit);
short get_a_number(short which_dlog,short default_value,short min,short max);
void change_ter_event_filter (short item_hit);
void change_ter(short *change_from,short *change_to,short *chance);
void outdoor_details_event_filter (short item_hit);
void outdoor_details();
void edit_out_wand_event_filter (short item_hit);
void edit_out_wand(short mode);
void edit_town_details_event_filter (short item_hit);
void edit_town_details();
void edit_town_wand_event_filter (short item_hit);
void edit_town_wand();
void pick_out_event_filter (short item_hit);
short pick_out(location default_loc,short max_x,short max_y);

// q_3DModStart
void change_outdoor_size_event_filter (short item_hit);
Boolean change_outdoor_size();
// q_3DModEnd

void new_town_event_filter (short item_hit);
Boolean new_town();
void delete_last_town();
void pick_import_town_event_filter (short item_hit);
short pick_import_town(short which_dlog /*,short def */);
Boolean terrains_match(short terrain_drawn,short terrain_there);
Boolean floors_match(short floor_drawn,short floor_there);
void edit_area_rect_event_filter (short item_hit);
Boolean edit_area_rect_str(short which_str /*,short mode */);
void edit_out_strs_event_filter (short item_hit);
void edit_out_strs();
void edit_town_strs_event_filter (short item_hit);
void edit_town_strs();
void edit_item_placement();
void edit_item_placement_event_filter (short item_hit);
void edit_add_town();
void edit_add_town_event_filter (short item_hit);
void edit_make_scen_1_event_filter (short item_hit);
void edit_make_scen_2_event_filter (short item_hit);
void build_scenario();
void edit_scen_details_event_filter (short item_hit);
void edit_scen_details();
void edit_scen_intro_event_filter (short item_hit);
void edit_scen_intro(short which_part);
void edit_scen_intro_pic_event_filter (short item_hit);
void edit_scen_intro_pic();
