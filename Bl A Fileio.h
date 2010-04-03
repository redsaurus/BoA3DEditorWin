// Bl A Fileio.h
// The intention is to eventually replace this header file with an external file that is loaded at run-time and
// can be edited by the user.
#pragma once

char *BOAPortOutScriptnames[100] = {"o00Outdscript","o01Outdscript","o02Outdscript",
"o03Outdscript","o04Outdscript","o05Outdscript","o06Outdscript","o07Outdscript",
"o08Outdscript","o09Outdscript","o10Outdscript","o11Outdscript","o12Outdscript",
"o13Outdscript","o14Outdscript","o15Outdscript","o16Outdscript","o17Outdscript",
"o18Outdscript","o19Outdscript","o20Outdscript","o21Outdscript","o22Outdscript",
"o23Outdscript","o24Outdscript","o25Outdscript","o26Outdscript","o27Outdscript",
"o28Outdscript","o29Outdscript","o30Outdscript","o31Outdscript","o32Outdscript",
"o33Outdscript","o34Outdscript","o35Outdscript","o36Outdscript","o37Outdscript",
"o38Outdscript","o39Outdscript","o40Outdscript","o41Outdscript","o42Outdscript",
"o43Outdscript","o44Outdscript","o45Outdscript","o46Outdscript","o47Outdscript",
"o48Outdscript","o49Outdscript","o50Outdscript","o51Outdscript","o52Outdscript",
"o53Outdscript","o54Outdscript","o55Outdscript","o56Outdscript","o57Outdscript",
"o58Outdscript","o59Outdscript","o60Outdscript","o61Outdscript","o62Outdscript",
"o63Outdscript","o64Outdscript","o65Outdscript","o66Outdscript","o67Outdscript",
"o68Outdscript","o69Outdscript","o70Outdscript","o71Outdscript","o72Outdscript",
"o73Outdscript","o74Outdscript","o75Outdscript","o76Outdscript","o77Outdscript",
"o78Outdscript","o79Outdscript","o80Outdscript","o81Outdscript","o82Outdscript",
"o83Outdscript","o84Outdscript","o85Outdscript","o86Outdscript","o87Outdscript",
"o88Outdscript","o89Outdscript","o90Outdscript","o91Outdscript","o92Outdscript",
"o93Outdscript","o94Outdscript","o95Outdscript","o96Outdscript","o97Outdscript",
"o98Outdscript","o99Outdscript"};

char *BOAPortOutnames[100] = {"o00Outdoorzone name","o01Outdoorzone name","o02Outdoorzone name",
"o03Outdoorzone name","o04Outdoorzone name","o05Outdoorzone name","o06Outdoorzone name",
"o07Outdoorzone name","o08Outdoorzone name","o09Outdoorzone name","o10Outdoorzone name",
"o11Outdoorzone name","o12Outdoorzone name","o13Outdoorzone name","o14Outdoorzone name",
"o15Outdoorzone name","o16Outdoorzone name","o17Outdoorzone name","o18Outdoorzone name",
"o19Outdoorzone name","o20Outdoorzone name","o21Outdoorzone name","o22Outdoorzone name",
"o23Outdoorzone name","o24Outdoorzone name","o25Outdoorzone name","o26Outdoorzone name",
"o27Outdoorzone name","o28Outdoorzone name","o29Outdoorzone name","o30Outdoorzone name",
"o31Outdoorzone name","o32Outdoorzone name","o33Outdoorzone name","o34Outdoorzone name",
"o35Outdoorzone name","o36Outdoorzone name","o37Outdoorzone name","o38Outdoorzone name",
"o39Outdoorzone name","o40Outdoorzone name","o41Outdoorzone name","o42Outdoorzone name",
"o43Outdoorzone name","o44Outdoorzone name","o45Outdoorzone name","o46Outdoorzone name",
"o47Outdoorzone name","o48Outdoorzone name","o49Outdoorzone name","o50Outdoorzone name",
"o51Outdoorzone name","o52Outdoorzone name","o53Outdoorzone name","o54Outdoorzone name",
"o55Outdoorzone name","o56Outdoorzone name","o57Outdoorzone name","o58Outdoorzone name",
"o59Outdoorzone name","o60Outdoorzone name","o61Outdoorzone name","o62Outdoorzone name",
"o63Outdoorzone name","o64Outdoorzone name","o65Outdoorzone name","o66Outdoorzone name",
"o67Outdoorzone name","o68Outdoorzone name","o69Outdoorzone name","o70Outdoorzone name",
"o71Outdoorzone name","o72Outdoorzone name","o73Outdoorzone name","o74Outdoorzone name",
"o75Outdoorzone name","o76Outdoorzone name","o77Outdoorzone name","o78Outdoorzone name",
"o79Outdoorzone name","o80Outdoorzone name","o81Outdoorzone name","o82Outdoorzone name",
"o83Outdoorzone name","o84Outdoorzone name","o85Outdoorzone name","o86Outdoorzone name",
"o87Outdoorzone name","o88Outdoorzone name","o89Outdoorzone name","o90Outdoorzone name",
"o91Outdoorzone name","o92Outdoorzone name","o93Outdoorzone name","o94Outdoorzone name",
"o95Outdoorzone name","o96Outdoorzone name","o97Outdoorzone name","o98Outdoorzone name",
"o99Outdoorzone name"};

