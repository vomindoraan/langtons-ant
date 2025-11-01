/**
 * @file sprites.h
 * Sprite data and function declarations
 * @author vomindoraan
 */
#ifndef __SPRITES_H__
#define __SPRITES_H__

#include "logic.h"


/*---------------------------- Sprite dimensions -----------------------------*/

#define SPRITE_ANT_SIZE_SMALL   3
#define SPRITE_ANT_SIZE_MEDIUM  8
#define SPRITE_ANT_SIZE_LARGE   24
#define SPRITE_LOGO_WIDTH       38
#define SPRITE_LOGO_HEIGHT      10
#define SPRITE_UDARROW_WIDTH    3
#define SPRITE_UDARROW_HEIGHT   2
#define SPRITE_RLARROW_WIDTH    SPRITE_UDARROW_HEIGHT
#define SPRITE_RLARROW_HEIGHT   SPRITE_UDARROW_WIDTH
#define SPRITE_STEPUP_SIZE      3
#define SPRITE_BUTTON_WIDTH     5
#define SPRITE_BUTTON_HEIGHT    5
#define SPRITE_DIGIT_WIDTH      3
#define SPRITE_DIGIT_HEIGHT     5
#define SPRITE_INFINITY_WIDTH   11
#define SPRITE_INFINITY_HEIGHT  SPRITE_DIGIT_HEIGHT


/*------------------------- Sprite macros and types --------------------------*/

#define SPRITE_SZ(w, h)  CDIV((w) * (h), 8)
#define SPRITE_SZ_(s)    SPRITE_SZ(s, s)

#define SI(d, w, h)      (SpriteInfo) { d, w, h }
#define SI_NONE          SI(NULL, 0, 0)

#define SPRITE_LOGO_SZ   SPRITE_SZ(SPRITE_LOGO_WIDTH, SPRITE_LOGO_HEIGHT)
#define SI_LOGO(s)       SI(s,     SPRITE_LOGO_WIDTH, SPRITE_LOGO_HEIGHT)

/** Sprite bytearray data types */
///@{
typedef const byte       sprite_t;
typedef sprite_t *const  sprite_ptr_t;
///@}

/** Immutable container for sprite pointer and size */
typedef struct sprite_info {
	sprite_ptr_t    data;           /**< Sprite data */  /**@{*/
	const unsigned  width, height;  /**< Sprite size */  /**@}*/
} SpriteInfo;

typedef struct logo_sprite {
	sprite_t  base[SPRITE_LOGO_SZ];
	sprite_t  hl[SPRITE_LOGO_SZ];
} LogoSprites;

typedef enum {
	UI_ARROW,
	UI_STEPUP,
	UI_BUTTON,
	UI_DIGIT,
	UI_INFINITY,
	_UI_COUNT
} UISpriteType;

typedef enum {
	UIB_PLAY,
	UIB_PAUSE,
	UIB_STOP,
	UIB_CLEAR,
	UIB_PENDING,
	_UIB_COUNT
} UIButtonType;


/*------------------------- Global sprite constants --------------------------*/

extern const LogoSprites  logo1_sprites, logo2_sprites;


/*----------------------------------------------------------------------------*
 *                                 sprites.c                                  *
 *----------------------------------------------------------------------------*/

/**
 * Finds a suitable sprite for the given cell size and ant direction
 * @param size Cell size
 * @param dir Current ant direction
 * @return Ant sprite with requested size and direction, if one exists; NULL otherwise
 */
SpriteInfo ant_sprite(unsigned size, Direction dir);

// TODO: Write docs
SpriteInfo logo_sprite(unsigned index, bool highlight);
SpriteInfo ui_sprite(UISpriteType type, int arg);

#endif  // __SPRITES_H__
