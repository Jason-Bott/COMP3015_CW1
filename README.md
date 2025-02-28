# COMP3015_CW1

## Introduction

For my CW1 I have created a prototype for what I will be aiming to create in CW2. The aim is to create a game in which the player has to navigate a repeating corridor that occasionally has anomalies making decisions on where to go based on their observations. For this prototype I have created a basic corridor with a pulsing red light to simulate a 'red alert' environment along with working blast doors and a spaceship as my model outside the windows.

![Game](/SampleScreens/game.png)

## Video Link

https://youtu.be/2pzCSWHPar4

## Software Used

Visual Studio 2022 (17.12.4)
Windows 11 (24H2)

Microsoft Paint 3D
Blender 3.5

## How It Works (Code Structure)

### On Load

On load the constructor for the scene and the initScene function are called. In the constructor you can find the loading of all the models, textures and normal maps into their respective variables. The initScene function handles the initial setup of the MVP and the lights as well as setting up the skybox.

![Load](/SampleScreens/load.png)

### Update Function

During the update function the game updates a few things in this order:
- Player Movement
	- Simple WASD controls moving the player based on the direction they are looking
	- The y height of the player is fixed to keep it as if they are 'walking' on the floor

![Movement](/SampleScreens/movement.png)

- Collision Checks
	- After the player has been moved this area checks if they are now in an invalid area
	- If they are they are sent back to a valid position

![Collision](/SampleScreens/collision.png)

- Animations (Blast doors and Spaceship)
	- After the players position is finalised the animated object can check whether they should be animated or not
	- If they are then their heights are updated accordingly

![Animation](/SampleScreens/animate.png)

- Lighting
	- Finally the view in the MVP is updated and can be used to update the lights
	- First the spot lights have their positions and directions updated with the new view
	- Then their ambient light is changed which creates the 'red alert' theme
	- To finish off the update function the final point light also has its position updated with the new view

![Lighting](/SampleScreens/light.png)

### Render Function

This is the function where each model is assigned its material, texture, normal map and position/scale/rotation. Each model in my scene uses the same material which is set at the start of this function however everything else is set again for each model. This includes the texture, normal and model (position/scale/rotation). For objects rendered multiple times like the blast doors that use the same texture and normal map, this only gets set once and the model is the only thing updated between their rendering.

Here is the list of objects rendered in the order they are rendered:
- Skybox
- Floor
- Window Wall
- Wall
- Ceiling
- Doorframes x4
- Blast doors x4
- Spcaeship

![Render](/SampleScreens/render.png)

### Mouse Callback

Each time the mouse is moved this function gets called to update the cameras front direction. This involves storing the pitch and yaw of the player and calculating the front direction accordingly.

![Mouse](/SampleScreens/mouse.png)

### Vertex Shader

My vertex shader transforms the vertex position using the MVP matrix for rendering the objects. It also calculates variables like the normal, tangent and binormal for my normal mapping and passes this all to the fragment shader along with the texture coordinates and skybox direction.

![Vertex](/SampleScreens/vertex.png)

### Fragment Shader

The fragment shader applies Blinn-Phong lighting using 5 lights, 4 spot lights and 1 point light. This can be easily altered to any combination of spot lights and point lights. The shader samples a colour texture and a normal map adjusting normals and blending lighting contributions to create the scene shown. The skybox is also rendered whenever far enough away and gamma correction is applied for proper colour output.

![Fragment](/SampleScreens/fragment.png)

## Credits

Spaceship Model by Ebal Studios on Sketchfab: https://sketchfab.com/3d-models/star-sparrow-modular-spaceship-28806b168f8043bbb5c1c922f98452c9

Skybox by kurtk84 on OpenGameArt: https://opengameart.org/content/space-skybox-1