
/*

	This program will genrate the source code that updates the TSL LCD. You should run this program on a 
	computer (not on the TSL) and then capture the output, paste it into the output.c file in the TSL project,
	and then recompile the TSL project. 

	See code2code.MD for more info on how this all works. 

*/


// You will need these to make this block compile independently...
#include <stdio.h>

#ifndef uint8_t
	typedef unsigned char uint8_t;
#endif


	//---------------CUT HERE FOR SEG MAP PRINTOUT
	// This code should be copy/pasted from LCD.C to ConsoleApplication1.cpp any time
	// it is changed and you want to regenerate the LCD updater code in output.c. 


	// Maximum number of common lines. Defined in datasheets.
#define LCD_MAX_NBR_OF_COM  4
// Maximum number of segment lines. Defined in datasheets.
#define LCD_MAX_NBR_OF_SEG  25


// Use these for addressing the LCD pixel registers
#define LCD_REGS_BASE_HIGH	(0x0D)		// High byte of the LCD registers base address
#define LCD_REGS_BASE_LOW	(0x10)		// Low byte of the LCD registers base address


// Map LCD segments to registers
// compute at compile time
// Taken from 25.5.11 DATA � LCD Data Memory Mapping

// Return the offset of the register that holds the requested pixel

	inline static uint8_t LCD_REG_OFF(uint8_t com, uint8_t seg) {

		uint8_t reg = (com * ((LCD_MAX_NBR_OF_SEG + 7) / 8)) + (seg / 8);

		return reg;
	}


	// Return the bit in the register that holds the requested pixel

	inline static uint8_t LCD_REG_BIT(uint8_t com, uint8_t seg) {

		return  seg % 8;

	}


	// LCD FONT----

	// Map 7 segments A-G to internal representation bits
	// A=0b00000001, B=0b00000010, etc
	// Just luck that 7 segments fits into an 8 bit byte

#define LCD_SEG_BIT( letter ) ( 1 << (letter - 'A') )

#define SEG_A LCD_SEG_BIT('A')
#define SEG_B LCD_SEG_BIT('B')
#define SEG_C LCD_SEG_BIT('C')
#define SEG_D LCD_SEG_BIT('D')
#define SEG_E LCD_SEG_BIT('E')
#define SEG_F LCD_SEG_BIT('F')
#define SEG_G LCD_SEG_BIT('G')

/*

	NOTE: LCD modules are mounted upside down for maximum contrast when viewed from
		  30 deg above

	Here is the segment map in the upside down position:

		D
	C       E
		G
	B       F
		A

*/


	const uint8_t lcd_font_digits[] = {
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F ,         // 0
		SEG_E | SEG_F ,                                         // 1
		SEG_A | SEG_B | SEG_G | SEG_E | SEG_D ,                 // 2
		SEG_A | SEG_F | SEG_G | SEG_E | SEG_D ,                 // 3
		SEG_F | SEG_G | SEG_E | SEG_C ,                         // 4
		SEG_A | SEG_F | SEG_G | SEG_C | SEG_D ,                 // 5
		SEG_A | SEG_C | SEG_D | SEG_B | SEG_F | SEG_G ,         // 6
		SEG_D | SEG_E | SEG_F ,                                 // 7
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G , // 8
		SEG_A | SEG_E | SEG_C | SEG_D | SEG_F | SEG_G ,         // 9
	};


	const uint8_t lcd_font_char_dash = { SEG_G };                                 // '-'
	const uint8_t lcd_font_char_c = { SEG_G | SEG_B | SEG_A };                 // c
	const uint8_t lcd_font_char_L = { SEG_C | SEG_B | SEG_A };                 // L
	const uint8_t lcd_font_char_o = { SEG_G | SEG_B | SEG_A | SEG_F };         // o
	const uint8_t lcd_font_char_E = { SEG_D | SEG_C | SEG_G | SEG_B | SEG_A }; // E
	const uint8_t lcd_font_char_r = { SEG_B | SEG_G };                         // r
	const uint8_t lcd_font_char_P = { SEG_E | SEG_D | SEG_C | SEG_G | SEG_B }; // P

	const uint8_t lcd_font_char_J = { SEG_B | SEG_A | SEG_F | SEG_F | SEG_E }; // J
	const uint8_t lcd_font_char_O = { SEG_D | SEG_C | SEG_B | SEG_A | SEG_F | SEG_E }; // O
	const uint8_t lcd_font_char_S = { SEG_D | SEG_C | SEG_G | SEG_F | SEG_A }; // S
	const uint8_t lcd_font_char_H = { SEG_B | SEG_C | SEG_F | SEG_G | SEG_E }; // H

	const uint8_t lcd_font_char_n = { SEG_B | SEG_G | SEG_F };                 // n
	const uint8_t lcd_font_char_t = { SEG_B | SEG_C | SEG_A | SEG_G };         // t
	const uint8_t lcd_font_char_i = { SEG_B };                                 // i
	const uint8_t lcd_font_char_G = { SEG_D | SEG_C | SEG_B | SEG_A | SEG_F }; // G

	const uint8_t lcd_font_char_V = { SEG_C | SEG_B | SEG_A | SEG_F | SEG_E }; // V (ok, not really)
	const uint8_t lcd_font_char_l = { SEG_C | SEG_B };                         // l

	const uint8_t lcd_font_char_b = { SEG_C | SEG_B | SEG_A | SEG_F | SEG_G }; // b
	const uint8_t lcd_font_char_A = { SEG_C | SEG_B | SEG_D | SEG_E | SEG_G | SEG_F };   // A
	const uint8_t lcd_font_char_d = { SEG_G | SEG_B | SEG_A | SEG_F | SEG_E };  // d
	const uint8_t lcd_font_char_U = { SEG_C | SEG_B | SEG_A | SEG_F | SEG_E };  // d


	// Map LCD seg pins to MCU seg pins (set by PCB layout)

