#include <math.h>
#include <Arduboy2.h>

//*************************************************
// Enum
//*************************************************
enum COLOR_ENUM {
  TRANSPARENT = -1,DARK1,DARK2,DARK3,LIGHT1,LIGHT2,LIGHT3,INVERT1,INVERT2,COLOR_COUNT,DEFAULT_COLOR = LIGHT1,};

//*************************************************
// Constant and Macro
//*************************************************
#define VIEW_SIZE_X 128
#define VIEW_SIZE_Y 64
#define MAX_HIT_BOX_COUNT 120
#define VALID_HIT_LENGTH 64
//
#define HIT_BOX_INDEX_COLOR_BASE     0
#define HIT_BOX_INDEX_TEXT_BASE      (HIT_BOX_INDEX_COLOR_BASE + COLOR_COUNT)
#define HIT_BOX_INDEX_CHARACTER_BASE (HIT_BOX_INDEX_TEXT_BASE + TEXT_PATTERN_COUNT)
//
#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 6
#define TEXT_PATTERN_COUNT 94
#define MAX_CHARACTER_PATTERN_COUNT 26
//
#define M_PI 3.14159265358979323846
//
#define VEC_XY(v) v.x, v.y
#define getColor(color) (pgm_read_byte(&colorTable[color]))

//*************************************************
// New Type
//*************************************************
typedef struct {uint8_t index;int8_t  x;int8_t  y;uint8_t w;uint8_t h;} HitBox;
typedef struct {uint16_t isColliding[8];} Collision;
typedef struct {float x;float y;} Vector;
typedef struct {uint8_t x:4;uint8_t y:4;uint8_t w:4;uint8_t h:4;} CharacterHitBox;
typedef struct {uint8_t grid[CHARACTER_WIDTH];CharacterHitBox hitBox;} CharacterData;

