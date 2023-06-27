#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include "Image.h"
#include "Button.h"
#include "Input.h"

#include <SDL.h>
#include <SDL_image.h>

void removeFilters(Image* img) {
    SDL_Surface* originalSurface = img->getOriginalSurface();
    img->setSurface(originalSurface);
}

void applyBlur(Image * img, int iterations)
{
    SDL_Surface* blurredSurface = SDL_ConvertSurfaceFormat(img->getOriginalSurface(), SDL_PIXELFORMAT_RGBA8888, 0);

    float kernel[9] = {
        1.0f / 9, 1.0f / 9, 1.0f / 9,
        1.0f / 9, 1.0f / 9, 1.0f / 9,
        1.0f / 9, 1.0f / 9, 1.0f / 9
    };

    Uint32* pixels = (Uint32*)blurredSurface->pixels;
    int width = blurredSurface->w;
    int height = blurredSurface->h;

    for (int i = 0; i < iterations; i++)
    {
        for (int y = 1; y < height - 1; y++)
        {
            for (int x = 1; x < width - 1; x++)
            {
                float red = 0.0f, green = 0.0f, blue = 0.0f;

                for (int ky = -1; ky <= 1; ky++)
                {
                    for (int kx = -1; kx <= 1; kx++)
                    {
                        Uint32 pixel = pixels[(y + ky) * width + (x + kx)];
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, blurredSurface->format, &r, &g, &b);

                        red += r * kernel[(ky + 1) * 3 + (kx + 1)];
                        green += g * kernel[(ky + 1) * 3 + (kx + 1)];
                        blue += b * kernel[(ky + 1) * 3 + (kx + 1)];
                    }
                }

                Uint32 blurredPixel = SDL_MapRGBA(blurredSurface->format, (Uint8)red, (Uint8)green, (Uint8)blue, 255);
                pixels[y * width + x] = blurredPixel;
            }
        }
    }

    SDL_BlitSurface(blurredSurface, nullptr, img->getSurface(), nullptr);
    img->setSurface(blurredSurface);
    SDL_FreeSurface(blurredSurface);
}

void applySharpen(Image* img)
{
    SDL_Surface* surface = img->getOriginalSurface();
    SDL_Surface* sharpenedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Surface* originalSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);

    int kernel[3][3] = {
        { -1, -1, -1 },
        { -1,  9, -1 },
        { -1, -1,  -1 }
    };

    Uint32* pixels = (Uint32*)sharpenedSurface->pixels;
    Uint32* surfacePixels = (Uint32*)originalSurface->pixels;

    int width = sharpenedSurface->w;
    int height = sharpenedSurface->h;

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int red = 0, green = 0, blue = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    Uint32 pixel = surfacePixels[(y + ky) * width + (x + kx)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, sharpenedSurface->format, &r, &g, &b);

                    red += ((int)r) * kernel[ky + 1][kx + 1];
                    green += ((int)g) * kernel[ky + 1][kx + 1];
                    blue += ((int)b) * kernel[ky + 1][kx + 1];
                }
            }

            red = red < 0 ? 0 : (red > 255 ? 255 : red);
            green = green < 0 ? 0 : (green > 255 ? 255 : green);
            blue = blue < 0 ? 0 : (blue > 255 ? 255 : blue);

            Uint32 sharpenedPixel = SDL_MapRGBA(sharpenedSurface->format, red, green, blue, 255);
            pixels[y * width + x] = sharpenedPixel;
        }
    }

    SDL_BlitSurface(sharpenedSurface, nullptr, surface, nullptr);
    img->setSurface(sharpenedSurface);
}

void applyEmboss(Image* img)
{
    SDL_Surface* surface = img->getOriginalSurface();
    SDL_Surface* embossedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Surface* originalSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);

    int kernel[3][3] = {
        {-2, -1,  0},
        {-1,  1,  1},
        { 0,  1,  2}
    };

    Uint32* pixels = (Uint32*)embossedSurface->pixels;
    Uint32* surfacePixels = (Uint32*)originalSurface->pixels;
    int width = embossedSurface->w;
    int height = embossedSurface->h;

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int red = 0, green = 0, blue = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    Uint32 pixel = surfacePixels[(y + ky) * width + (x + kx)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, embossedSurface->format, &r, &g, &b);

                    red += r * kernel[ky + 1][kx + 1];
                    green += g * kernel[ky + 1][kx + 1];
                    blue += b * kernel[ky + 1][kx + 1];
                }
            }

            red = red < 0 ? 0 : (red > 255 ? 255 : red);
            green = green < 0 ? 0 : (green > 255 ? 255 : green);
            blue = blue < 0 ? 0 : (blue > 255 ? 255 : blue);

            Uint32 embossedPixel = SDL_MapRGBA(embossedSurface->format, red, green, blue, 255);
            pixels[y * width + x] = embossedPixel;
        }
    }

    SDL_BlitSurface(embossedSurface, nullptr, surface, nullptr);
    img->setSurface(embossedSurface);

}

