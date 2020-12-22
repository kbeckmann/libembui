#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "embui.h"


void render_shape_rgb565(eui_renderer_t *renderer, eui_color_t color, eui_rect_t rect)
{
    uint16_t draw_color;
    eui_size_t fb_size = renderer->size;
    uint16_t *fb = (uint16_t *) renderer->framebuffer;
    uint32_t stride = fb_size.width;

    if (color.rgba.a == 0xff) {
        // Opaque
        draw_color = ((color.rgba.r & 0xf8) <<  8) |
                     ((color.rgba.g & 0xfc) <<  3) |
                     ((color.rgba.b)        >>  3);

        int32_t start_x = rect.pos.x < 0 ? 0 : rect.pos.x;
        int32_t end_x   = rect.pos.x + rect.size.width;
        end_x = end_x >= fb_size.width ? (fb_size.width - 1) : end_x;

        int32_t start_y = rect.pos.y < 0 ? 0 : rect.pos.y;
        int32_t end_y   = rect.pos.y + rect.size.height;
        end_y = end_y >= fb_size.height ? (fb_size.height - 1) : end_y;

        for (int32_t y = start_y; y < end_y; y++) {
            uint16_t *row = &fb[y * stride];
            for (int32_t x = start_x; x < end_x; x++) {
                row[x] = draw_color;
            }
        }
    }
}

eui_err_t eui_renderer_init(eui_renderer_t *renderer)
{
    memset(&renderer->priv, 0, sizeof(eui_renderer_priv_t));

    return EUI_ERR_OK;
}

#define eui_animate(input_animator, param) do { \
    eui_animation_node_t *animator = input_animator; \
    while(animator != NULL) { \
        animator->apply_to_data(&param, animator); \
        animator = (eui_animation_node_t *) animator->node.next;  \
    } \
} while(0)

eui_err_t eui_render_shape(eui_renderer_t *renderer, eui_shape_t *shape)
{
    eui_pixel_format_t dest_fmt = renderer->format;

    eui_rect_t rect = shape->rect;
    eui_animate(shape->rect_animator, rect);

    eui_color_t color = shape->color;
    eui_animate(shape->color_animator, color);

    switch (dest_fmt) {
    case EUI_PIXEL_FORMAT_RGB_565:
        render_shape_rgb565(renderer, color, rect);
        break;
    default:
        assert(!"Unhandled pixel format.");
    }

    return EUI_ERR_OK;
}

eui_err_t eui_renderer_run(eui_renderer_t *renderer)
{
    eui_node_t *node = renderer->priv.root;
    eui_shape_t *shape;

    while (node != NULL) {
        shape = (eui_shape_t *) node;
        assert(shape->type == EUI_NODE_TYPE_SHAPE);

        switch (shape->type) {
        case EUI_NODE_TYPE_SHAPE:
            eui_render_shape(renderer, shape);
            break;
        case EUI_NODE_TYPE_IMAGE:
            break;
        }

        node = node->next;
    }

    return EUI_ERR_OK;
}

void eui_node_insert(eui_node_t *node, eui_node_t *next)
{
    eui_node_t *old_next = node->next;
    node->next = next;
    next->next = old_next;
}

void eui_node_add_last(eui_node_t *node, eui_node_t *next)
{
    eui_node_t *old_next = node->next;

    while (old_next != NULL) {
        node = old_next;
        old_next = node->next;
    }

    node->next = next;
    next->next = old_next;
}

eui_err_t eui_renderer_set_root(eui_renderer_t *renderer, eui_node_t *root)
{
    renderer->priv.root = root;

    return EUI_ERR_OK;
}

void eui_update_animation_state_linear(eui_animation_state_t *state, float time_delta)
{
    if (state->running) {
        state->anim_value += time_delta / state->duration;
        if (state->repeat_count != 0 && state->anim_value > 1 ) {
            state->anim_value -= 1;
            state->repeat_count--;
        }
        if (state->repeat_count != 0 && state->anim_value < 0 ) {
            state->anim_value += 1;
            state->repeat_count--;
        }
        if (state->repeat_count == 0 && state->anim_value > 1) {
            state->anim_value = 1;
            state->running = false;
        }
        if (state->repeat_count == 0 && state->anim_value < 0) {
            state->anim_value = 0;
            state->running = false;
        }
    }

    state->value = state->ease_cb(state->anim_value) + state->offset - (state->duration > 0 ? 1 : 0);
}

eui_err_t eui_update_animation_states(eui_context_t *ctx)
{
    eui_animation_state_t *state = ctx->animation_states;

    for (int i=0; i < ctx->state_count; i++) {
        eui_update_animation_state_linear(&state[i], 1000/60.f);
    }

    return EUI_ERR_OK;
}
eui_err_t eui_set_animation_states(eui_context_t *context, eui_animation_state_t *states, uint32_t state_count){
    context->animation_states = states;
    context->state_count = state_count;

    return EUI_ERR_OK;
}

eui_err_t eui_init(eui_context_t *ctx)
{
    return EUI_ERR_OK;
}

eui_err_t eui_post_event(eui_context_t *ctx, eui_event_t *event)
{
    return EUI_ERR_OK;
}

eui_err_t eui_run(eui_context_t *ctx)
{
    eui_err_t ret;

    ret = eui_update_animation_states(ctx);
    assert(ret == EUI_ERR_OK);

    return eui_renderer_run(ctx->renderer);
}