//*************************************************
// Text Data
//*************************************************
PROGMEM const uint8_t textPatterns[TEXT_PATTERN_COUNT][CHARACTER_WIDTH] = {
    { 0x00, 0x00, 0x00, 0x2E, 0x00, 0x00 },
    { 0x00, 0x00, 0x06, 0x00, 0x06, 0x00 },
    { 0x00, 0x14, 0x3E, 0x14, 0x3E, 0x14 },
    { 0x00, 0x04, 0x2A, 0x3E, 0x2A, 0x10 },
    { 0x00, 0x26, 0x10, 0x08, 0x04, 0x32 },
    { 0x00, 0x14, 0x2A, 0x2C, 0x10, 0x28 },
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00 },
    { 0x00, 0x00, 0x1C, 0x22, 0x00, 0x00 },
    { 0x00, 0x00, 0x22, 0x1C, 0x00, 0x00 },
    { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },
    { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },
    { 0x00, 0x00, 0x20, 0x10, 0x00, 0x00 },
    { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },
    { 0x00, 0x00, 0x00, 0x20, 0x00, 0x00 },
    { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },
    { 0x00, 0x1C, 0x32, 0x2A, 0x26, 0x1C },
    { 0x00, 0x24, 0x22, 0x3E, 0x20, 0x20 },
    { 0x00, 0x24, 0x32, 0x2A, 0x2A, 0x24 },
    { 0x00, 0x14, 0x22, 0x2A, 0x2A, 0x14 },
    { 0x00, 0x18, 0x14, 0x12, 0x3E, 0x10 },
    { 0x00, 0x2E, 0x2A, 0x2A, 0x2A, 0x12 },
    { 0x00, 0x1C, 0x2A, 0x2A, 0x2A, 0x10 },
    { 0x00, 0x06, 0x22, 0x12, 0x0A, 0x06 },
    { 0x00, 0x14, 0x2A, 0x2A, 0x2A, 0x14 },
    { 0x00, 0x04, 0x2A, 0x2A, 0x2A, 0x1C },
    { 0x00, 0x00, 0x00, 0x14, 0x00, 0x00 },
    { 0x00, 0x00, 0x20, 0x14, 0x00, 0x00 },
    { 0x00, 0x08, 0x14, 0x14, 0x22, 0x22 },
    { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },
    { 0x00, 0x22, 0x22, 0x14, 0x14, 0x08 },
    { 0x00, 0x04, 0x02, 0x2A, 0x0A, 0x04 },
    { 0x00, 0x1C, 0x22, 0x2A, 0x2A, 0x0C },
    { 0x00, 0x3C, 0x0A, 0x0A, 0x0A, 0x3C },
    { 0x00, 0x3E, 0x2A, 0x2A, 0x2A, 0x14 },
    { 0x00, 0x1C, 0x22, 0x22, 0x22, 0x14 },
    { 0x00, 0x3E, 0x22, 0x22, 0x22, 0x1C },
    { 0x00, 0x3E, 0x2A, 0x2A, 0x2A, 0x22 },
    { 0x00, 0x3E, 0x0A, 0x0A, 0x0A, 0x02 },
    { 0x00, 0x1C, 0x22, 0x22, 0x2A, 0x18 },
    { 0x00, 0x3E, 0x08, 0x08, 0x08, 0x3E },
    { 0x00, 0x22, 0x22, 0x3E, 0x22, 0x22 },
    { 0x00, 0x10, 0x20, 0x22, 0x1E, 0x02 },
    { 0x00, 0x3E, 0x08, 0x08, 0x14, 0x22 },
    { 0x00, 0x3E, 0x20, 0x20, 0x20, 0x20 },
    { 0x00, 0x3E, 0x04, 0x08, 0x04, 0x3E },
    { 0x00, 0x3E, 0x04, 0x08, 0x10, 0x3E },
    { 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C },
    { 0x00, 0x3E, 0x0A, 0x0A, 0x0A, 0x04 },
    { 0x00, 0x1C, 0x22, 0x22, 0x32, 0x3C },
    { 0x00, 0x3E, 0x0A, 0x0A, 0x0A, 0x34 },
    { 0x00, 0x24, 0x2A, 0x2A, 0x2A, 0x12 },
    { 0x00, 0x02, 0x02, 0x3E, 0x02, 0x02 },
    { 0x00, 0x1E, 0x20, 0x20, 0x20, 0x1E },
    { 0x00, 0x0E, 0x10, 0x20, 0x10, 0x0E },
    { 0x00, 0x1E, 0x20, 0x1C, 0x20, 0x1E },
    { 0x00, 0x22, 0x14, 0x08, 0x14, 0x22 },
    { 0x00, 0x02, 0x04, 0x38, 0x04, 0x02 },
    { 0x00, 0x22, 0x32, 0x2A, 0x26, 0x22 },
    { 0x00, 0x00, 0x00, 0x3E, 0x22, 0x00 },
    { 0x00, 0x02, 0x04, 0x08, 0x10, 0x20 },
    { 0x00, 0x00, 0x00, 0x22, 0x3E, 0x00 },
    { 0x00, 0x00, 0x04, 0x02, 0x04, 0x00 },
    { 0x00, 0x20, 0x20, 0x20, 0x20, 0x20 },
    { 0x00, 0x00, 0x00, 0x04, 0x08, 0x00 },
    { 0x00, 0x18, 0x24, 0x24, 0x3C, 0x00 },
    { 0x00, 0x3E, 0x28, 0x28, 0x10, 0x00 },
    { 0x00, 0x18, 0x24, 0x24, 0x24, 0x00 },
    { 0x00, 0x10, 0x28, 0x28, 0x3E, 0x00 },
    { 0x00, 0x18, 0x34, 0x2C, 0x08, 0x00 },
    { 0x00, 0x00, 0x08, 0x3C, 0x0A, 0x00 },
    { 0x00, 0x04, 0x2A, 0x2A, 0x1C, 0x00 },
    { 0x00, 0x00, 0x3E, 0x08, 0x30, 0x00 },
    { 0x00, 0x00, 0x00, 0x34, 0x00, 0x00 },
    { 0x00, 0x00, 0x20, 0x1A, 0x00, 0x00 },
    { 0x00, 0x00, 0x3E, 0x10, 0x28, 0x00 },
    { 0x00, 0x00, 0x22, 0x3E, 0x20, 0x00 },
    { 0x00, 0x3C, 0x04, 0x1C, 0x04, 0x38 },
    { 0x00, 0x3C, 0x04, 0x04, 0x38, 0x00 },
    { 0x00, 0x18, 0x24, 0x24, 0x18, 0x00 },
    { 0x00, 0x3C, 0x14, 0x14, 0x08, 0x00 },
    { 0x00, 0x08, 0x14, 0x14, 0x3C, 0x00 },
    { 0x00, 0x3C, 0x08, 0x04, 0x04, 0x00 },
    { 0x00, 0x28, 0x2C, 0x34, 0x14, 0x00 },
    { 0x00, 0x00, 0x08, 0x1C, 0x28, 0x00 },
    { 0x00, 0x1C, 0x20, 0x20, 0x3C, 0x00 },
    { 0x00, 0x0C, 0x30, 0x30, 0x0C, 0x00 },
    { 0x00, 0x1C, 0x20, 0x18, 0x20, 0x1C },
    { 0x00, 0x24, 0x18, 0x18, 0x24, 0x00 },
    { 0x00, 0x24, 0x18, 0x08, 0x04, 0x00 },
    { 0x00, 0x24, 0x34, 0x2C, 0x24, 0x00 },
    { 0x00, 0x00, 0x08, 0x36, 0x22, 0x00 },
    { 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00 },
    { 0x00, 0x00, 0x22, 0x36, 0x08, 0x00 },
    { 0x00, 0x08, 0x04, 0x08, 0x10, 0x08 },
};
PROGMEM static const CharacterData characters_set[] = {
  {{ 0x0F, 0x39, 0x0F, 0x09, 0x3F, 0x0F }, { 0, 0, 6, 6 }},
  {{ 0x3E, 0x32, 0x1E, 0x12, 0x3E, 0x3E }, { 0, 1, 6, 5 }},
  {{ 0x00, 0x0E, 0x3B, 0x3D, 0x1F, 0x0E }, { 1, 0, 5, 6 }},
};



