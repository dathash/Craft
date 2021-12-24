Graphics Final
Alex Hartford
Professor Wood
Fall 2021

To compile and run the project, input the following commands from the root of the project dir.:

mkdir build
cd build
cmake ..
make
./Craft

Instructions
WASD to move
hold Shift to move quicker
Scroll to move view
G activates Bezier Curve mode

------------------------------------------
The project involves the following aspects:
Blinn-Phong lighting with distance attenuation
Texture mapped meshes
Particle system
Basic collision detection
Bezier Curve

------------------------------------------
Planned, but incomplete
Text Rendering
   I spent a significant amount of time trying to understand
   how CMake resolves included packages and links libraries,
   but I had no success with installing any.
   Instead, I decided to simply paste text onto a jpg image,
   treat the image as a texture, and apply the texture to the screen, 
   like a FBO. This method was working in isolation, but I ended up
   with shader bugs that I had no method of resolving, so I scrapped the idea
   after a long time fiddling with things I really don't understand.
   Given more time I'd like to try these things again, because they seem
   within my reach, but I honestly have no idea why my program breaks and
   have had middling success finding help online.
IrrKlang sound
   I had the same issue with text rendering, just had no luck
   with installing outside libraries and spent most of my project
   time trying to learn about how the linker and cmake work.
   Pretty interesting stuff, but it didn't help with the results of my
   project.
