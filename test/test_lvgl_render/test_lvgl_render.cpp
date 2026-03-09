/**
 * LVGL Headless Rendering Test
 *
 * Renders LVGL widgets to a memory framebuffer and exports as PPM.
 * Uses LVGL 7.x display driver API with a custom flush callback.
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

static void lvgl_init_headless() {
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_HOR_RES * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = headless_flush;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    display = lv_disp_drv_register(&disp_drv);

    memset(framebuffer, 0, sizeof(framebuffer));
    tick_val = 0;
}

static void lvgl_render() {
    tick_val += 50;
    lv_tick_inc(50);
    for (int i = 0; i < 10; i++) lv_task_handler();
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

static void save_screenshot(const char* name) {
    char path[256];
    const char* dir = getenv("SIM_SCREENSHOT_DIR");
    if (!dir) dir = "screenshots";
    // Create dir (best effort)
    char cmd[300];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir);
    system(cmd);
    snprintf(path, sizeof(path), "%s/%02d_%s.ppm", dir, screenshot_count++, name);
    save_ppm(path);
    printf("  Screenshot: %s\n", path);
}

void setUp(void) {}
void tearDown(void) {}

void test_lvgl_init(void) {
    lvgl_init_headless();
    TEST_ASSERT_NOT_NULL(display);
    save_screenshot("init");
}

void test_label_render(void) {
    lvgl_init_headless();
    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "TTGO T-Watch");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -40);
    lvgl_render();
    TEST_ASSERT_NOT_NULL(label);
    save_screenshot("label");
}

void test_button_render(void) {
    lvgl_init_headless();
    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, "START");
    lvgl_render();
    TEST_ASSERT_NOT_NULL(btn);
    save_screenshot("button");
}

void test_bar_widget(void) {
    lvgl_init_headless();
    lv_obj_t *bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_bar_set_value(bar, 75, LV_ANIM_OFF);
    lvgl_render();
    TEST_ASSERT_NOT_NULL(bar);
    save_screenshot("bar_75pct");
}

void test_arc_widget(void) {
    lvgl_init_headless();
    lv_obj_t *arc = lv_arc_create(lv_scr_act(), NULL);
    lv_obj_set_size(arc, 150, 150);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_value(arc, 60);
    lvgl_render();
    TEST_ASSERT_NOT_NULL(arc);
    save_screenshot("arc_60");
}

void test_ppm_export_valid(void) {
    lvgl_init_headless();
    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "Export Test");
    lvgl_render();

    const char* dir = getenv("SIM_SCREENSHOT_DIR");
    if (!dir) dir = "screenshots";
    char path[256];
    snprintf(path, sizeof(path), "%s/export_test.ppm", dir);
    bool ok = save_ppm(path);
    TEST_ASSERT_TRUE(ok);

    // Verify file size: P6 header + 240*240*3 bytes
    FILE *f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    // Header is ~15 bytes + 172800 pixel bytes
    TEST_ASSERT_GREATER_THAN(172800, size);
}

void test_multiple_screens(void) {
    lvgl_init_headless();

    // Screen 1: Watch face
    lv_obj_t *scr1 = lv_obj_create(NULL, NULL);
    lv_scr_load(scr1);
    lv_obj_t *time_label = lv_label_create(scr1, NULL);
    lv_label_set_text(time_label, "12:34");
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, 0);
    lvgl_render();
    save_screenshot("screen_watchface");

    // Screen 2: Steps
    lv_obj_t *scr2 = lv_obj_create(NULL, NULL);
    lv_scr_load(scr2);
    lv_obj_t *steps_arc = lv_arc_create(scr2, NULL);
    lv_obj_set_size(steps_arc, 200, 200);
    lv_obj_align(steps_arc, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_value(steps_arc, 45);
    lv_obj_t *steps_label = lv_label_create(scr2, NULL);
    lv_label_set_text(steps_label, "4500\nsteps");
    lv_obj_align(steps_label, NULL, LV_ALIGN_CENTER, 0, 0);
    lvgl_render();
    save_screenshot("screen_steps");

    // Screen 3: Battery
    lv_obj_t *scr3 = lv_obj_create(NULL, NULL);
    lv_scr_load(scr3);
    lv_obj_t *batt_bar = lv_bar_create(scr3, NULL);
    lv_obj_set_size(batt_bar, 180, 30);
    lv_obj_align(batt_bar, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_bar_set_value(batt_bar, 85, LV_ANIM_OFF);
    lv_obj_t *batt_label = lv_label_create(scr3, NULL);
    lv_label_set_text(batt_label, "Battery: 85%");
    lv_obj_align(batt_label, NULL, LV_ALIGN_CENTER, 0, -20);
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
