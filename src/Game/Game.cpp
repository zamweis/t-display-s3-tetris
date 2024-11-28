#include "Game/Game.h"
#include "Config.h"
#include "TetrisAI/TetrisAI.h"
#include "ShapeFactory.h"

TetrisAI::Move currentMove;
bool isGravityActive = false;
bool directionChosen = false;


Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE),
      tetrisAI() {} // Initialize TetrisAI instance

void Game::displayStartScreenLoop() {
    tft.fillScreen(TFT_BLACK);
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
    highScoreManager.loadHighScores();

    while (true) {
        if (digitalRead(BUTTON_LEFT) == LOW) {
            handleHighScoreDisplay();
        } else if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            break; // Exit loop and start the game
        }
    }
}

void Game::handleHighScoreDisplay() {
    inputHandler.waitForButtonRelease(BUTTON_LEFT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    highScoreManager.displayHighScores(tft);
    displayManager.displayNavigation("     ", "Back");
    inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for a button click to return to start screen
    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
}

void Game::setup() {
    //displayStartScreenLoop();
    resetGame(); // Only reset and start the game after button press
}

void Game::loop() {
    unsigned long currentTime = millis();

    if (blockMap.checkGameOver()) {
        Serial.println("Game Over detected.");
        handleGameOver();
        return;
    }

    if (!shape) {
        Serial.println("Creating new shape...");
        createNewShape();
    } else {
        Serial.println("Executing AI step...");
        if (!executeAIStep()) {
            Serial.println("AI step not completed, updating shape position...");
            updateShapePosition(currentTime);
        } else {
            Serial.println("AI step completed.");
        }
    }

    blockMap.drawAllBlocks(tft);
    Serial.println("Blocks drawn.");
}


void Game::createNewShape() {
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        if (!shape->canMoveToPosition(shape->getBlock(0).getX(), shape->getBlock(0).getY(), blockMap)) {
            delete shape;
            shape = nullptr;
            return;
        }

        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft);
    } else {
        Serial.println("Error: Shape could not be created.");
    }
}

void Game::resetGame() {
    score = 0;
    level = 1;
    linesCleared = 0;
    delete shape;
    shape = nullptr;
    blockMap = BlockMap();
    displayManager.drawGrid();
    createNewShape();
}

void Game::handleGameOver() {
    displayManager.drawScreen();
    displayManager.displayGameOverScreen(score);
    highScoreManager.loadHighScores();

    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    inputHandler.waitForButtonClick(BUTTON_RIGHT);

    if (highScoreManager.isHighScore(score)) {
        handleHighScoreEntry();
    }

    displayStartScreenLoop();
    resetGame(); // Only reset and start the game after button press
}

void Game::handleHighScoreEntry() {
    displayManager.clearScreen();
    displayManager.drawScreen();
    if (displayManager.promptPlayerForName()) {
        highScoreManager.updateHighScores(score, displayManager.getPlayerName());
        displayManager.resetNameEntry();
        inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    }
}

void Game::updateScoreAndLevel(int clearedLines) {
    const int pointsPerLine[4] = {40, 100, 300, 1200};
    if (clearedLines > 0 && clearedLines <= 4) {
        score += pointsPerLine[clearedLines - 1] * (level + 1);
        linesCleared += clearedLines;
        level = 1 + linesCleared / 10;
        //Serial.printf("Score: %d, Level: %d\n", score, level);
    }
}