//*************************************************
// Global variable
//*************************************************
Arduboy2 arduboy;
//static Arduboy2Base arduboy;

static HitBox hitBoxes[MAX_HIT_BOX_COUNT];
static uint8_t hitBoxesIndex;

PROGMEM static const uint8_t colorTable[] = {
  BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, INVERT, INVERT
};

int8_t color;
float thickness;
float barCenterPosRatio;
bool hasCollision;

static uint8_t drawingHitBoxesIndex;

static const CharacterData *characters;

  
//*************************************************
// Vector
//*************************************************
Vector* vectorSet(Vector* vec, float x, float y) {vec->x = x;vec->y = y;return vec;}
Vector* vectorAdd(Vector* vec, float x, float y) {vec->x += x;vec->y += y;return vec;}
Vector* vectorMul(Vector* vec, float v) {vec->x *= v;vec->y *= v;return vec;}
Vector* rotate(Vector* vec, float angle) {float tx = vec->x;vec->x = tx * cosf(angle) - vec->y * sinf(angle);vec->y = tx * sinf(angle) + vec->y * cosf(angle);return vec;}
Vector* addWithAngle(Vector* vec, float angle, float length) {vec->x += cosf(angle) * length; vec->y += sinf(angle) * length; return vec;}
float angleTo(Vector* vec, float x, float y) {return atan2f(y - vec->y, x - vec->x);}
float distanceTo(Vector* vec, float x, float y) {float ox = x - vec->x;float oy = y - vec->y;return sqrtf(ox * ox + oy * oy);}
float vectorAngle(Vector* vec) { return atan2f(vec->y, vec->x); }
float vectorLength(Vector* vec) {return sqrtf(vec->x * vec->x + vec->y * vec->y);}


//*************************************************
// Collision and HitBox
//*************************************************
static void initCollision(Collision *collision) {memset(collision, 0, sizeof(Collision));}
static bool testCollision(HitBox r1, HitBox r2) {int16_t ox = r2.x - r1.x; int16_t oy = r2.y - r1.y; return -r2.w < ox && ox < r1.w && -r2.h < oy && oy < r1.h;}
static void checkHitBox(Collision *cl, HitBox hitBox) {
  //arduboy.print(hitBoxesIndex);
  for (uint8_t i = 0; i < hitBoxesIndex; i++) {
    HitBox hb = hitBoxes[i];
    if (testCollision(hb, hitBox)) {
      bitSet(cl->isColliding[(hb.index >> 4)], hb.index & 0xF);
      //arduboy.print(":");arduboy.print(hb.index);arduboy.print(":");arduboy.print(hb.index>>4);arduboy.print(":");arduboy.print(hb.index & 0xF);
    }
  }
  //arduboy.print("\n");
}