#define LCD_SEG_R01 24
#define LCD_SEG_R02 23
#define LCD_SEG_R03 22
#define LCD_SEG_R04 21
#define LCD_SEG_R05 20
#define LCD_SEG_R06 19
#define LCD_SEG_R07 18
#define LCD_SEG_R08 17
#define LCD_SEG_R09 16
#define LCD_SEG_R10 15
#define LCD_SEG_R11 14
#define LCD_SEG_R12 13
#define LCD_SEG_R13 12


#define LCD_SEG_L01 24
#define LCD_SEG_L02 11
#define LCD_SEG_L03 10
#define LCD_SEG_L04  9
#define LCD_SEG_L05  8
#define LCD_SEG_L06  7
#define LCD_SEG_L07  6
#define LCD_SEG_L08  5
#define LCD_SEG_L09  4
#define LCD_SEG_L10  3
#define LCD_SEG_L11  2
#define LCD_SEG_L12  1
#define LCD_SEG_L13  0


// Map LCD COM pins to MCU com pins (set by PCB layout)

#define LCD_COM_R1 0
#define LCD_COM_R2 1
#define LCD_COM_R3 2
#define LCD_COM_R4 3

#define LCD_COM_L1 0
#define LCD_COM_L2 1
#define LCD_COM_L3 2
#define LCD_COM_L4 3



// Map LCD icons to seg/com (set by LCD datasheet)