char *BOAPortTownScriptnames[200] = {"t000Townscrip","t001Townscrip","t002Townscrip",
"t003Townscrip","t004Townscrip","t005Townscrip","t006Townscrip","t007Townscrip","t008Townscrip",
"t009Townscrip","t010Townscrip","t011Townscrip","t012Townscrip","t013Townscrip","t014Townscrip",
"t015Townscrip","t016Townscrip","t017Townscrip","t018Townscrip","t019Townscrip","t020Townscrip",
"t021Townscrip","t022Townscrip","t023Townscrip","t024Townscrip","t025Townscrip","t026Townscrip",
"t027Townscrip","t028Townscrip","t029Townscrip","t030Townscrip","t031Townscrip","t032Townscrip",
"t033Townscrip","t034Townscrip","t035Townscrip","t036Townscrip","t037Townscrip","t038Townscrip",
"t039Townscrip","t040Townscrip","t041Townscrip","t042Townscrip","t043Townscrip","t044Townscrip",
"t045Townscrip","t046Townscrip","t047Townscrip","t048Townscrip","t049Townscrip","t050Townscrip",
"t051Townscrip","t052Townscrip","t053Townscrip","t054Townscrip","t055Townscrip","t056Townscrip",
"t057Townscrip","t058Townscrip","t059Townscrip","t060Townscrip","t061Townscrip","t062Townscrip",
"t063Townscrip","t064Townscrip","t065Townscrip","t066Townscrip","t067Townscrip","t068Townscrip",
"t069Townscrip","t070Townscrip","t071Townscrip","t072Townscrip","t073Townscrip","t074Townscrip",
"t075Townscrip","t076Townscrip","t077Townscrip","t078Townscrip","t079Townscrip","t080Townscrip",
"t081Townscrip","t082Townscrip","t083Townscrip","t084Townscrip","t085Townscrip","t086Townscrip",
"t087Townscrip","t088Townscrip","t089Townscrip","t090Townscrip","t091Townscrip","t092Townscrip",
"t093Townscrip","t094Townscrip","t095Townscrip","t096Townscrip","t097Townscrip","t098Townscrip",
"t099Townscrip","t100Townscrip","t101Townscrip","t102Townscrip","t103Townscrip","t104Townscrip",
"t105Townscrip","t106Townscrip","t107Townscrip","t108Townscrip","t109Townscrip","t110Townscrip",
"t111Townscrip","t112Townscrip","t113Townscrip","t114Townscrip","t115Townscrip","t116Townscrip",
"t117Townscrip","t118Townscrip","t119Townscrip","t120Townscrip","t121Townscrip","t122Townscrip",
"t123Townscrip","t124Townscrip","t125Townscrip","t126Townscrip","t127Townscrip","t128Townscrip",
"t129Townscrip","t130Townscrip","t131Townscrip","t132Townscrip","t133Townscrip","t134Townscrip",
"t135Townscrip","t136Townscrip","t137Townscrip","t138Townscrip","t139Townscrip","t140Townscrip",
"t141Townscrip","t142Townscrip","t143Townscrip","t144Townscrip","t145Townscrip","t146Townscrip",
"t147Townscrip","t148Townscrip","t149Townscrip","t150Townscrip","t151Townscrip","t152Townscrip",
"t153Townscrip","t154Townscrip","t155Townscrip","t156Townscrip","t157Townscrip","t158Townscrip",
"t159Townscrip","t160Townscrip","t161Townscrip","t162Townscrip","t163Townscrip","t164Townscrip",
"t165Townscrip","t166Townscrip","t167Townscrip","t168Townscrip","t169Townscrip","t170Townscrip",
"t171Townscrip","t172Townscrip","t173Townscrip","t174Townscrip","t175Townscrip","t176Townscrip",
"t177Townscrip","t178Townscrip","t179Townscrip","t180Townscrip","t181Townscrip","t182Townscrip",
"t183Townscrip","t184Townscrip","t185Townscrip","t186Townscrip","t187Townscrip","t188Townscrip",
"t189Townscrip","t190Townscrip","t191Townscrip","t192Townscrip","t193Townscrip","t194Townscrip",
"t195Townscrip","t196Townscrip","t197Townscrip","t198Townscrip","t199Townscrip"};