void Game::handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &)) {
    switch (state) {
        case IDLE:
            if (inputHandler.isButtonPressed(buttonPin)) {
                state = PRESSED;
                inputHandler.startButtonPressTimer(currentTime);
            }
            break;
        case PRESSED:
            if (inputHandler.isButtonReleased(buttonPin)) {
                state = (currentTime - inputHandler.getButtonPressStart() < rotationHoldThreshold) ? SHORT_PRESS : IDLE;
            } else if (currentTime - inputHandler.getButtonPressStart() >= rotationHoldThreshold) {
                state = LONG_PRESS;
            }
            break;
        case SHORT_PRESS:
            if (shape) {
                shape->eraseShape(tft, displayManager.getBackgroundColor());
                (shape->*moveFunc)(blockMap);
                shape->drawShape(tft);
            }
            state = IDLE;
            break;
        case LONG_PRESS:
            if (inputHandler.isButtonReleased(buttonPin)) {
                state = IDLE;
            } else {
                if (currentTime - inputHandler.getLastRotateTime() >= rotationDebounceInterval) {
                    if (shape) {
                        shape->eraseShape(tft, displayManager.getBackgroundColor());
                        (shape->*rotateFunc)(blockMap);
                        shape->drawShape(tft);
                        inputHandler.updateLastRotateTime(currentTime);
                    }
                }
            }
            break;
    }
}

bool Game::executeAIStep() {
    if (!shape) {
        // No active shape to operate on
        return false;
    }

    // Align the shape's rotation
    if (shape->getRotatePosition() != currentMove.rotation &&
        shape->canRotateToPosition(currentMove.rotation, blockMap, currentMove.rotateClockwise)) {
        shape->eraseShape(tft, displayManager.getBackgroundColor());

        if (currentMove.rotateClockwise) {
            shape->rotateClockwise(blockMap);
        } else {
            shape->rotateAntiClockwise(blockMap);
        }

        shape->drawShape(tft);
        return true; // Continue aligning rotation in the next loop iteration
    }

    // Move horizontally to the target column
    int currentX = shape->getBlock(0).getX();

    // Check if the shape can move horizontally before attempting the move
    if (currentX != currentMove.x) {
        shape->eraseShape(tft, displayManager.getBackgroundColor());

        if (currentX < currentMove.x) {
            // Attempt to move right
            if (shape->isMovableToTheRight(blockMap)) {
                shape->moveRight();
            } else {
                // Recalculate a new best move if moving right is not possible
                if (!calculateBestMove()) {
                    Serial.println("Debug: Failed to calculate a new best move.");
                    return false;
                }
                return true;
            }
        } else {
            // Attempt to move left
            if (shape->isMovableToTheLeft(blockMap)) {
                shape->moveLeft();
            } else {
                // Recalculate a new best move if moving left is not possible
                if (!calculateBestMove()) {
                    Serial.println("Debug: Failed to calculate a new best move.");
                    return false;
                }
                return true;
            }
        }

        shape->drawShape(tft);
        return true; // Continue horizontal movement in the next loop iteration
    }

    // Drop the shape one block down if possible
    if (shape->isMovableDownWards(blockMap)) {
        shape->eraseShape(tft, displayManager.getBackgroundColor());
        shape->moveDown();
        shape->drawShape(tft);

        // Recalculate the best move after moving down
        if (!calculateBestMove()) {
            Serial.println("Debug: Failed to recalculate the best move.");
            return false;
        }

        return true; // Wait for the next loop iteration
    }

    // Finalize the shape's placement
    finalizeShapePlacement();
    delete shape;
    shape = nullptr;
    currentMove = {0, 0, std::numeric_limits<double>::lowest(), true}; // Reset for the next shape
    return true;
}

bool Game::calculateBestMove() {
    currentMove = tetrisAI.findBestMove(blockMap, *shape);

    if (currentMove.score == std::numeric_limits<int>::min()) {
        //Serial.println("AI could not find a valid move.");
        return false;
    }

    //Serial.printf("AI Best Move Calculated: Target X=%d, Rotation=%d, Score=%.2f, RotateClockwise=%s\n",
    //            currentMove.x, currentMove.rotation, currentMove.score,
    //            currentMove.rotateClockwise ? "true" : "false");

    return true;
}

