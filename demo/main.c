#include <SDL2/SDL.h>
#include <assert.h>

#include "embui.h"


#define WIDTH  320
#define HEIGHT 240
#define BPP      2
#define SCALE    4

SDL_Window *window;
SDL_Renderer *sdl_renderer;
SDL_Texture *fb_texture;
uint16_t fb_data[WIDTH * HEIGHT * BPP];

int init_window(int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 0;

    window = SDL_CreateWindow("demo",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width * SCALE, height * SCALE,
        0);
    if (!window)
        return 0;

    sdl_renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer)
        return 0;

    fb_texture = SDL_CreateTexture(sdl_renderer,
        SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING,
        width, height);
    if (!fb_texture)
        return 0;

    return 0;
}

typedef struct eui_animation_node_ex {
    eui_animation_node_t node;
    float period, diameter, offset;
} eui_animation_node_ex_t;

void animation_apply_x(void *transformed_source, const eui_animation_node_t * const anim_node)
{
    eui_animation_node_ex_t *st = (eui_animation_node_ex_t*) anim_node;
    eui_rect_t *rect = (eui_rect_t *)transformed_source;

    float value = (eui_easeinout(*st->node.state))*3.14159*2 * st->period + st->offset;

    float s = sin(value);
    float c = cos(value);

    float w = rect->size.width;
    float h = rect->size.height;

    rect->pos.x += s * st->diameter - cos(value) * (w/2);
    rect->pos.y += c * st->diameter + sin(value) * (h/2);
}

int main(int argc, char *argv[])
{
    SDL_Event event;
    int32_t running = 1;

    eui_err_t res;
    eui_renderer_t renderer = {
        .framebuffer = fb_data,
        .format = EUI_PIXEL_FORMAT_RGB_565,
        .size.width = WIDTH,
        .size.height = HEIGHT,
    };

    init_window(WIDTH, HEIGHT);

    eui_shape_t shape_bg;
    eui_shape_t shape_fg0, shape_fg1, shape_fg2, shape_fg3;

    eui_color_t red   = { .value = 0xff0000ff };
    eui_color_t green = { .value = 0xff00ff00 };
    eui_color_t blue  = { .value = 0xffff0000 };

    EUI_INIT_SHAPE(shape_bg,  0,     0, WIDTH, HEIGHT, red);
    EUI_INIT_SHAPE(shape_fg0, WIDTH/2, HEIGHT/2,    10,     10, green);
    EUI_INIT_SHAPE(shape_fg1, WIDTH/2, HEIGHT/2,    10,     10, blue);
    EUI_INIT_SHAPE(shape_fg2, WIDTH/2, HEIGHT/2,    10,     10, blue);
    EUI_INIT_SHAPE(shape_fg3, WIDTH/2, HEIGHT/2,    10,     10, blue);

    eui_node_insert(&shape_bg.node,  &shape_fg0.node);
    eui_node_insert(&shape_fg0.node, &shape_fg1.node);
    eui_node_insert(&shape_fg1.node, &shape_fg2.node);
    eui_node_insert(&shape_fg2.node, &shape_fg3.node);

    res = eui_renderer_init(&renderer);
    assert(res == EUI_ERR_OK);
    res = eui_renderer_set_root(&renderer, &shape_bg.node);
    assert(res == EUI_ERR_OK);

    eui_context_t context = {
        .renderer = &renderer,
    };
    eui_init(&context);

    eui_animation_state_t anim_x_states[] = {
        eui_create_animation_state(8000, -1, true),
    };

    eui_set_animation_states(&context, anim_x_states, sizeof(anim_x_states)/sizeof(anim_x_states[0]));

    eui_animation_node_ex_t anim_fg0 = {
        .node = eui_create_animation_node(animation_apply_x, anim_x_states[0]),
        .period = 1, .diameter = 30, .offset = 0,
    };
    eui_animation_node_ex_t anim_fg1 = {
        .node = eui_create_animation_node(animation_apply_x, anim_x_states[0]),
        .period = 1, .diameter = 30, .offset = 3.14159265/2,
    };
    eui_animation_node_ex_t anim_fg2 = {
        .node = eui_create_animation_node(animation_apply_x, anim_x_states[0]),
        .period = 1, .diameter = 30, .offset = 3.14159265,
    };
    eui_animation_node_ex_t anim_fg3 = {
        .node = eui_create_animation_node(animation_apply_x, anim_x_states[0]),
        .period = 1, .diameter = 30, .offset = 3.14159265*3.f/2,
    };

    shape_fg0.rect_animator = &anim_fg0.node;
    shape_fg1.rect_animator = &anim_fg1.node;
    shape_fg2.rect_animator = &anim_fg2.node;
    shape_fg3.rect_animator = &anim_fg3.node;

    while (running) {

        res = eui_run(&context);
        assert(res == EUI_ERR_OK);

        SDL_UpdateTexture(fb_texture, NULL, fb_data, WIDTH * BPP);
        SDL_RenderCopy(sdl_renderer, fb_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    running = 0;
                    break;
                case SDLK_SPACE:
                    anim_x_states[0].running = ! anim_x_states[0].running;
                    break;
                default:
                    break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                default:
                    break;
                }
            }
        }
    }

    SDL_DestroyTexture(fb_texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);

    return 0;
}
