/**
 * LVGL Headless Rendering Test
 *
 * Renders LVGL widgets to a memory framebuffer and exports as PPM.
 * Uses LVGL 7.x display driver API with a custom flush callback.
 *
 * IMPORTANT: LVGL 7.x lv_init() must only be called ONCE. Each test
 * creates a fresh screen, loads it immediately, and renders.
 */

#include <unity.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "../../src/lvgl/lvgl.h"

#define DISP_HOR_RES 240
#define DISP_VER_RES 240

static lv_color_t framebuffer[DISP_HOR_RES * DISP_VER_RES];
static lv_disp_buf_t disp_buf;
static lv_color_t buf1[DISP_HOR_RES * 10];

static void headless_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            framebuffer[y * DISP_HOR_RES + x] = *color_p;
            color_p++;
        }
    }
    lv_disp_flush_ready(drv);
}

static uint32_t tick_val = 0;
static lv_disp_t *display = nullptr;
static bool lvgl_initialized = false;

/* Color palette */
static const lv_color_t COLOR_TEAL = LV_COLOR_MAKE(0x00, 0x96, 0x88);
static const lv_color_t COLOR_DARK = LV_COLOR_MAKE(0x26, 0x32, 0x38);
static const lv_color_t COLOR_BG = LV_COLOR_MAKE(0xEC, 0xEF, 0xF1);
static const lv_color_t COLOR_GRAY = LV_COLOR_MAKE(0xB0, 0xBE, 0xC5);
static const lv_color_t COLOR_WHITE = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);

static void lvgl_ensure_init() {
    if (lvgl_initialized) return;
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_HOR_RES * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = headless_flush;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    display = lv_disp_drv_register(&disp_drv);
    lvgl_initialized = true;
}

static lv_obj_t* new_screen() {
    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_bg_opa(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_disp_load_scr(scr);  /* Immediate load, no animation */
    return scr;
}

static void lvgl_render() {
    /* Multiple ticks + handler calls ensure full flush to framebuffer */
    for (int i = 0; i < 30; i++) {
        tick_val += 30;
        lv_tick_inc(30);
        lv_task_handler();
    }
    /* Force full refresh by invalidating entire display area */
    lv_obj_invalidate(lv_scr_act());
    for (int i = 0; i < 10; i++) {
        tick_val += 10;
        lv_tick_inc(10);
        lv_task_handler();
    }
}

static bool save_ppm(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return false;
    fprintf(fp, "P6\n%d %d\n255\n", DISP_HOR_RES, DISP_VER_RES);
    for (int i = 0; i < DISP_HOR_RES * DISP_VER_RES; i++) {
        lv_color_t c = framebuffer[i];
        uint8_t rgb[3];
#if LV_COLOR_DEPTH == 16
        rgb[0] = (LV_COLOR_GET_R(c) * 255) / 31;
        rgb[1] = (LV_COLOR_GET_G(c) * 255) / 63;
        rgb[2] = (LV_COLOR_GET_B(c) * 255) / 31;
#else
        rgb[0] = LV_COLOR_GET_R(c);
        rgb[1] = LV_COLOR_GET_G(c);
        rgb[2] = LV_COLOR_GET_B(c);
#endif
        fwrite(rgb, 1, 3, fp);
    }
    fclose(fp);
    return true;
}

static int screenshot_count = 0;
static const char* screenshot_dir = nullptr;

static void save_screenshot(const char* name) {
    char path[256];
    if (!screenshot_dir) {
        screenshot_dir = getenv("SIM_SCREENSHOT_DIR");
        if (!screenshot_dir) screenshot_dir = "screenshots";
    }
    char cmd[300];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", screenshot_dir);
    system(cmd);
    snprintf(path, sizeof(path), "%s/%02d_%s.ppm", screenshot_dir, screenshot_count++, name);
    save_ppm(path);
    printf("  Screenshot: %s\n", path);
}

void setUp(void) {}
void tearDown(void) {}

void test_lvgl_init(void) {
    lvgl_ensure_init();
    new_screen();
    lvgl_render();
    TEST_ASSERT_NOT_NULL(display);
    save_screenshot("init");
}

void test_label_render(void) {
    lvgl_ensure_init();
    lv_obj_t *scr = new_screen();

    lv_obj_t *label = lv_label_create(scr, NULL);
    lv_label_set_text(label, "TTGO T-Watch");
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, LV_STATE_DEFAULT, &lv_font_montserrat_22);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &title_style);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *sub = lv_label_create(scr, NULL);
    lv_label_set_text(sub, "Simulator");
    lv_obj_set_style_local_text_color(sub, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_align(sub, NULL, LV_ALIGN_CENTER, 0, 15);

    lvgl_render();
    TEST_ASSERT_NOT_NULL(label);
    save_screenshot("label");
}

void test_button_render(void) {
    lvgl_ensure_init();
    lv_obj_t *scr = new_screen();

    /* Styled button with teal background */
    lv_obj_t *btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, COLOR_TEAL);
    lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 12);
    lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_shadow_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 6);
    lv_obj_set_style_local_shadow_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, COLOR_GRAY);

    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, "START");
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_WHITE);

    lvgl_render();
    TEST_ASSERT_NOT_NULL(btn);
    save_screenshot("button");
}