bool Game::alignShapeRotation() {
    static bool rotateClockwise;  // Persist chosen rotation direction
    static int failedAttempts = 0; // Track failed attempts for fail-safe
    const int MAX_ROTATION_ATTEMPTS = 4; // Maximum retries for rotation

    int currentRotation = shape->getRotatePosition();
    int targetRotation = currentMove.rotation;

    // Skip further alignment if already aligned
    if (currentRotation == targetRotation) {
        //Serial.println("AI: Rotation alignment complete.");
        directionChosen = false;  // Reset direction
        failedAttempts = 0;  // Reset attempt counter
        return true;
    }

    // Decide rotation direction if not chosen yet
    if (!directionChosen) {
        int clockwiseSteps = (targetRotation - currentRotation + 4) % 4;
        int counterClockwiseSteps = (currentRotation - targetRotation + 4) % 4;
        rotateClockwise = clockwiseSteps <= counterClockwiseSteps;
        directionChosen = true;  // Lock direction choice
        //Serial.printf("AI: Chosen rotation direction: %s\n", rotateClockwise ? "Clockwise" : "Anti-clockwise");
    }

    // Abort alignment if too many failed attempts
    if (++failedAttempts >= MAX_ROTATION_ATTEMPTS) {
        //Serial.println("AI: Rotation alignment failed after multiple attempts.");
        directionChosen = false;  // Reset for next alignment
        failedAttempts = 0;
        return false;
    }

    // Attempt rotation
    shape->eraseShape(tft, displayManager.getBackgroundColor());
    if (rotateClockwise) {
        if (shape->isRotatableClockwise(blockMap)) {
            shape->rotateClockwise(blockMap);
        } else {
            //Serial.println("AI: Clockwise rotation blocked. Switching to anti-clockwise.");
            rotateClockwise = false;  // Switch direction
        }
    } else {
        if (shape->isRotatableAntiClockwise(blockMap)) {
            shape->rotateAntiClockwise(blockMap);
        } else {
            //Serial.println("AI: Anti-clockwise rotation blocked. Switching to clockwise.");
            rotateClockwise = true;  // Switch direction
        }
    }

    // Redraw shape and debug
    shape->drawShape(tft);
    //Serial.printf("AI: Rotated shape to position %d (Target=%d).\n", shape->getRotatePosition(), targetRotation);

    // Check alignment completion
    if (shape->getRotatePosition() == targetRotation) {
        //Serial.println("AI: Rotation alignment successful.");
        directionChosen = false;
        failedAttempts = 0;
        return true;
    }

    return false;  // Continue alignment
}


