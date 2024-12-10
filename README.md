# Battleship Mania - README

## Description
**Battleship Mania** is a 3D simulation game built using OpenGL. Players control a battleship navigating through a visually immersive starfield while avoiding obstacles and enemy fire. The project demonstrates mastery of core computer graphics concepts while extending beyond course material with advanced features like collision detection, physics-based animations and complex 3D model generation. The game currently features:

- A battleship with realistic movement physics
- Shooting mechanics
- Collision detection for the battleship
- Collision detection for bullets and asteroids
- Collision detection for bullets and UFOs
- Multiple UFOs with a futuristic design attacking the battleship at random
- Health system for the battleship and asteroids
- Dynamic star field
- An asteroid belt with destructible asteroids
- A comet with particle effects
- A space station orbiting the Earth
- Lighting effects

## Features Implemented
1. **Battleship Mechanics**
   - Battleship designed with a fighter jet-like appearance, featuring a grey and red color scheme.
   - Fire effects from the thrusters for added realism.
   - Real life movements for the battleship to enable movement in all directions.

2. **Starfield**
   - A dynamic starfield visible in all directions.
   - Stars change positions relative to the battleship's movements.
   - Continuous introduction of new stars as old ones move out of view.

3. **Camera**
   - Third-person perspective centered directly behind the battleship.
   - The entire scenery changes around the battleship to simulate realistic movement of the battleship itself. 

4. **Movement and Combat**
   - Tilt and directional controls for realistic battleship movement.
   - Ability to fire bullets at enemies or obstacles.
   - Hit an asteroid three times to make it explode.
   - Hit a UFO three times to make it explode.
   - The UFOs follow a random path and hit the battleship randomly as well reducing the health.
   
5. **Collision detection and Ship Health**
    - The ship starts with 5 health points and it reduces every time it crashes into a celestial body.
    - Crashing into asteroids, and UFOs reduces 1 health point.
    - Crashing into planets and the sun causes instant destruction.
    - Crashing into comet reduces 3 health points.
    - Crashing into the space station reduces 2 health points.

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

## Why I deserve an A

- A futuristic looking battleship resembling an F22 took a long time to design and implement and is done completely using manually created shapes instead of GLUT Objects to enable texture mapping.
- Collision detection of the battleship with asteroids, and UFOs. Collision detection of bullets with asteroids and UFOs.
- Dynamic starfield. My scene is never ending. New stars are continuously created as the old stars move out of sight to simulate the never endingness of space.
- Complex design for asteroids. The asteroid design has dynamic roughness on the surface to make each asteroid look a little different from the others. The craters have a shadow effect when the Sun's light falls on the asteroid.
- Complex design of the space station orbiting around the Earth, with solar panels and docking stations (Cannot dock the battleship on this though).
- Particle effect for the comet tail. Please go near the comet to see the beautiful tail. 


## Code references

1. Most knowledge was taken from the code examples in https://learnopengl.com/

2. The shapes utiltiy functions in utils.c and the LoadBMP function in utils.c was taken from the assignments we did in class.

3. The planets code was referred to but not copied from CSCIx229 Ex. 19

