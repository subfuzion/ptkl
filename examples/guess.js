function guessNumberGame(low, high) {
  const randomNumber = Math.floor(Math.random() * high) + low;
  let guessesLeft = 10;
  let lastGuess = 0;
  let side = 0;
  let newLow = low;
  let newHigh = high;

  function getRandomIntInclusive(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1) + min);
  }

  function prompt(question, low, high, lastGuess, side) {
    print (question);

    let answer;
    if (lastGuess === 0) {
      answer = getRandomIntInclusive(low, high);
    } else if (side < 0) {
      newLow = lastGuess + 1;
      answer = newLow + Math.floor((newHigh - newLow) / 2);
    } else if (side > 0) {
      newHigh = lastGuess - 1;
      answer = newLow + Math.floor((newHigh - newLow) / 2) ;
    }
    answer = Math.floor(answer);
    print (`> ${answer}\n`);
    return answer < low ? low : answer > high ? high : answer;
  }

  function askForGuess(low, high, lastGuess, side) {
    const guess =
        prompt(`Guess a number between ${low} and ${high}. You have ${guessesLeft} guesses left:`,
            low, high, lastGuess, side);

    print(guess);

    if (isNaN(guess) || guess < 1 || guess > 100) {
      print("Invalid input. Please enter a number between 1 and 100.");
      askForGuess(low, high, 0, 0);
      return;
    }

    guessesLeft--;
    lastGuess = guess;

    if (guess === randomNumber) {
      print(`Congratulations! You guessed the number ${randomNumber} correctly!`);
      return;
    }

    if (guessesLeft === 0) {
      print(`You ran out of guesses. The number was ${randomNumber}.`);
      return;
    }

    if (guess < randomNumber) {
      print("Too low! Try again.");
      side = -1;
    } else {
      print("Too high! Try again.");
      side = +1;
    }
    askForGuess(low, high, lastGuess, side);
  }
  print("Welcome to the Number Guessing Game!");
  askForGuess(low, high, lastGuess, side);
}

guessNumberGame(1, 100);