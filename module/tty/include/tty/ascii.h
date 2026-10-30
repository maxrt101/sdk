#pragma once

#define ASCII_NULL                      0x0
#define ASCII_START_OF_HEADING          0x1 // Commonly ^A - move to the line start
#define ASCII_START_OF_TEXT             0x2
#define ASCII_END_OF_TEXT               0x3
#define ASCII_END_OF_TRANSMISSION       0x4
#define ASCII_ENQUIRY                   0x5 // Commonly ^E - move to the line end
#define ASCII_ACKNOWLEDGE               0x6
#define ASCII_BELL                      0x7
#define ASCII_BACKSPACE                 0x8
#define ASCII_HORIZONTAL_TAB            0x9
#define ASCII_LINE_FEED                 0xA // LF - \n
#define ASCII_VERTICAL_TAB              0xB
#define ASCII_FORM_FEED                 0xC
#define ASCII_CARRIAGE_RETURN           0xD // CR - \r
#define ASCII_SHIFT_OUT                 0xE
#define ASCII_SHIFT_IN                  0xF
#define ASCII_DATA_LINK_ESCAPE          0x10
#define ASCII_DEVICE_CONTROL_1          0x11
#define ASCII_DEVICE_CONTROL_2          0x12
#define ASCII_DEVICE_CONTROL_3          0x13
#define ASCII_DEVICE_CONTROL_4          0x14
#define ASCII_NEGATIVE_ACKNOWLEDGE      0x15
#define ASCII_SYNCHRONOUS_IDLE          0x16
#define ASCII_END_OF_TRANSMISSION_BLOCK 0x17
#define ASCII_CANCEL                    0x18
#define ASCII_END_OF_MEDIUM             0x19
#define ASCII_SUBSTITUTE                0x1A
#define ASCII_ESCAPE                    0x1B
#define ASCII_FILE_SEPARATOR            0x1C
#define ASCII_GROUP_SEPARATOR           0x1D
#define ASCII_RECORD_SEPARATOR          0x1E
#define ASCII_UNIT_SEPARATOR            0x1F
#define ASCII_DEL                       0x7F // In MacOS - same as BACKSPACE, but only as input
