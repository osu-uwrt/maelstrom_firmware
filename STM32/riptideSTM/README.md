# Notes for this folder:
1. Leave this as a top level folder with /Src, /Inc, /Middlewares and such
2. This is setup for the STM32F405
3. In main.c you need to add these lines:
    #include "riptideMain.h"        -in /* USER CODE BEGIN Includes */

    riptideMain();                  -in /* USER CODE END 2 */

## Compiling Notes
### For Makefiles:
1. for the C_Includes ection add this line:
    -IriptideSTM/inc

2. for the c_Sources section add these lines:
    riptideSTM/src/riptideFunctions.c \
    riptideSTM/src/riptideMain.c \
    riptideSTM/src/riptideThreads.c
