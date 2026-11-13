#pragma once

// Font Awesome 7 icon codepoints used by this menu.
// The values are Unicode codepoints encoded as UTF-8 string literals.
// Add the uploaded Font Awesome 7 OTF files to src/resources/fonts/ for rendering.
#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf8ff

#define ICON_FA_CROSSHAIRS "\xef\x81\x9b" // f05b
#define ICON_FA_EYE        "\xef\x81\xae" // f06e
#define ICON_FA_CODE       "\xef\x84\xa1" // f121
#define ICON_FA_SLIDERS    "\xef\x87\x9e" // f1de
#define ICON_FA_FOLDER     "\xef\x81\xbb" // f07b
#define ICON_FA_GEAR       "\xef\x80\x93" // f013

#define ICON_FA_BULLSEYE   "\xef\x85\x80" // f140
#define ICON_FA_FILTER     "\xef\x82\xb0" // f0b0
#define ICON_FA_USERS      "\xef\x83\x80" // f0c0
#define ICON_FA_USER       "\xef\x80\x87" // f007
#define ICON_FA_SHIELD     "\xef\x84\xb2" // f132
#define ICON_FA_BOMB       "\xef\x87\xa2" // f1e2
#define ICON_FA_BOX        "\xef\x91\xa6" // f466
#define ICON_FA_PALETTE    "\xef\x94\xbf" // f53f
#define ICON_FA_TERMINAL   "\xef\x84\xa0" // f120
#define ICON_FA_CLOCK      "\xef\x80\x97" // f017
#define ICON_FA_WRENCH     "\xef\x82\xad" // f0ad
#define ICON_FA_LIST       "\xef\x80\xba" // f03a
#define ICON_FA_CIRCLE_DOT "\xef\x86\x92" // f192
#define ICON_FA_LAYER      "\xef\x97\xbd" // f5fd
#define ICON_FA_BRUSH      "\xef\x87\xbc" // f1fc
#define ICON_FA_FOLDER_OPEN "\xef\x81\xbc" // f07c

#define ICON_FA_GLOBE      "\xef\x82\xac" // f0ac
#define ICON_FA_BONE       "\xef\x97\x97" // f5d7

// Fire / flame icon (Font Awesome 'fire' = f06d)
#define ICON_FA_FIRE       "\xef\x81\xad" // f06d

// Alias for a 'devil' style icon: use the fire glyph as a thematic replacement.
// If you prefer a different glyph, replace ICON_FA_FIRE with the desired UTF-8 sequence.
#define ICON_FA_DEVIL ICON_FA_FIRE
