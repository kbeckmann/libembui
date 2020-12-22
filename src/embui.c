#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "embui.h"


void render_shape_rgb565(eui_renderer_t *renderer, eui_shape_t *shape)
{
    eui_color_t source_color = shape->color;
    uint16_t draw_color;
    eui_rect_t area = renderer->active_area;
    eui_size_t fb_size = renderer->size;
    uint16_t *fb = (uint16_t *) renderer->framebuffer;
    uint32_t stride = fb_size.width;

    if (source_color.rgba.a == 0xff) {
        // Opaque
        draw_color = ((source_color.rgba.r & 0xf8) <<  8) |
                     ((source_color.rgba.g & 0xfc) <<  3) |
                     ((source_color.rgba.b)        >>  3);

        int32_t start_x = area.pos.x + shape->rect.pos.x;
        int32_t end_x   = area.pos.x + shape->rect.pos.x + shape->rect.size.width;
        int32_t start_y = area.pos.y + shape->rect.pos.y;
        int32_t end_y   = area.pos.y + shape->rect.pos.y + shape->rect.size.height;
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

eui_err_t eui_renderer_run(eui_renderer_t *renderer)
{
    eui_node_t *node = renderer->priv.root;
    eui_pixel_format_t dest_fmt = renderer->format;
    eui_shape_t *shape;

    while (node != NULL) {
        shape = (eui_shape_t *) node;
        assert(shape->type == EUI_NODE_TYPE_SHAPE);

        switch (shape->type) {
        case EUI_NODE_TYPE_SHAPE:
            switch (dest_fmt) {
            case EUI_PIXEL_FORMAT_RGB_565:
                render_shape_rgb565(renderer, shape);
                break;
            default:
                assert(!"Unhandled pixel format.");
            }
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
    eui_node_t * old_next = node->next;
    node->next = next;
    next->next = old_next;
}

eui_err_t eui_renderer_set_root(eui_renderer_t *renderer, eui_node_t *root)
{
    renderer->priv.root = root;

    return EUI_ERR_OK;
}