bool Game::moveShapeToTargetColumn() {
    if (shape->getRotatePosition() != currentMove.rotation) {
        //Serial.println("AI: Shape rotation mismatch detected. Aborting horizontal movement.");
        return false;
    }

    int shapeX = shape->getBlock(0).getX(); // Main block X position

    if (shapeX != currentMove.x) {
        if (shapeX < currentMove.x && shape->isMovableToTheRight(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveRight();
            shape->drawShape(tft);
            //Serial.printf("AI: Moved shape right to X=%d.\n", shape->getBlock(0).getX());
        } else if (shapeX > currentMove.x && shape->isMovableToTheLeft(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveLeft();
            shape->drawShape(tft);
           // Serial.printf("AI: Moved shape left to X=%d.\n", shape->getBlock(0).getX());
        } else {
            //Serial.printf("AI: Cannot move shape to X=%d. Aborting move.\n", currentMove.x);
            currentMove.score = std::numeric_limits<int>::min(); // Force recalculation
            return false; // Abort movement
        }
        return false; // Wait for the next loop iteration to continue
    }

    return true; // Target column is reached
}

void Game::dropShape() {
    shape->eraseShape(tft, displayManager.getBackgroundColor());
    shape->fallDown(blockMap);
    shape->drawShape(tft);
    finalizeShapePlacement();

   // Serial.println("AI dropped the shape to finalize placement.");

    // Reset shape and AI move for the next turn
    delete shape;
    shape = nullptr;
    currentMove.score = std::numeric_limits<int>::min();
}

void Game::updateShapePosition(unsigned long currentTime) {
    if (currentTime - lastMoveDownTime >= displayManager.getMoveDownSpeed(level)) {
        isGravityActive = true; // Gravitation aktiv
        lastMoveDownTime = currentTime;

        if (shape->isMovableDownWards(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveDown();
            shape->drawShape(tft);
        } else {
            finalizeShapePlacement();
            delete shape;
            shape = nullptr;
            currentMove.score = std::numeric_limits<int>::min();
        }

        isGravityActive = false;
    }
}

void Game::finalizeShapePlacement() {
    blockMap.addBlocks(shape->getBlockList(), Shape::NUM_BLOCKS);
    //blockMap.printBlockMap();

    int clearedLines = blockMap.clearAndMoveAllFullLines(tft, *shape, displayManager.getBackgroundColor());
    if (clearedLines > 0) {
       //Serial.printf("Lines cleared: %d\n", clearedLines);
        updateScoreAndLevel(clearedLines);
    } else {
       // Serial.println("No lines cleared.");
    }
}

void Game::runSimulation(int numSimulations) {
    for (int i = 0; i < numSimulations; i++) {
        if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.equalsIgnoreCase("stop")) {
            //Serial.println("Stopping simulations...");
            break;
        }
    }

        float heightWeight = randomFloat(1, 10);
        float maxHeightWeight = randomFloat(1, 10);
        float holeWeight = randomFloat(1, 10);
        float bumpinessWeight = randomFloat(1, 10);
        float lineClearWeight = randomFloat(1, 10);
        
        tetrisAI.setWeights(heightWeight, maxHeightWeight, holeWeight, bumpinessWeight, lineClearWeight);

        resetGame();
        while (!blockMap.checkGameOver()) {
            loop();
        }

        tetrisAI.printSimulationResults(score, linesCleared);
    }
}

void Game::runSimulationGridSearch() {
    // Updated ranges based on your optimized weights
    float heightWeightRange[] = {-5.0, -4.0, -3.0};      // Penalize high stacks
    float maxHeightWeightRange[] = {-5.0, -4.0, -3.0};      // Penalize high stacks
    float holeWeightRange[] = {-7.0, -6.0, -5.0};        // Penalize holes
    float bumpinessWeightRange[] = {-4.0, -3.0, -2.0};   // Penalize unevenness
    float lineClearWeightRange[] = {9.0, 10.0, 11.0};    // Reward clearing lines

    int numHeightWeights = sizeof(heightWeightRange) / sizeof(heightWeightRange[0]);
    int numMaxHeightWeights = sizeof(maxHeightWeightRange) / sizeof(maxHeightWeightRange[0]);
    int numHoleWeights = sizeof(holeWeightRange) / sizeof(holeWeightRange[0]);
    int numBumpinessWeights = sizeof(bumpinessWeightRange) / sizeof(bumpinessWeightRange[0]);
    int numLineClearWeights = sizeof(lineClearWeightRange) / sizeof(lineClearWeightRange[0]);

    // Iterate over all combinations of weights
    for (int h = 0; h < numHeightWeights; h++) {
        for (int ho = 0; ho < numHoleWeights; ho++) {
            for (int b = 0; b < numBumpinessWeights; b++) {
                for (int l = 0; l < numLineClearWeights; l++) {
                    // Set weights
                    tetrisAI.setWeights(
                        heightWeightRange[h],
                        1,
                        holeWeightRange[ho],
                        bumpinessWeightRange[b],
                        lineClearWeightRange[l]
                    );

                    // Reset game
                    resetGame();

                    // Run game until game over
                    while (!blockMap.checkGameOver()) {
                        loop();
                    }

                    // Print results
                    tetrisAI.printSimulationResults(score, linesCleared);
                }
            }
        }
    }

    Serial.println("Grid Search Completed.");
}

float Game::randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void Game::runGeneticAlgorithm() {
    const int populationSize = 20;
    const int generations = 50;
    const float initialMutationRate = 0.1f;
    const float finalMutationRate = 0.01f;
    const float crossoverRate = 0.8f;
    const int eliteCount = 3; // Ensure top performers are preserved

    std::vector<Chromosome> population;
    for (int i = 0; i < populationSize; ++i) {
        Chromosome chromo = generateRandomChromosome();
        population.push_back(chromo);
    }

    for (int gen = 0; gen < generations; ++gen) {
        Serial.printf("Generation %d\n", gen + 1);
        // Evaluate fitness
        for (auto& chromo : population) {
            evaluateFitness(chromo);
            tetrisAI.printSimulationResults(chromo.score, chromo.linesCleared);
        }

        // Sort by fitness
        std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness > b.fitness;
        });

        // Carry over elites
        std::vector<Chromosome> newPopulation(population.begin(), population.begin() + eliteCount);

        // Diversity Monitoring
        float diversity = calculateDiversity(population);
        Serial.printf("Generation %d | Diversity: %.2f\n", gen + 1, diversity);

        // Adaptive Mutation
        float mutationRate = initialMutationRate - gen * (initialMutationRate - finalMutationRate) / generations;
        if (diversity < 0.1f) mutationRate *= 2.0f; // Boost mutation if diversity drops

        // Generate new population
        while (newPopulation.size() < populationSize) {
            if (randomFloat(0.0f, 1.0f) < crossoverRate) {
                int parent1Idx = random(0, populationSize / 2);
                int parent2Idx = random(0, populationSize / 2);
                Chromosome offspring = crossover(population[parent1Idx], population[parent2Idx]);
                mutate(offspring, mutationRate);
                newPopulation.push_back(offspring);
            } else {
                int parentIdx = random(0, populationSize / 2);
                newPopulation.push_back(population[parentIdx]);
            }
        }

        // Periodic Random Injection
        if (gen % 10 == 0) {
            for (int i = 0; i < eliteCount; ++i) {
                newPopulation[random() % populationSize] = generateRandomChromosome();
            }
        }

        population = newPopulation;
    }

    Chromosome bestChromo = population[0];
    Serial.printf("Best Weights: HeightWeight: %.2f, HoleWeight: %.2f, BumpinessWeight: %.2f, LineClearWeight: %.2f, MaxHeightWeight: %.2f\n",
                  bestChromo.heightWeight, bestChromo.holeWeight, bestChromo.bumpinessWeight,
                  bestChromo.lineClearWeight, bestChromo.maxHeightWeight);
}

