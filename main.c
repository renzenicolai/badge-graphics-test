#include <stdio.h>
#include <unistd.h>
#include <pax_gfx.h>
#include <pax_codecs.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480

pax_buf_t fb;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

typedef enum asset_index {
    ASSET_LOGO,
    ASSET_USER_ICON,
    ASSET_BATTERY_ICON,
    ASSET_WIFI_ICON,
    ASSET_APP_ICON,
    ASSET_MESSAGES_ICON,
    ASSET_BADGEHUB_ICON,
    ASSET_SETTINGS_ICON,
    ASSET_POWER_ICON,
    ASSET_LAST
} asset_index_t;

pax_buf_t assets[ASSET_LAST];

bool initialize_asset(asset_index_t index, const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        printf("Failed to open %s\n", file_path);
        return false;
    }
    bool res = pax_decode_png_fd(&assets[index], file, PAX_BUF_32_8888ARGB, 0);
    fclose(file);
    return res;
}

bool initialize_assets() {
    if (!initialize_asset(ASSET_LOGO, "resources/badgeteam.png")) return false;
    if (!initialize_asset(ASSET_USER_ICON, "resources/user_icon.png")) return false;
    return true;
}

void sdl2_blit(pax_buf_t* pax_buffer) {
    int texture_pitch = 0;
    void* texture_pixels = NULL;
    if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0) {
        SDL_Log("Unable to lock texture: %s", SDL_GetError());
    }
    else {
        memcpy(texture_pixels, pax_buffer->buf, texture_pitch * WINDOW_HEIGHT);
    }
    SDL_UnlockTexture(texture);
}

int sdl2_start() {
    int i;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Graphics test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);
    if (texture == NULL) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

void sdl2_stop() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void save_to_file() {
    FILE* file = fopen("output.raw", "wb");
    if (file) {
        fwrite(fb.buf, fb.width * fb.height * 2, 1, file);
        fclose(file);
    }
}

void draw_textbox(pax_buf_t* pax_buffer, float position_x, float position_y, float width, float height, float text_height, pax_col_t text_color, pax_col_t bg_color, const char* label) {
    const pax_font_t* font = pax_font_saira_regular;
    pax_draw_rect(pax_buffer, bg_color, position_x, position_y, width, height);
    //pax_clip(pax_buffer, position_x, position_y, width, height);
    pax_draw_text(pax_buffer, text_color, font, text_height, position_x, position_y + ((height - text_height) / 2) + 1, label);
    //pax_noclip(pax_buffer);
}

void draw_menu(pax_buf_t* pb, float position_x, float position_y, float width, float height) {
    pax_outline_rect(pb, pax_col_rgb(255, 0, 0), position_x - 1, position_y - 1, width + 1, height + 1);
    float remaining_height = height;
    if (remaining_height >= 32) {
        draw_textbox(pb, position_x, position_y, width, 32, 18, pax_col_rgb(0, 0, 0), pax_col_rgb(255, 255, 255), "Test");
        remaining_height -= 32;
        position_y += 32;
    }

    uint8_t i = 0;
    while (remaining_height >= 32) {
        draw_textbox(pb, position_x, position_y, width, 32, 18, pax_col_rgb(0, 0, 0), pax_col_rgb(50*i, 0, 0), "Test");
        i+=1;
        remaining_height -= 32;
        position_y += 32;
    }
}

void draw_background(pax_buf_t* buffer, float width, float height, char* header_text_l, char* header_text_c, char* header_text_r, char* footer_text) {
    float header_height = 32;
    float footer_height = 24;
    float corner_radius = header_height * 0.5;
    
    pax_col_t background_color = 0xffffffff;
    pax_col_t header_background_color = 0xff491d88;
    pax_col_t header_text_color = 0xfffec859;

    const pax_font_t* font = pax_font_saira_regular;

    pax_background(buffer, header_background_color);
    pax_draw_text(buffer, header_text_color, font, header_height - 4, 2, 2, header_text_l);
    pax_center_text(buffer, header_text_color, font, header_height - 4, width / 2, 2, header_text_c);
    pax_right_text(buffer, header_text_color, font, header_height - 4, width, 2, header_text_r);
    pax_draw_round_rect(buffer, background_color, 0, header_height, width, height - header_height - footer_height, corner_radius);
    pax_draw_text(buffer, header_text_color, font, footer_height - 4, 2, height - footer_height + 2, footer_text);
}

void draw_test(pax_buf_t* buffer) {
    char* header_text_l = "12:34";
    char* header_text_c = "Main menu";
    char* header_text_r = "Batt. full";
    char* footer_text = "WHY2025 interface mockup";
    draw_background(buffer, buffer->width, buffer->height, header_text_l, header_text_c, header_text_r, footer_text);
}

int main() {
    bool res;

    res = initialize_assets();

    if (!res) {
        printf("Failed to initialize assets\n");
        return 1;
    }

    printf("Linux graphics test\n");
    pax_buf_init(&fb, NULL, 800, 480, PAX_BUF_16_565RGB);
    pax_buf_reversed(&fb, false);
    pax_background(&fb, pax_col_rgb(0, 255, 0));

    // Pre-render background
    pax_buf_t background;
    pax_buf_init(&background, NULL, 800, 480, PAX_BUF_16_565RGB);
    draw_test(&background);

    pax_draw_image(&fb, &background, 0, 0); // Copy background to framebuffer

    pax_draw_image(&fb, &assets[ASSET_LOGO], (fb.width - assets[ASSET_LOGO].width) / 2, (fb.height - assets[ASSET_LOGO].height) / 2); // Logo

    //draw_menu(&fb, fb.width / 4, fb.height / 4, fb.width / 2, fb.height / 2);
    
    // Save to raw file then exit
    save_to_file();
    //return 0;

    //Or start SDL2 and render in a window

    sdl2_start();

    bool should_quit = false;
    SDL_Event e;

    while (!should_quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                should_quit = true;
            }
        }

        //pax_draw_image(&fb, &background, 0, 0); // Copy background to framebuffer
        //... Add your own drawing stuff here

        // render PAX buffer to SDL2 screen
        sdl2_blit(&fb);

        // render on screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    sdl2_stop();

    return 0;
}
