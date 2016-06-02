#include <pebble.h>
#include <surfacemap.c>
#include <shading.c>

#define ANTIALIASING true

static uint8_t shadowtable[] = {
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,
  192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,
  192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,
  208,208,208,209,208,208,208,209,208,208,208,209,212,212,212,213,
  192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202,
  192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202,
  208,208,209,210,208,208,209,210,212,212,213,214,216,216,217,218,
  224,224,225,226,224,224,225,226,228,228,229,230,232,232,233,234,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

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

static uint8_t un_heptet(uint16_t heptetindex, uint8_t *map) {
  uint16_t heptoffset = heptetindex/8;
  uint8_t heptet = map[heptetindex-heptoffset];
  // actually don't even bother getting this right, it looks fine even without decoding the spread out heptet
  /*if (heptetindex%8 == 6) {
    heptet =
      (map[heptetindex-heptoffset-6]&0b00000001)<<6 |
      (map[heptetindex-heptoffset-5]&0b00000001)<<5 |
      (map[heptetindex-heptoffset-4]&0b00000001)<<4 |
      (map[heptetindex-heptoffset-3]&0b00000001)<<3 |
      (map[heptetindex-heptoffset-2]&0b00000001)<<2 |
      (map[heptetindex-heptoffset-1]&0b00000001)<<1 |
      (map[heptetindex-heptoffset]&0b00000001);
  } else {
    heptet = heptet >> 1;
  }*/
  heptet = heptet >> 1;
  return (heptet);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect texturebounds = GRect(s_center.x-64, s_center.y-64, 127, 127);
  GRect bounds_mo = grect_inset(texturebounds, GEdgeInsets(9));
  GRect bounds_ho = grect_inset(texturebounds, GEdgeInsets(23));

  // Adjust for minutes through the hour
  int32_t hour_deg = get_angle_for_hour(s_last_time.hours, s_last_time.minutes);
  int32_t minute_deg = get_angle_for_minute(s_last_time.minutes);
  GPoint minute_hand_outer = gpoint_from_polar(bounds_mo, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(minute_deg));
  GPoint hour_hand_outer = gpoint_from_polar(bounds_ho, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(hour_deg));
  
  // draw the watch hands that will be used as texture
  graphics_context_set_antialiased(ctx, ANTIALIASING);
  graphics_context_set_fill_color(ctx, GColorRajah);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  graphics_context_set_stroke_color(ctx, GColorWindsorTan);
  graphics_context_set_stroke_width(ctx, whwidth);
  graphics_draw_line(ctx, s_center, minute_hand_outer);
  
  graphics_context_set_stroke_color(ctx, GColorBulgarianRose);
  graphics_context_set_stroke_width(ctx, whwidth);
  graphics_draw_line(ctx, s_center, hour_hand_outer);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, s_center, whwidth/4);
  
  // define mapping metadata
  GSize mapdimensions = GSize(125, 80);
  GPoint master_offset = GPoint(s_center.x-(mapdimensions.w/2), s_center.y-(mapdimensions.h/2)+4);
  GRect mapbounds = GRect(master_offset.x, master_offset.y, mapdimensions.w, mapdimensions.h);
  GRect shadowbounds = GRect(master_offset.x-2, master_offset.y+16, 122, 71);
  GRect specularbounds = GRect(master_offset.x+31, master_offset.y+8, 74, 47);
  
  GColor bgcolor = GColorPurple;
  GColor highlight = GColorIcterine;
  
  // capture frame buffer
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  
  // set up texture buffer
  GSize texturesize = GSize(texturebounds.size.w, texturebounds.size.h);
  GBitmap *texture = gbitmap_create_blank(texturesize, GBitmapFormat8Bit);
  uint8_t (*texture_matrix)[texturesize.w] = (uint8_t (*)[texturesize.w]) gbitmap_get_data(texture);
  
  // capture texture and fill frame buffer with new background color
  for(uint8_t y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for(uint8_t x = info.min_x; x <= info.max_x; x++) {
      if (x >= texturebounds.origin.x && y >= texturebounds.origin.y && x < texturebounds.origin.x+texturebounds.size.w && y < texturebounds.origin.y+texturebounds.size.h) {
        texture_matrix[y-texturebounds.origin.y][x-texturebounds.origin.x] = info.data[x];
      }
      memset(&info.data[x], bgcolor.argb, 1);
    }
  }
  
  // render texture mapped and shaded object
  for(uint8_t y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for(uint8_t x = info.min_x; x <= info.max_x; x++) {
      // render texture mapped object by looking up pixels in the lookup table
      if (x >= mapbounds.origin.x && y >= mapbounds.origin.y && x < mapbounds.origin.x+mapbounds.size.w && y < mapbounds.origin.y+mapbounds.size.h) {
        uint16_t surfindex = (x-mapbounds.origin.x)+((y-mapbounds.origin.y)*mapbounds.size.w);
        //uint8_t xpos = 16+un_heptet_x(surfindex);
        uint8_t xpos = 16+un_heptet(surfindex, donutsurfacemapred);
        uint8_t ypos = 127-un_heptet(surfindex, donutsurfacemapgreen);
        if (xpos > 0 && ypos < 127) {
          memset(&info.data[x], texture_matrix[ypos][xpos], 1);
        }
      }
      // render shadows
      if (x >= shadowbounds.origin.x && y >= shadowbounds.origin.y && x < shadowbounds.origin.x+shadowbounds.size.w && y < shadowbounds.origin.y+shadowbounds.size.h) {
        uint16_t shad_index = (x-shadowbounds.origin.x)+((y-shadowbounds.origin.y)*shadowbounds.size.w);
        uint8_t shad_octet = donutshadow[shad_index/8];
        bool shad_dark = 0;
        if (info.data[x] != bgcolor.argb && (x+y) % 2 == 0) {
          // keep shadowless, creating dithering effect
        } else {
          // fetch single bit out of bitfield
          shad_dark = (0b10000000 >> shad_index%8) & shad_octet;
        }
        if (shad_dark) {
          memset(&info.data[x], shadowtable[alpha & info.data[x]], 1);
        }
      }
      // render specular highlights
      if (x >= specularbounds.origin.x && y >= specularbounds.origin.y && x < specularbounds.origin.x+specularbounds.size.w && y < specularbounds.origin.y+specularbounds.size.h) {
        uint16_t spec_index = (x-specularbounds.origin.x)+((y-specularbounds.origin.y)*specularbounds.size.w);
        uint8_t spec_octet = donutspecular[spec_index/8];
        bool spec_white = 0;
        if ((x+y) % 2 == 0) {
          // keep highlightless, creating dithering effect
        } else {
          // fetch single bit out of bitfield
          spec_white = (0b10000000 >> spec_index%8) & spec_octet;
        }
        if (spec_white) {
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


