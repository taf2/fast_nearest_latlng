#include <ruby.h>
#include <stdlib.h>
#include <math.h>

VALUE cFastLoc;
VALUE cFastLoc;

#define pi 3.14159265358979323846

typedef struct {
  int id;
  double lat, lng;
} Location;

typedef struct {
  int id;
  double dist;
} ldat;

typedef struct {
  Location **locations;
  size_t count;
  char unit;
} ruby_fast_loc;

static double deg2rad(double deg) {
  return (deg * pi / 180);
}

static double rad2deg(double rad) {
  return (rad * 180 / pi);
}

// see: http://www.zipcodeworld.com/samples/distance.c.html
static double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.1515;
  switch(unit) {
  case 'M':
    break;
  case 'K':
    dist = dist * 1.609344;
    break;
  case 'N':
    dist = dist * 0.8684;
    break;
  }
  return (dist);
}

static VALUE ruby_fast_loc_add(VALUE self, VALUE latdeg, VALUE lngdeg, VALUE id) {
  int i = 0, count = 0;
  VALUE add;
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);

  floc->count++;

  Location **locations = (Location**)malloc(sizeof(Location*)*floc->count);

  Location *location = (Location*)malloc(sizeof(Location));

  location->lat = NUM2DBL(latdeg);
  location->lng = NUM2DBL(lngdeg);
  location->id  = FIX2INT(id);
  locations[floc->count-1] = location;

  if (floc->locations) {
    count = floc->count-1;
    for (i = 0; i < count; ++i) {
      locations[i] = floc->locations[i];
    }
    free(floc->locations);
  }
  floc->locations = locations;

  return self;
}
static int distance_compare(const void *a, const void *b) {
  ldat* v1 = ((ldat*)a);
  ldat* v2 = ((ldat*)b);
  if (v1->dist < v2->dist) { return -1; }
  else if (v1->dist == v2->dist) { return 0; }
  return 1;
}

static VALUE ruby_fast_loc_nearest(VALUE self, VALUE latdeg, VALUE lngdeg) {
  VALUE sorted = rb_ary_new();
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);
  int i = 0;
  double lat = NUM2DBL(latdeg);
  double lng = NUM2DBL(lngdeg);
  ldat distances[floc->count];

  for (i = 0; i < floc->count; ++i) {
    distances[i].id = floc->locations[i]->id;
    distances[i].dist = distance(lat, lng, floc->locations[i]->lat, floc->locations[i]->lng, floc->unit);
  }

  // void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
  qsort(distances, floc->count, sizeof(ldat), distance_compare);

  for (i = 0; i < floc->count; ++i) {
    VALUE result = rb_ary_new2(2);
    rb_ary_push(result, rb_float_new(distances[i].dist));
    rb_ary_push(result, rb_int_new(distances[i].id));
    rb_ary_push(sorted, result);
  }
  return sorted;
}

static VALUE ruby_fast_loc_locations(VALUE self) {
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);

  if (!floc->count) {
    return rb_ary_new();
  }

  VALUE add = Qnil;
  VALUE locations = rb_ary_new();
  int i;
  Location *location;

  for (i = 0; i < floc->count && floc->locations; ++i) {
    location = floc->locations[i];

    add = rb_ary_new2(3);
    rb_ary_push(add, rb_float_new(location->lat));
    rb_ary_push(add, rb_float_new(location->lng));
    rb_ary_push(add, rb_int_new(location->id));
    rb_ary_push(locations, add);
  }

  return locations;
}
static VALUE ruby_fast_loc_get_unit(VALUE self) {
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);

  return rb_str_new(&floc->unit,1);
}

static VALUE ruby_fast_loc_set_unit(VALUE self, VALUE unit_sym) {
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);

  if (unit_sym == ID2SYM(rb_intern("miles"))) {
    floc->unit = 'M';
  } else if (unit_sym == ID2SYM(rb_intern("kilometers"))) {
    floc->unit = 'K';
  } else if (unit_sym == ID2SYM(rb_intern("nautical_miles"))) {
    floc->unit = 'N';
  }

  return rb_str_new(&floc->unit,1);
}

void fast_loc_mark(ruby_fast_loc *floc) {
}

static void fast_loc_clear(ruby_fast_loc *floc) {
  while(floc->count) {
    free(floc->locations[--floc->count]);
  }
  free(floc->locations);
  floc->locations = NULL;
}
static void fast_loc_free(ruby_fast_loc *floc) {
  fast_loc_clear(floc);
  free(floc);
}


static VALUE ruby_fast_loc_new(int argc, VALUE *argv, VALUE klass) {
  VALUE new_floc, options, unit_sym;
  ruby_fast_loc *floc;

  rb_scan_args(argc, argv, "01", &options);

  floc = ALLOC(ruby_fast_loc);
  floc->unit = 'M';
  floc->locations = NULL;
  floc->count = 0;

  new_floc = Data_Wrap_Struct(klass, fast_loc_mark, fast_loc_free, floc);

  if (!NIL_P(options) && TYPE(options) == T_HASH) {
    unit_sym = rb_hash_aref(options, ID2SYM(rb_intern("unit")));
    ruby_fast_loc_set_unit(new_floc, unit_sym);
  }

  return new_floc;
}
static VALUE ruby_fast_loc_reset(VALUE self) {
  ruby_fast_loc *floc;

  Data_Get_Struct(self, ruby_fast_loc, floc);

  fast_loc_clear(floc);

  return self;
}

void Init_fastloc() {
  cFastLoc = rb_define_class("FastLoc", rb_cObject);
  rb_define_singleton_method(cFastLoc, "new", ruby_fast_loc_new, -1);
  rb_define_method(cFastLoc, "add", ruby_fast_loc_add, 3);
  rb_define_method(cFastLoc, "unit", ruby_fast_loc_get_unit, 0);
  rb_define_method(cFastLoc, "unit=", ruby_fast_loc_set_unit, 1);
  rb_define_method(cFastLoc, "locations", ruby_fast_loc_locations, 0);
  rb_define_method(cFastLoc, "nearest", ruby_fast_loc_nearest, 2);
  rb_define_method(cFastLoc, "reset", ruby_fast_loc_reset, 0);
}
