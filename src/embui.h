#pragma once

#include <stdint.h>

typedef enum {
    EUI_PIXEL_FORMAT_MONO_1 = 1,    // 1 bit per pixel
    EUI_PIXEL_FORMAT_RGB_565,       // 16 bits per pixel
    EUI_PIXEL_FORMAT_RGBA_8888,     // 32 bits per pixel
} eui_pixel_format_t;

typedef enum {
    EUI_NODE_TYPE_SHAPE = 1,
    EUI_NODE_TYPE_IMAGE,
} eui_node_type_t;

typedef struct {
    int32_t x;
    int32_t y;
} eui_pos_t;

typedef struct {
    int32_t width;
    int32_t height;
} eui_size_t;

typedef struct {
    eui_pos_t pos;
    eui_size_t size;
} eui_rect_t;




typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
    uint32_t value;
} eui_color_t;

typedef struct eui_node {
    struct eui_node *prev;
    struct eui_node *next;
} eui_node_t;

typedef struct {
    eui_node_t node;

    eui_node_type_t type;
    eui_rect_t rect;
    eui_color_t color;
    // type (rectangle, circle ...)
} eui_shape_t;

typedef struct {
    void *data;
    eui_pixel_format_t format;
    eui_size_t size;
    eui_rect_t source_rect;
} eui_asset_t;

typedef struct {
    eui_shape_t shape;
    eui_asset_t asset;
} eui_image_t;




typedef struct {
    eui_node_t *root;
} eui_renderer_priv_t;

typedef struct {
    void *framebuffer;
    eui_pixel_format_t format;
    eui_size_t size;
    eui_rect_t active_area;

    eui_renderer_priv_t priv;
} eui_renderer_t;



// typedef struct {
//     eui_renderer_t *renderer;
// } eui_context_t;

// typedef struct {
    
// } eui_event_t;


typedef enum {
    EUI_ERR_OK = 0,
    EUI_ERR_ERROR,
} eui_err_t;

#define RGB888_TO_RGB565(_r, _g, _b) \
    (                                \
        (((_r) & 0xf8) <<  8) |      \
        (((_g) & 0xfc) <<  3) |      \
        (((_b))        >>  3)        \
    )


#define EUI_INIT_SHAPE(_shape, _x, _y, _width, _height, _color) do { \
    (_shape).node.prev = NULL;               \
    (_shape).node.next = NULL;               \
    (_shape).type = EUI_NODE_TYPE_SHAPE;     \
    (_shape).type = EUI_NODE_TYPE_SHAPE;     \
    (_shape).rect.pos.x = (_x);              \
    (_shape).rect.pos.y = (_y);              \
    (_shape).rect.size.width = (_width);     \
    (_shape).rect.size.height = (_height);   \
    (_shape).color = (_color);               \
} while (0)


eui_err_t eui_renderer_init(eui_renderer_t *renderer);
eui_err_t eui_renderer_run(eui_renderer_t *renderer);
eui_err_t eui_renderer_set_root(eui_renderer_t *renderer, eui_node_t *root);

eui_err_t eui_node_insert(eui_node_t *node, eui_node_t *next);



// void eui_init(eui_context_t *ctx);
// void eui_post_event(eui_context_t *ctx, eui_event_t *event);
// void eui_run(eui_context_t *ctx);


