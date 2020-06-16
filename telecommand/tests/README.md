# Unit testing the telecommand code
In this folder you can add unit tests. These unit tests are executed on the host PC which allows for testing large parts of the code base without target hardware. For testing we use a framework called CMOCKA whose source is already included in the project.

## Build setup
To add your test add the name of the test file in the CMakeLists.txt file. Then to build the tests go inside the test directory run:
`mkdir build`
`cd build`
`cmake ../`
`make`

## Running tests
Inside your build builder either run the executable of your test or run:
`ctest` 
