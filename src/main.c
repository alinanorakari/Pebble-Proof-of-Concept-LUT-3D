#include <pebble.h>
#include <surfacemap.c>
#include <shading.c>

#define ANTIALIASING true

static uint8_t shadowtable[] = {192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192, \
                                192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192, \
                                192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192, \
                                192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192, \
                                192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197, \
                                192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197, \
                                192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197, \
                                208,208,208,209,208,208,208,209,208,208,208,209,212,212,212,213, \
                                192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202, \
                                192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202, \
                                208,208,209,210,208,208,209,210,212,212,213,214,216,216,217,218, \
                                224,224,225,226,224,224,225,226,228,228,229,230,232,232,233,234, \
                                192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207, \
                                208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223, \
                                224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239, \
                                240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

// alpha should only be 0b??111111 where ?? = 00 (full shade), 01 (much shade), 10 (some shade), 11 (none shade)
static uint8_t alpha = 0b10111111;

typedef struct {
  int hours;
  int minutes;
} Time;

static Window *s_main_window;
static Layer *s_canvas_layer;

static GPoint s_center;
static Time s_last_time;
static int whwidth = 18;
static bool debug = false;

/************************************ UI **************************************/

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  // dummy time in emulator
  if (debug) {
    s_last_time.hours = 0;
    s_last_time.minutes = tick_time->tm_sec;
  } else {
    s_last_time.hours = tick_time->tm_hour;
    s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
    s_last_time.minutes = tick_time->tm_min;
  }

  // Redraw
  if(s_canvas_layer) {
    layer_mark_dirty(s_canvas_layer);
  }
}

static int32_t get_angle_for_minute(int minute) {
  // Progress through 60 minutes, out of 360 degrees
  return ((minute * 360) / 60);
}

static int32_t get_angle_for_hour(int hour, int minute) {
  // Progress through 12 hours, out of 360 degrees
  return (((hour * 360) / 12)+(get_angle_for_minute(minute)/12));
}

