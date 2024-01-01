#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

// Function to calculate distance between two points
double calculateDistance(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    // Get the screen dimensions
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    int screenWidth = dm.w;
    int screenHeight = dm.h;

    // Calculate the window size (3:2 aspect ratio, 3/4 of the screen width)
    int windowWidth = (3 * screenWidth) / 4;
    int windowHeight = (windowWidth * 2) / 3;

    // Center the window on the screen
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // Create the window
    SDL_Window *window = SDL_CreateWindow("ERANGEL", windowX, windowY, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        return 1;
    }

    // Load a font (replace "path/to/your/font.ttf" with the path to your TTF font file)
    TTF_Font *font = TTF_OpenFont("font.ttf", 24); // Adjust the size as needed
    if (!font)
    {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Surface *imageSurface = IMG_Load("world_map.jpg"); // Replace "world_map.jpg" with the path to your image
    if (!imageSurface)
    {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_Event e;
    int quit = 0;

    int clickCount = 0;
    int clickLocations[2][2]; // Array to store two clicked locations (x, y)

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                // Handle mouse click event
                if (clickCount < 2)
                {
                    clickLocations[clickCount][0] = e.button.x;
                    clickLocations[clickCount][1] = e.button.y;
                    clickCount++;
                }

                if (clickCount == 2)
                {
                    // Calculate distance between points
                    double distance = calculateDistance(clickLocations[0][0] - windowWidth / 2, windowHeight / 2 - clickLocations[0][1],
                                                        clickLocations[1][0] - windowWidth / 2, windowHeight / 2 - clickLocations[1][1]);

                    // Display distance on the window
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_Rect textRect = {10, 10, 200, 30};
                    char distanceText[50];
                    snprintf(distanceText, sizeof(distanceText), "Distance: %.2f", distance);

                    // Set text color (white in this case)
                    SDL_Color textColor = {255, 255, 255};

                    SDL_Surface *textSurface = TTF_RenderText_Solid(font, distanceText, textColor);
                    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_FreeSurface(textSurface);

                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_DestroyTexture(textTexture);

                    // Draw a blue path between the points
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderDrawLine(renderer, clickLocations[0][0], clickLocations[0][1],
                                       clickLocations[1][0], clickLocations[1][1]);

                    // Present the renderer
                    SDL_RenderPresent(renderer);

                    // Wait for 1000 milliseconds (1 second)
                    SDL_Delay(2000);

                    // Reset click count for the next pair of points
                    clickCount = 0;
                }
            }
        }

        // Draw the world map as the background
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Draw clicked points as filled circles
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < clickCount; ++i)
        {
            SDL_RenderDrawPoint(renderer, clickLocations[i][0], clickLocations[i][1]);
            SDL_RenderDrawPoint(renderer, clickLocations[i][0] + 1, clickLocations[i][1]);
            SDL_RenderDrawPoint(renderer, clickLocations[i][0], clickLocations[i][1] + 1);
            SDL_RenderDrawPoint(renderer, clickLocations[i][0] + 1, clickLocations[i][1] + 1);

            // Increase the size of the dots
            SDL_RenderDrawPoint(renderer, clickLocations[i][0] - 1, clickLocations[i][1]);
            SDL_RenderDrawPoint(renderer, clickLocations[i][0], clickLocations[i][1] - 1);
            SDL_RenderDrawPoint(renderer, clickLocations[i][0] - 1, clickLocations[i][1] - 1);
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