void Game::mutate(Chromosome& chromo, float mutationRate) {
    // Adjust mutation range for finer tuning
    const float explorationFactor = 0.2f; // Controls the extent of exploration

    if (randomFloat(0.0f, 1.0f) < mutationRate) {
        // Mutate heightWeight with some randomness
        float adjustment = randomFloat(-explorationFactor, explorationFactor);
        chromo.heightWeight += adjustment;
        chromo.heightWeight = clamp(chromo.heightWeight, heightWeightMin, heightWeightMax);
    }
    if (randomFloat(0.0f, 1.0f) < mutationRate) {
        // Mutate holeWeight with some randomness
        float adjustment = randomFloat(-explorationFactor, explorationFactor);
        chromo.holeWeight += adjustment;
        chromo.holeWeight = clamp(chromo.holeWeight, holeWeightMin, holeWeightMax);
    }
    if (randomFloat(0.0f, 1.0f) < mutationRate) {
        // Mutate bumpinessWeight with some randomness
        float adjustment = randomFloat(-explorationFactor, explorationFactor);
        chromo.bumpinessWeight += adjustment;
        chromo.bumpinessWeight = clamp(chromo.bumpinessWeight, bumpinessWeightMin, bumpinessWeightMax);
    }
    if (randomFloat(0.0f, 1.0f) < mutationRate) {
        // Mutate lineClearWeight with some randomness
        float adjustment = randomFloat(-explorationFactor, explorationFactor);
        chromo.lineClearWeight += adjustment;
        chromo.lineClearWeight = clamp(chromo.lineClearWeight, lineClearWeightMin, lineClearWeightMax);
    }
    if (randomFloat(0.0f, 1.0f) < mutationRate) {
        // Mutate maxHeightWeight with some randomness
        float adjustment = randomFloat(-explorationFactor, explorationFactor);
        chromo.maxHeightWeight += adjustment;
        chromo.maxHeightWeight = clamp(chromo.maxHeightWeight, maxHeightWeightMin, maxHeightWeightMax);
    }
}