static bool isValidHitCoord(float x, float y) {return x >= -VALID_HIT_LENGTH && x < VIEW_SIZE_X + VALID_HIT_LENGTH && y >= -VALID_HIT_LENGTH && y < VIEW_SIZE_Y + VALID_HIT_LENGTH;}
static bool isCollidingSomething(Collision *cl, uint8_t index) {return cl->isColliding[(index >> 4)] & bit(index & 0xF);}
bool colRect(Collision *cl, uint8_t color) {
  if (color >= DARK1 && color < COLOR_COUNT) {
    uint8_t index = HIT_BOX_INDEX_COLOR_BASE + color;
    return isCollidingSomething(cl, index);
  }
  return false;
}

//*************************************************
// Drawing
//*************************************************
void md_drawRect(float _x, float _y, float _w, float _h, int8_t color) {
  /*  Check parameters  */
  int16_t x = (int16_t)_x, y = (int16_t)_y, w = (int16_t)_w, h = (int16_t)_h;
  if (x < 0) {
    if (w <= -x) return;
    w += x;
    x = 0;
  }
  if (y < 0) {
    if (h <= -y) return;
    h += y;
    y = 0;
  }
  if (w <= 0 || x >= WIDTH || h <= 0 || y >= HEIGHT) return;
  if (x + w > WIDTH) w = WIDTH - x;
  if (y + h > HEIGHT) h = HEIGHT - y;

  /*  Draw a filled rectangle  */
  uint8_t *p = arduboy.getBuffer() + x + (y / 8) * WIDTH;
  uint8_t yOdd = y & 7, d = 0xFF << yOdd, c = getColor(color);
  for (h += yOdd; h > 0; h -= 8, p += WIDTH - w) {
    if (h < 8) d &= 0xFF >> (8 - h);
    if (c == WHITE) {
      for (uint8_t i = w; i > 0; i--, *p++ |= d) { ; }
    } else if (c == BLACK) {
      for (uint8_t i = w, invD = ~d; i > 0; i--, *p++ &= invD) { ; }
    } else {
      for (uint8_t i = w; i > 0; i--, *p++ ^= d) { ; }
    }
    d = 0xFF;
  }
}
static void beginAddingRects(void) { drawingHitBoxesIndex = hitBoxesIndex; }

static int tst=0;

static void addRect(bool isAlignCenter, float x, float y, float w, float h,
                    Collision *hitCollision) {
  if (isAlignCenter) {
    x -= w / 2;
    y -= h / 2;
  }
  if (hasCollision && isValidHitCoord(x, y)) {
    HitBox hb;
    hb.index = HIT_BOX_INDEX_COLOR_BASE + color;
    //if(!tst){arduboy.print(hb.index); tst=1;}
    hb.x = x;
    hb.y = y;
    hb.w = w;
    hb.h = h;
    checkHitBox(hitCollision, hb);
    if (color > TRANSPARENT && drawingHitBoxesIndex < MAX_HIT_BOX_COUNT) {
      //arduboy.print(drawingHitBoxesIndex);arduboy.print(":");arduboy.print(hb.index);arduboy.print(":");arduboy.print(hb.index>>4);arduboy.print(":");arduboy.print(hb.index & 0xF);arduboy.print("\n");
      hitBoxes[drawingHitBoxesIndex] = hb;
      drawingHitBoxesIndex++;
    }
  }
  if (color > TRANSPARENT && color < COLOR_COUNT) {
    md_drawRect(x, y, w, h, color);
  }
}

static void addHitBox(HitBox hb) {
  if (hitBoxesIndex < MAX_HIT_BOX_COUNT) {
    hitBoxes[hitBoxesIndex] = hb;
    hitBoxesIndex++;
  } else {
    // Too many hit boxes!
  }
}

static void endAddingRects(void) {
  hitBoxesIndex = drawingHitBoxesIndex;
}


void md_drawPixel(float _x, float _y, int8_t color) {
  /*  Check parameters  */
  int16_t x = (int16_t)_x, y = (int16_t)_y;
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

  /*  Draw a pixel  */
  uint8_t *p = arduboy.getBuffer() + x + (y / 8) * WIDTH;
  uint8_t d = bit(y & 7), c = getColor(color);
  if (c != INVERT) *p |= d;
  if (c != WHITE) *p ^= d;
  /*
  unsigned char* sBuffer = arduboy.getBuffer();
  uint8_t row = (uint8_t)y / 8;
  sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  */
}

