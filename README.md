# Battleship Mania - README

## Description
**Battleship Mania** is a 3D simulation game built using OpenGL. Players control a battleship navigating through a visually immersive starfield while avoiding obstacles and enemy fire. The game currently features:

- A battleship with dynamic camera controls.
- A starfield with 3D effects and stars dynamically introduced as the ship moves.
- Basic movement and navigation controls for the battleship.
- Firing mechanics for engaging enemies or obstacles.

## Features Implemented
1. **Battleship Mechanics**
   - Battleship designed with a fighter jet-like appearance, featuring a grey and red color scheme.
   - Fire effects from the thrusters for added realism.

2. **Starfield**
   - A dynamic starfield visible in all directions.
   - Stars change positions relative to the battleship's movements.
   - Continuous introduction of new stars as old ones move out of view.

3. **Camera**
   - Third-person perspective centered directly behind the battleship.

4. **Movement and Combat**
   - Tilt and directional controls for realistic battleship movement.
   - Ability to fire bullets at enemies or obstacles.
   - Hit an asteroid three times to make it explode.
   
5. **Ship Health**
    - The ship starts with 5 health points and it reduces every time it crashes into a celestial body.
    - Crashing into asteroids reduces 1 health point.
    - Crashing into planets and the sun causes instant destruction.
    - Crashing into comet reduces 3 health points.

## How to Operate the Game
### Setup and Run
1. Compile the project using the provided `Makefile`:
   ```
   make
2. Run the game using 
```
./battleship_simulator
```

### Control
#### Movement 
* `W`: Tilt battleship downwards (pitch down). 
* `S`: Tilt battleship upwards (pitch up). 
* `A`: Turn battleship left (yaw left). 
* `D`: Turn battleship right (yaw right). 
* Arrow keys: 
    * `Up`: Move battleship forward. 
    * `Down`: Move battleship backward. 
    * `Left`: Move battleship left. 
    * `Right`: Move battleship right.
* `F`: Fire bullets (Press once to fire once, hold for continuous firing)
* `R`: Reset game
* `ESC`: Exit

## Features Pending
1. **Celestial Bodies**
   - Addition of more planets, nebulas, and other celestial objects to enhance the visual environment.

2. **Enemy Spaceships**
   - Automated enemy spaceships that will:
     - Navigate the environment.
     - Shoot at the player's battleship, adding challenge and interaction.


## Code references

1. In SetupCamera code in battleship.c, AI was used to arrive at the conclusion for lines:

   `float shipRad = -shipState.yaw * M_PI / 180.0f;`
   `float pitchRad = shipState.pitch * M_PI / 180.0f;` since I was not able to get the reverse movement of the environment around the ship to work.

2. updateUFOs has the random movement of the UFO which was taken and modified from AI to add a special random effect.

3. Most knowledge was taken from https://learnopengl.com/

4. The shapes utiltiy functions in utils.c and the LoadBMP function in utils.c was taken from the assignments we did in class.

5. The planets code was referred to but not copied from CSCIx229 Ex. 19