void test_bar_widget(void) {
    lvgl_ensure_init();
    lv_obj_t *scr = new_screen();

    /* Title label */
    lv_obj_t *title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Progress: 75%");
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    lv_obj_align(title, NULL, LV_ALIGN_CENTER, 0, -30);

    /* Styled progress bar */
    lv_obj_t *bar = lv_bar_create(scr, NULL);
    lv_obj_set_size(bar, 180, 24);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_radius(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, 12);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, COLOR_TEAL);
    lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_radius(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 12);
    lv_bar_set_value(bar, 75, LV_ANIM_OFF);

    lvgl_render();
    TEST_ASSERT_NOT_NULL(bar);
    save_screenshot("bar_75pct");
}

void test_arc_widget(void) {
    lvgl_ensure_init();
    lv_obj_t *scr = new_screen();

    /* Styled arc gauge */
    lv_obj_t *arc = lv_arc_create(scr, NULL);
    lv_obj_set_size(arc, 160, 160);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_local_line_color(arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, COLOR_TEAL);
    lv_obj_set_style_local_line_width(arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_line_color(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_set_style_local_line_width(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 10);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_arc_set_angles(arc, 135, 297);
    lv_arc_set_value(arc, 60);

    /* Center label */
    lv_obj_t *lbl = lv_label_create(scr, NULL);
    lv_label_set_text(lbl, "60%");
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    lv_obj_align(lbl, arc, LV_ALIGN_CENTER, 0, 0);

    lvgl_render();
    TEST_ASSERT_NOT_NULL(arc);
    save_screenshot("arc_60");
}

void test_ppm_export_valid(void) {
    lvgl_ensure_init();
    lv_obj_t *scr = new_screen();

    lv_obj_t *label = lv_label_create(scr, NULL);
    lv_label_set_text(label, "Export Test");
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lvgl_render();

    if (!screenshot_dir) {
        screenshot_dir = getenv("SIM_SCREENSHOT_DIR");
        if (!screenshot_dir) screenshot_dir = "screenshots";
    }
    char cmd[300];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", screenshot_dir);
    system(cmd);

    char path[256];
    snprintf(path, sizeof(path), "%s/export_test.ppm", screenshot_dir);
    bool ok = save_ppm(path);
    TEST_ASSERT_TRUE(ok);

    FILE *f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    /* Header ~15 bytes + 240*240*3 = 172800 pixel bytes */
    TEST_ASSERT_GREATER_THAN(172800, size);
}

void test_multiple_screens(void) {
    lvgl_ensure_init();

    /* Screen 1: Watch face */
    lv_obj_t *scr1 = new_screen();
    lv_obj_t *time_label = lv_label_create(scr1, NULL);
    lv_label_set_text(time_label, "12:34");
    lv_obj_set_style_local_text_color(time_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    static lv_style_t big_font;
    lv_style_init(&big_font);
    lv_style_set_text_font(&big_font, LV_STATE_DEFAULT, &lv_font_montserrat_28);
    lv_obj_add_style(time_label, LV_LABEL_PART_MAIN, &big_font);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *date_label = lv_label_create(scr1, NULL);
    lv_label_set_text(date_label, "March 9, 2026");
    lv_obj_set_style_local_text_color(date_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 20);

    lvgl_render();
    save_screenshot("screen_watchface");

    /* Screen 2: Steps with arc */
    lv_obj_t *scr2 = new_screen();
    lv_obj_t *steps_arc = lv_arc_create(scr2, NULL);
    lv_obj_set_size(steps_arc, 180, 180);
    lv_obj_align(steps_arc, NULL, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_local_line_color(steps_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, COLOR_TEAL);
    lv_obj_set_style_local_line_width(steps_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 12);
    lv_obj_set_style_local_line_color(steps_arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_set_style_local_line_width(steps_arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 12);
    lv_arc_set_bg_angles(steps_arc, 135, 45);
    lv_arc_set_angles(steps_arc, 135, 256);
    lv_arc_set_value(steps_arc, 45);

    lv_obj_t *steps_label = lv_label_create(scr2, NULL);
    lv_label_set_text(steps_label, "4500\nsteps");
    lv_obj_set_style_local_text_color(steps_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    lv_label_set_align(steps_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(steps_label, NULL, LV_ALIGN_CENTER, 0, -5);

    lvgl_render();
    save_screenshot("screen_steps");

    /* Screen 3: Battery with bar */
    lv_obj_t *scr3 = new_screen();
    lv_obj_t *batt_label = lv_label_create(scr3, NULL);
    lv_label_set_text(batt_label, "Battery: 85%");
    lv_obj_set_style_local_text_color(batt_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_DARK);
    lv_obj_align(batt_label, NULL, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t *batt_bar = lv_bar_create(scr3, NULL);
    lv_obj_set_size(batt_bar, 180, 28);
    lv_obj_align(batt_bar, NULL, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_local_bg_color(batt_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_GRAY);
    lv_obj_set_style_local_bg_opa(batt_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_radius(batt_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, 14);
    lv_obj_set_style_local_bg_color(batt_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, COLOR_TEAL);
    lv_obj_set_style_local_bg_opa(batt_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_radius(batt_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 14);
    lv_bar_set_value(batt_bar, 85, LV_ANIM_OFF);

    lvgl_render();
    save_screenshot("screen_battery");

    TEST_ASSERT_NOT_NULL(scr3);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lvgl_init);
    RUN_TEST(test_label_render);
    RUN_TEST(test_button_render);
    RUN_TEST(test_bar_widget);
    RUN_TEST(test_arc_widget);
    RUN_TEST(test_ppm_export_valid);
    RUN_TEST(test_multiple_screens);
    return UNITY_END();
}
