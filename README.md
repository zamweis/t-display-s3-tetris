
# T-Display S3 Tetris

A Tetris game implementation for the T-Display S3 device using the PlatformIO environment and the ESP32-S3 microcontroller. This project utilizes the TFT_eSPI library to handle display rendering and features an interactive Tetris experience.

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building and Uploading](#building-and-uploading)
- [Folder Structure](#folder-structure)
- [Contributing](#contributing)
- [License](#license)

## Description

This project implements a classic Tetris game on the LilyGO T-Display S3 device. It leverages the ESP32-S3's processing capabilities and the TFT_eSPI library for graphics display. This is a hobby project aimed at demonstrating game development and graphics rendering capabilities for embedded devices.

## Features

- Tetris game logic and block handling
- Interactive graphical interface using the TFT_eSPI library
- Multiple Tetris shapes and their rotation mechanisms
- Collision detection and line clearing logic
- User input handling (buttons or touch interactions)

## Prerequisites

- [PlatformIO IDE](https://platformio.org/) (recommended for building and uploading)
- [ESP32 Board Support](https://docs.platformio.org/en/latest/boards/espressif32/index.html)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [LilyGO T-Display S3](https://www.lilygo.cc/products/t-display-s3)

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

## Building and Uploading

1. Open the project folder in PlatformIO or VS Code with the PlatformIO extension installed.
2. Connect your T-Display S3 device via USB.
3. To build and upload the firmware to your device:

   \`\`\`bash
   pio run --target upload
   \`\`\`

## Folder Structure

\`\`\`
├── include             # Header files
├── lib                 # External libraries
├── src                 # Source files
│   ├── main.cpp        # Main entry point
│   ├── Shapes          # Shape-related classes
│   ├── Block.cpp       # Block logic
│   └── BlockMap.cpp    # Block map handling
├── platformio.ini      # PlatformIO project configuration
└── README.md           # Project documentation
\`\`\`

## Contributing

Contributions are welcome! Please fork the repository and create a pull request with your changes. Make sure to follow coding best practices and add comments where applicable.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