float clamp(float v, float low, float high) {return fmaxf(low, fminf(v, high));}

static void drawLine(float x, float y, float ox, float oy,
                     Collision *hitCollision) {

  //drawLine(127,0, -127, 63, &hitCollision);
  //arduboy.drawLine(x,y,x+ox,y+oy);
  //return;
                      
  float lx = fabsf(ox);
  float ly = fabsf(oy);
  float t = thickness * 1.5f;
  t = clamp(t, 3, 10);
  uint8_t rn = ceilf(lx > ly ? lx / t : ly / t);
  rn = clamp(rn, 3, 49);
  ox /= (rn - 1);
  oy /= (rn - 1);
  for (uint8_t i = 0; i < rn; i++) {
    addRect(true, x, y, thickness, thickness, hitCollision);
    x += ox;
    y += oy;
  }
}

Collision rect(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(false, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision box(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(true, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision line(float x1, float y1, float x2, float y2) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  drawLine(x1, y1, x2 - x1, y2 - y1, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision bar(float x, float y, float length, float angle) {
  Collision hitCollision;
  initCollision(&hitCollision);
  Vector l;
  rotate(vectorSet(&l, length, 0), angle);
  Vector p;
  vectorSet(&p, x - l.x * barCenterPosRatio, y - l.y * barCenterPosRatio);
  beginAddingRects();
  drawLine(p.x, p.y, l.x, l.y, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision arc(float centerX, float centerY, float radius, float angleFrom,
              float angleTo) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  float af, ao;
  if (angleFrom > angleTo) {
    af = angleTo;
    ao = angleFrom - angleTo;
  } else {
    af = angleFrom;
    ao = angleTo - angleFrom;
  }
  if (ao < 0.01f) {
    return hitCollision;
  }
  if (ao < 0) {
    ao = 0;
  } else if (ao > M_PI * 2) {
    ao = M_PI * 2;
  }
  uint8_t lc = ceilf(ao * sqrtf(radius * 0.125f));
  lc = clamp(lc, 1, 18);
  float ai = ao / lc;
  float a = af;
  float p1x = radius * cosf(a) + centerX;
  float p1y = radius * sinf(a) + centerY;
  float p2x, p2y;
  float ox, oy;
  for (uint8_t i = 0; i < lc; i++) {
    a += ai;
    p2x = radius * cosf(a) + centerX;
    p2y = radius * sinf(a) + centerY;
    ox = p2x - p1x;
    oy = p2y - p1y;
    drawLine(p1x, p1y, ox, oy, &hitCollision);
    p1x = p2x;
    p1y = p2y;
  }
  endAddingRects();
  return hitCollision;
}

void md_drawCharacter(const uint8_t grid[CHARACTER_WIDTH], float x, float y, int8_t color) {
  arduboy.drawBitmap((int16_t)x, (int16_t)y, grid, CHARACTER_WIDTH, CHARACTER_HEIGHT, getColor(color));
}

static void drawCharacter(uint8_t index, float x, float y, bool _hasCollision,
                          bool isText, Collision *hitCollision) {
  if ((isText && (index < '!' || index > '~')) ||
      (!isText && (index < 'a' || index > 'z'))) {
    return;
  }
  if (color > TRANSPARENT && color < COLOR_COUNT &&
      x > -CHARACTER_WIDTH && x < VIEW_SIZE_X &&
      y > -CHARACTER_HEIGHT && y < VIEW_SIZE_Y) {
    const uint8_t *grid =
        isText ? textPatterns[index - '!'] : characters[index - 'a'].grid;
    md_drawCharacter(grid, x, y, color);
  }
  if (hasCollision && _hasCollision && isValidHitCoord(x, y)) {
    HitBox hb;
    if (isText) {
      hb.index = HIT_BOX_INDEX_TEXT_BASE + (index - '!');
      hb.x = x + 1;
      hb.y = y + 1;
      hb.w = CHARACTER_WIDTH - 1;
      hb.h = CHARACTER_HEIGHT - 1;
    } else {
      hb.index = HIT_BOX_INDEX_CHARACTER_BASE + (index - 'a');
      CharacterHitBox chb;
      *((uint16_t *)&chb) = pgm_read_word(&characters[index - 'a'].hitBox); 
      hb.x = x + chb.x;
      hb.y = y + chb.y;
      hb.w = chb.w;
      hb.h = chb.h;
    }
    checkHitBox(hitCollision, hb);
    if (color > TRANSPARENT) {
      addHitBox(hb);
    }
  }
}

Collision character(char character, float x, float y) {
  Collision hitCollision;
  initCollision(&hitCollision);
  x -= CHARACTER_WIDTH / 2;
  y -= CHARACTER_HEIGHT / 2;
  drawCharacter(character, x, y, true, false, &hitCollision);
  return hitCollision;
}


float acc=0;
float x=-99,y=-99;
int frame=0;

void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.clear();
  //
  hitBoxesIndex = 0;
  characters = characters_set;
  //
  hasCollision = true;
  color = LIGHT1;
  thickness = 1.0f;
  barCenterPosRatio = 0.5;
  
  arduboy.display();
}

void UserInput(){
  if(arduboy.pressed(LEFT_BUTTON)) x -= 1;
  if(arduboy.pressed(RIGHT_BUTTON)) x += 1;
  if(arduboy.pressed(UP_BUTTON)) y -= 1;
  if(arduboy.pressed(DOWN_BUTTON)) y += 1;  
}

void example_collision(){
  // Init
  if(x==-99 && y==-99){x=60; y=30; hasCollision = true;}
  //  
  UserInput();
  color = LIGHT1; 
  rect(40,10,20,20);
  Collision r = rect(x,y,20,20);
  if(colRect(&r, LIGHT1)) {arduboy.print("Y");}else{arduboy.print("N");}
}

void example_rotate_bar(){
  float deg = (PI/180); // PI*2/360 = PI/180 = one deg
  // Init
  if(x==-99 && y==-99){x=64; y=32; acc=0;}
  //
  acc += deg;           // add 1 degree
  if(acc>=PI*2)acc=0;   // reset when 360 deg
  //
  barCenterPosRatio = 0.5; thickness = 2;
  bar(64,32, 30, acc);
  //
  barCenterPosRatio = 0; thickness = 1;
  bar(32,32, 30, acc+(deg*90));
  //
  barCenterPosRatio = 0.8; thickness = 1;
  bar(96,32, 30, acc+(deg*90));
}

void example_rotate_arc(){
  float deg1 = (PI/180), deg360=PI*2;
  // Init
  if(x==-99 && y==-99){x=64; y=32; acc=0; }
  //
  acc += deg1;
  thickness=2; arc(64,32, 30, acc, acc+PI*2);   // clockwise
  thickness=1; arc(64,32, 20, -acc, -acc+PI*2); // counter clockwise
}

void example_lines(){
  // Init
  if(x==-99 && y==-99){x=64; y=32; hasCollision=true;}
  // static objects
  color = LIGHT1;
  line(0, 0,128, 0);
  line(0,63,128,63);
  bar(64,32, 20, 0);    // 0 deg
  bar(64,32, 20, PI/2); // 90 deg
  // actor
  UserInput();
  Collision r = box(x,y, 10,10);
  if(colRect(&r, LIGHT1)) {arduboy.print("Y");}else{arduboy.print("N");}
}

void example_draw_chars(){
  // Init
  if(x==-99 && y==-99){x=64; y=32; acc=0; frame=0;}
  // floor
  color = LIGHT1; rect(0,59, 128,4);
  // actor
  UserInput();
  if(acc++>10){acc=0; frame=(frame+1)%2;}
  Collision r = character((frame==0?'a':'b'), x,y);
  if(colRect(&r, LIGHT1)) {arduboy.print("Y");}else{arduboy.print("N");}
}

void loop()
{
  if (!arduboy.nextFrame()) return; 
  arduboy.clear();
  hitBoxesIndex = 0;

  // Check collision object
  //example_collision();

  // Draw rotating bar
  example_rotate_bar();

  // Draw rotating circle(arc)
  //example_rotate_arc();

  // Check collision with line
  //example_lines();

  // Draw character
  //example_draw_chars();

  arduboy.display();
}