char *BOAPortTownnames[200] = {"t000Town name (BoA)","t001Town name (BoA)",
"t002Town name (BoA)","t003Town name (BoA)","t004Town name (BoA)","t005Town name (BoA)",
"t006Town name (BoA)","t007Town name (BoA)","t008Town name (BoA)","t009Town name (BoA)",
"t010Town name (BoA)","t011Town name (BoA)","t012Town name (BoA)","t013Town name (BoA)",
"t014Town name (BoA)","t015Town name (BoA)","t016Town name (BoA)","t017Town name (BoA)",
"t018Town name (BoA)","t019Town name (BoA)","t020Town name (BoA)","t021Town name (BoA)",
"t022Town name (BoA)","t023Town name (BoA)","t024Town name (BoA)","t025Town name (BoA)",
"t026Town name (BoA)","t027Town name (BoA)","t028Town name (BoA)","t029Town name (BoA)",
"t030Town name (BoA)","t031Town name (BoA)","t032Town name (BoA)","t033Town name (BoA)",
"t034Town name (BoA)","t035Town name (BoA)","t036Town name (BoA)","t037Town name (BoA)",
"t038Town name (BoA)","t039Town name (BoA)","t040Town name (BoA)","t041Town name (BoA)",
"t042Town name (BoA)","t043Town name (BoA)","t044Town name (BoA)","t045Town name (BoA)",
"t046Town name (BoA)","t047Town name (BoA)","t048Town name (BoA)","t049Town name (BoA)",
"t050Town name (BoA)","t051Town name (BoA)","t052Town name (BoA)","t053Town name (BoA)",
"t054Town name (BoA)","t055Town name (BoA)","t056Town name (BoA)","t057Town name (BoA)",
"t058Town name (BoA)","t059Town name (BoA)","t060Town name (BoA)","t061Town name (BoA)",
"t062Town name (BoA)","t063Town name (BoA)","t064Town name (BoA)","t065Town name (BoA)",
"t066Town name (BoA)","t067Town name (BoA)","t068Town name (BoA)","t069Town name (BoA)",
"t070Town name (BoA)","t071Town name (BoA)","t072Town name (BoA)","t073Town name (BoA)",
"t074Town name (BoA)","t075Town name (BoA)","t076Town name (BoA)","t077Town name (BoA)",
"t078Town name (BoA)","t079Town name (BoA)","t080Town name (BoA)","t081Town name (BoA)",
"t082Town name (BoA)","t083Town name (BoA)","t084Town name (BoA)","t085Town name (BoA)",
"t086Town name (BoA)","t087Town name (BoA)","t088Town name (BoA)","t089Town name (BoA)",
"t090Town name (BoA)","t091Town name (BoA)","t092Town name (BoA)","t093Town name (BoA)",
"t094Town name (BoA)","t095Town name (BoA)","t096Town name (BoA)","t097Town name (BoA)",
"t098Town name (BoA)","t099Town name (BoA)","t100Town name (BoA)","t101Town name (BoA)",
"t102Town name (BoA)","t103Town name (BoA)","t104Town name (BoA)","t105Town name (BoA)",
"t106Town name (BoA)","t107Town name (BoA)","t108Town name (BoA)","t109Town name (BoA)",
"t110Town name (BoA)","t111Town name (BoA)","t112Town name (BoA)","t113Town name (BoA)",
"t114Town name (BoA)","t115Town name (BoA)","t116Town name (BoA)","t117Town name (BoA)",
"t118Town name (BoA)","t119Town name (BoA)","t120Town name (BoA)","t121Town name (BoA)",
"t122Town name (BoA)","t123Town name (BoA)","t124Town name (BoA)","t125Town name (BoA)",
"t126Town name (BoA)","t127Town name (BoA)","t128Town name (BoA)","t129Town name (BoA)",
"t130Town name (BoA)","t131Town name (BoA)","t132Town name (BoA)","t133Town name (BoA)",
"t134Town name (BoA)","t135Town name (BoA)","t136Town name (BoA)","t137Town name (BoA)",
"t138Town name (BoA)","t139Town name (BoA)","t140Town name (BoA)","t141Town name (BoA)",
"t142Town name (BoA)","t143Town name (BoA)","t144Town name (BoA)","t145Town name (BoA)",
"t146Town name (BoA)","t147Town name (BoA)","t148Town name (BoA)","t149Town name (BoA)",
"t150Town name (BoA)","t151Town name (BoA)","t152Town name (BoA)","t153Town name (BoA)",
"t154Town name (BoA)","t155Town name (BoA)","t156Town name (BoA)","t157Town name (BoA)",
"t158Town name (BoA)","t159Town name (BoA)","t160Town name (BoA)","t161Town name (BoA)",
"t162Town name (BoA)","t163Town name (BoA)","t164Town name (BoA)","t165Town name (BoA)",
"t166Town name (BoA)","t167Town name (BoA)","t168Town name (BoA)","t169Town name (BoA)",
"t170Town name (BoA)","t171Town name (BoA)","t172Town name (BoA)","t173Town name (BoA)",
"t174Town name (BoA)","t175Town name (BoA)","t176Town name (BoA)","t177Town name (BoA)",
"t178Town name (BoA)","t179Town name (BoA)","t180Town name (BoA)","t181Town name (BoA)",
"t182Town name (BoA)","t183Town name (BoA)","t184Town name (BoA)","t185Town name (BoA)",
"t186Town name (BoA)","t187Town name (BoA)","t188Town name (BoA)","t189Town name (BoA)",
"t190Town name (BoA)","t191Town name (BoA)","t192Town name (BoA)","t193Town name (BoA)",
"t194Town name (BoA)","t195Town name (BoA)","t196Town name (BoA)","t197Town name (BoA)",
"t198Town name (BoA)","t199Town name (BoA)"};

