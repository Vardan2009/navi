#ifndef _NAVI_CONFIG_H
#define _NAVI_CONFIG_H

#define _NAVI_VERSION "Development Build"

#define _NAVI_LISTING_DRAWING_TOP_Y 4
#define _NAVI_LISTING_DRAWING_TOP_X 2

#define _NAVI_LISTING_DRAWING_BOTTOM_Y 3

#define _NAVI_PWD_DRAWING_TOP_X 2
#define _NAVI_PWD_DRAWING_TOP_Y 1

// -102 is the character injected to stdin when
// the terminal is resized (or at least that's the case for me)
#define _NAVI_KEY_RESIZE -102
#define _NAVI_KEY_SCROLLUP 65
#define _NAVI_KEY_SCROLLDOWN 66
#define _NAVI_KEY_OPEN 67
#define _NAVI_KEY_BACK 68
#define _NAVI_KEY_SELECT 10

// NOTE: These are color pair numbers, not actual color definitions
#define _NAVI_COLORS_LISTING_NORMAL 1
#define _NAVI_COLORS_LISTING_HIGHLIGHTED 2
#define _NAVI_COLORS_LISTING_DIMMED 3
#define _NAVI_COLORS_LISTING_DIMMED_HIGHLIGHTED 4

#ifdef _WIN32

#define _NAVI_PATH_DIV '\\'

#define _NAVI_CFG_FILE_LOCATION "AppData\\Local\\navi.cfg"

#define _NAVI_HOME_ENV_VAR "USERPROFILE"

#elif defined(__APPLE__) && defined(__MACH__)

#define _NAVI_PATH_DIV '/'

#define _NAVI_HOME_ENV_VAR "HOME"

#define _NAVI_CFG_FILE_LOCATION "Library/Application Support/navi.cfg"

#elif defined(__linux__)

#define _NAVI_PATH_DIV '/'

#define _NAVI_CFG_FILE_LOCATION ".config/navi.cfg"
#define _NAVI_HOME_ENV_VAR "HOME"

#else

#error "config.h: Unsupported platform"

#endif

#endif  // _NAVI_CONFIG_H