// Map each of the 7 visible LCD icons A-G in each of the 6 digits per LCD

	typedef struct {
		uint8_t com;
		uint8_t seg;
	} lcd_visible_segment;

	// TODO: PROGMEM and then statically compiled transitions

	// Maps segments in each digit to corresponding SEG and COM pin
	// 0=rightmost digit, 11=leftmost

	const lcd_visible_segment  digitmap[12][7] = {

		// Right LCD

		{     //1 on the datasheet= 0 in digitmap array
			{LCD_COM_R1 , LCD_SEG_R03 },       //A
			{LCD_COM_R2 , LCD_SEG_R03 },       //B
			{LCD_COM_R3 , LCD_SEG_R03 },       //C
			{LCD_COM_R4 , LCD_SEG_R02 },       //D
			{LCD_COM_R3 , LCD_SEG_R02 },       //E
			{LCD_COM_R1 , LCD_SEG_R02 },       //F
			{LCD_COM_R2 , LCD_SEG_R02 },       //G
		},

		{     //2
			{LCD_COM_R1 , LCD_SEG_R05 },       //A
			{LCD_COM_R2 , LCD_SEG_R05 },       //B
			{LCD_COM_R3 , LCD_SEG_R05 },       //C
			{LCD_COM_R4 , LCD_SEG_R04 },       //D
			{LCD_COM_R3 , LCD_SEG_R04 },       //E
			{LCD_COM_R1 , LCD_SEG_R04 },       //F
			{LCD_COM_R2 , LCD_SEG_R04 },       //G
		},

		{     //3
			{LCD_COM_R1 , LCD_SEG_R07 },       //A
			{LCD_COM_R2 , LCD_SEG_R07 },       //B
			{LCD_COM_R3 , LCD_SEG_R07 },       //C
			{LCD_COM_R4 , LCD_SEG_R06 },       //D
			{LCD_COM_R3 , LCD_SEG_R06 },       //E
			{LCD_COM_R1 , LCD_SEG_R06 },       //F
			{LCD_COM_R2 , LCD_SEG_R06 },       //G
		},

		{     //4
			{LCD_COM_R1 , LCD_SEG_R09 },       //A
			{LCD_COM_R2 , LCD_SEG_R09 },       //B
			{LCD_COM_R3 , LCD_SEG_R09 },       //C
			{LCD_COM_R4 , LCD_SEG_R08 },       //D
			{LCD_COM_R3 , LCD_SEG_R08 },       //E
			{LCD_COM_R1 , LCD_SEG_R08 },       //F
			{LCD_COM_R2 , LCD_SEG_R08 },       //G
		},

		{     //5
			{LCD_COM_R1 , LCD_SEG_R11 },       //A
			{LCD_COM_R2 , LCD_SEG_R11 },       //B
			{LCD_COM_R3 , LCD_SEG_R11 },       //C
			{LCD_COM_R4 , LCD_SEG_R10 },       //D
			{LCD_COM_R3 , LCD_SEG_R10 },       //E
			{LCD_COM_R1 , LCD_SEG_R10 },       //F
			{LCD_COM_R2 , LCD_SEG_R10 },       //G
		},

		{     //6
			{LCD_COM_R1 , LCD_SEG_R13 },       //A
			{LCD_COM_R2 , LCD_SEG_R13 },       //B
			{LCD_COM_R3 , LCD_SEG_R13 },       //C
			{LCD_COM_R4 , LCD_SEG_R12 },       //D
			{LCD_COM_R3 , LCD_SEG_R12 },       //E
			{LCD_COM_R1 , LCD_SEG_R12 },       //F
			{LCD_COM_R2 , LCD_SEG_R12 },       //G
		},

		// Left LCD

		{     //1 on the datasheet= 0 in digitmap array
			{LCD_COM_L1 , LCD_SEG_L03 },       //A
			{LCD_COM_L2 , LCD_SEG_L03 },       //B
			{LCD_COM_L3 , LCD_SEG_L03 },       //C
			{LCD_COM_L4 , LCD_SEG_L02 },       //D
			{LCD_COM_L3 , LCD_SEG_L02 },       //E
			{LCD_COM_L1 , LCD_SEG_L02 },       //F
			{LCD_COM_L2 , LCD_SEG_L02 },       //G
		},

		{     //2
			{LCD_COM_L1 , LCD_SEG_L05 },       //A
			{LCD_COM_L2 , LCD_SEG_L05 },       //B
			{LCD_COM_L3 , LCD_SEG_L05 },       //C
			{LCD_COM_L4 , LCD_SEG_L04 },       //D
			{LCD_COM_L3 , LCD_SEG_L04 },       //E
			{LCD_COM_L1 , LCD_SEG_L04 },       //F
			{LCD_COM_L2 , LCD_SEG_L04 },       //G
		},

		{     //3
			{LCD_COM_L1 , LCD_SEG_L07 },       //A
			{LCD_COM_L2 , LCD_SEG_L07 },       //B
			{LCD_COM_L3 , LCD_SEG_L07 },       //C
			{LCD_COM_L4 , LCD_SEG_L06 },       //D
			{LCD_COM_L3 , LCD_SEG_L06 },       //E
			{LCD_COM_L1 , LCD_SEG_L06 },       //F
			{LCD_COM_L2 , LCD_SEG_L06 },       //G
		},

		{     //4
			{LCD_COM_L1 , LCD_SEG_L09 },       //A
			{LCD_COM_L2 , LCD_SEG_L09 },       //B
			{LCD_COM_L3 , LCD_SEG_L09 },       //C
			{LCD_COM_L4 , LCD_SEG_L08 },       //D
			{LCD_COM_L3 , LCD_SEG_L08 },       //E
			{LCD_COM_L1 , LCD_SEG_L08 },       //F
			{LCD_COM_L2 , LCD_SEG_L08 },       //G
		},

		{     //5
			{LCD_COM_L1 , LCD_SEG_L11 },       //A
			{LCD_COM_L2 , LCD_SEG_L11 },       //B
			{LCD_COM_L3 , LCD_SEG_L11 },       //C
			{LCD_COM_L4 , LCD_SEG_L10 },       //D
			{LCD_COM_L3 , LCD_SEG_L10 },       //E
			{LCD_COM_L1 , LCD_SEG_L10 },       //F
			{LCD_COM_L2 , LCD_SEG_L10 },       //G
		},

		{     //6
			{LCD_COM_L1 , LCD_SEG_L13 },       //A
			{LCD_COM_L2 , LCD_SEG_L13 },       //B
			{LCD_COM_L3 , LCD_SEG_L13 },       //C
			{LCD_COM_L4 , LCD_SEG_L12 },       //D
			{LCD_COM_L3 , LCD_SEG_L12 },       //E
			{LCD_COM_L1 , LCD_SEG_L12 },       //F
			{LCD_COM_L2 , LCD_SEG_L12 },       //G
		},



	};

