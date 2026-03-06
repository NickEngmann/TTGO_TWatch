/**
 * @file lv_conf.h
 * Configuration file for LVGL v7.7.2
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_COLOR_SCREEN_TRANSP 0
#define LV_COLOR_MIX_ROUND_OFS 0

/*==================
   CONFIG SETTINGS
 *==================*/
#define LV_USE_EVAL 0

/*==================
   FONT SETTINGS
 *==================*/
#define LV_FONT_FMT_TXT_LARGE 0

/*===================
 * HAL SETTINGS
 *===================*/
#define LV_TICK_PERIOD_MS 1
#define LV_DISP_DEF_REFR_PERIOD 30
#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_USE_SYS_TICK 0

/*========================
 * RENDERING CONFIG
 *========================*/
#define LV_USE_GPU_STM32_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_GPU_SDL 0
#define LV_USE_GPU_OSDP 0
#define LV_USE_GPU_FREETYPE 0
#define LV_USE_GPU_MCX 0
#define LV_USE_BLEND_MODES 0

/*==================
 * DEVICES USAGE
 *==================*/
#define LV_USE_X11 0

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
#define LV_USE_GESTURE 0

/*==================
 * THEME USAGE
 *==================*/
#define LV_USE_THEME_SIMPLE 1
#define LV_USE_THEME_MONO 1
#define LV_USE_THEME_ALIEN 1
#define LV_USE_THEME_DEFAULT 1

/*==================
 * WIDGETS
 *==================*/
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LED 1
#define LV_USE_LINE 1
#define LV_USE_LIST 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1
#define LV_USE_CALENDAR 1
#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 1
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1

/*==================
 * EXTRA COMPONENTS
 *==================*/
#define LV_USE_ANIMIMG 1
#define LV_USE_BARCHART 1

/*==================
 * LOGGING
 *==================*/
#define LV_USE_LOG 0
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0

/*==================
 * DISPLAY USAGE
 *==================*/
#define LV_USE_DRIVER 0

/*===================
 * INPUT DEVICE
 *===================*/
#define LV_USE_INPUTDRIVER 0

/*===================
 * LAYOUT USAGE
 *===================*/
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*===================
 * STRING USAGE
 *===================*/
#define LV_USE_UTF8 1

/*===================
 * OTHER USAGE
 *===================*/
#define LV_USE_USER_INTERFACE 0
#define LV_USE_FILESYSTEM 0
#define LV_USE_ANIMATION 1

/*===================
 * COMPILE TIME
 *===================*/
#define LV_CONF_INCLUDE_SIMPLE 1


/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_COLOR_SCREEN_TRANSP 0
#define LV_COLOR_MIX_ROUND_OFS 0

/*==================
   CONFIG SETTINGS
 *==================*/
#define LV_USE_EVAL 0

/*==================
   FONT SETTINGS
 *==================*/
#define LV_FONT_FMT_TXT_LARGE 0

/*===================
 * HAL SETTINGS
 *===================*/
#define LV_TICK_PERIOD_MS 1
#define LV_DISP_DEF_REFR_PERIOD 30
#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_USE_SYS_TICK 0

/*========================
 * RENDERING CONFIG
 *========================*/
#define LV_USE_GPU_STM32_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_GPU_SDL 0
#define LV_USE_GPU_OSDP 0
#define LV_USE_GPU_FREETYPE 0
#define LV_USE_GPU_MCX 0
#define LV_USE_BLEND_MODES 0

/*==================
 * DEVICES USAGE
 *==================*/
#define LV_USE_X11 0

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
#define LV_USE_GESTURE 0

/*==================
 * THEME USAGE
 *==================*/
#define LV_USE_THEME_SIMPLE 1
#define LV_USE_THEME_MONO 1
#define LV_USE_THEME_ALIEN 1
#define LV_USE_THEME_DEFAULT 1

/*==================
 * WIDGETS
 *==================*/
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LED 1
#define LV_USE_LINE 1
#define LV_USE_LIST 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1
#define LV_USE_CALENDAR 1
#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 1
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1

/*==================
 * EXTRA COMPONENTS
 *==================*/
#define LV_USE_ANIMIMG 1
#define LV_USE_BARCHART 1

/*==================
 * LOGGING
 *==================*/
#define LV_USE_LOG 0
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0

/*==================
 * DISPLAY USAGE
 *==================*/
#define LV_USE_DRIVER 0

/*===================
 * INPUT DEVICE
 *===================*/
#define LV_USE_INPUTDRIVER 0

/*===================
 * LAYOUT USAGE
 *===================*/
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*===================
 * STRING USAGE
 *===================*/
#define LV_USE_UTF8 1

/*===================
 * OTHER USAGE
 *===================*/
#define LV_USE_USER_INTERFACE 0
#define LV_USE_FILESYSTEM 0
#define LV_USE_IMG 1
#define LV_USE_ANIMATION 1

#endif /*LV_CONF_H*/
