#ifndef _NAVI_CONFIG_H
#define _NAVI_CONFIG_H

#define _NAVI_VERSION "0.1"

#define _NAVI_LISTING_DRAWING_TOP_Y 4
#define _NAVI_LISTING_DRAWING_TOP_X 1

#define _NAVI_PWD_DRAWING_TOP_X 1
#define _NAVI_PWD_DRAWING_TOP_Y 1

// -102 is the character injected to stdin when
// the terminal is resized (or at least that's the case for me)
#define _NAVI_KEY_RESIZE -102
#define _NAVI_KEY_SCROLLUP 65
#define _NAVI_KEY_SCROLLDOWN 66
#define _NAVI_KEY_OPEN 67
#define _NAVI_KEY_BACK 68
#define _NAVI_KEY_SELECT 10

#define _NAVI_DEFAULT_FILE_ICON 0xf4a5
#define _NAVI_DEFAULT_DIRECTORY_ICON 0xf413

#endif // _NAVI_CONFIG_H