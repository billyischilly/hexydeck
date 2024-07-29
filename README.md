# The Hexydeck

![Hexydeck Image](https://github.com/billyischilly/hexydeck/blob/main/Build%20Pictures/IMG_5344%20Large.jpeg)

The Hexydeck is a multiplayer groovebox, designed with a mind to getting young neurodivergent children engaging with each other and with their music teachers. It was commissioned by Drake Music as part of their Tiny Techies research project, exploring the use of accessible music tech in early years settings, and developed in response to a series of music sessions at Newlands School in Rochdale.

The primary aim of the instrument was to encourage engagement. Although some of the children at Newlands clearly enjoyed and actively participated in the music sessions, all were non-verbal and many were reluctant to join in with the music-making, preferring instead to explore the soft-play and sensory toys in the room. The instrument needed to be able to compete with these distractions and so was designed to be big and attention grabbing, robust enough to survive being climbed on, and to reward exploration and engagement.

A big part of the inspiration for the instrument was the Dato Duo, an instrument that came out a few years ago which allowed two people to sit opposite each other and operate a synth cooperatively. The Hexydeck aims to take this idea further by allowing for up to six people to play the instrument at once, each side of the hexagon having its own method of musical interaction and sound, and some opposing sides needing cooperation to fully function.

## Features

1. **Touch Harp** - Lots of small touch keys that make a sound like a plucked string and trigger twinkly lights.
2. **Big Hand and Small Hands** – Completing a circuit between any of the small hands and the big hand through human touch will play a big bass synth.
3. **Drum Machine** – One side allows you to change the drum patterns using a knob for each drum (Kick, Snare, Hi Hat). The other allows you to control the volume of each drum, start and stop the beat, control the tempo, and control a low pass, DJ style filter with an ultrasonic sensor.
4. **Singing Robots** – Waving your hand over the robots’ eyes (ultrasonic sensors) creates a synthesized robot singing voice. Going closer and further away varies the formant (vowel sound) and vibrato of the singer.

## Constructing the Enclosure

The Hexydeck uses a Teensy 4.1 along with a Teensy Audio board to generate all the sound. I would recommend having a pretty good knowledge of the Arduino platform to take on this project. Hopefully this guide will lead you through a problem-free build, but my experience building my second Hexydeck was that it still required a lot of troubleshooting, even following the familiar build process of the first, so it helps to know your way around. There is also a LOT of wiring and soldering, some of which is very zen and meditative, other bits are fiddly and annoying so first step is to grab a coffee, find an artist/podcast with a large back catalogue and tell your loved ones you’ll miss them!

### Tools You Will Need

- Laser Cutter (capable of cutting 600mm x 900mm)
- Mitre Saw
- Soldering Iron
- Drill
- Wood drill bits
- 40mm Forster/Spade bit
- Screwdriver
- Hammer
- Hot glue gun

### Parts

- Teensy 4.1
- Teensy Audio Board (Rev D)
- Brass split pins/Paper fasteners
- 3 x 800mm x 600mm Laser Ply
- 2.4m x 700mm x 18mm PAR Wood
- 5 x RCWL 1601 Ultrasonic Sensor
- 4 x MPR121 Capacitive Touch Board
- 3 x Arcade Buttons
- 50 x WS2811 Addressable Pixel Light String (Non-Waterproof)
- Panel Mount USB socket to Micro USB
- Wire
- Dupont wires
- Brass Foil 200mm Roll
- 7 x 10k Linear Potentiometers
- Cool Knobs
- Male and female headers
- Stackable Headers
- PAM8403 Amplifier Board (optional)
- 3” Full Range Speaker (optional)
- An Old Leather Belt (optional)
- Teensy 4.1 breakout board

### Step-by-Step Instructions for Constructing the Hexydeck Enclosure

#### Instructions

1. **Cutting the Panels:**
   - Cut the 3 sheets of plywood using the provided design files. You should end up with:
     - 3 hexagonal panels.
     - 1 small circular panel for mounting the USB and audio sockets.

2. **Cutting the Sides:**
   - Use a mitre saw to cut the sides of the hexagon from the timber plank.
   - Ensure the cuts match the lengths of the hexagon's sides.
   - Set the mitre saw to a 60-degree angle to achieve the correct bevel.

3. **Creating Touch Sensors:**
   - The top layer of the enclosure has two layers to house the touch sensors.
   - Sandwich the brass foil between the two sheets underneath the hand cutouts.
   - Ensure the brass foil pieces do not touch each other to prevent short circuits.
   - Stick the brass foil to the bottom sheet using PVA glue and smooth out any air bubbles.

4. **Assembling the Top Layers:**
   - Carefully line up the top and bottom layers of the top panel ensuring the laser etched details (robot faces, etc.) are on the outside.
   - Stick them together using PVA glue.
   - Use clamps to hold the layers together. Alternatively, start adding hardware components (potentiometers, brass split pins, arcade buttons) to help secure the panels.

5. **Constructing the Hexagonal Frame:**
   - Attach the hexagonal frame to the sides of the bottom panel using a combination of glue and pins.
   - Ensure good contact between the hexagon sides for a strong bond.
   - Use pins hammered flush to secure the sides. Brass pins can be used to match the aesthetics of the Hexydeck.
   - Use G clamps to hold everything in place while the glue dries.
   - Leave the glue to dry completely before proceeding.

6. **Attaching the Bottom Panel:**
   - Screw the final sheet to the bottom of the hexagon. This allows for easy removal for repairs.
   - You can attach this now or after completing the wiring.
   - Screwing the bottom panel during the gluing process can add mechanical stability.

7. **Mounting the USB and Audio Sockets:**
   - Use a Forstner/spade bit to drill a hole in one of the hexagon sides (recommended near the big hand).
   - Attach the panel mount USB port to the small circular plywood panel.
   - Mount this panel over the drilled hole.
   - Ensure there is enough space for the USB port, a potentiometer, and an audio jack.
   - Use small screws or brass pins to secure the panel.

8. **Adding a Handle:**
   - Optional: Use a spare leather belt as a handle for portability.
   - Cut a 25-30cm length of the belt.
   - Screw the belt to the side of the hexagon with the robot eyes to create a handle.

Your Hexydeck enclosure is now complete! Follow the wiring instructions separately to finish your build.

## Wiring Diagram 



### Notes On Wiring

I have provided a wiring diagram. I have left it up to your preference how to approach this – whether with veroboard, a breadboard or even point-to-point. There is a breakout board available for the Teensy 4.1 which breaks out all the pins as screw terminals which gives a nice balance of providing a strong, stable connection but allowing you to reverse any wiring mistakes. Most of the wiring is fairly simple going directly from the sensors to pins on the Teensy but the hand sensors will need high value resistors. I used 500k resistors but you may find that going higher (to 1M and above) may be a bit more sensitive and thus more effective.

In the diagram, I have put one electrolytic capacitor on the 5V and ground lines close to the LED strip. This helps deal with noise on the ground which can leak into your audio signal. In reality, placing lots of high-value capacitors at different points around the light strip will help to further reduce ground noise and is probably a good idea. These provide an emergency power source for when the light tries to pull too much power and thus reduces noise getting into other parts of the circuit.

I have not included all the paper fastener touch sensors in the wiring diagram. I would recommend attaching these with nice, long Dupont connectors. Remove a connector at one end and solder this end to one of the paper fasteners’ legs. Attach the other end to the MPR121, plugging the Dupont connector into the header. There are touch sensors in total so this will take some time and it is worth being neat as it can get complicated pretty fast. Using cable ties to neaten up your wiring wouldn’t be a bad idea.

You will need to attach the Teensy Audio Board to your Teensy 4.1. The best way of doing this is to put stackable female headers on the Teensy, and regular male headers on the Teensy Audio Board. It should then just slot in on top (although pay close attention to the orientation).

The Audio Board will need some additional wiring to breakout the headphone output and volume control to the panel on the side of the Hexydeck. Connect the three pins marked GND, VOL, 3.3V to a potentiometer to control the volume. For the headphone output, I took a normal aux cable and chopped off one end, and soldered the wires to a 3.5mm panel mount jack socket. The jack can be inserted into the headphone output of the Teensy Audio Board and the socket can be mounted