//---------------- END CUT

#include <stdio.h>

#include <iostream>
using namespace std;

// Total number of LCD registers
#define LCD_REG_COUNT (((LCD_MAX_NBR_OF_SEG + 7)/8)*LCD_MAX_NBR_OF_COM)

// For each of the LCD data regs, put an X if any pixel of the digit touches that reg
// Good for getting a view of which digits impact other digits when updating


void printLcdMap() {

	// Step though each register, listing them vertically down the page

	printf("### LCD register usage by digit map\r\n");

	// Print table header

	printf("\r\n| Reg  | ");

	for (uint8_t dc = 0; dc < 12; dc++) {
		// Digit #11 is the leftmost on the display

		uint8_t d = 11 - dc;
		printf("%2.2d | ", d);

	}


	printf("\r\n| ---  | ");

	for (uint8_t dc = 0; dc < 12; dc++) {
		// Digit #11 is the leftmost on the display

		printf("-- | ");

	}


	// Print rows

	for (uint8_t reg = 0; reg <= LCD_REG_COUNT; reg++) {

		printf("\r\n| [%2.2d] |", reg);


		// Step though each of the 12 digits, listing them horizontally across the page


		for (uint8_t dc = 0; dc < 12; dc++) {


			// Digit #11 is the leftmost on the display

			uint8_t d = 11 - dc;

			// step though the pixels in this digit to see if current register is used.

			// Note that a register can be used multiple times by the same digit, be we only care if it is used at all
			uint8_t reg_used_flag = 0;

			for (uint8_t p = 0; p < 7; p++) {

				const lcd_visible_segment current_pixel = digitmap[d][p];

				if (reg == LCD_REG_OFF(current_pixel.com, current_pixel.seg)) {

					reg_used_flag = 1;

				}

			}

			printf("  %c |", reg_used_flag ? 'X' : ' ');

		}

	}

}





// For each of the LCD data regs, put an X if any pixel of the digit touches that reg
// Good for getting a view of which digits impact other digits when updating

void printLcdLocationMap() {

	// Step though each pixel in each digit and show which register it is in

	printf("\r\n\r\n### LCD register by pixel map (register:bit)\r\n");

	// Print table header

	printf("\r\n| Digit | A | B | C | D | E | F |\r\n");
	printf("| - | - | - | - | - | - | - |\r\n");

	// Print rows

	// Step though each of the 12 digits, listing them horizontally across the page


	for (uint8_t dc = 0; dc < 12; dc++) {


		// Digit #11 is the leftmost on the display

		uint8_t d = 11 - dc;

		printf("| %2.2d | ", d);

		// step though the pixels in this digit and print the register used (A-F = 7 pixels)

		for (uint8_t p = 0; p < 7; p++) {

			const lcd_visible_segment current_pixel = digitmap[d][p];

			printf(" %2.2d:%1.1d |", LCD_REG_OFF(current_pixel.com, current_pixel.seg), current_pixel.seg % 8);

		}

		printf("\r\n");

	}

}

void printLcdCodeHeader() {

	cout << "    // Ths code was generated by the code2code console app " << endl;
	cout << "    // so don't manually edit it. See code2code.MD for more info." << endl;
	cout << "    // " << endl;
	cout << "    // The setuplcdsequence() function gets everything ready and only needs to" << endl;
	cout << "    // be called once when the TSL poweres up. After that, every time you call " << endl;
	cout << "    // runlcdsequence() it will step though a full hour of 1 second updates." << endl;
	cout << "    " << endl;
	cout << "    void setuplcdsequence() {" << endl;

}

