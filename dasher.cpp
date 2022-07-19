#include "raylib.h"

struct AnimData{                                                                // declare new custom data type --> struct
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

// Own function declarations
    bool isOnGround(AnimData data, int window_height){
        return data.pos.y >= window_height - data.rec.height;                  //will be true if scarfy is on ground and false if in air
    }

    AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame){
          // update running time
        data.runningTime += deltaTime;
        if (data.runningTime >= data.updateTime){
            data.runningTime = 0.0;
            // update animation frame
            data.rec.x = data.frame * data.rec.width;
            data.frame++;
            if (data.frame > maxFrame){
                data.frame = 0;
            }
        }
    return data;
    }

int main(){

    // window dimension
    int windowDimensions[2];                                                    //creates an array of 2 integers
    windowDimensions[0] = 512;                                                  //window width in first element of array
    windowDimensions[1] = 380;                                                  //window height in first element of array                              
                                                                                
    // initialse the window                                                     // at the upper left of window, coordinates are (0;0)                                                                                                             
    InitWindow(windowDimensions[0],windowDimensions[1],                         // down and right of the window are positive
    "Zai's Dapper Dasher Game!"); 

    const int gravity=1000;                                                     // acceleration due to gravity - (pixels/frame)/frame
    
   // SCARFY VARIABLES                                                         // Raylib data types
        Texture2D scarfy = LoadTexture("textures/scarfy.png");                  // Texture2D is a compound data type

        AnimData scarfyData;                                                    // custom data variable - created with struct
        scarfyData.rec.width = scarfy.width/6; 
        scarfyData.rec.height = scarfy.height;
        scarfyData.rec.x =0;
        scarfyData.rec.y =0;
        scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
        scarfyData.pos.y = windowDimensions[1]/2 - scarfyData.rec.height;
        scarfyData.frame = 0.0;
        scarfyData.updateTime =1.0/12.0 ;                                       // amnt of time that should pass between each change in animation 
        scarfyData.runningTime = 0.0;

    const int jump_velocity = -600;                                             // scarfy jump velocity - pixels per second
    bool IsInAir{false};                                                        // initialise scarfy to the ground
    int velocity{-10};


   // NEBULA VARIABLES 
        Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

        const int sizeOfNebulae{3};
        AnimData nebulae[sizeOfNebulae]{};

        for (int i = 0; i < sizeOfNebulae; i++){
                nebulae[i].rec.x = 0.0;
                nebulae[i].rec.y = 0.0;
                nebulae[i].rec.width = nebula.width/8;
                nebulae[i].rec.height = nebula.height/8;
                nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
                nebulae[i].frame = 0;
                nebulae[i].runningTime = 0.0;
                nebulae[i].updateTime = 0.0;

                nebulae[i].pos.x =  windowDimensions[0] + i * 300;
            }

                //has the last value in the nebula array (-1 because elemnts start with the index of 0)
        float finishLine{ nebulae[sizeOfNebulae - 1].pos.x }; // finish line to win

        int nebVel{-200}; //// nebula X velocity (pixels/second)

   // background
     Texture2D background = LoadTexture("textures/far-buildings.png");
     float bgX{}; //initialises it to value of zero

   // midground
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};

   // foreground
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};
 
 bool collision = false;
 
 SetTargetFPS(60);                                                            

 // body of while loop executes each frame                                      // if FPS is 60, it will execute 60 times exery second
     
    while (!WindowShouldClose()){                                                                    
       
        const float deltaTime = GetFrameTime();                                 // time since last frame - allows control over animation speed 
                                                                                
        BeginDrawing();                                                         // start drawing 
        ClearBackground(WHITE);

           // scroll the background x
                bgX -= 20 * deltaTime;                                              // update 20 pixels per second - multiply by deltaTime so it is frame independant 
                // draw background twice for loop
                if (bgX <= -background.width*2){                                    // to loop the two background images 
                    bgX = 0.0;
                }

           // scroll the midground
                mgX -= 40 * deltaTime;
                if (mgX <= - midground.width*2){
                    mgX = 0.0;
                }
            
           // scroll the midground
                fgX -= 80 * deltaTime;
                if (fgX <= -foreground.width*2){
                    fgX = 0.0;
                }

       // draw background
            Vector2 bg1Pos{bgX, 0.0};                                           
            DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);                 // remember to draw this before drawing scarfy and the nebulae
            Vector2 bg2Pos{bgX + background.width*2, 0.0};                      // whatever you draw first is sent to back, what you draw after is drwan on top of it
            DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);    

       // draw the midground
            Vector2 mg1Pos{mgX, 0.0};
            DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
            Vector2 mg2Pos{mgX + midground.width*2, 0.0};
            DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

       // draw the foreground
            Vector2 fg1Pos{fgX, 0.0};
            DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
            Vector2 fg2Pos{fgX + foreground.width*2, 0.0};
            DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);     
           

       //CHECK FOR COLLISSION BEFORE DRAWING SCARFY AND NEBULAE
                //this is a range based for loop
            for (AnimData nebula : nebulae){                                    // can also say for (int i = 0; i < sizeOfNebulae; i++){}                   
                float pad{50};                                                  // the nebula sprite has a bit of padding - meaning there will be a collision before scarfy even gets close to the nebula
                                                                                // so we can fix thois by creating a smaller rectangle around the nebula more representative of the nebulas actual bounds
                                                                                
                Rectangle nebRec{
                    nebula.pos.x + pad,
                    nebula.pos.y + pad,
                    nebula.rec.width - 2*pad,
                    nebula.rec.height - 2*pad
                };
                Rectangle scarfyRec{                                            // we dont need a padding for scarfy sprite as the scarfy character reaches all the way to the edges of each animation frame
                    scarfyData.pos.x,
                    scarfyData.pos.y,
                    scarfyData.rec.width,
                    scarfyData.rec.height
                };
                if (CheckCollisionRecs(nebRec, scarfyRec)){
                    collision = true;
                }
            }

            if (collision){ //LOSE GAME
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
            }
            else if (scarfyData.pos.x >= finishLine){
            // win the game
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
            }
            else{
                for (int i = 0; i < sizeOfNebulae; i++){
                    // draw nebula
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                }
                // draw scarfy
                DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            }

            //perform ground check                                              // scarfy is on the ground for groundcheck inIsAir = true
            if (isOnGround(scarfyData, windowDimensions[1])){                   // calliing our own function
                velocity = 0;                                                   // velocity in pixels per frame
             //scarfys animation frame
                IsInAir = false;}                                               // only apply gravity if in air
            else {
             //apply gravity
                velocity += gravity*deltaTime;                                  // allows exponential rise and fall in the jump
                IsInAir = true;
            }

            //jump check
            if (IsKeyPressed(KEY_SPACE) && IsInAir==false){                     // && IsInAir==false allows for no double/air jumping
                velocity += jump_velocity;                                      // inc velocity with scarfy jump velocity in pixels per second
                IsInAir = true;
            }

            // update scarfy's animation frame using our own function updateAnimData
            if (!IsInAir){
                scarfyData = updateAnimData(scarfyData, deltaTime, 5);
            }

            // update nebula animation frame using our own function updateAnimData
            for (int i = 0; i < sizeOfNebulae; i++){ 
                nebulae[i] = updateAnimData(nebulae[i], deltaTime, 7);
            }
            
            // update position for scarfy
             scarfyData.pos.y += velocity * deltaTime;                           // update scarfy position (scaled by deltaTime)
             
             for (int i = 0; i < sizeOfNebulae; i++){
            // update the position of each nebula
                nebulae[i].pos.x += nebVel * deltaTime;
             }

            // update finishLine
             finishLine += nebVel * deltaTime;


     EndDrawing();                                                              // end drawing   
    }                                                                           // end while loop                                        

    UnloadTexture(nebula);                                                      // 1. unload the textures we loaded in the Texture2D
    UnloadTexture(scarfy);                                                      // 2. Texture2D scarfy = LoadTexture("textures/scarfy.png"); etc
    UnloadTexture(background);                                                  // 3. unload the background
    UnloadTexture(midground);                                                   // 4. unload the midground
    UnloadTexture(foreground);                                                  // 5. unload the foreground


    CloseWindow();                                                              // Raylib shuts down the window properly
    
