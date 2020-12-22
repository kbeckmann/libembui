#pragma once

#include <stdint.h>
#include <stdbool.h>

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

typedef struct eui_rect {
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

typedef struct eui_animation_state {
    float duration;
    int32_t repeat_count;
    bool running;
    float value; // [0..1]
} eui_animation_state_t;

struct eui_animation_node;

typedef void (*eui_animation_apply_cb)(void *transformed_source, const struct eui_animation_node * const animation_state);

struct eui_animation_node {
    eui_node_t node;
    eui_animation_apply_cb apply_to_data;
    float *state;
};
typedef struct eui_animation_node eui_animation_node_t;

typedef struct {
    eui_node_t node;

    eui_node_type_t type;
    bool visible;
    eui_rect_t rect;
    eui_color_t color;
    // type (rectangle, circle ...)

    eui_animation_node_t *color_animator, *rect_animator;
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

    eui_renderer_priv_t priv;
} eui_renderer_t;



typedef struct {
    eui_renderer_t *renderer;
    eui_animation_state_t *animation_states;
    uint32_t state_count;
} eui_context_t;

typedef struct {
    
} eui_event_t;



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
    (_shape).rect_animator = NULL;           \
    (_shape).color_animator = NULL;          \
} while (0)

#define eui_create_animation_state(_duration, _repeat_count, _running) \
    { \
        .duration = _duration, \
        .repeat_count = _repeat_count, \
        .running = _running, \
        .value = (_duration<0?1:0), \
    }

#define eui_create_animation_node(_animation_apply_cb, _target_state) \
    { \
        .apply_to_data = _animation_apply_cb, \
        .node.prev = NULL, \
        .node.next = NULL, \
        .state = &_target_state.value, \
    }

static inline float eui_easein(float x)
{
    // f(x) = x^2
    return x*x;
}

static inline float eui_easeout(float x)
{
    // f(x) = -x^2 + 2x
    return -x * x + 2 * x;
}

static inline float eui_easeinout(float x)
{
    // x<= 0.5: f(x) = 2x^2
    // x > 0.5: f(x) = -2x^2 + 4x - 1
    float mix;
    float x2 = x * x;

    if (x <= 0.5) {
        mix = 2 * x2;
    } else {
        mix = -2*x2 + 4*x - 1;
    }

    return mix;
}

eui_err_t eui_renderer_init(eui_renderer_t *renderer);
eui_err_t eui_renderer_run(eui_renderer_t *renderer);
eui_err_t eui_renderer_set_root(eui_renderer_t *renderer, eui_node_t *root);
eui_err_t eui_set_animation_states(eui_context_t *context, eui_animation_state_t *states, uint32_t state_count);
void eui_node_insert(eui_node_t *node, eui_node_t *next);
eui_err_t eui_init(eui_context_t *ctx);
eui_err_t eui_post_event(eui_context_t *ctx, eui_event_t *event);
eui_err_t eui_run(eui_context_t *ctx);


