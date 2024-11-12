
# T-Display S3 Tetris

A Tetris game implementation for the T-Display S3 device using the PlatformIO environment and the ESP32-S3 microcontroller. This project utilizes the TFT_eSPI library to handle display rendering and features an interactive Tetris experience.

## Table of Contents

- [Screenshot](#screenshot)
- [Description](#description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building and Uploading](#building-and-uploading)
- [Folder Structure](#folder-structure)
- [Contributing](#contributing)
- [License](#license)

## Description

This project implements a classic Tetris game on the LilyGO T-Display S3 device. It leverages the ESP32-S3's processing capabilities and the TFT_eSPI library for graphics display. This is a hobby project aimed at demonstrating game development and graphics rendering capabilities for embedded devices, with the main goal being to have fun and spark interest in creating engaging projects on microcontrollers.

## Screenshot

![Screenshot of the Project](screenshot.png)

## Features

- Tetris game logic and block handling
- Interactive graphical interface using the TFT_eSPI library
- Multiple Tetris shapes and their rotation mechanisms
- Collision detection and line clearing logic
- User input handling (buttons or touch interactions)
- Highscores
- Navigationbar

## Prerequisites

- [PlatformIO IDE](https://platformio.org/) (recommended for building and uploading)
- [ESP32 Board Support](https://docs.platformio.org/en/latest/boards/espressif32/index.html)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [LilyGO T-Display S3](https://www.lilygo.cc/products/t-display-s3)

### Note on ESP Compatibility

This project is compatible with any ESP device that supports the TFT_eSPI library, making it flexible for other displays and ESP boards. Simply adjust the display's dimensions.

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/zamweis/t-display-s3-tetris.git
   cd t-display-s3-tetris
   ```

2. Install the necessary dependencies using PlatformIO:

   ```bash
   pio lib install
   ```

3. Ensure that your board is correctly configured in `platformio.ini`.

### Configuring the TFT_eSPI Library

To use the TFT display correctly, you need to configure the `User_Setup.h` file in the TFT_eSPI library:

- Uncomment the appropriate lines that correspond to your display model and connection setup in `User_Setup.h`.
- For example, if using a parallel connection or a specific display model, make sure that the configuration matches your hardware setup.

Refer to the [TFT_eSPI documentation](https://github.com/Bodmer/TFT_eSPI) for more details on configuring the display settings.

## Building and Uploading

1. Open the project folder in PlatformIO or VS Code with the PlatformIO extension installed.
2. Connect your T-Display S3 device via USB.
3. To build and upload the firmware to your device:

   ```bash
   pio run --target upload
   ```

## Folder Structure

```
.
├── src/
│   ├── Config.cpp              # Configuration source file.
│   ├── Config.h                # Configuration constants and macros.
│   ├── main.cpp                # Entry point for the application.
│   ├── Display/
│   │   ├── DisplayManager.cpp  # Implementation of display management.
│   │   └── DisplayManager.h    # Header for display-related functionalities.
│   ├── Game/
│   │   ├── BlockMap.cpp        # Game grid management implementation.
│   │   ├── BlockMap.h          # Game grid management header.
│   │   ├── Game.cpp            # Main game logic implementation.
│   │   ├── Game.h              # Header for core game logic.
│   │   ├── ShapeFactory.cpp    # Shape creation implementation.
│   │   └── ShapeFactory.h      # Header for shape creation.
│   ├── HighScore/
│   │   ├── HighScoreManager.cpp # High score management implementation.
│   │   └── HighScoreManager.h   # Header for high score management.
│   ├── Input/
│   │   ├── InputHandler.cpp    # Implementation of input handling.
│   │   └── InputHandler.h      # Header for input handling.
│   └── Model/
│       ├── Block.cpp           # Block model implementation.
│       ├── Block.h             # Header for block model.
│       ├── Point.cpp           # Point model implementation.
│       ├── Point.h             # Header for point model.
│       └── Shapes/
│           ├── Shape.cpp       # Base shape implementation.
│           ├── Shape.h         # Header for base shape.
│           ├── ShapeI.cpp      # Specific shape implementation (I).
│           ├── ShapeI.h        # Header for shape (I).
│           ├── ShapeJ.cpp      # Specific shape implementation (J).
│           ├── ShapeJ.h        # Header for shape (J).
│           ├── ShapeL.cpp      # Specific shape implementation (L).
│           ├── ShapeL.h        # Header for shape (L).
│           ├── ShapeO.cpp      # Specific shape implementation (O).
│           ├── ShapeO.h        # Header for shape (O).
│           ├── ShapeS.cpp      # Specific shape implementation (S).
│           ├── ShapeS.h        # Header for shape (S).
│           ├── ShapeT.cpp      # Specific shape implementation (T).
│           ├── ShapeT.h        # Header for shape (T).
│           ├── ShapeZ.cpp      # Specific shape implementation (Z).
│           └── ShapeZ.h        # Header for shape (Z).
```

## Contributing

Contributions are welcome! Please fork the repository and create a pull request with your changes. Make sure to follow coding best practices and add comments where applicable.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