// Print the linear progression of LCD steps as digits increment

void printLcdSegSteps() {

	printf("\r\n\r\n### LCD changes by digit\r\n");

	// Print table header

	printf("\r\n| Digit |  COM0 | COM1 | COM2 | COM3 | \r\n");
	printf("| - | - | - | - | - | \r\n");

	// Print rows

	uint8_t coms[LCD_MAX_NBR_OF_COM];

	coms[0] = LCD_COM_L1;
	coms[1] = LCD_COM_L2;
	coms[2] = LCD_COM_L3;
	coms[3] = LCD_COM_L4;

	uint8_t prev_pixels = 0;      // So we can track what changes

	for (uint8_t c = 0; c < 13; c++) {     // We count up to 11 so we can see the transition from 9 to 0 to 1

		uint8_t digit = c % 10;

		uint8_t current_pixels = lcd_font_digits[digit];

		printf("| %2.2d |", digit);

		// Step though each com to see if its register for this digit had to change

		for (uint8_t com = 0; com < LCD_MAX_NBR_OF_COM; com++) {

			uint8_t com_changed_flag = 0;

			for (uint8_t p = 0; p < 7; p++) {

				// Check to see if (1) this pixel changed since last digit, and (2) this pixel is on the COM we are checking right now.

				if (((prev_pixels& (1 << p)) != (current_pixels & (1 << p))) && (digitmap[digit][p].com == coms[com])) {
					com_changed_flag = 1;
				}

			}

			printf(" %c |", com_changed_flag ? 'X' : ' ');
		}

		prev_pixels = current_pixels;

		printf("\r\n");

	}
}

#include <iostream> 
#include <map> 
#include <set>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <sstream>      // std::stringstream


using namespace std;

// A snapshot of the LCD regs

typedef struct {
	int regs[LCD_REG_COUNT];		// You can use -1 to mean "not known" since it will always be different than a real value
} lcd_reg_state;

#define REG_VALUE_UNKNOWN (-1)

// Count all occurances of a key so you can find the most popular ones

template <class key_type>
struct Top_finder {

	map<key_type, int> keys_to_counts;

	void increment(key_type key) {

		keys_to_counts[key]++;

	}


	int get_count_by_key(key_type key) {

		return keys_to_counts[key];

	}

	bool cmp_by_key(int a, int b) {

		return a < b;

	}

	struct Record {

		key_type key;
		int count;

		Record(key_type key, int count) : key(key), count(count) {}

	};


	static bool compare_count_decending(Record a, Record b)
	{
		return (a.count > b.count);
	}

	static bool compare_count_ascending(Record a, Record b)
	{
		return (a.count < b.count);
	}


	vector<Record> sorted_by_count(bool (*sort_function)(Record a, Record b)) {

		vector<Record> keys_sorted_by_count;

		// Add alll the keys/counts to a vector

		for (auto const& pair : keys_to_counts)			// C+++17 only!!!
		{

			key_type key = pair.first;
			int count = pair.second;

			Record new_record(key, count);

			keys_sorted_by_count.push_back(new_record);		// Not insert()!! Ahhh!!!!

		}

		// Sort by count decending

		sort(keys_sorted_by_count.begin(), keys_sorted_by_count.end(), sort_function);


		return keys_sorted_by_count;

	}

	vector<Record> sorted_by_count_decending() {
		return sorted_by_count(compare_count_decending);
	}

	vector<Record> sorted_by_count_ascending() {
		return sorted_by_count(compare_count_ascending);
	}


};

const int string_bufer_len = 80;
typedef char string_buffer[string_bufer_len];

void printasm(const char* s, const char * clobbers , const char *comment) {

	string_buffer clobbers_string;
	string_buffer comment_string;

	if (clobbers && *clobbers) {
		sprintf_s(clobbers_string, string_bufer_len, "\"%s\"", clobbers);
	}
	else {
		sprintf_s(clobbers_string, string_bufer_len, "     " );		// empty (5 spaces balance the quotes and 3 letter register name so everythign lines up nice)
	}

	if (comment && *comment ) {
		sprintf_s(comment_string, string_bufer_len, "    // %s", comment);
	}
	else {
		sprintf_s(comment_string, string_bufer_len, "");		// empty
	}

	printf("    asm(\"%-20s\":::%s);%s\n",  s , clobbers_string , comment_string );

}


