<h3>What is this?</h3>

This is currently a small engine that is being pieced together via the usage of OpenGL with a handful of simple algorithms. A recent decisions has been made to rewrite the entire engine due to the lack of code standardization and maintainability.

The language of C has been chosen for its simplicity and enjoyable usage.

<h3>What it do?</h3>

The current engine is basically a simple chunk loader that uses a fragment shader to render a tile-based world that generates at runtime using a custom chunk generation and management algorithm. Originally, this engine operated using another graphics library called MiniFB, but it was quickly realized that even something as simple as this requires additional hardware acceleration.

In the future, it is planned to turn this from an engine into an interesting experience in the form of a game. Many ideas are floating about undocumented, and so they will be written shortly.