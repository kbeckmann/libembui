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

int main(int argc, char *argv[])
{
    SDL_Event event;
    int32_t running = 1;

    eui_err_t res;
    eui_renderer_t renderer = {
        .framebuffer = fb_data,
        .active_area.pos.x = 0,
        .active_area.pos.y = 0,
        .active_area.size.width = WIDTH,
        .active_area.size.height = HEIGHT,
        .format = EUI_PIXEL_FORMAT_RGB_565,
        .size.width = WIDTH,
        .size.height = HEIGHT,
    };

    init_window(WIDTH, HEIGHT);

    eui_shape_t shape_bg;
    eui_shape_t shape_fg1;
    eui_shape_t shape_fg2;

    eui_color_t red   = { .value = 0xff0000ff };
    eui_color_t green = { .value = 0xff00ff00 };
    eui_color_t blue  = { .value = 0xffff0000 };

    EUI_INIT_SHAPE(shape_bg,  0,     0, WIDTH, HEIGHT, red);
    EUI_INIT_SHAPE(shape_fg1, 100, 100,    50,     50, green);
    EUI_INIT_SHAPE(shape_fg2, 125, 125,    50,     50, blue);

    eui_node_insert(&shape_bg.node, &shape_fg1.node);
    eui_node_insert(&shape_fg1.node, &shape_fg2.node);

    res = eui_renderer_init(&renderer);
    assert(res == EUI_ERR_OK);
    res = eui_renderer_set_root(&renderer, &shape_bg.node);
    assert(res == EUI_ERR_OK);

    while (running) {
        res = eui_renderer_run(&renderer);
        assert(res == EUI_ERR_OK);

        SDL_UpdateTexture(fb_texture, NULL, fb_data, WIDTH * BPP);
        SDL_RenderCopy(sdl_renderer, fb_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = 0;
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
