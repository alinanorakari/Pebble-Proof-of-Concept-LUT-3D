#include <pebble.h>
#include <surfacemap.c>
#include <shading.c>

#define ANTIALIASING true
#define WHWIDTH 18

/*
static uint8_t bayer4x4[] = {
   1, 9, 3,11,
  13, 5,15, 7,
   4,12, 2,10,
  16, 8,14, 6,
};
*/

static uint8_t bayer8x8[] = {
   0,32, 8,40, 2,34,10,42,
  48,16,56,24,50,18,58,26,
  12,44, 4,36,14,46, 6,38,
  60,28,52,20,62,30,54,22,
   3,35,11,43, 1,33, 9,41,
  51,19,59,27,49,17,57,25,
  15,47, 7,39,13,45, 5,37,
  63,31,55,23,61,29,53,21,
};

typedef union GColor32 {
  uint32_t argb;
  struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };
} GColor32;

uint8_t ditherfactor = 85;

typedef struct {
  int hours;
  int minutes;
} Time;

static Window *s_main_window;
static Layer *s_canvas_layer;

static GPoint s_center;
static Time s_last_time;

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
  graphics_context_set_stroke_width(ctx, WHWIDTH);
  graphics_draw_line(ctx, s_center, minute_hand_outer);
  
  graphics_context_set_stroke_color(ctx, GColorBulgarianRose);
  graphics_context_set_stroke_width(ctx, WHWIDTH);
  graphics_draw_line(ctx, s_center, hour_hand_outer);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, s_center, WHWIDTH/4);
  
  // define mapping metadata
  GSize mapdimensions = GSize(125, 80);
  GPoint master_offset = GPoint(s_center.x-(mapdimensions.w/2), s_center.y-(mapdimensions.h/2)+4);
  GRect mapbounds = GRect(master_offset.x, master_offset.y, mapdimensions.w, mapdimensions.h);
  GRect shadowbounds = GRect(master_offset.x-2, master_offset.y+16, 122, 71);
  GRect specularbounds = GRect(master_offset.x+31, master_offset.y+8, 74, 47);
  
  GColor bgcolor = GColorPurple;
  
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
    }
  }
  
  // render texture mapped and shaded object
  for(uint8_t y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for(uint8_t x = info.min_x; x <= info.max_x; x++) {
      GColor fbpixel = GColorWhite;
      // render texture mapped object by looking up pixels in the lookup table
      if (x >= mapbounds.origin.x && y >= mapbounds.origin.y && x < mapbounds.origin.x+mapbounds.size.w && y < mapbounds.origin.y+mapbounds.size.h) {
        uint16_t surfindex = (x-mapbounds.origin.x)+((y-mapbounds.origin.y)*mapbounds.size.w);
        uint8_t xpos = 16+un_heptet(surfindex, donutsurfacemapred);
        uint8_t ypos = 127-un_heptet(surfindex, donutsurfacemapgreen);
        if (xpos > 0 && ypos < 127) {
          fbpixel.argb = texture_matrix[ypos][xpos];
        }
      }
      
      // transpose to 24 bit color
      GColor32 newpixel;
      newpixel.r = fbpixel.r*ditherfactor;
      newpixel.g = fbpixel.g*ditherfactor;
      newpixel.b = fbpixel.b*ditherfactor;
      newpixel.a = 0xff;
      
      // render some test shading from the LUT to test dithering
      if (x >= mapbounds.origin.x && y >= mapbounds.origin.y && x < mapbounds.origin.x+mapbounds.size.w && y < mapbounds.origin.y+mapbounds.size.h) {
        uint16_t surfindex = (x-mapbounds.origin.x)+((y-mapbounds.origin.y)*mapbounds.size.w);
        uint8_t green = 2*(127-un_heptet(surfindex, donutsurfacemapgreen));
        if (green < 254) {
          if (newpixel.r - green > 0) {
            newpixel.r -= green;
          } else {
            newpixel.r = 0;
          }
          if (newpixel.g - green > 0) {
            newpixel.g -= green;
          } else {
            newpixel.g = 0;
          }
          if (newpixel.b - green > 0) {
            newpixel.b -= green;
          } else {
            newpixel.b = 0;
          }
        }
      }
      if (x >= mapbounds.origin.x && y >= mapbounds.origin.y && x < mapbounds.origin.x+mapbounds.size.w && y < mapbounds.origin.y+mapbounds.size.h) {
        uint16_t surfindex = (x-mapbounds.origin.x)+((y-mapbounds.origin.y)*mapbounds.size.w);
        uint8_t red = un_heptet(surfindex, donutsurfacemapred);
        if (red > 0) {
          if (red + newpixel.r < 255) {
            newpixel.r += red;
          } else {
            newpixel.r = 255;
          }
          if (red + newpixel.g < 255) {
            newpixel.g += red;
          } else {
            newpixel.g = 255;
          }
          if (red + newpixel.b < 255) {
            newpixel.b += red;
          } else {
            newpixel.b = 255;
          }
        }
      }
      
      // TODO remove test shading and add better shadows and highlights back in later from resources
      
      // render shadows
      /*
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
      }*/
      
      // here comes the actual dithering
      uint8_t bayerpixel = bayer8x8[((x%8)+(y*8))%64];
      fbpixel.r = (newpixel.r+bayerpixel)/ditherfactor;
      fbpixel.g = (newpixel.g+bayerpixel)/ditherfactor;
      fbpixel.b = (newpixel.b+bayerpixel)/ditherfactor;
      fbpixel.a = 0b11;
      
      memset(&info.data[x], fbpixel.argb, 1);
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