//    1     secret door
//    2     door
//    3     locked door
//    4     m locked door
//    5     impass door
//    6     open door
//    7     window
//    8     closed gate
//    9     open gate
//  10     cracked
//  11     Sign and wall
//  12     wall
//  13     road
//  14     bridge
//  15     floor
//  16     terrain

short old_ter_variety[256] = {
	15,15,15,15,15,12,12,12,12,12,
	12,12,12,12,12,12,12,12,12,12,
	12,12,12,12,12,12,12,12,12,12,
	12,12,12,12,12,12,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,14,14,14,14,14,
	14,15,14,14,15,15,15,15,15,13,
	13,13,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,12,1,1,2,3,4,5,6,
	8,9,11,10,10,10,12,12,7,12,
	1,1,2,3,4,5,6,8,9,11,
	10,10,10,7,12,1,1,2,3,4,
	5,6,8,9,11,10,10,10,12,7,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15
};

short old_monst_to_new[256] = {
	0,1,74,76,78,2,75,77,79,86,
	86,3,4,6,5,131,20,175,7,8,
	132,10,11,12,88,13,89,14,15,16,
	105,17,82,81,81,19,133,187,21,134,
	22,23,24,25,26,32,33,34,35,36,

	37,85,182,38,39,40,41,135,42,136, // 50
	43,48,44,45,137,46,47,138,87,49,
	139,140,51,52,141,50,142,143,53,54,
	72,144,56,58,59,60,95,61,145,146,
	148,149,150,151,152,153,154,155,156,62,

	63,64,93,84,90,66,66,157,67,80, // 100
	186,68,188,189,190,191,192,193,69,70,
	130,91,99,158,55,120,121,166,167,168,
	159,160,169,170,171,172,173,174,195,196,
	197,198,199,200,201,202,202,204,205,206,

	207,208,209,210,211,212,213,214,215,216, // 150
	217,218,219,220,221,222,223,224,225,226,
	227,228,229,230,231,232,233,101,161,162,
	163,164,65,94,129,102,103,128,235,236,
	237,238,239,240,241,242,243,244,245,246,

	247,248,249,250,251,252,253,254,255,255, // 200
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,

	255,255,255,255,255,255
};

