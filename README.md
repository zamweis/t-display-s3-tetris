# T-Display S3 Tetris

A Tetris game implementation for the T-Display S3 device using the PlatformIO environment and the ESP32-S3 microcontroller. This project utilizes the TFT_eSPI library to handle display rendering and features an interactive Tetris experience.

## Table of Contents

- [Description](#description)
- [Screenshot](#screenshot)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building and Uploading](#building-and-uploading)
- [Folder Structure](#folder-structure)
- [Contributing](#contributing)
- [Security](#security)
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
- Navigation bar

## Prerequisites

- [PlatformIO IDE](https://platformio.org/) (recommended for building and uploading)
- [ESP32 Board Support](https://docs.platformio.org/en/latest/boards/espressif32/index.html)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [LilyGO T-Display S3](https://www.lilygo.cc/products/t-display-s3)

### Note on ESP Compatibility

This project is compatible with any ESP device that supports the TFT_eSPI library, making it flexible for other displays and ESP boards. Simply adjust the display's dimensions.

## Installation

1. Clone the repository:

   \`\`\`bash
   git clone https://github.com/zamweis/t-display-s3-tetris.git
   cd t-display-s3-tetris
   \`\`\`

2. Install the necessary dependencies using PlatformIO:

   \`\`\`bash
   pio lib install
   \`\`\`

3. Ensure that your board is correctly configured in \`platformio.ini\`.

### Configuring the TFT_eSPI Library

To use the TFT display correctly, you need to configure the \`User_Setup.h\` file in the TFT_eSPI library:

- Uncomment the appropriate lines that correspond to your display model and connection setup in \`User_Setup.h\`.
- For example, if using a parallel connection or a specific display model, make sure that the configuration matches your hardware setup.

Refer to the [TFT_eSPI documentation](https://github.com/Bodmer/TFT_eSPI) for more details on configuring the display settings.

## Building and Uploading

1. Open the project folder in PlatformIO or VS Code with the PlatformIO extension installed.
2. Connect your T-Display S3 device via USB.
3. To build and upload the firmware to your device:

   \`\`\`bash
   pio run --target upload
   \`\`\`

## Folder Structure

\`\`\`
.
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ Config.cpp              # Configuration source file.
â”‚   â”œâ”€â”€ Config.h                # Configuration constants and macros.
â”‚   â”œâ”€â”€ main.cpp                # Entry point for the application.
â”‚   â”œâ”€â”€ Display/
â”‚   â”‚   â”œâ”€â”€ DisplayManager.cpp  # Implementation of display management.
â”‚   â”‚   â””â”€â”€ DisplayManager.h    # Header for display-related functionalities.
â”‚   â”œâ”€â”€ Game/
â”‚   â”‚   â”œâ”€â”€ BlockMap.cpp        # Game grid management implementation.
â”‚   â”‚   â”œâ”€â”€ BlockMap.h          # Game grid management header.
â”‚   â”‚   â”œâ”€â”€ Game.cpp            # Main game logic implementation.
â”‚   â”‚   â”œâ”€â”€ Game.h              # Header for core game logic.
â”‚   â”‚   â”œâ”€â”€ ShapeFactory.cpp    # Shape creation implementation.
â”‚   â”‚   â””â”€â”€ ShapeFactory.h      # Header for shape creation.
â”‚   â”œâ”€â”€ HighScore/
â”‚   â”‚   â”œâ”€â”€ HighScoreManager.cpp # High score management implementation.
â”‚   â”‚   â””â”€â”€ HighScoreManager.h   # Header for high score management.
â”‚   â”œâ”€â”€ Input/
â”‚   â”‚   â”œâ”€â”€ InputHandler.cpp    # Implementation of input handling.
â”‚   â”‚   â””â”€â”€ InputHandler.h      # Header for input handling.
â”‚   â””â”€â”€ Model/
â”‚       â”œâ”€â”€ Block.cpp           # Block model implementation.
â”‚       â”œâ”€â”€ Block.h             # Header for block model.
â”‚       â”œâ”€â”€ Point.cpp           # Point model implementation.
â”‚       â”œâ”€â”€ Point.h             # Header for point model.
â”‚       â””â”€â”€ Shapes/
â”‚           â”œâ”€â”€ Shape.cpp       # Base shape implementation.
â”‚           â”œâ”€â”€ Shape.h         # Header for base shape.
â”‚           â”œâ”€â”€ ShapeI.cpp      # Specific shape implementation (I).
â”‚           â”œâ”€â”€ ShapeI.h        # Header for shape (I).
â”‚           â”œâ”€â”€ ShapeJ.cpp      # Specific shape implementation (J).
â”‚           â”œâ”€â”€ ShapeJ.h        # Header for shape (J).
â”‚           â”œâ”€â”€ ShapeL.cpp      # Specific shape implementation (L).
â”‚           â”œâ”€â”€ ShapeL.h        # Header for shape (L).
â”‚           â”œâ”€â”€ ShapeO.cpp      # Specific shape implementation (O).
â”‚           â”œâ”€â”€ ShapeO.h        # Header for shape (O).
â”‚           â”œâ”€â”€ ShapeS.cpp      # Specific shape implementation (S).
â”‚           â”œâ”€â”€ ShapeS.h        # Header for shape (S).
â”‚           â”œâ”€â”€ ShapeT.cpp      # Specific shape implementation (T).
â”‚           â”œâ”€â”€ ShapeT.h        # Header for shape (T).
â”‚           â”œâ”€â”€ ShapeZ.cpp      # Specific shape implementation (Z).
â”‚           â””â”€â”€ ShapeZ.h        # Header for shape (Z).
â”œâ”€â”€ CONTRIBUTING.md             # Contribution guidelines
â”œâ”€â”€ SECURITY.md                 # Security policy and reporting guidelines
\`\`\`

## Contributing

Contributions are welcome! Please see the [CONTRIBUTING.md](./CONTRIBUTING.md) file for guidelines on how to get started.

## Security

For information on reporting security vulnerabilities, please see the [SECURITY.md](./SECURITY.md) file.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