void printcomment(const char* comment) {

	printf("    // %s\n", comment);

}

void printblankline() {
	printf("\n");
}


// Actually write out the code block.
// Note that we want the entire list of all state changes here at once rather than taking them one at a time.
// This lets us potentially do global optimizations on them. 

void emit_code_for_lcd_steps(lcd_reg_state initial_lcd_reg_state, lcd_reg_state sequence_of_lcd_reg_states[], int sequence_count) {

	cout << "" << endl;
	cout << "    //  sequence count: " << sequence_count << endl;
	cout << "" << endl;

	lcd_reg_state regs_now;		// Used to hold current register values between steps

	// First we do a trial pass to find which 
	// LCD registers are accessed most often and values are stored most often 

	Top_finder<int> lcd_reg_access_top_finder;
	Top_finder<int> lcd_load_values_access_top_finder;

	// Now count up the accesses!

	regs_now = initial_lcd_reg_state;

	for (int i = 0; i < sequence_count; i++) {

		lcd_reg_state regs_next = sequence_of_lcd_reg_states[i];

		for (uint8_t r = 0; r < LCD_REG_COUNT; r++) {

			// Only care if (1) new value is set, and new value is different from old value
			// note that this will catch a change from unknown to anything else

			if (regs_next.regs[r] != REG_VALUE_UNKNOWN && regs_now.regs[r] != regs_next.regs[r]) {		

				// Count the access to this register
				lcd_reg_access_top_finder.increment(r);

				lcd_load_values_access_top_finder.increment(regs_next.regs[r]);

				regs_now.regs[r] = regs_next.regs[r];

			}

		}

	}

	/*

	printf("| LCD Register | Access Count |\n");
	printf("| - | - |\n");
	lcd_reg_access_counter.print_sorted_key_count_md();

	printf("| LCD Register Value | Access Count |\n");
	printf("| - | - |\n");
	lcd_load_values_access_counter.print_sorted_key_count_md();

	printf("sorted:\n");

	*/

	cout << "// Most accessed LCD registers: " << endl;

	auto sorted_lcd_regsisters = lcd_reg_access_top_finder.sorted_by_count_decending();

	for (auto const& reg : sorted_lcd_regsisters) {

		printf("//    REG: %2d Count: %3d\n", reg.key , reg.count );

	}

	cout << "" << endl;
	cout << "// Most common values saved to LCD registers: " << endl;

	auto sorted_lcd_values = lcd_load_values_access_top_finder.sorted_by_count_decending();

	for (auto const& reg : sorted_lcd_values) {

		printf("//    Value: %02x Count: %3d\n", reg.key, reg.count);

	}

	cout << "" << endl;

	cout << "// Step though a full hour of LCD updates (3600 in all)\n\n";

	cout << "void lcd_run_for_an_hour() {" << endl;

	cout << "" << endl;

	// Remeber the ASM string will will output to store a AVR register to this LCD register
	// This output string must have a %d in it it that will accept the XMEGA register number



	printcomment("First we will load up the indirect pointers to point to the top 3 LCD addresses");
	printcomment("so we can get to those in 1 cycle with ST rather than 2 cycles for an STS");

	string_buffer reg_access_asm_strings[LCD_REG_COUNT];
	string_buffer reg_access_comment_strings[LCD_REG_COUNT];
	int reg_access_cyclecount[LCD_REG_COUNT];					// Keep track of how many cycles this instruction takes for metrics

	bool x_alocated_flag = true;	// TODO: 
	bool y_alocated_flag = true;
	bool z_alocated_flag = true;

	// Run though the LCD registers, most popular first, and asign the top ones to index registers. 

	for (auto const& reg : sorted_lcd_regsisters) {


		if (!x_alocated_flag ) { 

			string_buffer asm_buffer;
			string_buffer comment_buffer;

			sprintf_s( asm_buffer , string_bufer_len, "LDI XH,0x%2.02x" , LCD_REGS_BASE_HIGH );		// Store to index reg X the location of the LCD register
			sprintf_s( comment_buffer, string_bufer_len, "Store high byte of address of LCD reg %02d to X", reg.key);		// Store to index reg Z the location of the LCD register
			printasm(asm_buffer, "r27", comment_buffer);	// Apparently clobbers filed doesnt know about *H register name


			sprintf_s(asm_buffer, string_bufer_len, "LDI XL,0x%2.02x", LCD_REGS_BASE_LOW + reg.key );		// Store to index reg X the location of the LCD register
			sprintf_s(comment_buffer, string_bufer_len, "Store low byte of address of LCD reg %02d to X", reg.key);		// Store to index reg X the location of the LCD register
			printasm(asm_buffer, "r26", comment_buffer); // Apparently clobbers filed doesnt know about *L register name


			sprintf_s( reg_access_asm_strings[reg.key] , string_bufer_len, "ST X,r%%02d" );		// Store to index reg X location
			sprintf_s( reg_access_comment_strings[reg.key], string_bufer_len, "%d accesses to R%02d" , reg.count , reg.key );		// Store to index reg X location
			reg_access_cyclecount[reg.key] = 1; // ST only 1 cycle!
			x_alocated_flag = true;

		}
		else {
					
			sprintf_s( reg_access_asm_strings[reg.key] ,  string_bufer_len, "STS 0x%04X,r%%02d" , (LCD_REGS_BASE_HIGH<<8) + LCD_REGS_BASE_LOW + reg.key );		// // All other registers have to use lowly direct store (2 cycles)
			sprintf_s( reg_access_comment_strings[reg.key], string_bufer_len, "direct store to LCD register %02d", reg.key);		// Store to index reg X location
			reg_access_cyclecount[reg.key] = 2; // STS 2 cycles on XMEGA

		}

	}


	printcomment("Next we will load up the some available XMEGA registers with the most commonly");
	printcomment("stored values so we can save an LDI to load them again.");

	/*
	struct avr_reg_type {

		bool available;		// Can we use this reg?
		bool callsaved;		// do we need to save it? 

	};

	const avr_reg_type avr_regs[32] = {

		{false , false},				// 0 = temp_reg. We will use this for values that are not cached
		{false , false},				// 1 = zero_reg. This comes preloaded. We will use this for 0. 

		{true  , true },				// r2-r17 "Call-used registers. Assembler subroutines are responsible for saving and restoring these registers, if changed."
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },
		{true  , true },

		{true  , false},				// r18-r25 "You may use them freely in assembler subroutines."
		{true  , false},
		{true  , false},
		{true  , false},
		{true  , false},
		{true  , false},
		{true  , false},
		{true  , false},

		{false  , false},				// r26-r27. This is the X index. We will use it for addressing. Need not be saved.
		{false  , false},

		{false  , true },				// r28-r29. This is the Y index. We will use it for addressing. It is call saved (used for stack frame)
		{false  , true },

		{false  , false},				// r30-r31. This is the Z index. We will use it for addressing. Need not be saved.
		{false  , false},

	};

	*/


	// Reset back to start
	regs_now = initial_lcd_reg_state;

	// USed for building emmited strings
	string_buffer asm_buffer;
	string_buffer comment_buffer;

	int total_cycle_count = 0;		// Keep track of total cycles to run though the sequence (assming no blocks or banches)

	//sequence_count = 61;

	for (int i = 0; i < sequence_count; i++) {

		int step_cycle_count = 0;		// How many cycles in this step? 

		printblankline();
		sprintf_s(comment_buffer, string_bufer_len, "---- Step %4d (%02d:%02d)", i , i/60 , i%60 );
		printcomment(comment_buffer);

		lcd_reg_state regs_next = sequence_of_lcd_reg_states[i];

		for (uint8_t r = 0; r < LCD_REG_COUNT; r++) {

			// Only care if (1) new value is set, and new value is different from old value
			// note that this will catch a change from unknown to anything else

			if (regs_next.regs[r] != REG_VALUE_UNKNOWN && regs_now.regs[r] != regs_next.regs[r] ) {

				// Note: we use R18 becuase it is not expected to be saved across calls and LDI needs a register higher than r16 so we can't use temp_reg

				int source_reg;

				if (regs_next.regs[r] == 0x00) {

					// Easy case - we need to store a zero so use the zero reg with is in R01
					// Note that we do not have to load it, it is always zero

					source_reg = 1;

				} 

				sprintf_s( asm_buffer , string_bufer_len , "LDI r18,0x%2.02x" , regs_next.regs[r] );

				if (regs_now.regs[r] == REG_VALUE_UNKNOWN) {
					sprintf_s(comment_buffer, string_bufer_len, "Was UNKNOWN");
				}
				else {
					sprintf_s(comment_buffer, string_bufer_len, "Was %2.2x", regs_now.regs[r]);
				}

				step_cycle_count += 1;		// LDI = 1 cycle

				printasm(asm_buffer, "r18", comment_buffer);	// Apparently clobber field only likes lower case

				sprintf_s(asm_buffer, string_bufer_len, reg_access_asm_strings[r], 18 );
				printasm(asm_buffer, "", reg_access_comment_strings[r] );

				step_cycle_count += reg_access_cyclecount[r];		// Count how ever many cycles this store took

				regs_now.regs[r] = regs_next.regs[r];

			}

		}

		sprintf_s(comment_buffer, string_bufer_len, "--- Cycles in this step: %d", step_cycle_count);
		printcomment(comment_buffer);

		total_cycle_count += step_cycle_count;


		printasm("SLEEP", "", "Wait for interrupt from TX8900");

		/*
		printf("    asm(\"sleep\");  //\n");
		printf("    asm(\"sleep\");  // while (!PORTB.IN & _BV(2))\n");
		printf("    asm(\"sleep\");  \n");
		printf("    asm(\"sleep\");  // while (PORTB.IN & _BV(2))\n");
		*/

	}

	sprintf_s(comment_buffer, string_bufer_len, "--- Total display update cycles in sequence: %d", total_cycle_count );
	printcomment(comment_buffer);


	cout << "}" << endl;

}