short old_ter_to_floor[256] = {
	0,1,37,38,39,255,255,0,255,255,
	0,255,255,0,255,255,0,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,41,42,47,48,
	49,50,51,52,45,46,53,54,55,56,

	57,65,68,59,67,61,70,63,69,64, // 50,
	62,60,66,23,23,63,23,59,61,23,
	65,58,58,58,58,79,80,58,0,0,
	37,41,123,123,0,0,0,37,37,37,
	71,0,0,0,0,0,75,0,0,0,

	0,0,0,0,0,0,0,0,0,0, // 100
	37,77,37,37,37,37,37,37,72,37,
	37,37,95,95,95,95,95,95,95,95,
	95,95,95,95,95,95,95,95,95,95,
	95,95,95,95,95,95,95,95,95,95,

	95,95,95,95,95,95,95,95,95,95, // 150
	95,95,95,95,95,95,95,95,95,95,
	95,129,95,95,95,95,95,95,95,95,
	95,95,95,95,95,95,95,95,95,95,
	95,95,95,95,95,95,95,95,95,95,

	95,95,95,95,95,95,95,95,95,95, // 200
	98,130,98,98,98,98,98,100,100,100,
	100,100,100,100,100,100,100,100,100,100,
	100,100,0,0,37,37,37,37,37,37,
	37,37,37,0,0,0,0,95,98,100,
	37,41,0,0,0,0
};

short old_ter_to_ter[256] = {
	0,0,0,0,0,0,40,56,44,41,
	57,45,38,54,42,39,55,43,0,0,
	0,0,0,0,40,44,41,45,38,42,
	39,43,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0, // 50
	0,0,0,300,301,298,298,298,299,299,
	299,0,299,298,300,0,0,336,268,0,
	0,0,0,0,155,156,155,157,158,157,
	0,147,148,141,142,285,0,167,166,170,

	171,200,269,223,209,211,213,215,216,243, // 100
	128,0,151,152,153,144,319,318,283,170,
	171,214,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0, // 150
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,270,218,229,257,257,257,258,
	258,258,221,222,221,222,217,231,208,242,
	243,234,267,276,202,225,226,227,228,206,

	278,238,248,277,240,247,189,188,374,375, // 200
	0,0,276,277,267,278,232,0,345,346,
	241,348,349,350,270,225,355,355,355,355,
	355,286,323,304,329,305,0,332,333,332,
	333,302,305,311,313,311,0,0,0,0,

	0,0,0,0,0,0
};

short old_item_to_new_item[400] = {
	0,0,0,0,18,19,20,21,22,109,
	109,79,79,163,168,165,165,170,172,196,
	197,198,196,192,191,171,171,0,0,0,
	385,45,45,50,50,65,65,45,50,50,
	55,55,65,55,55,55,60,70,75,60,

	60,46,51,56,56,66,56,56,56,61,
	71,76,61,61,47,52,57,57,67,57,
	57,57,62,72,77,62,62,48,53,58,
	58,68,58,58,58,63,73,78,63,63,
	370,371,380,380,381,381,383,384,85,87,

	88,85,86,100,101,103,86,87,90,91, // 100
	92,93,95,96,98,105,106,108,314,315,
	25,26,25,30,35,40,26,31,36,41,
	27,32,37,42,28,33,38,43,38,121,
	122,122,123,124,126,127,127,128,129,131,

	132,132,133,134,136,137,137,138,139,141,
	142,142,143,144,146,0,0,0,0,114,
	114,174,499,499,499,499,205,204,175,176,
	499,499,326,499,499,499,327,499,499,435,
	436,437,499,499,499,499,244,247,0,0,

	246,248,251,254,499,499,257,499,499,499, // 200
	499,236,238,239,243,0,204,140,356,357,
	358,359,386,201,202,420,360,361,362,363,
	364,365,115,366,367,368,113,421,422,423,
	424,425,426,427,428,130,229,228,231,235,

	233,234,236,221,223,231,227,225,226,236,
	221,223,231,227,225,226,236,237,395,242,
	243,243,237,499,499,499,353,354,352,355,
	441,499,499,282,285,282,499,284,286,289,
	289,499,499,499,276,281,278,264,208,499,

	499,270,275,272,265,207,499,499,270,275, // 300
	272,266,206,499,430,431,432,433,434,499,
	499,303,305,499,499,212,211,499,499,499,
	311,212,211,499,420,240,241,390,499,499,
	343,342,351,346,369,344,348,499,499,499,

	438,439,499,308,273,306,499,287,440,429,
	499,499,280,215,214,215,216,216,217,218,
	219,499,49,49,54,44,125,69,269,74,
	389,388,442,450,451,452,453,454,455,456,
	457,458,459,460,461,462,463,464,465,466
};

char BOAPortOutWriteallow[100] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1
};

char BOAPortTownWriteallow[200] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1
};

short old_potion_to_new[20] = {
			1,0,-1,2,-1,7,1,8,5,3,-1,-1,7,-1,6,9,16,11,12,9
};

short old_stat_to_new[19] = {
			0,1,2,4,4,5,7,6,9,11,12,13,14,41,15,15,10,10,18
};