static uint8_t un_heptet_x(uint16_t heptetindex) {
  uint16_t heptoffset = heptetindex / 8;
  uint8_t heptet = donutsurfacemapred[heptetindex-heptoffset];
  // actually don't even bother getting this right, it looks fine even without decoding the spread out heptet
  /*if (heptetindex%8 == 6) {
    heptet =
      (donutsurfacemapred[heptetindex-heptoffset-6]&0b00000001)<<6 | 
      (donutsurfacemapred[heptetindex-heptoffset-5]&0b00000001)<<5 |
      (donutsurfacemapred[heptetindex-heptoffset-4]&0b00000001)<<4 |
      (donutsurfacemapred[heptetindex-heptoffset-3]&0b00000001)<<3 |
      (donutsurfacemapred[heptetindex-heptoffset-2]&0b00000001)<<2 |
      (donutsurfacemapred[heptetindex-heptoffset-1]&0b00000001)<<1 |
      (donutsurfacemapred[heptetindex-heptoffset]&0b00000001);
  } else {*/
    heptet = heptet >> 1;
  /*}*/
  return (heptet+16);
}
static uint8_t un_heptet_y(uint16_t heptetindex) {
  uint16_t heptoffset = heptetindex / 8;
  uint8_t heptet = donutsurfacemapgreen[heptetindex-heptoffset];
  heptet = heptet >> 1;
  return (127-heptet);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GColor bgcolor = GColorPurple;
  
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorRajah);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_antialiased(ctx, ANTIALIASING);
  GRect bounds_h = bounds;
  bounds_h.size.h = bounds_h.size.w;
  bounds_h.origin.x -= (bounds_h.size.w-bounds.size.w)/2;

  GRect bounds_mo = grect_inset(bounds_h, GEdgeInsets(19));
  GRect bounds_ho = grect_inset(bounds_h, GEdgeInsets(32));

  // Adjust for minutes through the hour
  int32_t hour_deg = get_angle_for_hour(s_last_time.hours, s_last_time.minutes);
  int32_t minute_deg = get_angle_for_minute(s_last_time.minutes);
  
  // draw the watch hands that will be used as texture
  GPoint minute_hand_outer = gpoint_from_polar(bounds_mo, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(minute_deg));
  GPoint hour_hand_outer = gpoint_from_polar(bounds_ho, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(hour_deg));
  graphics_context_set_stroke_color(ctx, GColorWindsorTan);
  graphics_context_set_stroke_width(ctx, whwidth);
  graphics_draw_line(ctx, s_center, minute_hand_outer);
  graphics_context_set_stroke_color(ctx, GColorBulgarianRose);
  graphics_context_set_stroke_width(ctx, whwidth);
  graphics_draw_line(ctx, s_center, hour_hand_outer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, s_center, whwidth/4);
  
  // define mapping metadata
  GPoint donutoffset = GPoint(9, 42);
  GPoint shadowoffset = GPoint(7, 58);
  GPoint specularoffset = GPoint(40, 50);
  
  GSize donutsurfacemapsize = GSize(125, 80);
  GSize donutshadowsize = GSize(122, 71);
  GSize donutspecularsize = GSize(74, 47);
  
  // capture frame buffer
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  
  // set up texture buffer
  GRect texturebounds = GRect(s_center.x-64, s_center.y-64, 127, 127);
  GSize texturesize = GSize(texturebounds.size.w, texturebounds.size.h);
  GBitmap *texture = gbitmap_create_blank(texturesize, GBitmapFormat8Bit);
  uint8_t (*texture_matrix)[texturesize.w] = (uint8_t (*)[texturesize.w]) gbitmap_get_data(texture);
  
  // capture texture and fill frame buffer with new background color
  for(int y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for(int x = info.min_x; x < info.max_x; x++) {
      if (x >= texturebounds.origin.x && y >= texturebounds.origin.y && x < texturebounds.origin.x+texturebounds.size.w && y < texturebounds.origin.y+texturebounds.size.h) {
        texture_matrix[y-texturebounds.origin.y][x-texturebounds.origin.x] = info.data[x];
      }
      memset(&info.data[x], bgcolor.argb, 1);
    }
  }
  
  // render texture mapped around shape by looking up pixels in the lookup table
  for(int y = 0; y < donutsurfacemapsize.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y+donutoffset.y);
    for(int x = donutoffset.x; x < donutsurfacemapsize.w+donutoffset.x; x++) {
      if (x+donutoffset.x < info.max_x && x+donutoffset.x >= info.min_x) {
        uint16_t surfindex = x+(y*donutsurfacemapsize.w)-donutoffset.x;
        uint8_t xpos = un_heptet_x(surfindex);
        uint8_t ypos = un_heptet_y(surfindex);
        if (xpos > 0 && ypos < 127) {
          memset(&info.data[x], texture_matrix[ypos][xpos], 1);
        }
      }
    }
  }
  
  // render shadows
  for(int y = 0; y < donutshadowsize.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y+shadowoffset.y);
    for(int x = shadowoffset.x; x < donutshadowsize.w+shadowoffset.x; x++) {
      if (x+shadowoffset.x < info.max_x && x+shadowoffset.x >= info.min_x) {
        uint16_t shad_index = x+(y*donutshadowsize.w)-shadowoffset.x;
        uint8_t shad_octet = donutshadow[shad_index/8];
        bool shad_dark = 0;
        if (info.data[x] != bgcolor.argb && (x+y) % 2 == 0) {
          shad_dark = 0;
        } else {
          switch (shad_index % 8) {
            case 0:
              shad_dark = 0b10000000 & shad_octet;
              break;
            case 1:
              shad_dark = 0b01000000 & shad_octet;
              break;
            case 2:
              shad_dark = 0b00100000 & shad_octet;
              break;
            case 3:
              shad_dark = 0b00010000 & shad_octet;
              break;
            case 4:
              shad_dark = 0b00001000 & shad_octet;
              break;
            case 5:
              shad_dark = 0b00000100 & shad_octet;
              break;
            case 6:
              shad_dark = 0b00000010 & shad_octet;
              break;
            case 7:
              shad_dark = 0b00000001 & shad_octet;
              break;
          }
        }
        if (shad_dark) {
          memset(&info.data[x], shadowtable[alpha & info.data[x]], 1);
        }
      }
    }
  }
  
  // render highlights
  for(int y = 0; y < donutspecularsize.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y+specularoffset.y);
    for(int x = specularoffset.x; x < donutspecularsize.w+specularoffset.x; x++) {
      if (x+specularoffset.x < info.max_x && x+specularoffset.x >= info.min_x) {
        uint16_t spec_index = x+(y*donutspecularsize.w)-specularoffset.x;
        uint8_t spec_octet = donutspecular[spec_index/8];
        bool spec_white = 0;
        if ((x+y) % 2 == 0) {
          spec_white = 0;
        } else {
          switch (spec_index % 8) {
            case 0:
              spec_white = 0b10000000 & spec_octet;
              break;
            case 1:
              spec_white = 0b01000000 & spec_octet;
              break;
            case 2:
              spec_white = 0b00100000 & spec_octet;
              break;
            case 3:
              spec_white = 0b00010000 & spec_octet;
              break;
            case 4:
              spec_white = 0b00001000 & spec_octet;
              break;
            case 5:
              spec_white = 0b00000100 & spec_octet;
              break;
            case 6:
              spec_white = 0b00000010 & spec_octet;
              break;
            case 7:
              spec_white = 0b00000001 & spec_octet;
              break;
          }
        }
        if (spec_white) {
          GColor highlight = GColorIcterine;
          memset(&info.data[x], highlight.argb, 1);
        }
      }
    }
  }
  
  // release frame buffer and destroy texture
  graphics_release_frame_buffer(ctx, fb);
  gbitmap_destroy(texture);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  s_center = grect_center_point(&window_bounds);
  s_center.x -= 1;
  s_center.y -= 1;

  s_canvas_layer = layer_create(window_bounds);
  layer_set_update_proc(s_canvas_layer, update_proc);
  layer_add_child(window_layer, s_canvas_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

/*********************************** App **************************************/

static void init() {
  srand(time(NULL));

  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  if (debug) {
    tick_handler(time_now, SECOND_UNIT);
  } else {
    tick_handler(time_now, MINUTE_UNIT);
  }

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  if (debug) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
  
  if (debug) {
    light_enable(true);
  }

}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}

