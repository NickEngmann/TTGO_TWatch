/**
 * LVGL Headless Rendering Test
 * 
 * Renders LVGL widgets to a memory framebuffer and exports as PNG.
 * Uses LVGL 7.11 display driver API with a custom flush callback.
 */

#include <unity.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <png.h>

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

static bool save_png(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return false;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    if (setjmp(png_jmpbuf(png))) { fclose(fp); return false; }

    png_init_io(png, fp);
    png_set_IHDR(png, info, DISP_HOR_RES, DISP_VER_RES, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    uint8_t *row = (uint8_t *)malloc(DISP_HOR_RES * 3);
    for (int y = 0; y < DISP_VER_RES; y++) {
        for (int x = 0; x < DISP_HOR_RES; x++) {
            lv_color_t c = framebuffer[y * DISP_HOR_RES + x];
            // Use LVGL macros for portable color extraction
            uint8_t r8 = LV_COLOR_GET_R(c);
            uint8_t g8 = LV_COLOR_GET_G(c);
            uint8_t b8 = LV_COLOR_GET_B(c);
            // Scale from 5/6/5 bit to 8 bit
            row[x * 3 + 0] = (r8 << 3) | (r8 >> 2);
            row[x * 3 + 1] = (g8 << 2) | (g8 >> 4);
            row[x * 3 + 2] = (b8 << 3) | (b8 >> 2);
        }
        png_write_row(png, row);
    }
    free(row);

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    return true;
}

static void init_lvgl_headless(void) {
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_HOR_RES * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = headless_flush;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    display = lv_disp_drv_register(&disp_drv);

    memset(framebuffer, 0, sizeof(framebuffer));
}

static void pump_lvgl(int frames) {
    for (int i = 0; i < frames; i++) {
        tick_val += 5;
        lv_tick_inc(5);
        lv_task_handler();
    }
}

void setUp(void) {}
void tearDown(void) {}

void test_lvgl_init_succeeds(void) {
    init_lvgl_headless();
    TEST_ASSERT_NOT_NULL(display);
}

void test_render_label(void) {
    lv_obj_t *scr = lv_disp_get_scr_act(display);
    TEST_ASSERT_NOT_NULL(scr);

    lv_obj_t *label = lv_label_create(scr, NULL);
    lv_label_set_text(label, "TTGO T-Watch");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -40);
    pump_lvgl(20);

    bool found_nonblack = false;
    for (int i = 0; i < DISP_HOR_RES * DISP_VER_RES; i++) {
        if (framebuffer[i].full != 0) { found_nonblack = true; break; }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_nonblack, "Label should render non-black pixels");
}

void test_render_button(void) {
    lv_obj_t *scr = lv_disp_get_scr_act(display);
    lv_obj_t *btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_obj_t *btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Press Me");
    pump_lvgl(20);

    int cy = DISP_VER_RES / 2 + 20;
    int cx = DISP_HOR_RES / 2;
    lv_color_t px = framebuffer[cy * DISP_HOR_RES + cx];
    TEST_ASSERT_MESSAGE(px.full != 0, "Button center should not be black");
}

void test_render_bar(void) {
    lv_obj_t *scr = lv_disp_get_scr_act(display);
    lv_obj_t *bar = lv_bar_create(scr, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 80);
    lv_bar_set_value(bar, 70, LV_ANIM_OFF);
    pump_lvgl(20);
    TEST_ASSERT_NOT_NULL(bar);
}

void test_render_arc(void) {
    lv_obj_t *scr = lv_disp_get_scr_act(display);
    lv_obj_t *arc = lv_arc_create(scr, NULL);
    lv_arc_set_angles(arc, 0, 270);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, -40);
    pump_lvgl(20);
    TEST_ASSERT_NOT_NULL(arc);
}

void test_export_png(void) {
    pump_lvgl(30);
    const char *outfile = "/tmp/lvgl_test_render.png";
    bool ok = save_png(outfile);
    TEST_ASSERT_TRUE_MESSAGE(ok, "PNG export should succeed");

    FILE *f = fopen(outfile, "rb");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "PNG file should exist");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    TEST_ASSERT_GREATER_THAN_MESSAGE(100, size, "PNG should have content");
    printf("PNG exported: %s (%ld bytes)\n", outfile, size);
}

void test_multiple_screens(void) {
    lv_obj_t *scr2 = lv_obj_create(NULL, NULL);
    TEST_ASSERT_NOT_NULL(scr2);
    lv_obj_t *label2 = lv_label_create(scr2, NULL);
    lv_label_set_text(label2, "Screen 2");
    lv_obj_align(label2, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_scr_load(scr2);
    pump_lvgl(30);
    bool ok = save_png("/tmp/lvgl_screen2.png");
    TEST_ASSERT_TRUE(ok);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lvgl_init_succeeds);
    RUN_TEST(test_render_label);
    RUN_TEST(test_render_button);
    RUN_TEST(test_render_bar);
    RUN_TEST(test_render_arc);
    RUN_TEST(test_export_png);
    RUN_TEST(test_multiple_screens);
    return UNITY_END();
}
