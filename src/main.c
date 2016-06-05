#include <pebble.h>

#define ANTIALIASING true
#define WHWIDTH 18
#define DITHERFACTOR 85

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

typedef struct {
  int hours;
  int minutes;
} Time;

static Window *s_main_window;
static Layer *s_canvas_layer;

static GPoint s_center;
static Time s_last_time;

static bool debug = true;

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
  graphics_context_set_fill_color(ctx, GColorShockingPink);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
  graphics_context_set_stroke_width(ctx, WHWIDTH);
  graphics_draw_line(ctx, s_center, minute_hand_outer);
  
  graphics_context_set_stroke_color(ctx, GColorCeleste);
  graphics_context_set_stroke_width(ctx, WHWIDTH);
  graphics_draw_line(ctx, s_center, hour_hand_outer);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, s_center, WHWIDTH/4);
  
  GColor bgcolor = GColorIcterine; // background color for behind the objects
  
  // load map parts
  uint8_t map_numlines = 104; // max 168 (did 163)
  
  ResHandle lut_handle = resource_get_handle(RESOURCE_ID_MAP_TEST);
  size_t lut_res_size = 144*map_numlines*2;
  uint8_t *lut_buffer = (uint8_t*)malloc(lut_res_size);
  resource_load_byte_range(lut_handle, 0, lut_buffer, lut_res_size);
  
  ResHandle shad_handle = resource_get_handle(RESOURCE_ID_SHADING_TEST);
  size_t shad_res_size = 144*map_numlines;
  uint8_t *shad_buffer = (uint8_t*)malloc(shad_res_size);
  resource_load_byte_range(shad_handle, 0, shad_buffer, shad_res_size);
  
  // define mapping metadata
  //GSize mapdimensions = GSize(125, 80);
  GSize mapdimensions = GSize(144, map_numlines);
  //GPoint master_offset = GPoint(s_center.x-(mapdimensions.w/2), s_center.y-(mapdimensions.h/2)+4);
  GPoint master_offset = GPoint(0, 0);
  GRect mapbounds = GRect(master_offset.x, master_offset.y, mapdimensions.w, mapdimensions.h);
  
  // capture frame buffer
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  
  // set up texture buffer
  GSize texturesize = GSize(texturebounds.size.w, texturebounds.size.h);
  GBitmap *texture = gbitmap_create_blank(texturesize, GBitmapFormat8Bit);
  uint8_t (*texture_matrix)[texturesize.w] = (uint8_t (*)[texturesize.w]) gbitmap_get_data(texture);
  
  // capture texture before starting to modify the frame buffer
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
      GColor fbpixel = bgcolor;
      
      // convert to 24 bit color
      GColor32 newpixel;
      newpixel.r = fbpixel.r*DITHERFACTOR;
      newpixel.g = fbpixel.g*DITHERFACTOR;
      newpixel.b = fbpixel.b*DITHERFACTOR;
      newpixel.a = 0xff;
      
      // render texture mapped object by looking up pixels in the lookup table
      if (x >= mapbounds.origin.x && y >= mapbounds.origin.y && x < mapbounds.origin.x+mapbounds.size.w && y < mapbounds.origin.y+mapbounds.size.h) {
        uint16_t surfindex = (x-mapbounds.origin.x)+((y-mapbounds.origin.y)*mapbounds.size.w);
        
        uint8_t xpos = lut_buffer[surfindex*2];
        uint8_t ypos = lut_buffer[(surfindex*2)+1];
        if (xpos > 0 || ypos > 0) {
          uint8_t texturexpos = xpos/2;
          uint8_t textureypos = 127-(ypos/2);
          GColor texturepixel;
          texturepixel.argb = texture_matrix[textureypos][texturexpos];
          newpixel.r = texturepixel.r*DITHERFACTOR;
          newpixel.g = texturepixel.g*DITHERFACTOR;
          newpixel.b = texturepixel.b*DITHERFACTOR;
          if (xpos%2 == 1 || ypos%2 == 1) {
            // interpolate
            GColor texturepixel2;
            if (texturexpos < 255 && xpos%2 == 1) {
              texturepixel2.argb = texture_matrix[textureypos][texturexpos+1];
              newpixel.r = (newpixel.r + texturepixel2.r*DITHERFACTOR)/2;
              newpixel.g = (newpixel.g + texturepixel2.g*DITHERFACTOR)/2;
              newpixel.b = (newpixel.b + texturepixel2.b*DITHERFACTOR)/2;
            }
            if (textureypos > 1 && ypos%2 == 1) {
              texturepixel2.argb = texture_matrix[textureypos-1][texturexpos];
              newpixel.r = (newpixel.r + texturepixel2.r*DITHERFACTOR)/2;
              newpixel.g = (newpixel.g + texturepixel2.g*DITHERFACTOR)/2;
              newpixel.b = (newpixel.b + texturepixel2.b*DITHERFACTOR)/2;
            }
            newpixel.r = (newpixel.r/DITHERFACTOR)*DITHERFACTOR;
            newpixel.g = (newpixel.g/DITHERFACTOR)*DITHERFACTOR;
            newpixel.b = (newpixel.b/DITHERFACTOR)*DITHERFACTOR;
          }
        }
        
        uint8_t specularmap = shad_buffer[surfindex];
        uint8_t shadowmap = specularmap & 0b00001111;
        specularmap = (specularmap & 0b11110000) >> 4;
        specularmap *= 16;
        shadowmap *= 16;
        
        // subtract shadows
        newpixel.r -= (newpixel.r > shadowmap) ? shadowmap : newpixel.r;
        newpixel.g -= (newpixel.g > shadowmap) ? shadowmap : newpixel.g;
        newpixel.b -= (newpixel.b > shadowmap) ? shadowmap : newpixel.b;
        
        // add highlights
        newpixel.r += (255-newpixel.r > specularmap) ? specularmap : 255-newpixel.r;
        newpixel.g += (255-newpixel.g > specularmap) ? specularmap : 255-newpixel.g;
        newpixel.b += (255-newpixel.b > specularmap) ? specularmap : 255-newpixel.b;
      }
      
      // here comes the actual dithering
      uint8_t bayerpixel = bayer8x8[((x%8)+(y*8))%64];
      fbpixel.r = (newpixel.r+bayerpixel)/DITHERFACTOR;
      fbpixel.g = (newpixel.g+bayerpixel)/DITHERFACTOR;
      fbpixel.b = (newpixel.b+bayerpixel)/DITHERFACTOR;
      fbpixel.a = 0b11;
      
      memset(&info.data[x], fbpixel.argb, 1);
    }
  }
  
  // free memory
  free(lut_buffer);
  free(shad_buffer);
  gbitmap_destroy(texture);
  
  // fx (chromatic aberrations)
  for(uint8_t y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for(uint8_t x = info.min_x; x <= info.max_x; x++) {
      GColor fbpixel, fbpixel1, fbpixel2;
      fbpixel.argb = info.data[x];
      fbpixel1.argb = info.data[x];
      fbpixel2.argb = info.data[x];
      if (x+1 <= info.max_x) {
        fbpixel1.argb = info.data[x+1];
        fbpixel.g = fbpixel1.g;
      }
      if (x+2 <= info.max_x) {
        fbpixel2.argb = info.data[x+2];
        fbpixel.b = fbpixel2.b;
      }
      memset(&info.data[x], fbpixel.argb, 1);
    }
  }
  
  // release frame buffer
  graphics_release_frame_buffer(ctx, fb);
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