/*NOTES

 // BASIC OPERRATIONS--> the ! is a negation operator aka !bool returns false - aka negates a truth value

 // const correct = term for good programming practice - declaring constants.

 // FPS value is not guranteed- -cpu strain can lower the frame rate - thats why dont change animation in frames, but rather time

Velocity is raylib pixels per frame 


FOR LOOPS
1. initilisation of loop variable - i = index   
2. loop condition - boolean
3. statement for loop update


for (int i = 0; i < 5; i++){                        //initialisation; loop condition; loop update
    double dmg = damage[i]                          //body that will run each iteration of the loop
}

RANGED BASED FOR LOOPS
for (AnimData nebula : nebulae){                    // loop body is executed for each element in the array
}



FUNCTIONS
declare functions outside (above) the main function aka int main(){}

int addition(int a, int b){             //return type + function name //must specify the data type for each input parameter.       
    return a + b; 
}
                     
int result = addition(2,3)              //calling the function

int two = 2;                            //predefined variables
int three = 3;                      
int result= addition(two,three)         //calling the function with predefined variables 


A FUNCTION DOESNT HAVE TO RETURN VALUES AT ALL
a function with no return values is called a VOID type

void sayHi(){                           //void functions dont need to have return statements, they simply perform an action
    printf("Hi")
}

void makeTwo (int a){       //this function took an integer input parameter a and assigns a value of two to a inside the function body 
    a = 2;                  //changing a variable in a function is only local to the function
                            
}

//aka the variable will be left unchanged outside the function
int three=3;
makeTwo(three); //this function will take input parameter a and takes three as a copy. so a starts off with three and quickly becomes two
                    //after the function has finished, three will still have its original value
                    //this is because three is passed in to make 2, and the input parameter a gets the value of 3 as a copy
                    //hence, functions do not alter the values of variables outside the functions, they take those values in as copies
                        and perform any actions specified inside the function           

Refactoring --> the process of creating our own functions in order to replace some of the code in our program, and then call our functions instead


ARRAYS
double damage[4]; creates an array of size 4
every value in array is called elements
damage[0] = 20.0; //initialise the elements in an array
damage[1] = 20.0; 
damage[2] = 20.0;
damage[3] = 20.0; --> 0 - 3 = 4 elements
or
AnimData damage[4]{20.0, 20.0, 20.0, 20.0}; 


SPRTIES - 2d image
    Each pixel has RED, GREEN, BLUE and ALPHA
    Transparent background

SPRITE SHEET 
countains multiple images in one file
only draw certain parts of the sprite sheets at a time

Raylib has a function that allows us to call a certain part of the sprite sheet
DrawTextureRec(texture,source,position,tint); - It basically takes a rectangle from the spritesheet

texture - load the texture 
source - rectangle to specify which part of the sprite sheet to draw
            rectangles have an x and y and width and height
            because there are 6 images of scarfy on the sprite sheet... the width of the part that we want
            is the width of the whole sprite sheet divided by 6.

position - location that we want to draw our sprite in the window - upper left corner
tint - if you dont wanna tint the sprite - choose WHITE

THE STRUCT
custom data type - contain its own variables 
ideal for repeated coat / variables
//FORMAT                                                                        --> Declare outside the main function --> outside int main{}
struct MyCustomType{
    float x;                                                                    --> member variables in a struct
    float y;
}

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};                                                                              --> dont forget this semicolon at the end


BACKGROUNDS // foregrounds // midgrounds
Raylib has a function called DrawTextureEx()
- It draws a texture, and is useful for scaling an image up or down

void DrawTextureEx(
    Texture2D texture,
    Vector2 position,
    float rotation,                 // input 0.0 if you dont want to rotate the texture
    float scale,
    Color tint);                    //remember c++ is Case sensitive
)

COLLISSION DETECTION
Raylib has a function called CheckCollisionRecs()
- it takes two rectangle vectors and returns a boolean which is true if these rectangles are colliding

bool CheckCollisionRecs(
    Rectangle rec1,
    Rectangle rec2
);





//OLD DELETED CODE
// AnimData for nebula
        AnimData nebData{ 
        {0.0, 0.0, nebula.width/8, nebula.height/8},                            // Rectangle    rec
        {windowDimensions[0], windowDimensions[1] - nebula.height/8},           // Vector2      pos
        0,                                                                      // int          frame
        1.0/12.0,                                                               // float        updateTime
        0                                                                       // float        runningTme
        };

     // AnimData for multiple nebulas
        AnimData neb2Data{
        {0.0, 0.0, nebula.width/8, nebula.height/8},                            // Rectangle    rec
        {windowDimensions[0] + 300, windowDimensions[1] - nebula.height/8},     // Vector2      pos
        0,                                                                      // int          frame
        1.0/16.0,                                                               // float        updateTime
        0.0                                                                     // float        runningTime
        };
*/

}