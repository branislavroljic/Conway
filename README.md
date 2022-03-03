# Game-of-Life

![Alt Text](https://lh3.googleusercontent.com/C6HkzTZOrAtlLPkY6tHcUQMX1BoahTG_Gt4ueO_G0dV-J6dqSbT7ElD6Ddg_vg2cNI1D9cIBQMUNaPWIkPrqGVpbE9RY_9Q3Fn0k)

* Conway's Game of Life implemented in OpenCL using C and C++

* When launching the application, the user can enter the dimensions of the "world", ie. matrix, as well as to select the number of kernel launches in which the image processing will be performed.
* When initializing the matrix, the user has two options:
  * Enter the coordinates of the points
  * Enter the coordinates of the image, and the path to the image (the image must be in .pgm format)

* After each iteration, the user selects one of the options:
  * Next iteration + detection of repeating patterns
  * Next iteration
  * Isolation of a sub-segment (the image is saved as RGB, where the patterns that are repeated in that sub-segment are colored green)
  * Jumping to an arbitrary iteration 