// paint a number (0-9) into the indicated place (0-11, 0=right) on the LCD
// assumes pixels are off (does not clear any not set pixels).

void setlcdregsfordigit(lcd_reg_state * lcd_regs, uint8_t place, uint8_t n) {

	// Step though all the pixels in the current digit and see which ones are lit
	for (uint8_t p = 0; p < 7; p++) {

		if (lcd_font_digits[n] & (1 << p)) {

			// This pixel is lit, so set the bit in the show regs

			uint8_t target_reg = LCD_REG_OFF(digitmap[place][p].com, digitmap[place][p].seg);

			// If we have never set this register before, then set it to zero
			// we do this so we only need to set bits for pixels that should be on
			// (0 in the reg means all pixels off)

			if (lcd_regs->regs[target_reg] == REG_VALUE_UNKNOWN) {

				lcd_regs->regs[target_reg] = 0; 

			}

			lcd_regs->regs[target_reg] |= (1 << LCD_REG_BIT(digitmap[place][p].com, digitmap[place][p].seg));

			//printf("set place=%d n=%d p=%d\r\n", place, n, p);

		}

	}
}


// Emit code to update the LCD
// For an hour starting at 0000 going to 5959
// Overwrites whatever is in the pixels in the rightmost 4 digits
// does not change any other digits