Game::Chromosome Game::crossover(const Chromosome& parent1, const Chromosome& parent2) {
    Chromosome offspring;

    // Blend crossover: Combine weights from both parents with random weighting
    offspring.heightWeight = randomFloat(0.4f, 0.6f) * parent1.heightWeight +
                             (1.0f - randomFloat(0.4f, 0.6f)) * parent2.heightWeight;

    offspring.holeWeight = randomFloat(0.4f, 0.6f) * parent1.holeWeight +
                           (1.0f - randomFloat(0.4f, 0.6f)) * parent2.holeWeight;

    offspring.bumpinessWeight = randomFloat(0.4f, 0.6f) * parent1.bumpinessWeight +
                                (1.0f - randomFloat(0.4f, 0.6f)) * parent2.bumpinessWeight;

    offspring.lineClearWeight = randomFloat(0.4f, 0.6f) * parent1.lineClearWeight +
                                (1.0f - randomFloat(0.4f, 0.6f)) * parent2.lineClearWeight;

    offspring.maxHeightWeight = randomFloat(0.4f, 0.6f) * parent1.maxHeightWeight +
                                (1.0f - randomFloat(0.4f, 0.6f)) * parent2.maxHeightWeight;

    // Clamp offspring weights to valid ranges
    offspring.heightWeight = clamp(offspring.heightWeight, heightWeightMin, heightWeightMax);
    offspring.holeWeight = clamp(offspring.holeWeight, holeWeightMin, holeWeightMax);
    offspring.bumpinessWeight = clamp(offspring.bumpinessWeight, bumpinessWeightMin, bumpinessWeightMax);
    offspring.lineClearWeight = clamp(offspring.lineClearWeight, lineClearWeightMin, lineClearWeightMax);
    offspring.maxHeightWeight = clamp(offspring.maxHeightWeight, maxHeightWeightMin, maxHeightWeightMax);

    return offspring;
}

float Game::clamp(float value, float minValue, float maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

Game::Chromosome Game::generateRandomChromosome() {
    Chromosome chromo;
    chromo.heightWeight = randomFloat(heightWeightMin, heightWeightMax);
    chromo.holeWeight = randomFloat(holeWeightMin, holeWeightMax);
    chromo.bumpinessWeight = randomFloat(bumpinessWeightMin, bumpinessWeightMax);
    chromo.lineClearWeight = randomFloat(lineClearWeightMin, lineClearWeightMax);
    chromo.maxHeightWeight = randomFloat(maxHeightWeightMin, maxHeightWeightMax);
    return chromo;
}

void Game::evaluateFitness(Chromosome& chromo) {
    tetrisAI.setWeights(
        chromo.heightWeight,
        chromo.maxHeightWeight,
        chromo.holeWeight,
        chromo.bumpinessWeight,
        chromo.lineClearWeight
    );

    resetGame();

    while (!blockMap.checkGameOver()) {
        if (!shape) createNewShape();
        else executeAIStep();
    }

    chromo.score = score;
    chromo.linesCleared = linesCleared;
    chromo.fitness = (chromo.score * 1.0f) + (chromo.linesCleared * 0.5f) - (blockMap.checkGameOver() ? 1000.0f : 0);
}

float Game::calculateDiversity(const std::vector<Chromosome>& population) {
    float diversity = 0.0f;
    for (size_t i = 0; i < population.size(); ++i) {
        for (size_t j = i + 1; j < population.size(); ++j) {
            diversity += calculateChromosomeDifference(population[i], population[j]);
        }
    }
    return diversity / (population.size() * (population.size() - 1) / 2);
}

float Game::calculateChromosomeDifference(const Chromosome& a, const Chromosome& b) {
    return fabs(a.heightWeight - b.heightWeight) +
           fabs(a.holeWeight - b.holeWeight) +
           fabs(a.bumpinessWeight - b.bumpinessWeight) +
           fabs(a.lineClearWeight - b.lineClearWeight) +
           fabs(a.maxHeightWeight - b.maxHeightWeight);
}