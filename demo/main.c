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
    float offset;
} eui_animation_node_ex_t;

void animation_apply_x(void *transformed_source, const eui_animation_node_t * const anim_node)
{
    eui_animation_node_ex_t *st = (eui_animation_node_ex_t*) anim_node;
    eui_rect_t *rect = (eui_rect_t *)transformed_source;

    float value = (*st->node.state);

    rect->pos.x += (value+st->offset)*(WIDTH/3+10);
    rect->pos.y += 0;
}

void add_box(eui_shape_t *shape_bg, eui_shape_t *shape_fg0, eui_shape_t *shape_fg1, eui_shape_t *shape_fg2, eui_animation_state_t *anim, uint32_t offset, eui_animation_node_ex_t *anim_fg)
{
    eui_color_t green = { .value = 0xff00ff00 };
    eui_color_t blue  = { .value = 0xffff0000 };

    EUI_INIT_SHAPE(*shape_fg0, 10, 10,    WIDTH/3,     (HEIGHT*3)/4, green);
    EUI_INIT_SHAPE(*shape_fg1, 20, 10+10,    WIDTH/3-20,     30, blue);
    EUI_INIT_SHAPE(*shape_fg2, 20, 60,    WIDTH/3-20,     120, blue);
    eui_node_add_last(&shape_bg->node, &shape_fg0->node);
    eui_node_add_last(&shape_bg->node, &shape_fg1->node);
    eui_node_add_last(&shape_bg->node, &shape_fg2->node);

    *anim_fg = (eui_animation_node_ex_t){
        .node = eui_create_animation_node(animation_apply_x, *anim),
        .offset = offset
    };

    shape_fg0->rect_animator = (eui_animation_node_t*)anim_fg;
    shape_fg1->rect_animator = (eui_animation_node_t*)anim_fg;
    shape_fg2->rect_animator = (eui_animation_node_t*)anim_fg;


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

    eui_color_t red   = { .value = 0xff0000ff };

    EUI_INIT_SHAPE(shape_bg,  0, 0, WIDTH, HEIGHT, red);

    

    res = eui_renderer_init(&renderer);
    assert(res == EUI_ERR_OK);
    res = eui_renderer_set_root(&renderer, &shape_bg.node);
    assert(res == EUI_ERR_OK);

    eui_context_t context = {
        .renderer = &renderer,
    };
    eui_init(&context);

    eui_animation_state_t anim_states[] = {
        eui_create_animation_state(500, 0, false, true),
    };
    anim_states[0].ease_cb = eui_easeinout;

    eui_set_animation_states(&context, anim_states, sizeof(anim_states)/sizeof(anim_states[0]));

    eui_shape_t shape_fg[30];
    eui_animation_node_ex_t anim_fg[10];
    for(int i=0;i<10;++i){
        add_box(&shape_bg, &shape_fg[i*3], &shape_fg[i*3+1], &shape_fg[i*3+2], &anim_states[0], i+1, &anim_fg[i]);
    }

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
                case SDLK_RIGHT:
                    if(!anim_states[0].running && anim_states[0].offset > -9){
                        anim_states[0].offset-=1;
                        anim_states[0].duration = -500;
                        anim_states[0].anim_value = 1;
                        anim_states[0].running = true;
                    }
                    break;
                case SDLK_LEFT:
                    if(!anim_states[0].running && anim_states[0].offset < 0){
                        anim_states[0].offset+=1;
                        anim_states[0].duration = 500;
                        anim_states[0].anim_value = 0;
                        anim_states[0].running = true;
                    }
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