void lcdEmit1hourCode() {

	// All regs set to REG_VALUE_UNKNOWN

	lcd_reg_state regs_unknown;

	// Initialize
	for (auto  &reg : regs_unknown.regs) {
		reg = REG_VALUE_UNKNOWN;
	}


	lcd_reg_state regs_zero;

	// Initialize
	for (auto& reg : regs_zero.regs) {
		reg = 0;
	}


	lcd_reg_state regs_init = regs_unknown;		// We do't know any of the registers when we start

	lcd_reg_state reg_steps[60 * 60];			// 60 seconds * 60 minutes = 1 hour

	int step = 0;


	for (uint8_t m = 0; m < 60; m++) {

		for (uint8_t s = 0; s < 60; s++) {


			// Initial all regs to unknown. The set functions below check to see if a 
			// reg that wan to update is unknown, and if so they will set it to zero
			// before setting the pixel bits. 

			// This way any regs that are not touched will stay unknown.

			lcd_reg_state lcd_regs = regs_unknown;

			setlcdregsfordigit(&lcd_regs, 0, s % 10);
			setlcdregsfordigit(&lcd_regs, 1, s / 10);
			setlcdregsfordigit(&lcd_regs, 2, m % 10);
			setlcdregsfordigit(&lcd_regs, 3, m / 10);

			// Ok, now the shadow_regs_next have the values that should be in the LCD registers when we are done.

			reg_steps[step++] = lcd_regs;
		}

	}

	//printf("step=%d\r\n", step);

	emit_code_for_lcd_steps(regs_init, reg_steps, step);



}



int main() {
	lcdEmit1hourCode();
	return 0;
}