bool isSimilarColor(SDL_Color color1, SDL_Color color2, int tolerance) {
    int redDiff = abs(color1.r - color2.r);
    int greenDiff = abs(color1.g - color2.g);
    int blueDiff = abs(color1.b - color2.b);
    return (redDiff <= tolerance) && (greenDiff <= tolerance) && (blueDiff <= tolerance);
}

Uint32 SDL_GetPixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *reinterpret_cast<Uint16*>(p);
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *reinterpret_cast<Uint32*>(p);
        default:
            return 0;
    }
}

void SDL_SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *reinterpret_cast<Uint16*>(p) = pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *reinterpret_cast<Uint32*>(p) = pixel;
            break;
    }
}

int main(int argc, char * argv[]) {
    static const int HEIGHT = 800;
    static const int WIDTH = 600;
    
    //Input input = Input();
    
    
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao inicializar o SDL: %s", SDL_GetError());
        return 1;
    }
    
    
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao inicializar o SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Filtros de imagem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, HEIGHT, WIDTH, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 28);

    
    if (!window || !renderer || !font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar janela, renderer ou carregar a fonte: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    //Image * backgroundImage = new Image(renderer, input.getBackgroundPath(), 200, 150);
    //Image * imageOriginal = new Image(renderer, input.getImagePath(), 200, 150);

    Image* backgroundImage = new Image(renderer, "bkg.png", 700, 500);
    Image* imageOriginal = new Image(renderer, "meme.png", 700, 500);   

    SDL_Surface* backgroundImageSurface = backgroundImage->getSurface();
    SDL_Surface* imageOriginalSurface = imageOriginal->getSurface();

    SDL_Surface* outputImageWithBackground = SDL_CreateRGBSurface(0, backgroundImageSurface->w, backgroundImageSurface->h,
        backgroundImageSurface->format->BitsPerPixel,
        backgroundImageSurface->format->Rmask,
        backgroundImageSurface->format->Gmask,
        backgroundImageSurface->format->Bmask,
        backgroundImageSurface->format->Amask);

    SDL_Color chromaKeyColor = { 0, 255, 0 };
    int tolerance = 100;

    for (int y = 0; y < imageOriginalSurface->h; y++) {
        for (int x = 0; x < imageOriginalSurface->w; x++) {
            Uint32 originalImagePixel = SDL_GetPixel(imageOriginalSurface, x, y);
            SDL_Color originalImageColor;
            SDL_GetRGB(originalImagePixel, imageOriginalSurface->format, &originalImageColor.r, &originalImageColor.g, &originalImageColor.b);

            if (isSimilarColor(originalImageColor, chromaKeyColor, tolerance)) {
                Uint32 backgroundPixel = SDL_GetPixel(backgroundImageSurface, x, y);
                SDL_SetPixel(outputImageWithBackground, x, y, backgroundPixel);
            } else {
                SDL_SetPixel(outputImageWithBackground, x, y, originalImagePixel);
            }
        }
    }
    
      Image * outputImage = new Image(renderer, outputImageWithBackground, imageOriginal->getWidth(), imageOriginal->getHeight());


    Button * btnSaveImage = new Button(renderer, font, "Save Image", 570, 520, 200, 40);
    Button * btnAddBlur = new Button(renderer, font, "Blur", 100, 520, 100, 40);
    Button * btnAddSharpen = new Button(renderer, font, "Sharpen", 200, 520, 100, 40);
    Button * btnAddEmboss = new Button(renderer, font, "Emboss", 300, 520, 100, 40);
    Button* btnResetFilters = new Button(renderer, font, "Reset", 400, 520, 100, 40);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            btnSaveImage->handleEvent(event);
            btnAddBlur->handleEvent(event);
            btnAddSharpen->handleEvent(event);
            btnAddEmboss->handleEvent(event);
            btnResetFilters->handleEvent(event);
        }

        SDL_RenderClear(renderer);

        if (btnSaveImage->isClicked()){
            IMG_SavePNG(outputImage->getSurface(), "output.png");
        }
        if (btnAddBlur->isClicked()){
            applyBlur(outputImage, 3);
        }
        if (btnAddEmboss->isClicked()){
            applyEmboss(outputImage);
        }
        if (btnAddSharpen->isClicked()) {
            applySharpen(outputImage);
        }
        if (btnResetFilters->isClicked()) {
            removeFilters(outputImage);

        }
        outputImage->render(0, 0);
        
        btnSaveImage->render();
        btnAddBlur->render();
        btnAddSharpen->render();
        btnAddEmboss->render();
        btnResetFilters->render();

        SDL_RenderPresent(renderer);
    }
    
    
    // Finalização do SDL
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

